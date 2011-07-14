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
#include <QBuffer>
#include <QImage>
#include <QStyle>

// CTK includes
#include "ctkLogger.h"
#include "ctkVTKScalarsToColorsUtils.h"

// VTK includes
#include <vtkScalarsToColors.h>

//----------------------------------------------------------------------------
static ctkLogger logger("org.commontk.visualization.vtk.widgets.ctkVTKScalarsToColorsUtils");
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
QImage ctk::scalarsToColorsImage(vtkScalarsToColors* scalarsToColors, const QSize& size)
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
  delete [] values;
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
