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
#include <QDebug>
#include <QImage>
#include <QPainter>
#include <QStyle>
#include <QWidget>

// ctkWidgets includes
#include "ctkVTKOpenGLNativeWidget.h"
#include "ctkVTKWidgetsUtils.h"
#include "ctkWidgetsUtils.h"

// VTK includes
#include <vtkDataArray.h>
#include <vtkImageData.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPointData.h>
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
# if CTK_HAS_QVTKOPENGLNATIVEWIDGET_H
  foreach(QVTKOpenGLNativeWidget* vtkWidget, widget->findChildren<QVTKOpenGLNativeWidget*>())
# else
  foreach(QVTKOpenGLWidget* vtkWidget, widget->findChildren<QVTKOpenGLWidget*>())
# endif
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
  if (!imageData
    || !imageData->GetPointData()
    || !imageData->GetPointData()->GetScalars()
    || imageData->GetScalarType() != VTK_UNSIGNED_CHAR)
  {
    return QImage();
  }
  /// \todo retrieve just the UpdateExtent
  int width = imageData->GetDimensions()[0];
  int height = imageData->GetDimensions()[1];
  vtkIdType numberOfScalarComponents = imageData->GetNumberOfScalarComponents();
  QImage image;
  if (numberOfScalarComponents == 3)
  {
    image = QImage(width, height, QImage::Format_RGB888);
  }
  else if (numberOfScalarComponents == 4)
  {
    image = QImage(width, height, QImage::Format_RGBA8888);
  }
#if QT_VERSION >= QT_VERSION_CHECK(5,5,0)
  else if (numberOfScalarComponents == 1)
  {
    image = QImage(width, height, QImage::Format_Grayscale8);
  }
#endif
  else
  {
    // unsupported pixel format
    return QImage();
  }

  unsigned char* qtImageBuffer = image.bits();
  memcpy( qtImageBuffer,
    imageData->GetPointData()->GetScalars()->GetVoidPointer(0),
    numberOfScalarComponents * width * height);

  // Qt image is upside-down compared to VTK, so return mirrored image
  return image.mirrored();
}

//----------------------------------------------------------------------------
bool ctk::qImageToVTKImageData(const QImage& inputQImage, vtkImageData* outputVTKImageData, bool forceAlphaChannel/*=true*/)
{
  if (!outputVTKImageData)
  {
    qWarning() << Q_FUNC_INFO << " failed: outputVTKImageData is invalid";
    return false;
  }

  QSize size = inputQImage.size();
  vtkIdType width = size.width();
  vtkIdType height = size.height();
  if (width < 1 || height < 1)
  {
    qWarning() << Q_FUNC_INFO << " failed: input image is invalid";
    return false;
  }

  QImage normalizedQtImage;
  vtkIdType numberOfScalarComponents = 0;
  if (inputQImage.hasAlphaChannel() || forceAlphaChannel)
  {
    normalizedQtImage = inputQImage.convertToFormat(QImage::Format_RGBA8888).mirrored();
    numberOfScalarComponents = 4;
  }
  else
  {
    normalizedQtImage = inputQImage.convertToFormat(QImage::Format_RGB888).mirrored();
    numberOfScalarComponents = 3;
  }

  const unsigned char* normalizedQtImageBuffer = normalizedQtImage.bits();
  outputVTKImageData->SetExtent(0, width-1, 0, height-1, 0, 0);
  outputVTKImageData->AllocateScalars(VTK_UNSIGNED_CHAR, numberOfScalarComponents);
  memcpy(outputVTKImageData->GetPointData()->GetScalars()->GetVoidPointer(0),
    normalizedQtImageBuffer, numberOfScalarComponents * width * height);
  outputVTKImageData->GetPointData()->GetScalars()->Modified();

  return true;
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
