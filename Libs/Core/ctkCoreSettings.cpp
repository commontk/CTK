/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

#include "ctkCoreSettings.h"

// CTK includes
#include <ctkPimpl.h>

// Qt includes
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QVariant>


//-----------------------------------------------------------------------------
class ctkCoreSettingsPrivate
{
  Q_DECLARE_PUBLIC(ctkCoreSettings);
protected:
  ctkCoreSettings* const q_ptr;
public:
  ctkCoreSettingsPrivate(ctkCoreSettings& object);
  virtual ~ctkCoreSettingsPrivate();

  QString ApplicationHomeDirectory;
  QString ApplicationHomePlaceholder;
};

//-----------------------------------------------------------------------------
ctkCoreSettingsPrivate::ctkCoreSettingsPrivate(ctkCoreSettings& object)
  : q_ptr(&object)
{
  this->ApplicationHomePlaceholder = "<APPLICATION_HOME_DIR>";
}

// --------------------------------------------------------------------------
ctkCoreSettingsPrivate::~ctkCoreSettingsPrivate()
{
}

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkCoreSettings, QString, applicationHomeDirectory, ApplicationHomeDirectory);
CTK_SET_CPP(ctkCoreSettings, const QString&, setApplicationHomeDirectory, ApplicationHomeDirectory);
CTK_GET_CPP(ctkCoreSettings, QString, applicationHomePlaceholder, ApplicationHomePlaceholder);
CTK_SET_CPP(ctkCoreSettings, const QString&, setApplicationHomePlaceholder, ApplicationHomePlaceholder);

//-----------------------------------------------------------------------------
ctkCoreSettings::ctkCoreSettings(const QString& organization,
                         const QString& application,
                         QObject* parentObject)
  : QSettings(organization, application, parentObject)
  , d_ptr(new ctkCoreSettingsPrivate(*this))
{
}

//-----------------------------------------------------------------------------
ctkCoreSettings::~ctkCoreSettings()
{
}

//-----------------------------------------------------------------------------
ctkCoreSettings::ctkCoreSettings(QSettings::Scope scope,
                         const QString& organization,
                         const QString& application,
                         QObject* parentObject)
  : QSettings(scope, organization,application, parentObject)
  , d_ptr(new ctkCoreSettingsPrivate(*this))
{
}

//-----------------------------------------------------------------------------
ctkCoreSettings::ctkCoreSettings(QSettings::Format format,
                         QSettings::Scope scope,
                         const QString& organization,
                         const QString& application,
                         QObject* parentObject)
  : QSettings(format, scope, organization, application, parentObject)
  , d_ptr(new ctkCoreSettingsPrivate(*this))
{
}

//-----------------------------------------------------------------------------
ctkCoreSettings::ctkCoreSettings(const QString& fileName, QSettings::Format format, QObject* parentObject)
  : QSettings(fileName, format, parentObject)
  , d_ptr(new ctkCoreSettingsPrivate(*this))
{
}

//-----------------------------------------------------------------------------
ctkCoreSettings::ctkCoreSettings(QObject* parentObject)
  : QSettings(parentObject)
  , d_ptr(new ctkCoreSettingsPrivate(*this))
{
}

//-----------------------------------------------------------------------------
void ctkCoreSettings::setPathValue(const QString& key, const QVariant& value)
{
  if (!value.isValid() || value.isNull())
    {
    this->setValue(key, value);
    }
  else if (QString(value.typeName()).compare("QStringList") == 0)
    {
    // string list
    QStringList relativePaths = this->toApplicationHomeRelativePaths(value.toStringList());
    this->setValue(key, relativePaths);
    }
  else if (value.canConvert<QString>())
    {
    // single string
    QString relativePath = this->toApplicationHomeRelativePath(value.toString());
    this->setValue(key, relativePath);
    }
  else
    {
    qWarning() << Q_FUNC_INFO << ": cannot interpret value type " << value.typeName() << " as path";
    this->setValue(key, value);
    }
}

//-----------------------------------------------------------------------------
QVariant ctkCoreSettings::pathValue(const QString& key, const QVariant& defaultValue/*=QVariant()*/) const
{
  Q_D(const ctkCoreSettings);
  QVariant value = this->value(key, defaultValue);
  if (!value.isValid() || value.isNull())
    {
    return value;
    }
  else if (QString(value.typeName()).compare("QStringList") == 0)
    {
    // string list
    QStringList relativePaths = this->toApplicationHomeAbsolutePaths(value.toStringList());
    return relativePaths;
    }
  else if (value.canConvert<QString>())
    {
    // single string
    QString relativePath = this->toApplicationHomeAbsolutePath(value.toString());
    return relativePath;
    }
  else
    {
    qWarning() << Q_FUNC_INFO << ": cannot interpret value type " << value.typeName() << " as path";
    return value;
    }
}

//-----------------------------------------------------------------------------
QString ctkCoreSettings::toApplicationHomeAbsolutePath(const QString& path) const
{
  Q_D(const ctkCoreSettings);
  if (path.isEmpty() || d->ApplicationHomeDirectory.isEmpty())
    {
    return path;
    }
  QString absolutePath = path;
  absolutePath.replace(d->ApplicationHomePlaceholder, QDir::cleanPath(d->ApplicationHomeDirectory));
  return absolutePath;
}

//-----------------------------------------------------------------------------
QString ctkCoreSettings::toApplicationHomeRelativePath(const QString& path) const
{
  Q_D(const ctkCoreSettings);
  if (path.isEmpty() || d->ApplicationHomeDirectory.isEmpty())
    {
    return path;
    }
  if (path.contains(d->ApplicationHomePlaceholder))
    {
    // already relative path
    return path;
    }
  // Check if path is within ApplicationHomeDirectory
  // startsWith(const QString &s, Qt::CaseSensitivity cs = Qt::CaseSensitive) const
  QString cleanHomeDir = QDir::cleanPath(d->ApplicationHomeDirectory);
  QString cleanPath = QDir::cleanPath(path);
  // There is no way to know if a file system is case sensitive or insensitive
  // (and the path may not even exist), so we choose case insensitive comparison to
  // make child folder detection more robust.
  if (!cleanPath.startsWith(cleanHomeDir, Qt::CaseInsensitive))
    {
    return path;
    }  
  // relative path
  QString relativePath = QDir(cleanHomeDir).relativeFilePath(cleanPath);
  return QDir(d->ApplicationHomePlaceholder).filePath(relativePath);
}

//-----------------------------------------------------------------------------
QStringList ctkCoreSettings::toApplicationHomeAbsolutePaths(const QStringList& paths) const
{
  Q_D(const ctkCoreSettings);
  QStringList absolutePaths;
  foreach(QString path, paths)
    {
    absolutePaths << this->toApplicationHomeAbsolutePath(path);
    }
  return absolutePaths;
}


//-----------------------------------------------------------------------------
QStringList ctkCoreSettings::toApplicationHomeRelativePaths(const QStringList& paths) const
{
  Q_D(const ctkCoreSettings);
  QStringList relativePaths;
  foreach(QString path, paths)
    {
    relativePaths << this->toApplicationHomeRelativePath(path);
    }
  return relativePaths;
}
