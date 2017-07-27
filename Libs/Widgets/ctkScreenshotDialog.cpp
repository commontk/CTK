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
#include <QDebug>
#include <QPushButton>
#include <QMessageBox>

// CTK includes
#include "ctkScreenshotDialog.h"
#include "ctkScreenshotDialog_p.h"
#include "ctkWidgetsUtils.h"

//-----------------------------------------------------------------------------
// ctkScreenshotDialogPrivate methods

//-----------------------------------------------------------------------------
ctkScreenshotDialogPrivate::ctkScreenshotDialogPrivate(ctkScreenshotDialog& object)
  : QObject(&object), q_ptr(&object)
{
  this->CaptureButton = 0;
  this->CountDownValue = 0;
  this->AspectRatio = 1.0;
  this->AllowTransparency = true;
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
  connect(this->ScaleFactorRadioButton, SIGNAL(toggled(bool)), SLOT(selectScaleFactor(bool)));
  connect(this->OutputResolutionRadioButton, SIGNAL(toggled(bool)), SLOT(selectOutputResolution(bool)));
  connect(this->LockAspectToolButton, SIGNAL(toggled(bool)), SLOT(lockAspectRatio(bool)));
  connect(this->WidthSpinBox, SIGNAL(editingFinished()), SLOT(onWidthEdited()));
  connect(this->HeightSpinBox, SIGNAL(editingFinished()), SLOT(onHeightEdited()));

  this->CaptureButton = okButton;

  // Called to enable/disable buttons
  q->setWidgetToGrab(0);
  // Set a sufficient range (1, 2^16) on the spin boxes
  this->WidthSpinBox->setRange(1, 65536);
  this->HeightSpinBox->setRange(1, 65536);
  this->DirectoryPathLineEdit->setFilters(ctkPathLineEdit::Dirs);
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
void ctkScreenshotDialogPrivate::useScalarFactor(bool scale)
{
  this->ScaleFactorSpinBox->setEnabled(scale);
  this->WidthSpinBox->setEnabled(!scale);
  this->HeightSpinBox->setEnabled(!scale);
  this->xLabel->setEnabled(!scale);
  this->LockAspectToolButton->setEnabled(!scale);
}

//-----------------------------------------------------------------------------
void ctkScreenshotDialogPrivate::selectScaleFactor(bool scale)
{
  this->useScalarFactor(scale);
}

//-----------------------------------------------------------------------------
void ctkScreenshotDialogPrivate::selectOutputResolution(bool scale)
{
  this->useScalarFactor(!scale);
}

//-----------------------------------------------------------------------------
void ctkScreenshotDialogPrivate::lockAspectRatio(bool lock)
{
  Q_Q(ctkScreenshotDialog);
  if(lock)
    {
    QSize curSize = q->widgetSize();
    if(curSize.height() > 0)
      {
      this->AspectRatio = curSize.width()/static_cast<double>(curSize.height());
      }
    else
      {
      QString message = QString("Height of widget: ") + curSize.height() +\
        QString(" is invalid. Check widget dimensions. Using default aspect\
          ratio (1.0).");
      QMessageBox::warning(q, "Invalid widget dimensions", message,
        QMessageBox::Ok);
      this->AspectRatio = 1.0;
      }
    }
}

//-----------------------------------------------------------------------------
void ctkScreenshotDialogPrivate::onWidthEdited()
{
  Q_Q(ctkScreenshotDialog);
  if(this->LockAspectToolButton->isChecked())
    {
    if(this->AspectRatio > 0)
      {
      this->HeightSpinBox->setValue(static_cast<int>(this->WidthSpinBox->value()/this->AspectRatio));
      }
    else
      {
      QString message = QString("Aspect ratio: ") + this->AspectRatio +\
        QString(" is invalid. Check widget dimensions.");
      QMessageBox::warning(q, "Invalid aspect ratio", message, QMessageBox::Ok);
      }
    }
}

//-----------------------------------------------------------------------------
void ctkScreenshotDialogPrivate::onHeightEdited()
{
  if(this->LockAspectToolButton->isChecked())
    {
    this->WidthSpinBox->setValue(static_cast<int>(this->HeightSpinBox->value()*this->AspectRatio));
    }
}

//-----------------------------------------------------------------------------
void ctkScreenshotDialog::enforceResolution(int width, int height)
{
  Q_D(ctkScreenshotDialog);
  d->OutputResolutionRadioButton->setChecked(true);
  d->useScalarFactor(true);
  d->ScaleFactorRadioButton->setEnabled(false);
  d->OutputResolutionRadioButton->setEnabled(false);
  d->ScaleFactorSpinBox->setEnabled(false);
  d->WidthSpinBox->setValue(width);
  d->HeightSpinBox->setValue(height);
}

//-----------------------------------------------------------------------------
void ctkScreenshotDialog::enforceResolution(QSize size)
{
  this->enforceResolution(size.width(), size.height());
}

//-----------------------------------------------------------------------------
QSize ctkScreenshotDialog::widgetSize()
{
  Q_D(ctkScreenshotDialog);
  QImage viewportImage = ctk::grabWidget(d->WidgetToGrab.data());
  return viewportImage.size();
}

//-----------------------------------------------------------------------------
void ctkScreenshotDialog::enableTransparency(bool enable)
{
  Q_D(ctkScreenshotDialog);
  d->AllowTransparency = enable;
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
  QSize curSize = this->widgetSize();
  d->HeightSpinBox->setValue(curSize.height());
  d->WidthSpinBox->setValue(curSize.width());
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
  d->DirectoryPathLineEdit->setCurrentPath(newDirectory);
}

//-----------------------------------------------------------------------------
QString ctkScreenshotDialog::directory()const
{
  Q_D(const ctkScreenshotDialog);
  return d->DirectoryPathLineEdit->currentPath();
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

  QPixmap viewportPixmap = QPixmap::fromImage(
    ctk::grabWidget(d->WidgetToGrab.data()));

  if (d->isWaitingForScreenshot() && d->DelaySpinBox->value() != 0)
    {
    qApp->beep();
    }
  d->setWaitingForScreenshot(false);
  d->resetCountDownValue();

  // Rescale based on scale factor or output resolution specified
  QPixmap rescaledViewportPixmap = viewportPixmap;
  if(d->ScaleFactorRadioButton->isChecked())
    {
    rescaledViewportPixmap = viewportPixmap.scaled(
      viewportPixmap.size().width() * d->ScaleFactorSpinBox->value(),
      viewportPixmap.size().height() * d->ScaleFactorSpinBox->value());
    }
  else if(d->OutputResolutionRadioButton->isChecked())
    {
    rescaledViewportPixmap = viewportPixmap.scaled(
      d->WidthSpinBox->value(),
      d->HeightSpinBox->value());
    }

  QString filename = QString("%1/%2_%3.png").arg(d->DirectoryPathLineEdit->currentPath())
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

  QImage img = rescaledViewportPixmap.toImage();
  if( !d->AllowTransparency &&
      img.hasAlphaChannel())
    {
    img = img.convertToFormat(QImage::Format_RGB32);
    }
  img.save(filename);

  d->ImageVersionNumberSpinBox->setValue(d->ImageVersionNumberSpinBox->value() + 1);
}
