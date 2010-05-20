/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
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

#include "ctkPluginManager.h"

#include <QServiceManager>
#include <QDirIterator>
#include <QDebug>
#include <QLibrary>
#include <QApplication>


class PluginManagerPrivate
{
public:

  QList<QString> pluginPaths;
  QServiceManager serviceManager;
};

PluginManager::PluginManager()
: d_ptr(new PluginManagerPrivate())
{
  Q_D(PluginManager);

  QString libName("CTKCore");
  QLibrary lib(libName);
  QFileInfo fileInfo(libName);
  QString libBaseName(fileInfo.baseName());
  if (libBaseName.startsWith("lib"))
  {
    libBaseName.remove(0, 3);
  }
  qDebug() << libBaseName;
  lib.load();
  if (lib.isLoaded())
  {
    QString xyz = QString(":/") + libBaseName + "/servicedescriptor.xml";
    qDebug() << "resource string: " << xyz;
    QFile serviceDescriptor(xyz);
    qDebug() << "file exists: " << serviceDescriptor.exists();
    qDebug() << "open returns:" << serviceDescriptor.open(QIODevice::ReadOnly);
    qDebug() << "file open: " << serviceDescriptor.isOpen();
    qDebug() << "file is readable: " << serviceDescriptor.isReadable();
    //QByteArray serviceBA = serviceDescriptor.readAll();
    //qDebug() << serviceBA;
    qDebug() << "Service for " << libBaseName << " registered:" << d->serviceManager.addService(&serviceDescriptor);
    lib.unload();
  }
}

PluginManager::~PluginManager()
{
  Q_D(PluginManager);
  delete d;
}

QServiceManager* PluginManager::serviceManager()
{
  Q_D(PluginManager);
  return &(d->serviceManager);
}

void PluginManager::addSearchPath(const QString & searchPath)
{
  Q_D(PluginManager);
  d->pluginPaths.push_back(searchPath);
}

void PluginManager::startAllPlugins()
{
  Q_D(PluginManager);
  QDirIterator it(d->pluginPaths.front(), QDir::Files);
  if (it.hasNext())
  {
    qApp->addLibraryPath(d->pluginPaths.front());
  }
  while (it.hasNext()) {
       QString libName(it.next());
       QLibrary lib(libName);
       QFileInfo fileInfo(libName);
       QString libBaseName(fileInfo.baseName());
       if (libBaseName.startsWith("lib"))
       {
         libBaseName.remove(0, 3);
       }
       qDebug() << libBaseName;
       lib.load();
       if (lib.isLoaded())
       {
         QString xyz = QString(":/") + libBaseName + "/servicedescriptor.xml";
         qDebug() << "resource string: " << xyz;
         QFile serviceDescriptor(xyz);
         //qDebug() << "file exists: " << serviceDescriptor.exists();
         qDebug() << "open returns:" << serviceDescriptor.open(QIODevice::ReadOnly);
         qDebug() << "file open: " << serviceDescriptor.isOpen();
         qDebug() << "file is readable: " << serviceDescriptor.isReadable();
         //QByteArray serviceBA = serviceDescriptor.readAll();
         //qDebug() << serviceBA;
         qDebug() << "Service for " << libBaseName << " registered:" << d->serviceManager.addService(&serviceDescriptor);
         lib.unload();
       }
  }


}
