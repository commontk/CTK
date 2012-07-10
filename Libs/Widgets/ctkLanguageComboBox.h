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

#ifndef __ctkLanguageComboBox_h
#define __ctkLanguageComboBox_h

// QT includes
#include <QComboBox>

// CTK includes
#include "ctkWidgetsExport.h"

/// ctkLanguageComboBox is a simple QComboBox to select the language
/// of your application.

/// You have to set the default language of your application and then
/// set the directory to allow the comboBox to find the translation files.
/// ctkLanguageComboBox automaically recognizes the language of the
/// translation file by the suffix _en or _fr and add the associated
/// language to the comboBox.

/// \note:
/// Translation files names need to finish with the suffix of the
/// country.
/// Example: for a french traduction, xxxx_fr.ts

class ctkLanguageComboBoxPrivate;

class CTK_WIDGETS_EXPORT ctkLanguageComboBox : public QComboBox
{
  Q_OBJECT
  Q_PROPERTY(QString currentLanguage READ currentLanguage WRITE setCurrentLanguage)
  Q_PROPERTY(QString directory READ directory WRITE setDirectory)

public:
  typedef QComboBox Superclass;
  ctkLanguageComboBox(QWidget *parent = 0);
  virtual ~ctkLanguageComboBox();

  /// Return the currentLanguage of the combobox.
  QString currentLanguage()const;
  void setCurrentLanguage(const QString& language);

  /// Set the default language of your application.
  /// As the application doesn't have translation file for the default
  /// language, it's very important to set this variable. Otherwise,
  /// the default language is not added to the ComboBox.
  QString defaultLanguage()const;
  void setDefaultLanguage(const QString& language);

  /// Set the \a directory with all the translation files.
  /// The list of available languages will be populated based on
  /// the discovered translation files.
  QString directory()const;
  void setDirectory(const QString& dir);

protected slots:
  void onLanguageChanged(int index);

signals:
  /// Signals emitted when the current language changed.
  void currentLanguageNameChanged(const QString&);

protected:
  QScopedPointer<ctkLanguageComboBoxPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkLanguageComboBox);
  Q_DISABLE_COPY(ctkLanguageComboBox);
};
#endif // __ctkLanguageComboBox_h

