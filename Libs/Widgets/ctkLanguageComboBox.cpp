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
  void addLanguages(const QStringList& languages);
  void addLanguage(const QString& language);

  QString   DefaultLanguage;
  QString   Dir;
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
  /// Recover all the translation file from the directory Translations
  QDir translationDir = QDir(this->Dir);

  QStringList languages = translationDir.entryList(QStringList("*.qm"));

  /// Add default language.
  if (!this->DefaultLanguage.isEmpty())
    {
    this->addLanguage(this->DefaultLanguage);
    }
  /// Add all the languages availables
  this->addLanguages(languages);

  QObject::connect(q, SIGNAL(currentIndexChanged(int)),
                   q, SLOT(onLanguageChanged(int)));
}

// ----------------------------------------------------------------------------
void ctkLanguageComboBoxPrivate::addLanguages(const QStringList& languages)
{
  foreach(QString language, languages)
    {
    language.remove(0,language.indexOf('_') + 1);
    language.chop(3);
    this->addLanguage(language);
    }
}

// ----------------------------------------------------------------------------
void ctkLanguageComboBoxPrivate::addLanguage(const QString& language)
{
  Q_Q(ctkLanguageComboBox);
  QLocale lang(language);
  QString icon = ":Icons/Languages/";
  icon += language;
  icon += ".png";
  q->addItem(QIcon(icon), QLocale::languageToString(lang.language()), language);
}

// ----------------------------------------------------------------------------
ctkLanguageComboBox::ctkLanguageComboBox(QWidget* _parent)
  : QComboBox(_parent)
  , d_ptr(new ctkLanguageComboBoxPrivate(*this))
{
}

// ----------------------------------------------------------------------------
ctkLanguageComboBox::~ctkLanguageComboBox()
{
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
QString ctkLanguageComboBox::defaultLanguage() const
{
  Q_D(const ctkLanguageComboBox);
  return d->DefaultLanguage;
}

// ----------------------------------------------------------------------------
void ctkLanguageComboBox::setDefaultLanguage(const QString& language)
{
  Q_D(ctkLanguageComboBox);
  d->DefaultLanguage = language;
}

// ----------------------------------------------------------------------------
QString ctkLanguageComboBox::directory() const
{
  Q_D(const ctkLanguageComboBox);
  return d->Dir;
}

// ----------------------------------------------------------------------------
void ctkLanguageComboBox::setDirectory(const QString& dir)
{
  Q_D(ctkLanguageComboBox);
  d->Dir = dir;
  d->init();
  this->update();
}

// ----------------------------------------------------------------------------
void ctkLanguageComboBox::onLanguageChanged(int index)
{
  QVariant lang = this->itemData(index);
  //QLocale locale(lang.toString());
  emit currentLanguageNameChanged(lang.toString());
}
