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

#ifndef __ctkVTKRenderViewEventTranslator_h
#define __ctkVTKRenderViewEventTranslator_h

// QT includes
#include <QMouseEvent>

// QtTesting inlcudes
#include <pqWidgetEventTranslator.h>

// CTK includes
#include <ctkPimpl.h>
#include "ctkVisualizationVTKWidgetsExport.h"

class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKRenderViewEventTranslator :
  public pqWidgetEventTranslator
{
  Q_OBJECT

public:
  typedef pqWidgetEventTranslator Superclass;
  ctkVTKRenderViewEventTranslator(const QByteArray& Classname, QObject* Parent=0);
  ~ctkVTKRenderViewEventTranslator();

  using Superclass::translateEvent;
  virtual bool translateEvent(QObject *Object, QEvent *Event, bool &Error);

protected:
  QByteArray   mClassType;
  QMouseEvent  lastMoveEvent;
  QMouseEvent  oldMoveEvent;
  QMouseEvent  lastMouseEvent;

private:
  Q_DISABLE_COPY(ctkVTKRenderViewEventTranslator);
};

#endif
