/*=========================================================================

  Library:   ctk

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

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
