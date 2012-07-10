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

#ifndef __ctkTransferFunctionView_h
#define __ctkTransferFunctionView_h

/// Qt includes
#include <QGraphicsView>

/// CTK includes
#include "ctkWidgetsExport.h"
#include "ctkPimpl.h"

class ctkTransferFunction;
class ctkTransferFunctionViewPrivate;

//-----------------------------------------------------------------------------
/// \ingroup Widgets
class CTK_WIDGETS_EXPORT ctkTransferFunctionView: public QGraphicsView
{
  Q_OBJECT;
public:
  ctkTransferFunctionView(QWidget* parent = 0);
  virtual ~ctkTransferFunctionView();
protected:
  virtual void resizeEvent(QResizeEvent * event);
  /*
  virtual void dragEnterEvent ( QDragEnterEvent * event );
  virtual void mousePressEvent ( QMouseEvent * event );
  virtual void mouseReleaseEvent ( QMouseEvent * event );*/

protected:
  QScopedPointer<ctkTransferFunctionViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkTransferFunctionView);
  Q_DISABLE_COPY(ctkTransferFunctionView);
};

#endif
