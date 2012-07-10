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

#ifndef __ctkVTKScalarsToColorsWidget_h
#define __ctkVTKScalarsToColorsWidget_h

//Qt includes
#include <QWidget>

// CTK includes
#include <ctkVTKObject.h>
#include "ctkVisualizationVTKWidgetsExport.h"
class ctkVTKScalarsToColorsView;
class ctkVTKScalarsToColorsWidgetPrivate;

// VTK includes
class vtkControlPointsItem;
class vtkPlot;

/// \ingroup Visualization_VTK_Widgets
class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKScalarsToColorsWidget : public QWidget
{
  Q_OBJECT
  QVTK_OBJECT
  Q_PROPERTY(bool horizontalSliderVisible READ isHorizontalSliderVisible WRITE setHorizontalSliderVisible)
  Q_PROPERTY(bool verticalSliderVisible READ isVerticalSliderVisible WRITE setVerticalSliderVisible)
  Q_PROPERTY(bool editColors READ editColors WRITE setEditColors)
public:
  ctkVTKScalarsToColorsWidget(QWidget* parent = 0);
  virtual ~ctkVTKScalarsToColorsWidget();

  ctkVTKScalarsToColorsView* view()const;
  vtkControlPointsItem* currentControlPointsItem()const;

  bool isHorizontalSliderVisible()const;
  void setHorizontalSliderVisible(bool visible);

  bool isVerticalSliderVisible()const;
  void setVerticalSliderVisible(bool visible);
  
  bool editColors()const;
  void setEditColors(bool edit);
  
  void xRange(double* range)const;
  void yRange(double* range)const;

  ///
  /// Return the top-left corner widget if any.
  QWidgetList extraWidgets()const;

  ///
  /// Add a widget in the top-left corner.
  /// ctkVTKScalarsToColorsWidget takes ownership of the widget
  void addExtraWidget(QWidget* extraWidget);

public Q_SLOTS:
  void setCurrentControlPointsItem(vtkControlPointsItem* item);
  void setCurrentPoint(int pointId);
  void setXRange(double min, double max);
  void setYRange(double min, double max);

Q_SIGNALS:
  /// Be carefull, axesModified() can be fired inside the Render() function
  /// of the view. You might want to connect the slot using Qt::QueuedConnection
  void axesModified();

protected Q_SLOTS:
  void onPlotAdded(vtkPlot*);
  void onBoundsChanged();
  void setCurrentPoint(vtkObject* controlPointsItem, void* pointId);
  void updateNumberOfPoints();
  void updateCurrentPoint();
  void onCurrentPointChanged(int pointId);
  void onColorChanged(const QColor& color);
  void onXChanged(double x);
  void onOpacityChanged(double opacity);
  void onMidPointChanged(double midPoint);
  void onSharpnessChanged(double sharpness);
  void onAxesModified();
  void restorePalette();
  void onExpandButton(bool state);
protected:
  QScopedPointer<ctkVTKScalarsToColorsWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkVTKScalarsToColorsWidget);
  Q_DISABLE_COPY(ctkVTKScalarsToColorsWidget);
};

#endif
