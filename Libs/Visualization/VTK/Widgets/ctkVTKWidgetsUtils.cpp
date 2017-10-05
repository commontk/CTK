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
#include <QApplication>
#include <QImage>
#include <QPainter>
#include <QStyle>
#include <QWidget>

// ctkWidgets includes
#include "ctkVTKWidgetsUtils.h"
#include "ctkWidgetsUtils.h"

// VTK includes
#if CTK_USE_QVTKOPENGLWIDGET
#include <QVTKOpenGLWidget.h>
#else
#include <QVTKWidget.h>
#endif
#include <vtkImageData.h>
#include <vtkPiecewiseFunction.h>
#include <vtkScalarsToColors.h>
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
#if CTK_USE_QVTKOPENGLWIDGET
  foreach(QVTKOpenGLWidget* vtkWidget, widget->findChildren<QVTKOpenGLWidget*>())
#else
  foreach(QVTKWidget* vtkWidget, widget->findChildren<QVTKWidget*>())
#endif
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
#if CTK_USE_QVTKOPENGLWIDGET
    QImage subImage = vtkWidget->grabFramebuffer();
#else
    vtkImageData* imageData = vtkWidget->cachedImage();
    /// \todo retrieve just the rectangle.intersected(
    QImage subImage = ctk::vtkImageDataToQImage(imageData);
#endif
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

//----------------------------------------------------------------------------
QImage ctk::scalarsToColorsImage(vtkScalarsToColors* scalarsToColors,
  const QSize& size)
{
  if (!scalarsToColors ||
    scalarsToColors->GetNumberOfAvailableColors() <= 0)
  {
    return QImage();
  }
  int width = size.width();
  int height = size.height();
  if (size.isEmpty())
  {
    width = height = qApp->style()->pixelMetric(QStyle::PM_LargeIconSize);
  }

  double* values = new double[width];
  const double* range = scalarsToColors->GetRange();
  for (int i = 0; i < width; ++i)
  {
    values[i] = range[0] + i * (range[1] - range[0]) / (width - 1);
  }

  QImage transferFunctionImage(width, height, QImage::Format_RGB32);
  unsigned char* colors = transferFunctionImage.bits();
  // Map the first line
  scalarsToColors->MapScalarsThroughTable2(
    values, colors, VTK_DOUBLE, width, 1, VTK_RGBA);
  delete[] values;
  // Pixels are not correctly ordered, reorder them correctly
  unsigned char* colorsPtr = colors;
  QRgb* rgbPtr = reinterpret_cast<QRgb*>(colors);
  for (int i = 0; i < width; ++i)
  {
    *(rgbPtr++) = QColor(colorsPtr[0], colorsPtr[1], colorsPtr[2]).rgb();
    colorsPtr += 4;
  }
  // Fill the other lines
  for (int i = 1; i < height; ++i)
  {
    memcpy(colors + i*VTK_RGBA*width, colors, VTK_RGBA*width);
  }

  return transferFunctionImage;
}
