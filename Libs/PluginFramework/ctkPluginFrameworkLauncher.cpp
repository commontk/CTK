/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

#include <QStringList>
#include <QDirIterator>
#include <QFileInfo>
#include <QDebug>

#include "ctkPluginFrameworkLauncher.h"
#include "ctkPluginFrameworkFactory.h"
#include "ctkPluginFramework.h"
#include "ctkPluginContext.h"
#include "ctkPluginException.h"

#ifdef _WIN32
#include <windows.h>
#include <stdlib.h>
#endif // _WIN32

#include <CTKConfig.h>

class ctkPluginFrameworkLauncherPrivate
{
public:

  ctkPluginFrameworkLauncherPrivate()
    : fwFactory(0)
  {
#ifdef CMAKE_INTDIR
    QString pluginPath = CTK_PLUGIN_DIR CMAKE_INTDIR "/";
#else
    QString pluginPath = CTK_PLUGIN_DIR;
#endif

    pluginSearchPaths.append(pluginPath);

    pluginLibFilter << "*.dll" << "*.so" << "*.dylib";
  }

  QStringList pluginSearchPaths;
  QStringList pluginLibFilter;

  ctkProperties fwProps;

  ctkPluginFrameworkFactory* fwFactory;
};

const QScopedPointer<ctkPluginFrameworkLauncherPrivate> ctkPluginFrameworkLauncher::d(
  new ctkPluginFrameworkLauncherPrivate());

//----------------------------------------------------------------------------
void ctkPluginFrameworkLauncher::setFrameworkProperties(const ctkProperties& props)
{
  d->fwProps = props;
}

//----------------------------------------------------------------------------
bool ctkPluginFrameworkLauncher::install(const QString& symbolicName)
{
  QString pluginPath = getPluginPath(symbolicName);
  if (pluginPath.isEmpty()) return false;

  if (d->fwFactory == 0) {
    d->fwFactory = new ctkPluginFrameworkFactory(d->fwProps);
    try
    {
      d->fwFactory->getFramework()->init();
    }
    catch (const ctkPluginException& exc)
    {
      qCritical() << "Failed to initialize the plug-in framework:" << exc;
      delete d->fwFactory;
      d->fwFactory = 0;
      return false;
    }
  }

  try
  {
    getPluginContext()->installPlugin(QUrl::fromLocalFile(pluginPath));
  }
  catch (const ctkPluginException& exc)
  {
    qWarning() << "Failed to install plugin:" << exc;
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------
bool ctkPluginFrameworkLauncher::start(const QString& symbolicName, ctkPlugin::StartOptions options)
{
  QString pluginPath = getPluginPath(symbolicName);
  if (pluginPath.isEmpty()) return false;

  if (d->fwFactory == 0) {
    d->fwFactory = new ctkPluginFrameworkFactory(d->fwProps);
    try
    {
      d->fwFactory->getFramework()->start();
    }
    catch (const ctkPluginException& exc)
    {
      qCritical() << "Failed to start the plug-in framework:" << exc;
      delete d->fwFactory;
      d->fwFactory = 0;
      return false;
    }
  }
  else if (d->fwFactory->getFramework()->getState() != ctkPlugin::ACTIVE)
  {
    try
    {
      d->fwFactory->getFramework()->start(options);
    }
    catch (const ctkPluginException& exc)
    {
      qCritical() << "Failed to start the plug-in framework:" << exc;
      delete d->fwFactory;
      d->fwFactory = 0;
      return false;
    }
  }

  try
  {
    getPluginContext()->installPlugin(QUrl::fromLocalFile(pluginPath))->start(options);
  }
  catch (const ctkPluginException& exc)
  {
    qWarning() << "Failed to install plugin:" << exc;
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------
ctkPluginContext* ctkPluginFrameworkLauncher::getPluginContext()
{
  if (d->fwFactory == 0) return 0;
  return d->fwFactory->getFramework()->getPluginContext();
}

//----------------------------------------------------------------------------
QSharedPointer<ctkPluginFramework> ctkPluginFrameworkLauncher::getPluginFramework()
{
  if (d->fwFactory)
    return d->fwFactory->getFramework();
  return QSharedPointer<ctkPluginFramework>();
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkLauncher::appendPathEnv(const QString& path)
{
#ifdef _WIN32
#ifdef __MINGW32__
  QString pathVar("PATH");
  QString oldPath(getenv("PATH"));
  pathVar += "=" + oldPath + ";" + path;
  if(_putenv(qPrintable(pathVar)))
#else
  std::size_t bufferLength;
  getenv_s(&bufferLength, NULL, 0, "PATH");
  QString newPath = path;
  if (bufferLength > 0)
  {
    char* oldPath = new char[bufferLength];
    getenv_s(&bufferLength, oldPath, bufferLength, "PATH");
    newPath.append(";").append(oldPath);
    delete[] oldPath;
  }
  qDebug() << "new PATH:" << newPath;
  if(_putenv_s("PATH", qPrintable(newPath)))
#endif
  {
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    QString msg = QString("Adding '%1' to the PATH environment variable failed: %2")
      .arg(path).arg(QString((LPCTSTR)lpMsgBuf));

    qWarning() << msg;

    LocalFree(lpMsgBuf);
  }
#else
  Q_UNUSED(path)
#endif
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkLauncher::addSearchPath(const QString& searchPath, bool addToPathEnv)
{
  d->pluginSearchPaths.prepend(searchPath);
  if (addToPathEnv) appendPathEnv(searchPath);
}

//----------------------------------------------------------------------------
QString ctkPluginFrameworkLauncher::getPluginPath(const QString& symbolicName)
{
  QString pluginFileName(symbolicName);
  pluginFileName.replace(".", "_");
  foreach(QString searchPath, d->pluginSearchPaths)
  {
    QDirIterator dirIter(searchPath, d->pluginLibFilter, QDir::Files);
    while(dirIter.hasNext())
    {
      dirIter.next();
      QFileInfo fileInfo = dirIter.fileInfo();
      QString fileBaseName = fileInfo.baseName();
      if (fileBaseName.startsWith("lib")) fileBaseName = fileBaseName.mid(3);

      if (fileBaseName == pluginFileName)
      {
        return fileInfo.canonicalFilePath();
      }
    }
  }

  return QString();
}

