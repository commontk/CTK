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
///
/// This widget includes a ctkVTKScalarsToColorsView and a "top widget" to display and update its properties.
///
/// Features are:
/// * vertical and horizontal scrollbars (if needed, visible by default)
/// * a "top widget" including selected point index, its coordinate and color. An expand button allows to access
///   advanced properties like mid point and sharpness.
/// * if a piecewise or composite function are added to the view, the opacity is available in the "top widget" advanced properties.
/// * color associated with points can be updated in place (editable by default).
/// * support customization of widget shown in the top-left corner. See addExtraWidget().
/// * visibility of the "top widgets" can easily be updated.
///
/// Observing vtkControlPointsItem allows to be notified of point selection or
/// point update:
/// * event vtkControlPointsItem::CurrentPointChangedEvent is invoked each time a point is selected. Associated
///   call data is the point index.
/// * event vtkCommand::ModifiedEvent is invoked each time a point is updated.
///
class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKScalarsToColorsWidget : public QWidget
{
  Q_OBJECT
  QVTK_OBJECT
  Q_PROPERTY(bool horizontalSliderVisible READ isHorizontalSliderVisible WRITE setHorizontalSliderVisible)
  Q_PROPERTY(bool verticalSliderVisible READ isVerticalSliderVisible WRITE setVerticalSliderVisible)
  Q_PROPERTY(bool editColors READ editColors WRITE setEditColors)
  Q_PROPERTY(bool areTopWidgetsVisible READ areTopWidgetsVisible WRITE setTopWidgetsVisible)
public:
  ctkVTKScalarsToColorsWidget(QWidget* parent = 0);
  virtual ~ctkVTKScalarsToColorsWidget();

  Q_INVOKABLE ctkVTKScalarsToColorsView* view()const;
  Q_INVOKABLE vtkControlPointsItem* currentControlPointsItem()const;

  bool isHorizontalSliderVisible()const;
  void setHorizontalSliderVisible(bool visible);

  bool isVerticalSliderVisible()const;
  void setVerticalSliderVisible(bool visible);
  
  bool editColors()const;
  void setEditColors(bool edit);
  
  Q_INVOKABLE void xRange(double* range)const;
  Q_INVOKABLE void yRange(double* range)const;

  /// Hide all widgets displayed above the color view.
  ///
  /// This function internally keeps track of the selected visibility state
  /// by setting a "TopWidgetsVisible" property. This means that:
  /// (1) widgets for editing point coordinate and color are
  ///     not shown in the "top widgets" when a point is selected or modified.
  /// (2) widgets added using addExtraWidget() are explicitly hidden if it applies.
  bool areTopWidgetsVisible()const;
  void setTopWidgetsVisible(bool visible);

  /// Return the top-left corner widget if any.
  ///
  /// \sa addExtraWidget()
  Q_INVOKABLE QWidgetList extraWidgets()const;

  /// Add a widget in the top-left corner.
  ///
  /// ctkVTKScalarsToColorsWidget takes ownership of the widget.
  ///
  /// \sa extraWidgets()
  Q_INVOKABLE void addExtraWidget(QWidget* extraWidget);

public Q_SLOTS:
  void setCurrentControlPointsItem(vtkControlPointsItem* item);
  void setCurrentPoint(int pointId);
  void setXRange(double min, double max);
  void setYRange(double min, double max);
  void resetRange();

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
