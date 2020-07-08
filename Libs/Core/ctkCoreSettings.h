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

/// \file ctkCoreSettings.h
///

#ifndef __ctkCoreSettings_h
#define __ctkCoreSettings_h

// Qt includes
#include <QSettings>
#include <QString>
#include <QStringList>

// CTK includes
#include "ctkCoreExport.h"

class QDialog;
class QMainWindow;
class ctkCoreSettingsPrivate;

/// \ingroup Widgets
/// ctkCoreSettings is a QSettings that store absolute paths as relative paths
/// to allow the application to be relocatable.
class CTK_CORE_EXPORT ctkCoreSettings : public QSettings
{
  Q_OBJECT

  /// If applicationHomeDirectory is not empty then paths within this directory will be written out
  /// relative to this directory (by replacing absolute path of applicationHomeDirectory by
  /// applicationHomePlaceholder special string). This allows the application to read/write absolute paths
  /// to the settings, yet make all the files within applicationHomeDirectory relocatable.
  ///
  /// Example: If applicationHomeDirectory is set to /some/path/to/myapp 
  /// and applicationHomePlaceholder is left at the default <APPLICATION_HOME_DIR> value then
  /// then /some/path/to/myapp/data will be stored in settings as <APPLICATION_HOME_DIR>/data.
  /// If the application is moved over to /someotherplace/path/to/myapp
  /// then <APPLICATION_HOME_DIR>/data path stored in settings remains valid.

  /// Subfolders within this directory will be stored as relative folders.
  /// Empty by default, which means no substitution is performed.
  Q_PROPERTY(QString applicationHomeDirectory READ applicationHomeDirectory WRITE setApplicationHomeDirectory)

  /// applicationHomeDirectory absolute path is replaced by this string in settings file.
  Q_PROPERTY(QString applicationHomePlaceholder READ applicationHomePlaceholder WRITE setApplicationHomePlaceholder)

public:
  /// \see QSettings::QSettings(const QString& ,const QString& , QObject* )
  ctkCoreSettings(
    const QString& organization,
    const QString& application,
    QObject* parent = 0);
  /// \see QSettings::QSettings(QSettings::Scope ,const QString& ,const QString& , QObject* )
  ctkCoreSettings(
    QSettings::Scope scope,
    const QString& organization,
    const QString& application = QString(),
    QObject* parent = 0);
  /// \see QSettings::QSettings(QSettings::Format ,QSettings::Scope ,const QString& ,const QString& , QObject* )
  ctkCoreSettings(
    QSettings::Format format,
    QSettings::Scope scope,
    const QString& organization,
    const QString& application = QString(),
    QObject* parent = 0);
  /// \see QSettings::QSettings(const QString& , QSettings::Format , QObject* )
  ctkCoreSettings(const QString& fileName, QSettings::Format format, QObject* parent = 0);
  /// \see QSettings::QSettings(QObject*)
  ctkCoreSettings(QObject* parent = 0);
  virtual ~ctkCoreSettings();

  QString applicationHomeDirectory() const;
  QString applicationHomePlaceholder() const;

  /// Save paths(s) stored in QString or QStringList. If path is within applicationHomeDirectory then it will be written
  /// as relative path to the settings file.
  Q_INVOKABLE void setPathValue(const QString& key, const QVariant& value);

  /// Get path(s) QString or QStringList value as path. Paths relative to applicationHomeDirectory are
  /// converted to absolute path.
  Q_INVOKABLE QVariant pathValue(const QString& key, const QVariant& defaultValue = QVariant()) const;

  Q_INVOKABLE QString toApplicationHomeAbsolutePath(const QString& path) const;
  Q_INVOKABLE QString toApplicationHomeRelativePath(const QString& path) const;
  Q_INVOKABLE QStringList toApplicationHomeAbsolutePaths(const QStringList& paths) const;
  Q_INVOKABLE QStringList toApplicationHomeRelativePaths(const QStringList& paths) const;

public Q_SLOTS:
  void setApplicationHomeDirectory(const QString& dir);
  void setApplicationHomePlaceholder(const QString& dir);

protected:
  QScopedPointer<ctkCoreSettingsPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkCoreSettings);
  Q_DISABLE_COPY(ctkCoreSettings);
};

#endif
