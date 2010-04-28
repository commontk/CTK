/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 
=========================================================================*/

#ifndef __ctkTransferFunctionWidget_h
#define __ctkTransferFunctionWidget_h

/// Qt includes
#include <QGraphicsView>

/// CTK includes
#include "CTKWidgetsExport.h"
#include "ctkPimpl.h"

class ctkTransferFunction;
class ctkTransferFunctionWidgetPrivate;

//-----------------------------------------------------------------------------
class CTK_WIDGETS_EXPORT ctkTransferFunctionWidget: public QGraphicsView
{
  Q_OBJECT;
public:
  ctkTransferFunctionWidget(QWidget* parent = 0);
  ctkTransferFunctionWidget(ctkTransferFunction* transferFunction, QWidget* parent = 0);
  virtual ~ctkTransferFunctionWidget();

  void setTransferFunction(ctkTransferFunction* transferFunction);
  ctkTransferFunction* transferFunction()const;
protected:
  virtual void resizeEvent(QResizeEvent * event);
private:
  CTK_DECLARE_PRIVATE(ctkTransferFunctionWidget);
};

#endif
