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

#ifndef __ctkVTKScalarsToColorsView_h
#define __ctkVTKScalarsToColorsView_h

// CTK includes
#include "ctkVTKChartView.h"
#include "ctkVTKObject.h"
class ctkVTKScalarsToColorsViewPrivate;

// VTK includes
#include <vtkChartXY.h>

class vtkColorTransferFunction;
class vtkControlPointsItem;
class vtkLookupTable;
class vtkPiecewiseFunction;

/// \ingroup Visualization_VTK_Widgets
class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKScalarsToColorsView
  : public ctkVTKChartView
{
  Q_OBJECT
  QVTK_OBJECT
  /// Add an empty space around the view to give space to the corner control
  /// points. False by default
  Q_PROPERTY(bool bordersVisible READ areBordersVisible WRITE setBordersVisible)

public:
  typedef ctkVTKChartView Superclass;
  ctkVTKScalarsToColorsView(QWidget* parent = 0);
  virtual ~ctkVTKScalarsToColorsView();

  Q_INVOKABLE virtual void addPlot(vtkPlot* plot);

  Q_INVOKABLE vtkPlot* addLookupTable(vtkLookupTable* lut);
  Q_INVOKABLE vtkPlot* addColorTransferFunction(vtkColorTransferFunction* colorTF, bool editable = true);
  Q_INVOKABLE vtkPlot* addOpacityFunction(vtkPiecewiseFunction* opacityTF, bool editable = true);
  Q_INVOKABLE vtkPlot* addCompositeFunction(vtkColorTransferFunction* colorTF,
                                vtkPiecewiseFunction* opacityTF,
                                bool colorTFEditable = true,
                                bool opacityTFEditable = true);
  Q_INVOKABLE vtkPlot* addPiecewiseFunction(vtkPiecewiseFunction* piecewiseTF, bool editable = true);

  Q_INVOKABLE vtkPlot* addColorTransferFunctionControlPoints(vtkColorTransferFunction* colorTF);
  Q_INVOKABLE vtkPlot* addOpacityFunctionControlPoints(vtkPiecewiseFunction* opacityTF);
  Q_INVOKABLE vtkPlot* addCompositeFunctionControlPoints(vtkColorTransferFunction* colorTF,
                                             vtkPiecewiseFunction* opacityTF);
  Q_INVOKABLE vtkPlot* addPiecewiseFunctionControlPoints(vtkPiecewiseFunction* piecewiseTF);

  Q_INVOKABLE QList<vtkPlot*> plots()const;
  template<class T>
  QList<T*> plots()const;
  Q_INVOKABLE QList<vtkControlPointsItem*> controlPointsItems()const;
  Q_INVOKABLE QList<vtkPlot*> lookupTablePlots()const;
  Q_INVOKABLE QList<vtkPlot*> lookupTablePlots(vtkLookupTable* lut)const;
  Q_INVOKABLE QList<vtkPlot*> colorTransferFunctionPlots()const;
  Q_INVOKABLE QList<vtkPlot*> colorTransferFunctionPlots(vtkColorTransferFunction* colorTF)const;
  Q_INVOKABLE QList<vtkPlot*> opacityFunctionPlots()const;
  Q_INVOKABLE QList<vtkPlot*> opacityFunctionPlots(vtkPiecewiseFunction* opacityTF)const;

  Q_INVOKABLE void setLookuptTableToPlots(vtkLookupTable* lut);
  Q_INVOKABLE void setColorTransferFunctionToPlots(vtkColorTransferFunction* colorTF);
  Q_INVOKABLE void setOpacityFunctionToPlots(vtkPiecewiseFunction* opacityTF);
  Q_INVOKABLE void setPiecewiseFunctionToPlots(vtkPiecewiseFunction* piecewiseTF);

  bool areBordersVisible()const;
  void setBordersVisible(bool show);

  Q_INVOKABLE void validBounds(double bounds[4])const;
  Q_INVOKABLE void setValidBounds(double bounds[4]);

  Q_INVOKABLE void setPlotsUserBounds(double* bounds);

  /// Reimplemented to set the bounds to the plots as well
  Q_INVOKABLE virtual void boundAxesToChartBounds();

Q_SIGNALS:
  /// Emitted when a new function is set to the view
  /// \sa setLookuptTableToPlots, \sa setColorTransferFunctionToPlots,
  /// \sa setOpacityFunctionToPlots, \sa setPiecewiseFunctionToPlots
  void functionChanged();

public Q_SLOTS:
  void editPoint(vtkObject* plot, void * pointId);

  /// Move all the control points by a given offset.
  /// \sa vtkControlPoints::movePoints()
  void moveAllPoints(double xOffset, double yOffset = 0.,
                     bool dontMoveFirstAndLast = false);

  /// Spread all the control points by a given offset.
  /// A value >0 will space the control points, a value <0. will contract
  /// them.
  /// \sa vtkControlPoints::spreadPoints()
  void spreadAllPoints(double factor = 1.,
                       bool dontMoveFirstAndLast = false);

protected Q_SLOTS:
  void onBoundsChanged();

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
