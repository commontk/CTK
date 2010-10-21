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

#ifndef __ctkVTKChartView_h
#define __ctkVTKChartView_h

// CTK includes
#include <ctkPimpl.h>
#include "ctkVisualizationVTKWidgetsExport.h"
class ctkVTKChartViewPrivate;

// VTK includes
#include <QVTKWidget.h>

class vtkChartXY;
class vtkPlot;

class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKChartView : public QVTKWidget
{
  Q_OBJECT
  Q_PROPERTY(QString title READ title WRITE setTitle)

public:
  ctkVTKChartView(QWidget* parent = 0);
  virtual ~ctkVTKChartView();

  /// Generic function to add a custom plot. \a plot is added into the chart
  void addPlot(vtkPlot* plot);

  /// Utility function that returns the view chart. It can be used for customizing
  /// the chart display options (axes, legend...)
  vtkChartXY* chart()const;

  /// Title that appears inside the view
  QString title()const;
  void setTitle(const QString& title);

  void fitAxesToBounds();

protected:
  QScopedPointer<ctkVTKChartViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkVTKChartView);
  Q_DISABLE_COPY(ctkVTKChartView);
};

#endif
