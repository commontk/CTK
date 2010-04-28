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

#ifndef __ctkVTKAbstractMatrixWidget_p_h
#define __ctkVTKAbstractMatrixWidget_p_h

// Qt includes
#include <QObject>

// CTK includes
#include <ctkPimpl.h>
#include "ctkVTKAbstractMatrixWidget.h"

// VTK includes
#include <vtkWeakPointer.h>

class vtkMatrix4x4;

class ctkVTKAbstractMatrixWidgetPrivate: public QObject,
                                         public ctkPrivate<ctkVTKAbstractMatrixWidget>
{
  Q_OBJECT
  QVTK_OBJECT
public:  
  ctkVTKAbstractMatrixWidgetPrivate();
  void init();

  void setMatrix(vtkMatrix4x4* matrix);
  vtkMatrix4x4* matrix()const;

public slots:
  /// 
  /// Triggered upon VTK transform modified event
  void updateMatrix();

protected:
  vtkWeakPointer<vtkMatrix4x4> Matrix;
};

#endif 
