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

#ifndef __ctkVTKVolumePropertyWidget_h
#define __ctkVTKVolumePropertyWidget_h

//Qt includes
#include <QWidget>

// CTK includes
#include <ctkVTKObject.h>
#include "ctkVisualizationVTKWidgetsExport.h"
class ctkVTKVolumePropertyWidgetPrivate;

// VTK includes
class vtkVolumeProperty;

/// \ingroup Visualization_VTK_Widgets
class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKVolumePropertyWidget
  : public QWidget
{
  Q_OBJECT
  QVTK_OBJECT
  ///
  /// Control wether a range slider widget is used to edit the opacity
  /// function instead of a chart editor. False by default
  Q_PROPERTY(bool thresholdEnabled READ isThresholdEnabled WRITE setThresholdEnabled NOTIFY thresholdEnabledChanged)

  ///
  /// Show or hide the opacity threshold slider toggle button.
  /// True by default.
  Q_PROPERTY(bool thresholdToggleVisibility READ isThresholdToggleVisible WRITE setThresholdToggleVisible)

public:
  ctkVTKVolumePropertyWidget(QWidget* parent = 0);
  virtual ~ctkVTKVolumePropertyWidget();

  vtkVolumeProperty* volumeProperty()const;

  bool isThresholdEnabled()const;

  bool isThresholdToggleVisible()const;
  void setThresholdToggleVisible(bool showToggle);

  void chartsBounds(double bounds[4])const;
  void chartsExtent(double extent[4])const;

public Q_SLOTS:
  void setVolumeProperty(vtkVolumeProperty* volumeProperty);

  /// Move all the control points of the opacity, colors and gradient
  /// of a give offset.
  /// \sa vtkControlPoints::movePoints()
  void moveAllPoints(double xOffset, double yOffset = 0.,
                     bool dontSpreadFirstAndLast = false);

  /// Spread all the control points of the opacity, colors and gradient
  /// by a given offset.
  /// A value >0 will space the control points, a value <0. will contract
  /// them.
  /// \sa vtkControlPoints::spreadPoints()
  void spreadAllPoints(double factor = 1.,
                       bool dontSpreadFirstAndLast = false);

  void setThresholdEnabled(bool enable);

Q_SIGNALS:
  void thresholdEnabledChanged(bool enable);
  void chartsExtentChanged();

protected Q_SLOTS:
  void updateFromVolumeProperty();
  void updateRange();

  void setInterpolationMode(int mode);
  void setShade(bool);
  void setAmbient(double value);
  void setDiffuse(double value);
  void setSpecular(double value);
  void setSpecularPower(double value);

  void onThresholdOpacityToggled(bool);
  /// Called whenever a view (opacity, colors or gradient) has one of its axis
  /// modified. It synchronize all the views to see the same.
  void onAxesModified();

protected:
  QScopedPointer<ctkVTKVolumePropertyWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkVTKVolumePropertyWidget);
  Q_DISABLE_COPY(ctkVTKVolumePropertyWidget);
};

#endif
