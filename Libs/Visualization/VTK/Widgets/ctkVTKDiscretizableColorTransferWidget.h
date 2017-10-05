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

#ifndef __ctkVTKDiscretizableColorTransferWidget_h
#define __ctkVTKDiscretizableColorTransferWidget_h

// CTK includes
#include "ctkVisualizationVTKWidgetsExport.h"
class ctkVTKDiscretizableColorTransferWidgetPrivate;
class ctkVTKScalarsToColorsComboBox;

// VTK includes
#include <vtkSmartPointer.h>
class QVTKWidget;
class vtkImageAccumulate;
class vtkPiecewiseFunction;
class vtkScalarsToColors;

// Qt includes
#include <QWidget>
class QCheckBox;
class QLineEdit;
class QSpinBox;
class QToolButton;

class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKDiscretizableColorTransferWidget: public QWidget
{
  Q_OBJECT
  /// This property controls the color of the view.
  /// Dark gray by default.
  /// \accessors viewBackgroundColor() setViewBackgroundColor()
  Q_PROPERTY(QColor viewBackgroundColor READ viewBackgroundColor WRITE setViewBackgroundColor)
public:
  explicit ctkVTKDiscretizableColorTransferWidget(QWidget* parent_ = nullptr);
  virtual ~ctkVTKDiscretizableColorTransferWidget();

  void setColorTransferFunction(vtkScalarsToColors* ctf);
  vtkScalarsToColors* colorTransferFunction() const;

  void setHistogram(vtkImageAccumulate* hist);

  void setViewBackgroundColor(const QColor& i_color);
  QColor viewBackgroundColor() const;

  ctkVTKScalarsToColorsComboBox* scalarsToColorsSelector() const;

signals:
  void currentScalarsToColorsModified();
  void currentScalarsToColorsChanged(vtkScalarsToColors* ctf);

public slots:
  void onCurrentPointEdit();
  void onPaletteIndexChanged(vtkScalarsToColors* ctf);

  void setGlobalOpacity(double opacity);

  void resetColorTransferFunctionRange();
  void centerColorTransferFunctionRange();
  void invertColorTransferFunction();

  void setNaNColor();
  void setDiscretize(bool checked);
  void setNumberOfDiscreteValues(int value);
  void setColorTransferFunctionRange(double minValue, double maxValue);

protected:
  QScopedPointer<ctkVTKDiscretizableColorTransferWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkVTKDiscretizableColorTransferWidget);
  Q_DISABLE_COPY(ctkVTKDiscretizableColorTransferWidget);
};
#endif // __ctkVTKDiscretizableColorTransferWidget_h
