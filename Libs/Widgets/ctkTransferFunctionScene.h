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

#ifndef __ctkTransferFunctionScene_h
#define __ctkTransferFunctionScene_h

/// Qt includes
#include <QGraphicsScene>

/// CTK includes
#include "ctkPimpl.h"
#include "ctkTransferFunction.h"
#include "ctkWidgetsExport.h"

//class ctkTransferFunction;
class ctkTransferFunctionScenePrivate;
//class ctkControlPoint;
//class ctkPoint;

//-----------------------------------------------------------------------------
/// \ingroup Widgets
class CTK_WIDGETS_EXPORT ctkTransferFunctionScene: public QGraphicsScene
{
  Q_OBJECT

public:
  ctkTransferFunctionScene(QObject* parent = 0);
  virtual ~ctkTransferFunctionScene();

protected:
  QScopedPointer<ctkTransferFunctionScenePrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkTransferFunctionScene);
  Q_DISABLE_COPY(ctkTransferFunctionScene);
};

#endif
