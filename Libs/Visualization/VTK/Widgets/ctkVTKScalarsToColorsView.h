/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

#ifndef __ctkVTKScalarsToColorsView_h
#define __ctkVTKScalarsToColorsView_h

// CTK includes
#include <ctkPimpl.h>
#include "ctkVTKChartView.h"
class ctkVTKScalarsToColorsViewPrivate;

// VTK includes
#include <QVTKWidget.h>

class vtkColorTransferFunction;
class vtkLookupTable;
class vtkPiecewiseFunction;

class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKScalarsToColorsView : public ctkVTKChartView
{
  Q_OBJECT

public:
  ctkVTKScalarsToColorsView(QWidget* parent = 0);
  virtual ~ctkVTKScalarsToColorsView();

  vtkPlot* addLookupTable(vtkLookupTable* lut);
  vtkPlot* addColorTransferFunction(vtkColorTransferFunction* colorTF);
  vtkPlot* addOpacityFunction(vtkPiecewiseFunction* opacityTF);
  vtkPlot* addCompositeFunction(vtkColorTransferFunction* colorTF, vtkPiecewiseFunction* opacityTF);

protected:
  QScopedPointer<ctkVTKScalarsToColorsViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkVTKScalarsToColorsView);
  Q_DISABLE_COPY(ctkVTKScalarsToColorsView);
};

#endif
