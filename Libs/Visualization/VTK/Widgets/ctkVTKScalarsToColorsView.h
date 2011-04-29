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
#include "ctkVTKChartView.h"
#include "ctkVTKObject.h"
class ctkVTKScalarsToColorsViewPrivate;

// VTK includes
#include <QVTKWidget.h>
#include <vtkChartXY.h>

class vtkColorTransferFunction;
class vtkLookupTable;
class vtkPiecewiseFunction;

class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKScalarsToColorsView
  : public ctkVTKChartView
{
  Q_OBJECT
  QVTK_OBJECT
public:
  typedef ctkVTKChartView Superclass;
  ctkVTKScalarsToColorsView(QWidget* parent = 0);
  virtual ~ctkVTKScalarsToColorsView();

  virtual void addPlot(vtkPlot* plot);
  vtkPlot* addLookupTable(vtkLookupTable* lut);
  vtkPlot* addColorTransferFunction(vtkColorTransferFunction* colorTF, bool editable = true);
  vtkPlot* addOpacityFunction(vtkPiecewiseFunction* opacityTF, bool editable = true);
  vtkPlot* addCompositeFunction(vtkColorTransferFunction* colorTF,
                                vtkPiecewiseFunction* opacityTF,
                                bool colorTFEditable = true,
                                bool opacityTFEditable = true);
  vtkPlot* addPiecewiseFunction(vtkPiecewiseFunction* piecewiseTF, bool editable = true);

  vtkPlot* addColorTransferFunctionControlPoints(vtkColorTransferFunction* colorTF);
  vtkPlot* addOpacityFunctionControlPoints(vtkPiecewiseFunction* opacityTF);
  vtkPlot* addCompositeFunctionControlPoints(vtkColorTransferFunction* colorTF,
                                             vtkPiecewiseFunction* opacityTF);
  vtkPlot* addPiecewiseFunctionControlPoints(vtkPiecewiseFunction* piecewiseTF);

  QList<vtkPlot*> plots()const;
  template<class T>
  QList<T*> plots()const;
  QList<vtkPlot*> lookupTablePlots()const;
  QList<vtkPlot*> lookupTablePlots(vtkLookupTable* lut)const;
  QList<vtkPlot*> colorTransferFunctionPlots()const;
  QList<vtkPlot*> colorTransferFunctionPlots(vtkColorTransferFunction* colorTF)const;
  QList<vtkPlot*> opacityFunctionPlots()const;
  QList<vtkPlot*> opacityFunctionPlots(vtkPiecewiseFunction* opacityTF)const;

  void setLookuptTableToPlots(vtkLookupTable* lut);
  void setColorTransferFunctionToPlots(vtkColorTransferFunction* colorTF);
  void setOpacityFunctionToPlots(vtkPiecewiseFunction* opacityTF);
  void setPiecewiseFunctionToPlots(vtkPiecewiseFunction* piecewiseTF);

public slots:
  void editPoint(vtkObject* plot, void * pointId);

protected:
  QScopedPointer<ctkVTKScalarsToColorsViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkVTKScalarsToColorsView);
  Q_DISABLE_COPY(ctkVTKScalarsToColorsView);
};

// ----------------------------------------------------------------------------
template<class T>
QList<T*> ctkVTKScalarsToColorsView::plots()const
{
  QList<T*> res;
  const vtkIdType count = this->chart()->GetNumberOfPlots();
  for(vtkIdType i = 0; i < count; ++i)
    {
    vtkPlot* plot = this->chart()->GetPlot(i);
    if (T::SafeDownCast(plot) != 0)
      {
      res << T::SafeDownCast(plot);
      }
    }
  return res;
}


#endif
