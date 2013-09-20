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

#ifndef __ctkScreenshotDialog_p_h
#define __ctkScreenshotDialog_p_h

// Qt includes
#include <QPointer>
#include <QTimer>

// CTK includes
#include "ctkScreenshotDialog.h"
#include "ui_ctkScreenshotDialog.h"

//-----------------------------------------------------------------------------
/// \ingroup Widgets
class ctkScreenshotDialogPrivate : public QObject, public Ui_ctkScreenshotDialog
{
  Q_OBJECT
  Q_DECLARE_PUBLIC(ctkScreenshotDialog);
protected:
  ctkScreenshotDialog* const q_ptr;
public:
  explicit ctkScreenshotDialogPrivate(ctkScreenshotDialog& object);
  virtual ~ctkScreenshotDialogPrivate();

  void init();

  void setupUi(QDialog * widget);

  void setWaitingForScreenshot(bool waiting);
  bool isWaitingForScreenshot()const;

  void useScalarFactor(bool use = true);

public Q_SLOTS:

  void saveScreenshot(int delayInSeconds);

  void updateFullNameLabel();

  void setCountDownLabel(int newValue);

  void resetCountDownValue();

  void updateCountDown();

  void selectScaleFactor(bool scale);

  void selectOutputResolution(bool scale);

  void lockAspectRatio(bool lock);

  void onWidthEdited();

  void onHeightEdited();

public:
  QPointer<QWidget>     WidgetToGrab;
  QPushButton*          CaptureButton;
  int                   CountDownValue;
  QTimer                CountDownTimer;
  double                AspectRatio;
  bool                  AllowTransparency;
};

#endif
