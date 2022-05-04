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
// Qt includes
#include <QDir>
#include <QDebug>
#include <QLocale>

// CTK includes
#include "ctkLanguageComboBox.h"
#include <ctkPimpl.h>

// ----------------------------------------------------------------------------
class ctkLanguageComboBoxPrivate
{
  Q_DECLARE_PUBLIC(ctkLanguageComboBox);
protected:
  ctkLanguageComboBox* const q_ptr;
public:
  ctkLanguageComboBoxPrivate(ctkLanguageComboBox& object);
  void init();
  void updateLanguageItems();
  bool languageItem(const QString& localeCode,
                    QIcon& icon, QString& text,QVariant& data, bool showCountry);

  QString DefaultLanguage;
  QStringList LanguageDirectories;
  bool CountryFlagsVisible;
  bool UpdatingLanguageItems; // set to true while in UpdateLanguageItems()
};

// ----------------------------------------------------------------------------
ctkLanguageComboBoxPrivate::ctkLanguageComboBoxPrivate(ctkLanguageComboBox &object)
  : q_ptr(&object)
  , CountryFlagsVisible(true)
  , UpdatingLanguageItems(false)
{
}

// ----------------------------------------------------------------------------
void ctkLanguageComboBoxPrivate::init()
{
  Q_Q(ctkLanguageComboBox);

  QObject::connect(q, SIGNAL(currentIndexChanged(int)),
                   q, SLOT(onLanguageChanged(int)));

  // Add default language if any
  this->updateLanguageItems();
}

// ----------------------------------------------------------------------------
void ctkLanguageComboBoxPrivate::updateLanguageItems()
{
  Q_Q(ctkLanguageComboBox);

  if (this->UpdatingLanguageItems)
    {
    return;
    }
  this->UpdatingLanguageItems = true;
  QString languageBeforeUpdate = q->currentLanguage();

  // Save selection
  QString selectedLocaleCode = q->itemData(q->currentIndex()).toString();

  // Add default locale
  QStringList localeCodes;
  if (!this->DefaultLanguage.isEmpty())
  {
    localeCodes.append(this->DefaultLanguage);
  }

  // Get locale codes from translation files from all the specified directories
  foreach(const QString& languageDirectory, this->LanguageDirectories)
  {
    QDir translationDir = QDir(languageDirectory);
    QStringList fileNames = translationDir.entryList(QStringList("*.qm"));
    foreach(const QString & fileName, fileNames)
    {
      QFileInfo file(translationDir.filePath(fileName));
      if (!file.exists())
      {
        qWarning() << "File " << file.absoluteFilePath() << " doesn't exist.";
      }
      // localeCode is "de_ch" for a file named "/abc/def_de_ch.qm"
      QString localeCode = file.completeBaseName();
      localeCode.remove(0, localeCode.indexOf('_') + 1);
      localeCodes.append(localeCode);
    }
  }

  localeCodes.removeDuplicates();

  // Determine which languages have multiple locales
  QSet< QLocale::Language > languagesFound;
  QSet< QLocale::Language > languagesWithMultipleLocalesFound;
  foreach(QString localeCode, localeCodes)
  {
    QLocale locale(localeCode);
    if (languagesFound.contains(locale.language()))
    {
      // already found this language in another locale
      languagesWithMultipleLocalesFound.insert(locale.language());
    }
    else
    {
      languagesFound.insert(locale.language());
    }
  }

  // Set language items in combobox
  q->clear();
  foreach(QString localeCode, localeCodes)
  {
    QLocale locale(localeCode);
    bool showCountry = languagesWithMultipleLocalesFound.contains(locale.language());
    QIcon icon;
    QString text;
    QVariant data;
    bool res = this->languageItem(localeCode, icon, text, data, showCountry);
    if (res)
    {
      q->insertItem(q->count(), icon, text, data);
    }
  }

  // Items are inserted based on order in file folders (usually locale name,
  // so Spanish - "es" would be displayed above Finnish "fi"). To fix that
  // we now sort based on the displayed text.
  q->model()->sort(0, Qt::AscendingOrder);

  // Restore selection
  q->setCurrentIndex(q->findData(selectedLocaleCode));

  q->update();

  QString languageAfterUpdate = q->currentLanguage();
  this->UpdatingLanguageItems = false;

  // While this->UpdatingLanguageItems was set to true, current index changes were
  // ignored, therefore we need to emit the language name changed signal now.
  if (languageBeforeUpdate != languageAfterUpdate)
  {
    emit q->currentLanguageNameChanged(languageAfterUpdate);
  }
}

// ----------------------------------------------------------------------------
bool ctkLanguageComboBoxPrivate::languageItem(const QString& localeCode,
                                              QIcon& icon,
                                              QString& text,
                                              QVariant& data,
                                              bool showCountry)
{
  QLocale locale(localeCode);
  if (localeCode.isEmpty() ||
      locale.name() == "C")
    {
    icon = QIcon();
    text = QString();
    data = QVariant();
    return false;
    }

  if (this->CountryFlagsVisible)
  {
    QString countryFlag = locale.name();
    countryFlag.remove(0, countryFlag.lastIndexOf('_') + 1);
    countryFlag = countryFlag.toLower();
    icon = QIcon(QString(":Icons/Languages/%1.png").arg(countryFlag));
    text = QLocale::languageToString(locale.language());
  }
  else
  {
    icon = QIcon();
    if (showCountry)
    {
      // There are multiple countries for the locale's language therefore include the country name.
      text = QString("%1 (%2)")
        .arg(QLocale::languageToString(locale.language()))
        .arg(QLocale::countryToString(locale.country()));
    }
    else
    {
      // There is only one occurrence of the language, therefore omit the country name
      // (e.g., instead of "Hungarian (Hungary)" just show "Hungarian").
      text = QLocale::languageToString(locale.language());
    }
    
  }
  data = locale.name();

  return true;
}

CTK_GET_CPP(ctkLanguageComboBox, bool, countryFlagsVisible, CountryFlagsVisible);

// ----------------------------------------------------------------------------
ctkLanguageComboBox::ctkLanguageComboBox(QWidget* _parent)
  : QComboBox(_parent)
  , d_ptr(new ctkLanguageComboBoxPrivate(*this))
{
  Q_D(ctkLanguageComboBox);
  d->init();
}

// ----------------------------------------------------------------------------
ctkLanguageComboBox::ctkLanguageComboBox(const QString& defaultLanguage,
                                         QWidget* _parent)
  : QComboBox(_parent)
  , d_ptr(new ctkLanguageComboBoxPrivate(*this))
{
  Q_D(ctkLanguageComboBox);
  d->DefaultLanguage = defaultLanguage;
  d->init();
}

// ----------------------------------------------------------------------------
ctkLanguageComboBox::~ctkLanguageComboBox()
{
}

// ----------------------------------------------------------------------------
QString ctkLanguageComboBox::defaultLanguage() const
{
  Q_D(const ctkLanguageComboBox);
  return d->DefaultLanguage;
}

// ----------------------------------------------------------------------------
void ctkLanguageComboBox::setDefaultLanguage(const QString& localeCode)
{
  Q_D(ctkLanguageComboBox);
  d->DefaultLanguage = localeCode;
  d->updateLanguageItems();
}

// ----------------------------------------------------------------------------
QString ctkLanguageComboBox::currentLanguage() const
{
  return this->itemData(this->currentIndex()).toString();
}

// ----------------------------------------------------------------------------
void ctkLanguageComboBox::setCurrentLanguage(const QString &language)
{
  int index = this->findData(QVariant(language));
  this->setCurrentIndex(index);
}

// ----------------------------------------------------------------------------
QString ctkLanguageComboBox::directory() const
{
  Q_D(const ctkLanguageComboBox);
  if (d->LanguageDirectories.isEmpty())
  {
    return QString();
  }
  return d->LanguageDirectories.at(0);
}

// ----------------------------------------------------------------------------
void ctkLanguageComboBox::setDirectory(const QString& dir)
{
  this->setDirectories(QStringList() << dir);
}

// ----------------------------------------------------------------------------
QStringList ctkLanguageComboBox::directories() const
{
  Q_D(const ctkLanguageComboBox);
  return d->LanguageDirectories;
}

// ----------------------------------------------------------------------------
void ctkLanguageComboBox::setDirectories(const QStringList& dir)
{
  Q_D(ctkLanguageComboBox);
  d->LanguageDirectories = dir;
  d->updateLanguageItems();
}

// ----------------------------------------------------------------------------
void ctkLanguageComboBox::refreshFromDirectories()
{
  Q_D(ctkLanguageComboBox);
  d->updateLanguageItems();
}

// ----------------------------------------------------------------------------
void ctkLanguageComboBox::onLanguageChanged(int index)
{
  Q_UNUSED(index);
  Q_D(ctkLanguageComboBox);
  if (d->UpdatingLanguageItems)
    {
    // do not emit event during transient changes in UpdateLanguageItems()
    return;
    }
  QString currentLanguage = this->currentLanguage();
  emit currentLanguageNameChanged(currentLanguage);
}

// ----------------------------------------------------------------------------
void ctkLanguageComboBox::setCountryFlagsVisible(bool visible)
{
  Q_D(ctkLanguageComboBox);
  if (d->CountryFlagsVisible == visible)
  {
    return;
  }
  d->CountryFlagsVisible = visible;
  d->updateLanguageItems();
}
