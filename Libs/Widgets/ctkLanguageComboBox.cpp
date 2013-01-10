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

// ----------------------------------------------------------------------------
class ctkLanguageComboBoxPrivate
{
  Q_DECLARE_PUBLIC(ctkLanguageComboBox);
protected:
  ctkLanguageComboBox* const q_ptr;
public:
  ctkLanguageComboBoxPrivate(ctkLanguageComboBox& object);
  void init();
  void addLanguageFiles(const QStringList& fileNames);
  bool addLanguage(const QString& language);
  bool insertLanguage(int index, const QString& language);
  bool languageItem(const QString& language,
                    QIcon& icon, QString& text,QVariant& data);

  QString DefaultLanguage;
  QString LanguageDirectory;
};

// ----------------------------------------------------------------------------
ctkLanguageComboBoxPrivate::ctkLanguageComboBoxPrivate(ctkLanguageComboBox &object)
  : q_ptr(&object)
{
}

// ----------------------------------------------------------------------------
void ctkLanguageComboBoxPrivate::init()
{
  Q_Q(ctkLanguageComboBox);

  QObject::connect(q, SIGNAL(currentIndexChanged(int)),
                   q, SLOT(onLanguageChanged(int)));

  /// Add default language if any
  if (this->DefaultLanguage.isEmpty())
    {
    this->addLanguage(this->DefaultLanguage);
    }
}

// ----------------------------------------------------------------------------
void ctkLanguageComboBoxPrivate::addLanguageFiles(const QStringList& fileNames)
{
  foreach(QString fileName, fileNames)
    {
    QFileInfo file(fileName);
    if (!file.exists())
      {
      qWarning() << "File " << file.absoluteFilePath() << " doesn't exist.";
      }
    // language is "de_ch" for a file named "/abc/def_de_ch.qm"
    QString language = file.completeBaseName();
    language.remove(0,language.indexOf('_') + 1);
    this->addLanguage(language);
    }
}

// ----------------------------------------------------------------------------
bool ctkLanguageComboBoxPrivate::addLanguage(const QString& language)
{
  Q_Q(ctkLanguageComboBox);
  return this->insertLanguage(q->count(), language);
}

// ----------------------------------------------------------------------------
bool ctkLanguageComboBoxPrivate::insertLanguage(int index, const QString& language)
{
  Q_Q(ctkLanguageComboBox);
  QIcon icon;
  QString text;
  QVariant data;
  bool res =this->languageItem(language, icon, text, data);
  if (res)
    {
    q->insertItem(index, icon, text, data);
    }
  return res;
}

// ----------------------------------------------------------------------------
bool ctkLanguageComboBoxPrivate::languageItem(const QString& language,
                                              QIcon& icon,
                                              QString& text,
                                              QVariant& data)
{
  QLocale locale(language);
  if (language.isEmpty() ||
      locale.name() == "C")
    {
    icon = QIcon();
    text = QString();
    data = QVariant();
    return false;
    }
  QString countryFlag = locale.name();
  countryFlag.remove(0, countryFlag.lastIndexOf('_') + 1);
  countryFlag = countryFlag.toLower();
  icon = QIcon(QString(":Icons/Languages/%1.png").arg(countryFlag));
  text = QLocale::languageToString(locale.language());
  data = locale.name();
  return true;
}

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
void ctkLanguageComboBox::setDefaultLanguage(const QString& language)
{
  Q_D(ctkLanguageComboBox);
  bool isValid = false;
  if (!d->DefaultLanguage.isEmpty())
    {
    QIcon icon;
    QString text;
    QVariant data;
    isValid = d->languageItem(language, icon, text, data);
    if (isValid)
      {
      // Replace the default language
      this->setItemIcon(0, icon);
      this->setItemText(0, text);
      this->setItemData(0, data);
      }
    else
      {
      this->removeItem(0);
      }
    }
  else
    {
    isValid = d->insertLanguage(0, language);
    }
  d->DefaultLanguage = isValid ? this->itemData(0).toString() : QString();
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
  return d->LanguageDirectory;
}

// ----------------------------------------------------------------------------
void ctkLanguageComboBox::setDirectory(const QString& dir)
{
  Q_D(ctkLanguageComboBox);

  d->LanguageDirectory = dir;

  /// Recover all the translation file from the directory Translations
  QDir translationDir = QDir(d->LanguageDirectory);
  QStringList files;
  QStringList fileNames = translationDir.entryList(QStringList("*.qm"));
  foreach(const QString& fileName, fileNames)
    {
    files << translationDir.filePath(fileName);
    }

  /// Add all the languages availables
  d->addLanguageFiles(files);

  this->update();
}

// ----------------------------------------------------------------------------
void ctkLanguageComboBox::onLanguageChanged(int index)
{
  Q_UNUSED(index);
  QString currentLanguage = this->currentLanguage();
  emit currentLanguageNameChanged(currentLanguage);
}
