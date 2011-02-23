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
#include <QDebug>
#include <QPushButton>
#include <QMessageBox>

// CTK includes
#include "ctkScreenshotDialog.h"
#include "ctkScreenshotDialog_p.h"

//-----------------------------------------------------------------------------
// ctkScreenshotDialogPrivate methods

//-----------------------------------------------------------------------------
ctkScreenshotDialogPrivate::ctkScreenshotDialogPrivate(ctkScreenshotDialog& object)
  : QObject(&object), q_ptr(&object)
{
  this->CaptureButton = 0;
  this->CountDownValue = 0;
}

//-----------------------------------------------------------------------------
ctkScreenshotDialogPrivate::~ctkScreenshotDialogPrivate()
{
}

//-----------------------------------------------------------------------------
void ctkScreenshotDialogPrivate::init()
{
  Q_Q(ctkScreenshotDialog);
  this->setupUi(q);
}

//-----------------------------------------------------------------------------
void ctkScreenshotDialogPrivate::setupUi(QDialog * widget)
{
  Q_Q(ctkScreenshotDialog);

  this->Ui_ctkScreenshotDialog::setupUi(widget);

  QPushButton * okButton = this->ButtonBox->button(QDialogButtonBox::Ok);
  Q_ASSERT(okButton);

  // Update OK button text
  okButton->setText("Capture");

  connect(okButton, SIGNAL(clicked()), q, SLOT(saveScreenshot()));
  connect(this->ImageNameLineEdit, SIGNAL(textChanged(QString)), SLOT(updateFullNameLabel()));
  connect(this->ImageVersionNumberSpinBox, SIGNAL(valueChanged(int)), SLOT(updateFullNameLabel()));
  connect(this->DelaySpinBox, SIGNAL(valueChanged(int)), SLOT(resetCountDownValue()));
  connect(&this->CountDownTimer, SIGNAL(timeout()), SLOT(updateCountDown()));

  this->CaptureButton = okButton;

  // Called to enable/disable buttons
  q->setWidgetToGrab(0);
}

//-----------------------------------------------------------------------------
void ctkScreenshotDialogPrivate::setWaitingForScreenshot(bool waiting)
{
  this->DelaySpinBox->setDisabled(waiting);
  this->ButtonBox->setDisabled(waiting);
}

//-----------------------------------------------------------------------------
bool ctkScreenshotDialogPrivate::isWaitingForScreenshot()const
{
  Q_Q(const ctkScreenshotDialog);
  // Bad Qt const correctness, need to hack.
  ctkScreenshotDialog* parent = const_cast<ctkScreenshotDialog*>(q);
  Q_ASSERT(this->DelaySpinBox->isEnabledTo(parent) ==
           this->ButtonBox->isEnabledTo(parent));
  return this->ButtonBox->isEnabledTo(parent);
}

//-----------------------------------------------------------------------------
void ctkScreenshotDialogPrivate::updateFullNameLabel()
{
  QString text("%1_%2.png");
  this->ImageFullNameLabel->setText(
      text.arg(this->ImageNameLineEdit->text())
          .arg(this->ImageVersionNumberSpinBox->value()));
}

//-----------------------------------------------------------------------------
void ctkScreenshotDialogPrivate::setCountDownLabel(int newValue)
{
  this->CountDownLabel->setText(QString("%1").arg(newValue));
}

//-----------------------------------------------------------------------------
void ctkScreenshotDialogPrivate::resetCountDownValue()
{
  this->CountDownTimer.stop();
  this->CountDownValue = this->DelaySpinBox->value();
  this->setCountDownLabel(this->CountDownValue);
}

//-----------------------------------------------------------------------------
void ctkScreenshotDialogPrivate::updateCountDown()
{
  this->setCountDownLabel(--this->CountDownValue);
}

//-----------------------------------------------------------------------------
void ctkScreenshotDialogPrivate::saveScreenshot(int delayInSeconds)
{
  Q_Q(ctkScreenshotDialog);

  if (this->WidgetToGrab.isNull())
    {
    return;
    }

  if (delayInSeconds <= 0)
    {
    q->instantScreenshot();
    return;
    }
  this->setWaitingForScreenshot(true);
  this->CountDownValue = delayInSeconds;
  this->CountDownTimer.start(1000);
  // Add 1ms to give time to set the countdown at 0. 
  QTimer::singleShot(delayInSeconds * 1000 + 1, q, SLOT(instantScreenshot()));
}

//-----------------------------------------------------------------------------
// ctkScreenshotDialog methods

//-----------------------------------------------------------------------------
ctkScreenshotDialog::ctkScreenshotDialog(QWidget* newParent)
  : Superclass(newParent)
  , d_ptr(new ctkScreenshotDialogPrivate(*this))
{
  Q_D(ctkScreenshotDialog);
  d->init();
}

//-----------------------------------------------------------------------------
ctkScreenshotDialog::~ctkScreenshotDialog()
{
}

//-----------------------------------------------------------------------------
void ctkScreenshotDialog::setWidgetToGrab(QWidget* newWidgetToGrab)
{
  Q_D(ctkScreenshotDialog);

  d->OptionGroupBox->setEnabled(newWidgetToGrab != 0);
  d->CaptureButton->setEnabled(newWidgetToGrab != 0);

  d->WidgetToGrab = newWidgetToGrab;
}

//-----------------------------------------------------------------------------
QWidget* ctkScreenshotDialog::widgetToGrab() const
{
  Q_D(const ctkScreenshotDialog);
  return d->WidgetToGrab.data();
}

//-----------------------------------------------------------------------------
void ctkScreenshotDialog::setBaseFileName(const QString& newBaseName)
{
  Q_D(ctkScreenshotDialog);
  d->ImageNameLineEdit->setText(newBaseName);
}
  
//-----------------------------------------------------------------------------
QString ctkScreenshotDialog::baseFileName() const
{
  Q_D(const ctkScreenshotDialog);
  return d->ImageNameLineEdit->text();
}

//-----------------------------------------------------------------------------
void ctkScreenshotDialog::setDirectory(const QString& newDirectory)
{
  Q_D(ctkScreenshotDialog);
  d->DirectoryButton->setDirectory(newDirectory);
}

//-----------------------------------------------------------------------------
QString ctkScreenshotDialog::directory()const
{
  Q_D(const ctkScreenshotDialog);
  return d->DirectoryButton->directory();
}

//-----------------------------------------------------------------------------
void ctkScreenshotDialog::setDelay(int seconds)
{
  Q_D(ctkScreenshotDialog);
  d->DelaySpinBox->setValue(seconds);
}

//-----------------------------------------------------------------------------
int ctkScreenshotDialog::delay()const
{
  Q_D(const ctkScreenshotDialog);
  return d->DelaySpinBox->value();
}

//-----------------------------------------------------------------------------
void ctkScreenshotDialog::saveScreenshot()
{
  Q_D(ctkScreenshotDialog);
  d->saveScreenshot(this->delay());
}

//-----------------------------------------------------------------------------
void ctkScreenshotDialog::instantScreenshot()
{
  Q_D(ctkScreenshotDialog);

  if (d->WidgetToGrab.isNull())
    {
    return;
    }

  QPixmap viewportPixmap = QPixmap::grabWidget(d->WidgetToGrab.data());

  if (d->isWaitingForScreenshot() && d->DelaySpinBox->value() != 0)
    {
    qApp->beep();
    }
  d->setWaitingForScreenshot(false);
  d->resetCountDownValue();

  // Rescale
  QPixmap rescaledViewportPixmap = viewportPixmap.scaled(
      viewportPixmap.size().width() * d->ScaleFactorSpinBox->value(),
      viewportPixmap.size().height() * d->ScaleFactorSpinBox->value());

  QString filename = QString("%1/%2_%3.png").arg(d->DirectoryButton->directory())
                     .arg(d->ImageNameLineEdit->text())
                     .arg(d->ImageVersionNumberSpinBox->value());

  // Check if file exists
  bool overwrite = d->OverwriteCheckBox->isChecked();
  if (QFile::exists(filename) && !overwrite)
    {
    int answer = QMessageBox::question(this, "Screen Capture",
                                       tr("File already exists. Overwrite ?"),
                                       QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No);
    if (answer == QMessageBox::YesToAll)
      {
      overwrite = true;
      d->OverwriteCheckBox->setChecked(true);
      }
    else if(answer == QMessageBox::Yes)
      {
      overwrite = true;
      }
    else
      {
      return;
      }
    }

  rescaledViewportPixmap.save(filename);

  d->ImageVersionNumberSpinBox->setValue(d->ImageVersionNumberSpinBox->value() + 1);
}
