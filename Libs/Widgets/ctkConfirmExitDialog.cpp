/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QCheckBox>
#include <QDebug>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QMessageBox>
#include <QLabel>
#include <QSettings>
#include <QTimer>

// CTK includes
#include "ctkConfirmExitDialog.h"

//-----------------------------------------------------------------------------
// ctkConfirmExitDialogPrivate methods

//-----------------------------------------------------------------------------
class ctkConfirmExitDialogPrivate
{
  Q_DECLARE_PUBLIC(ctkConfirmExitDialog);
protected:
  ctkConfirmExitDialog* const q_ptr;
public:
  explicit ctkConfirmExitDialogPrivate(ctkConfirmExitDialog& object);
  virtual ~ctkConfirmExitDialogPrivate();

  void init();
  void readSettings();
  void writeSettings();
public:
  QString        DontShowKey;
  QCheckBox*     DontShowCheckBox;
  QLabel*        TextLabel;
  QLabel*        IconLabel;
};

//-----------------------------------------------------------------------------
ctkConfirmExitDialogPrivate::ctkConfirmExitDialogPrivate(ctkConfirmExitDialog& object)
  : q_ptr(&object)
{
  this->DontShowCheckBox = 0;
  this->TextLabel = 0;
  this->IconLabel = 0;
}

//-----------------------------------------------------------------------------
ctkConfirmExitDialogPrivate::~ctkConfirmExitDialogPrivate()
{
}

//-----------------------------------------------------------------------------
void ctkConfirmExitDialogPrivate::init()
{
  Q_Q(ctkConfirmExitDialog);
  QGridLayout* grid = new QGridLayout(q);
  this->IconLabel = new QLabel(q);
  this->IconLabel->setPixmap(QMessageBox::standardIcon(QMessageBox::Question));
  this->IconLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
  grid->addWidget(this->IconLabel, 0, 0, Qt::AlignTop);
  this->TextLabel = new QLabel(q);
  this->TextLabel->setText(q->tr("Are you sure you want to quit?"));
  grid->addWidget(this->TextLabel, 0, 1);
  this->DontShowCheckBox = new QCheckBox(q);
  this->DontShowCheckBox->setText(q->tr("Don't show this message again"));
  this->DontShowCheckBox->setChecked(false);
  grid->addWidget(this->DontShowCheckBox, 1, 1, Qt::AlignTop);
  QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Yes
                                                     | QDialogButtonBox::No);
  QObject::connect(buttonBox, SIGNAL(accepted()), q, SLOT(accept()));
  QObject::connect(buttonBox, SIGNAL(rejected()), q, SLOT(reject()));
  grid->addWidget(buttonBox, 2, 0, 1, 2, Qt::AlignCenter);
  grid->setSizeConstraint(QLayout::SetFixedSize);
}

//-----------------------------------------------------------------------------
void ctkConfirmExitDialogPrivate::readSettings()
{
  if (this->DontShowKey.isEmpty())
    {
    return;
    }
  QSettings settings;
  bool dontShow = settings.value(this->DontShowKey,
    this->DontShowCheckBox->isChecked()).toBool();
  this->DontShowCheckBox->setChecked(dontShow);
}

//-----------------------------------------------------------------------------
void ctkConfirmExitDialogPrivate::writeSettings()
{
  if (this->DontShowKey.isEmpty())
    {
    return;
    }
  QSettings settings;
  settings.setValue(this->DontShowKey, this->DontShowCheckBox->isChecked());
}

//-----------------------------------------------------------------------------
// ctkConfirmExitDialog methods

//-----------------------------------------------------------------------------
ctkConfirmExitDialog::ctkConfirmExitDialog(QWidget* newParent)
  : Superclass(newParent)
  , d_ptr(new ctkConfirmExitDialogPrivate(*this))
{
  Q_D(ctkConfirmExitDialog);
  d->init();
}

//-----------------------------------------------------------------------------
ctkConfirmExitDialog::~ctkConfirmExitDialog()
{
}

//-----------------------------------------------------------------------------
void ctkConfirmExitDialog::setPixmap(const QPixmap& pixmap)
{
  Q_D(ctkConfirmExitDialog);
  d->IconLabel->setPixmap(pixmap);
}

//-----------------------------------------------------------------------------
void ctkConfirmExitDialog::setText(const QString& text)
{
  Q_D(ctkConfirmExitDialog);
  d->TextLabel->setText(text);
}

//-----------------------------------------------------------------------------
void ctkConfirmExitDialog::setDontShowAnymoreSettingsKey(const QString& key)
{
  Q_D(ctkConfirmExitDialog);
  if (key == d->DontShowKey)
    {
    return;
    }
  d->DontShowKey = key;
  d->readSettings();
}

//-----------------------------------------------------------------------------
QString ctkConfirmExitDialog::dontShowAnymoreSettingsKey()const
{
  Q_D(const ctkConfirmExitDialog);
  return d->DontShowKey;
}

//-----------------------------------------------------------------------------
bool ctkConfirmExitDialog::dontShowAnymore()const
{
  Q_D(const ctkConfirmExitDialog);
  return d->DontShowCheckBox->isChecked();
}

//-----------------------------------------------------------------------------
void ctkConfirmExitDialog::setDontShowAnymore(bool dontShow)
{
  Q_D(ctkConfirmExitDialog);
  d->DontShowCheckBox->setChecked(dontShow);
  d->writeSettings();
}

//-----------------------------------------------------------------------------
void ctkConfirmExitDialog::accept()
{
  Q_D(ctkConfirmExitDialog);
  d->writeSettings();
  this->Superclass::accept();
}

//-----------------------------------------------------------------------------
void ctkConfirmExitDialog::setVisible(bool visible)
{
  Q_D(ctkConfirmExitDialog);
  if (visible)
    {
    d->readSettings();
    if (d->DontShowCheckBox->isChecked())
      {
      QTimer::singleShot(0, this, SLOT(accept()));
      return;
      }
    }
  this->Superclass::setVisible(visible);
}

//-----------------------------------------------------------------------------
bool ctkConfirmExitDialog
::confirmExit(const QString& dontShowAgainKey, QWidget* parentWidget)
{
  ctkConfirmExitDialog dialog(parentWidget);
  dialog.setDontShowAnymoreSettingsKey(dontShowAgainKey);
  return dialog.exec() == QDialog::Accepted;
}
