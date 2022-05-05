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
class ctkLanguageComboBoxPrivate;

/// ctkLanguageComboBox is a simple QComboBox to select the language
/// of your application.
/// You have to set the default language of your application and then
/// set the directory to allow the comboBox to find the translation files.
/// ctkLanguageComboBox automatically recognizes the language of the
/// translation file by the suffix "_en" or "_fr" and add the associated
/// language to the comboBox.
/// \note:
/// Translation files names need to finish with the suffix of the
/// country.
/// Example: for a french traduction, xxxx_fr.ts
/// \warning Please don't use QComboBox methods when using this class.
class CTK_WIDGETS_EXPORT ctkLanguageComboBox : public QComboBox
{
  Q_OBJECT
  /// \brief This property controls the default language of the application.
  ///
  /// The default language is the language in which all the texts in GUI
  /// elements are written.
  /// As the application doesn't have translation file for the default
  /// language, this property adds an item to the combobox.
  /// The language format is a lowercase, two-letter, ISO 639 language code.
  /// For example: "fr", "en" or "de_ch".
  /// If empty, there is no default language, and there is no entry added.
  /// By default, there is no default language.
  /// \sa defaultLanguage(), setDefaultLanguage(), QLocale::setDefault()
  Q_PROPERTY(QString defaultLanguage READ defaultLanguage WRITE setDefaultLanguage)

  /// This property controls the directory where the translation files are
  /// located. Setting it removes all other directories. When multiple directories
  /// are set then it returns the first directory.
  /// \sa directory(), setDirectory()
  Q_PROPERTY(QString directory READ directory WRITE setDirectory)

  /// This property controls the directories where the translation files are
  /// located.
  /// \sa directories(), setDirectories()
  Q_PROPERTY(QStringList directories READ directories WRITE setDirectories)

  /// This property controls the current language of the combobox.
  /// The \a defaultLanguage by default.
  /// \sa currentLanguage(), setCurrentLanguage()
  Q_PROPERTY(QString currentLanguage READ currentLanguage WRITE setCurrentLanguage NOTIFY currentLanguageNameChanged USER true)

  /// Controls visibility of country flags. It is enabled by default.
  /// \sa countryFlagsVisible(), setCountryFlagsVisible()
  Q_PROPERTY(bool countryFlagsVisible READ countryFlagsVisible WRITE setCountryFlagsVisible)

public:
  typedef QComboBox Superclass;
  /// Constructor of ctkLanguageComboBox
  ctkLanguageComboBox(QWidget *parent = 0);
  /// Constructor that specifies a default language.
  /// \sa defaultLanguage
  ctkLanguageComboBox(const QString& defaultLanguage, QWidget *parent = 0);
  virtual ~ctkLanguageComboBox();

  /// Return the default language.
  /// \sa defaultLanguage, setDefaultLanguage()
  QString defaultLanguage()const;

  /// Set the default language. The previous default language is removed and
  /// replaced with the new default language.
  /// \sa defaultLanguage, defaultLanguage()
  void setDefaultLanguage(const QString& language);

  /// Set the \a directory with all the translation files.
  /// \sa setDirectories
  QString directory()const;
  void setDirectory(const QString& dir);

  /// Set the \a directories that may contain translation files.
  /// The list of available languages will be populated based on
  /// the discovered translation files.
  /// The default language will still be the first item in the menu.
  /// Empty by default.
  /// If translation files are added/removed from the directories then
  /// refreshFromDirectories() slot can be used to update the displayed list.
  QStringList directories()const;
  void setDirectories(const QStringList& dir);

  /// Return the currently selected language of the combobox.
  /// \sa currentLanguage, setCurrentLanguage()
  QString currentLanguage()const;

  /// Controls visibility of country flags.
  /// If enabled then a country's flag and the language name are be displayed.
  /// If disabled then language name is displayed along with the country's name (if there are multiple translations
  /// for the language).
  /// Recommended to be disabled when working with languages that are associated with many countries, such as Spanish:
  /// displaying flag of Spain would not be appropriate for a generic Spanish translation;
  /// Latin-America (for es-419 locale, https://en.wikipedia.org/wiki/Language_code) does not have a flag.
  bool countryFlagsVisible()const;
  void setCountryFlagsVisible(bool visible);

public Q_SLOTS:
  /// Set the current language
  /// \sa currentLanguage, currentLanguage()
  void setCurrentLanguage(const QString& language);

  /// Refresh the list of languages from the currently set directories.
  /// \sa setDirectories()
  void refreshFromDirectories();

protected slots:
  void onLanguageChanged(int index);

signals:
  /// Signals emitted when the current language changed.
  /// \sa QLocale::name()
  void currentLanguageNameChanged(const QString&);

protected:
  QScopedPointer<ctkLanguageComboBoxPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkLanguageComboBox);
  Q_DISABLE_COPY(ctkLanguageComboBox);
};
#endif // __ctkLanguageComboBox_h

