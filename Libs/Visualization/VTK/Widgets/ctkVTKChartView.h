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

#ifndef __ctkVTKChartView_h
#define __ctkVTKChartView_h

// CTK includes
#include <ctkVTKObject.h>
#include <ctkVTKOpenGLNativeWidget.h>
#include "ctkVisualizationVTKWidgetsExport.h"
class ctkVTKChartViewPrivate;

// VTK includes
class vtkChartXY;
class vtkContextScene;
class vtkPlot;

/// \ingroup Visualization_VTK_Widgets
class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKChartView : public ctkVTKOpenGLNativeWidget
{
  Q_OBJECT
  QVTK_OBJECT
  Q_PROPERTY(QString title READ title WRITE setTitle)

public:
  typedef ctkVTKOpenGLNativeWidget Superclass;
  ctkVTKChartView(QWidget* parent = 0);
  virtual ~ctkVTKChartView();

  /// Generic function to add a custom plot. \a plot is added into the chart
  /// Emit the plotAdded(vtkPlot*) signal.
  Q_INVOKABLE virtual void addPlot(vtkPlot* plot);

  /// Remove the plot from the chart. Do nothing if plot is not in the chart.
  /// Emit the plotRemoved(vtkPlot*) signal.
  Q_INVOKABLE virtual void removePlot(vtkPlot* plot);

  /// Return the id of the plot in the chart.
  /// -1 if the plot is not found in the chart
  Q_INVOKABLE vtkIdType plotIndex(vtkPlot* plot);

  /// Utility function that returns the view chart. It can be used for customizing
  /// the chart display options (axes, legend...)
  Q_INVOKABLE vtkChartXY* chart()const;
  Q_INVOKABLE vtkContextScene* scene()const;

  /// Title that appears inside the view
  QString title()const;
  void setTitle(const QString& title);

  /// Return the chart extent for the 4 chart axes.
  /// extent must be an array of 8 doubles
  /// The current extent is the visible area on the chart.
  /// it is equivalent to the Minimum/Maximum of the axes
  void chartExtent(double bounds[8])const;
  void setChartUserExtent(double* userExtent);

  /// Return the chart bounds for the 4 chart axes.
  /// bounds must be an array of 8 doubles.
  /// If no bounds is provided by the user, compute the bounds for the 4 chart
  /// axes from the vtkPlots bounds.
  void chartBounds(double bounds[8])const;
  void setChartUserBounds(double* bounds);
  void chartUserBounds(double* bounds)const;

  /// Get underlying RenderWindow
  Q_INVOKABLE vtkRenderWindow* renderWindow()const;

public Q_SLOTS:

  /// Remove all the plots from the chart
  void removeAllPlots();
  
  /// 
  virtual void setAxesToChartBounds();
  virtual void boundAxesToChartBounds();

Q_SIGNALS:
  void plotAdded(vtkPlot* plot);
  void plotRemoved(vtkPlot* plot);
  /// Fired anytime the bound of a plot modifies the overall bounds
  void boundsChanged();
  /// Fired anytime an axis is modified.
  void extentChanged();

protected:
  QScopedPointer<ctkVTKChartViewPrivate> d_ptr;

  virtual void mouseDoubleClickEvent(QMouseEvent* event);
  virtual void onChartUpdated();
  void chartBoundsToPlotBounds(double bounds[8], double plotBounds[4])const;

private:
  Q_DECLARE_PRIVATE(ctkVTKChartView);
  Q_DISABLE_COPY(ctkVTKChartView);
};

#endif
