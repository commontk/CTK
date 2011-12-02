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

#ifndef __ctkVTKAbstractMatrixWidget_h
#define __ctkVTKAbstractMatrixWidget_h

/// CTK includes
#include <ctkMatrixWidget.h>
#include <ctkVTKObject.h>

#include "ctkVisualizationVTKWidgetsExport.h"

class vtkMatrix4x4;
class ctkVTKAbstractMatrixWidgetPrivate;

/// \ingroup Visualization_VTK_Widgets
/// This base class is primarily used by ctkVTKMatrixWidget but can be the base
/// of other classes (ctkVTKTransformWidget?) that don't want to expose
/// setMatrix() publicly.
class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKAbstractMatrixWidget
  : public ctkMatrixWidget
{
public:
  /// Self/Superclass typedef
  typedef ctkMatrixWidget   Superclass;

  /// Constructors
  ctkVTKAbstractMatrixWidget(QWidget* parent);
  virtual ~ctkVTKAbstractMatrixWidget();
  vtkMatrix4x4* matrix()const;

protected:
  void setMatrixInternal(vtkMatrix4x4* matrix);

  virtual void setColumnCount(int newColumnCount);
  virtual void setRowCount(int newRowCount);

protected:
  QScopedPointer<ctkVTKAbstractMatrixWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkVTKAbstractMatrixWidget);
  Q_DISABLE_COPY(ctkVTKAbstractMatrixWidget);
};

#endif
