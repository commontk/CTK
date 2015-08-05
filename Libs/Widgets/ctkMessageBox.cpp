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
#include <QPushButton>
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
  int dontShowAgainButtonOrRole();
  QAbstractButton* button(int buttonOrRole);
  int buttonFromSettings();
  void readSettings();
  void writeSettings(int button);
public:
  QString        DontShowAgainSettingsKey;
  QCheckBox*     DontShowAgainCheckBox;
  bool           SaveDontShowAgainSettingsOnAcceptOnly;
};

//-----------------------------------------------------------------------------
ctkMessageBoxPrivate::ctkMessageBoxPrivate(ctkMessageBox& object)
  : q_ptr(&object)
{
  this->DontShowAgainSettingsKey = QString();
  this->DontShowAgainCheckBox = 0;
  this->SaveDontShowAgainSettingsOnAcceptOnly = true;
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
  this->DontShowAgainCheckBox->hide();
}

//-----------------------------------------------------------------------------
int ctkMessageBoxPrivate::dontShowAgainButtonOrRole()
{
  int buttonOrRole = QMessageBox::InvalidRole;
  if (this->DontShowAgainCheckBox->isChecked())
    {
    buttonOrRole = this->buttonFromSettings();
    if (buttonOrRole == QMessageBox::InvalidRole)
      {
      buttonOrRole = QMessageBox::AcceptRole;
      }
    }
  return buttonOrRole;
}


//-----------------------------------------------------------------------------
QAbstractButton* ctkMessageBoxPrivate::button(int buttonOrRole)
{
  Q_Q(ctkMessageBox);
  QAbstractButton* autoAcceptButton = 0;
  // Special case when no button is in a dialog, QMessageBox adds the Ok
  // button automatically.
  if (q->buttons().size() == 0 &&
      buttonOrRole != QMessageBox::InvalidRole)
    {
    q->addButton(QMessageBox::Ok);
    }
  if ( q->standardButtons() & buttonOrRole)
    {
    autoAcceptButton = q->button(static_cast<QMessageBox::StandardButton>(buttonOrRole));
    }
  else
    {
    foreach(QAbstractButton* button, q->buttons())
      {
      if (q->buttonRole(button) == buttonOrRole)
        {
        autoAcceptButton = button;
        break;
        }
      }
    }
  return autoAcceptButton;
}

//-----------------------------------------------------------------------------
int ctkMessageBoxPrivate::buttonFromSettings()
{
  QSettings settings;
  int button = settings.value(this->DontShowAgainSettingsKey,
                              static_cast<int>(QMessageBox::InvalidRole)).toInt();
  return button;
}

//-----------------------------------------------------------------------------
void ctkMessageBoxPrivate::readSettings()
{
  if (this->DontShowAgainSettingsKey.isEmpty())
    {
    return;
    }
  int button = this->buttonFromSettings();
  this->DontShowAgainCheckBox->setChecked(button != QMessageBox::InvalidRole);
}

//-----------------------------------------------------------------------------
void ctkMessageBoxPrivate::writeSettings(int button)
{
  if (this->DontShowAgainSettingsKey.isEmpty())
    {
    return;
    }
  QSettings settings;
  settings.setValue(this->DontShowAgainSettingsKey,
                    QVariant(this->DontShowAgainCheckBox->isChecked() ?
                             button : QMessageBox::InvalidRole));
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
  d->DontShowAgainCheckBox->setVisible(true);
  // update the text from the button with the accept role
  QAbstractButton *acceptButton = d->button(QMessageBox::AcceptRole);
  if (acceptButton && !acceptButton->text().isEmpty())
    {
    QString dontShowAgainText =
      this->tr("Don't show this message again and always %1").arg(acceptButton->text());

    d->DontShowAgainCheckBox->setText(dontShowAgainText);
    }
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
  this->setDontShowAgainVisible(!key.isEmpty());
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
  // We don't write into settings here because we want the same behavior
  // as if the user clicked on the checkbox.
  // If you want to save the preference into settings even if the user
  // escape/cancel/reject the dialog, then you must set the QSettings value
  // manually
}

//-----------------------------------------------------------------------------
void ctkMessageBox::done(int resultCode)
{
  Q_D(ctkMessageBox);
  // Don't save if the button is not an accepting button
  if (!d->SaveDontShowAgainSettingsOnAcceptOnly ||
      this->buttonRole( this->clickedButton() ) == QMessageBox::AcceptRole )
    {
    d->writeSettings(resultCode);
    }

  this->Superclass::done(resultCode);
}

//-----------------------------------------------------------------------------
void ctkMessageBox::setVisible(bool visible)
{
  Q_D(ctkMessageBox);
  if (visible)
    {
    int dontShowAgainButtonOrRole = d->dontShowAgainButtonOrRole();
    QAbstractButton* autoAcceptButton = d->button(dontShowAgainButtonOrRole);
    if (autoAcceptButton)
      {
      // Don't call click now, it would destroy the message box. The calling
      // function might expect the message box to be still valid after
      // setVisible() return.
      QTimer::singleShot(0, autoAcceptButton, SLOT(click()));
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
  dialog.setIcon(QMessageBox::Question);
  dialog.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
  dialog.setDontShowAgainSettingsKey(dontShowAgainKey);
  return dialog.exec() == QMessageBox::Ok;
}
