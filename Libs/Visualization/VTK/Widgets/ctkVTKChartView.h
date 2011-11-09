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
#include "ctkVisualizationVTKWidgetsExport.h"
class ctkVTKChartViewPrivate;

// VTK includes
#include <QVTKWidget.h>

class vtkChartXY;
class vtkContextScene;
class vtkPlot;

class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKChartView : public QVTKWidget
{
  Q_OBJECT
  QVTK_OBJECT
  Q_PROPERTY(QString title READ title WRITE setTitle)

public:
  typedef QVTKWidget Superclass;
  ctkVTKChartView(QWidget* parent = 0);
  virtual ~ctkVTKChartView();

  /// Generic function to add a custom plot. \a plot is added into the chart
  virtual void addPlot(vtkPlot* plot);

  /// Utility function that returns the view chart. It can be used for customizing
  /// the chart display options (axes, legend...)
  vtkChartXY* chart()const;
  vtkContextScene* scene()const;

  /// Title that appears inside the view
  QString title()const;
  void setTitle(const QString& title);

  /// Return the chart extent for the 4 chart axes.
  /// extent must be an array of 8 doubles
  /// The current extent is the visible area on the chart.
  /// it is equivalent to the Minimum/Maximum of the axes
  void chartExtent(double bounds[8])const;

  /// Return the chart bounds for the 4 chart axes.
  /// bounds must be an array of 8 doubles.
  /// If no bounds is provided by the user, compute the bounds for the 4 chart
  /// axes from the vtkPlots bounds.
  void chartBounds(double bounds[8])const;
  void setChartUserBounds(double* bounds);
  void chartUserBounds(double* bounds)const;

  /// 
  virtual void setAxesToChartBounds();
  virtual void boundAxesToChartBounds();

signals:
  void plotAdded(vtkPlot* plot);
  /// Fired anytime the bound of a plot modifies the overal bounds
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
