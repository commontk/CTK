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
#include <QCheckBox>
#include <QDebug>
#include <QGridLayout>
#include <QSettings>
#include <QTimer>

// CTK includes
#include "ctkMessageBox.h"

//-----------------------------------------------------------------------------
// ctkMessageBoxPrivate methods

//-----------------------------------------------------------------------------
class ctkMessageBoxPrivate
{
  Q_DECLARE_PUBLIC(ctkMessageBox);
protected:
  ctkMessageBox* const q_ptr;
public:
  explicit ctkMessageBoxPrivate(ctkMessageBox& object);
  virtual ~ctkMessageBoxPrivate();

  void init();
  void readSettings();
  void writeSettings();
public:
  QString        DontShowAgainSettingsKey;
  QCheckBox*     DontShowAgainCheckBox;
};

//-----------------------------------------------------------------------------
ctkMessageBoxPrivate::ctkMessageBoxPrivate(ctkMessageBox& object)
  : q_ptr(&object)
{
  this->DontShowAgainCheckBox = 0;
}

//-----------------------------------------------------------------------------
ctkMessageBoxPrivate::~ctkMessageBoxPrivate()
{
}

//-----------------------------------------------------------------------------
void ctkMessageBoxPrivate::init()
{
  Q_Q(ctkMessageBox);
  this->DontShowAgainCheckBox = new QCheckBox(q);
  this->DontShowAgainCheckBox->setObjectName(QLatin1String("ctk_msgbox_dontshowcheckbox"));
  this->DontShowAgainCheckBox->setText(q->tr("Don't show this message again"));
  // The height policy being Fixed by default on a checkbox, if the message box icon
  // is bigger than the text+checkbox height, it would be truncated.
  this->DontShowAgainCheckBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  this->DontShowAgainCheckBox->setChecked(false);
}

//-----------------------------------------------------------------------------
void ctkMessageBoxPrivate::readSettings()
{
  if (this->DontShowAgainSettingsKey.isEmpty())
    {
    return;
    }
  QSettings settings;
  bool dontShow = settings.value(this->DontShowAgainSettingsKey,
    this->DontShowAgainCheckBox->isChecked()).toBool();
  this->DontShowAgainCheckBox->setChecked(dontShow);
}

//-----------------------------------------------------------------------------
void ctkMessageBoxPrivate::writeSettings()
{
  if (this->DontShowAgainSettingsKey.isEmpty())
    {
    return;
    }
  QSettings settings;
  settings.setValue(this->DontShowAgainSettingsKey, this->DontShowAgainCheckBox->isChecked());
  qDebug() << "write...";
}

//-----------------------------------------------------------------------------
// ctkMessageBox methods

//-----------------------------------------------------------------------------
ctkMessageBox::ctkMessageBox(QWidget* newParent)
  : Superclass(newParent)
  , d_ptr(new ctkMessageBoxPrivate(*this))
{
  Q_D(ctkMessageBox);
  d->init();
}

//-----------------------------------------------------------------------------
ctkMessageBox::ctkMessageBox(Icon icon, const QString& title,
                             const QString& text, StandardButtons buttons,
                             QWidget* parent, Qt::WindowFlags f)
  : QMessageBox(icon, title, text, buttons, parent, f)
{
  Q_D(ctkMessageBox);
  d->init();
}

//-----------------------------------------------------------------------------
ctkMessageBox::~ctkMessageBox()
{
}

//-----------------------------------------------------------------------------
void ctkMessageBox::setDontShowAgainVisible(bool visible)
{
  Q_D(ctkMessageBox);
  if (!visible)
    {
    this->layout()->removeWidget(d->DontShowAgainCheckBox);
    d->DontShowAgainCheckBox->hide();
    return;
    }
  QGridLayout *grid = static_cast<QGridLayout *>(this->layout());
  grid->addWidget(d->DontShowAgainCheckBox, 1, 1, 1, 1);
}

//-----------------------------------------------------------------------------
bool ctkMessageBox::isDontShowAgainVisible()const
{
  Q_D(const ctkMessageBox);
  return d->DontShowAgainCheckBox->isVisibleTo(const_cast<ctkMessageBox*>(this));
}

//-----------------------------------------------------------------------------
void ctkMessageBox::setDontShowAgainSettingsKey(const QString& key)
{
  Q_D(ctkMessageBox);
  if (key == d->DontShowAgainSettingsKey)
    {
    return;
    }
  d->DontShowAgainSettingsKey = key;
  d->readSettings();
}

//-----------------------------------------------------------------------------
QString ctkMessageBox::dontShowAgainSettingsKey()const
{
  Q_D(const ctkMessageBox);
  return d->DontShowAgainSettingsKey;
}

//-----------------------------------------------------------------------------
bool ctkMessageBox::dontShowAgain()const
{
  Q_D(const ctkMessageBox);
  return d->DontShowAgainCheckBox->isChecked();
}

//-----------------------------------------------------------------------------
void ctkMessageBox::setDontShowAgain(bool dontShow)
{
  Q_D(ctkMessageBox);
  d->DontShowAgainCheckBox->setChecked(dontShow);
  d->writeSettings();
}

//-----------------------------------------------------------------------------
void ctkMessageBox::done(int resultCode)
{
  Q_D(ctkMessageBox);
  if (resultCode == QDialog::Accepted)
    {
    d->writeSettings();
    }
  this->Superclass::done(resultCode);
}

//-----------------------------------------------------------------------------
void ctkMessageBox::setVisible(bool visible)
{
  Q_D(ctkMessageBox);
  if (visible)
    {
    d->readSettings();
    if (d->DontShowAgainCheckBox->isChecked())
      {
      QTimer::singleShot(0, this, SLOT(accept()));
      return;
      }
    }
  this->Superclass::setVisible(visible);
}

//-----------------------------------------------------------------------------
bool ctkMessageBox
::confirmExit(const QString& dontShowAgainKey, QWidget* parentWidget)
{
  ctkMessageBox dialog(parentWidget);
  dialog.setText(tr("Are you sure you want to exit?"));
  dialog.setDontShowAgainVisible(!dontShowAgainKey.isEmpty());
  dialog.setDontShowAgainSettingsKey(dontShowAgainKey);
  return dialog.exec() == QDialog::Accepted;
}
