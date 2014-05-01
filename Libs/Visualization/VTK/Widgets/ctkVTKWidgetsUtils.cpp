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

// Qt includes
#include <QImage>
#include <QPainter>
#include <QWidget>

// ctkWidgets includes
#include "ctkVTKWidgetsUtils.h"
#include "ctkWidgetsUtils.h"

// VTK includes
#include <QVTKWidget.h>
#include <vtkImageData.h>
#include <vtkVersion.h>

//----------------------------------------------------------------------------
QImage ctk::grabVTKWidget(QWidget* widget, QRect rectangle)
{
  if (!widget)
    {
    return QImage();
    }
  if (!rectangle.isValid())
    {
    rectangle = QRect(0,0,widget->width(),widget->height());
    }
  QImage widgetImage = ctk::grabWidget(widget, rectangle);
  QPainter painter;
  painter.begin(&widgetImage);
  foreach(QVTKWidget* vtkWidget, widget->findChildren<QVTKWidget*>())
    {
    if (!vtkWidget->isVisible())
      {
      continue;
      }
    QRect subWidgetRect = QRect(vtkWidget->mapTo(widget, QPoint(0,0)), vtkWidget->size());
    if (!rectangle.intersects(subWidgetRect))
      {
      continue;
      }
    vtkImageData* imageData = vtkWidget->cachedImage();
    /// \todo retrieve just the rectangle.intersected(
    QImage subImage = ctk::vtkImageDataToQImage(imageData);
    painter.drawImage(subWidgetRect, subImage);
    }
  painter.end();
  return widgetImage;
}

//----------------------------------------------------------------------------
QImage ctk::vtkImageDataToQImage(vtkImageData* imageData)
{
  if (!imageData)
    {
    return QImage();
    }
#if VTK_MAJOR_VERSION <= 5
  imageData->Update();
#endif
  /// \todo retrieve just the UpdateExtent
  int width = imageData->GetDimensions()[0];
  int height = imageData->GetDimensions()[1];
  QImage image(width, height, QImage::Format_RGB32);
  QRgb* rgbPtr = reinterpret_cast<QRgb*>(image.bits()) +
    width * (height-1);
  unsigned char* colorsPtr = reinterpret_cast<unsigned char*>(
    imageData->GetScalarPointer());
  // mirror vertically
  for(int row = 0; row < height; ++row)
    {
    for (int col = 0; col < width; ++col)
      {
      // Swap rgb
      *(rgbPtr++) = QColor(colorsPtr[0], colorsPtr[1], colorsPtr[2]).rgb();
      colorsPtr +=  3;
      }
    rgbPtr -= width * 2;
    }
  return image;
}
