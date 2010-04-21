/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __ctkVTKMatrixWidget_h
#define __ctkVTKMatrixWidget_h

// CTK includes
#include <ctkPimpl.h>
#include "ctkVTKAbstractMatrixWidget.h"

#include "CTKVisualizationVTKWidgetsExport.h"
 
class vtkMatrix4x4;

class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKMatrixWidget : public ctkVTKAbstractMatrixWidget
{
  Q_OBJECT
public:
  /// Self/Superclass typedef
  typedef ctkVTKMatrixWidget  Self;
  typedef ctkVTKAbstractMatrixWidget   Superclass;
  
  /// Constructors
  ctkVTKMatrixWidget(QWidget* parent);

public slots:
  void setMatrix(vtkMatrix4x4* matrix);
}; 

#endif
