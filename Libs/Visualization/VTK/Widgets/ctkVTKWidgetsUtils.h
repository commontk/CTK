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

namespace ctk {

///
/// \ingroup Visualization_VTK_Widgets
/// Grab the contents of a QWidget and all its children.
/// Handle correctly the case of QVTKWidget.
/// \sa ctk::grabWidget QWidget::grabWidget
QImage CTK_VISUALIZATION_VTK_WIDGETS_EXPORT grabVTKWidget(QWidget* widget, QRect rectangle = QRect());

///
/// \ingroup Visualization_VTK_Widgets
/// Convert a vtkImageData into a QImage
QImage CTK_VISUALIZATION_VTK_WIDGETS_EXPORT vtkImageDataToQImage(vtkImageData* imageData);

}

#endif
