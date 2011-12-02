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

#ifndef __ctkVTKAbstractMatrixWidget_p_h
#define __ctkVTKAbstractMatrixWidget_p_h

// Qt includes
#include <QObject>

// CTK includes
#include <ctkPimpl.h>
#include "ctkVTKAbstractMatrixWidget.h"

// VTK includes
#include <vtkSmartPointer.h>

class vtkMatrix4x4;

/// \ingroup Visualization_VTK_Widgets
class ctkVTKAbstractMatrixWidgetPrivate: public QObject
{
  Q_OBJECT
  QVTK_OBJECT
  Q_DECLARE_PUBLIC(ctkVTKAbstractMatrixWidget);

protected:
  ctkVTKAbstractMatrixWidget* const q_ptr;

public:
  ctkVTKAbstractMatrixWidgetPrivate(ctkVTKAbstractMatrixWidget& object);
  void init();

  void setMatrix(vtkMatrix4x4* matrix);
  vtkMatrix4x4* matrix()const;

public Q_SLOTS:
  ///
  /// Triggered upon VTK transform modified event
  void updateMatrix();
  void updateVTKMatrix();

protected:
  vtkSmartPointer<vtkMatrix4x4> Matrix;
};

#endif
