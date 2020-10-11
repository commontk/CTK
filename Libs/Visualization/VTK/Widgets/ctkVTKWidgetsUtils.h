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

#ifndef __ctkVTKWidgetsUtils_h
#define __ctkVTKWidgetsUtils_h

// Qt includes
#include <QRect>
class QImage;
class QWidget;

// CTKVTKWidgets includes
#include "ctkVisualizationVTKWidgetsExport.h"

// VTK includes
class vtkImageData;
class vtkScalarsToColors;

namespace ctk {

///
/// \ingroup Visualization_VTK_Widgets
/// Grab the contents of a QWidget and all its children.
/// Handle correctly the case of QVTKWidget.
/// \sa ctk::grabWidget QWidget::grab
QImage CTK_VISUALIZATION_VTK_WIDGETS_EXPORT grabVTKWidget(QWidget* widget, QRect rectangle = QRect());

///
/// \ingroup Visualization_VTK_Widgets
/// Convert a vtkImageData into a QImage with alpha channel (RGBA).
/// Creates a deep copy of the input image.
QImage CTK_VISUALIZATION_VTK_WIDGETS_EXPORT vtkImageDataToQImage(vtkImageData* imageData);

///
/// \ingroup Visualization_VTK_Widgets
/// Convert a QImage into a vtkImageData.
/// Creates a deep copy of the input image.
/// \param forceAlphaChannel If set to true then the VTK image data will always contain alpha channel.
///   If set to false then VTK image will be RGBA if QImage had alpha channel, RGB otherwise.
bool CTK_VISUALIZATION_VTK_WIDGETS_EXPORT qImageToVTKImageData(const QImage& image, vtkImageData* imageData, bool forceAlphaChannel=true);

///
/// \ingroup Visualization_VTK_Widgets
/// Convert a vtkScalarsToColors into a QImage
/// If size is empty, it will use the large icon size of the application style
QImage CTK_VISUALIZATION_VTK_WIDGETS_EXPORT scalarsToColorsImage(
  vtkScalarsToColors* scalarsToColors, const QSize& size = QSize());

///
/// \ingroup Visualization_VTK_Widgets
/// Sets the default format for QSurfaceFormat depending on the VTK OpenGL widget implementation.
void CTK_VISUALIZATION_VTK_WIDGETS_EXPORT vtkSetSurfaceDefaultFormat(void);

}

#endif
