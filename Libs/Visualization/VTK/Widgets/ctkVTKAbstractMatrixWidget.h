/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __ctkVTKAbstractMatrixWidget_h
#define __ctkVTKAbstractMatrixWidget_h

/// CTK includes
#include <ctkMatrixWidget.h>
#include <ctkPimpl.h>
#include <ctkVTKObject.h>

#include "CTKVisualizationVTKWidgetsExport.h"

class vtkMatrix4x4;
class ctkVTKAbstractMatrixWidgetPrivate;

class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKAbstractMatrixWidget : public ctkMatrixWidget
{
public:
  /// Self/Superclass typedef
  typedef ctkMatrixWidget   Superclass;
  
  /// Constructors
  ctkVTKAbstractMatrixWidget(QWidget* parent);
  vtkMatrix4x4* matrix()const;

protected:
  void setMatrixInternal(vtkMatrix4x4* matrix);

private:
  CTK_DECLARE_PRIVATE(ctkVTKAbstractMatrixWidget);
}; 

#endif
