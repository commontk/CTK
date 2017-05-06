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

#ifndef __ctkRangeSliderEventTranslator_h
#define __ctkRangeSliderEventTranslator_h

// QtTesting includes
#include <pqWidgetEventTranslator.h>

// CTK includes
#include <ctkPimpl.h>
#include "ctkWidgetsExport.h"

/// Translate low-level Qt events into high-level events for QtTesting

class CTK_WIDGETS_EXPORT ctkRangeSliderEventTranslator :
  public pqWidgetEventTranslator
{
  Q_OBJECT

public:
  typedef pqWidgetEventTranslator Superclass;
  ctkRangeSliderEventTranslator(QObject* parent = 0);

  using Superclass::translateEvent;
  virtual bool translateEvent(QObject *Object, QEvent *Event, bool &Error);

private:
  Q_DISABLE_COPY(ctkRangeSliderEventTranslator);

  QObject* CurrentObject;

private slots:
  void onMinimumPositionChanged(const int& min);
  void onMaximumPositionChanged(const int& max);
};

#endif
