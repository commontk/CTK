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
#include <QBuffer>
#include <QImage>
#include <QFile>
#include <QGLWidget>
#include <QIcon>
#include <QPainter>
#include <QWidget>

// ctkWidgets includes
#include "ctkWidgetsUtils.h"

//----------------------------------------------------------------------------
QString ctk::base64HTMLImageTagSrc(const QImage& image)
{
  QByteArray byteArray;
  QBuffer buffer(&byteArray);
  buffer.open(QIODevice::WriteOnly);
  image.save(&buffer, "PNG");
  return QString("data:image/png;base64,%1")
    .arg(QString(buffer.data().toBase64()));
}

//----------------------------------------------------------------------------
QImage ctk::grabWidget(QWidget* widget, QRect rectangle)
{
  if (!widget)
    {
    return QImage();
    }
  if (!rectangle.isValid())
    {
    // Let Qt trigger layout mechanism and compute widget size.
    rectangle = QRect(0,0,-1,-1);
    }
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
  QPixmap widgetPixmap = QPixmap::grabWidget(widget, rectangle);
#else
  QPixmap widgetPixmap = widget->grab(rectangle);
#endif
  QImage widgetImage = widgetPixmap.toImage();
  QPainter painter;
  painter.begin(&widgetImage);
  foreach(QGLWidget* glWidget, widget->findChildren<QGLWidget*>())
    {
    if (!glWidget->isVisible())
      {
      continue;
      }
    QRect subWidgetRect = QRect(glWidget->mapTo(widget, QPoint(0,0)), glWidget->size());
    if (!rectangle.intersects(subWidgetRect))
      {
      continue;
      }
    QImage subImage = glWidget->grabFrameBuffer();
    painter.drawImage(subWidgetRect, subImage);
    }
  painter.end();
  return widgetImage;
}

//----------------------------------------------------------------------------
QImage ctk::kwIconToQImage(const unsigned char *data, int width, int height, int pixelSize, unsigned int bufferLength, int options)
{
  Q_UNUSED(options); // not yet supported

  QByteArray imageData = QByteArray::fromRawData(
    reinterpret_cast<const char *>(data), bufferLength);
  unsigned int expectedLength = width * height * pixelSize;
  // Start with a zlib header ? If not, then it must be base64 encoded
  if (bufferLength != expectedLength &&
      static_cast<unsigned char>(imageData[0]) != 0x78 &&
      static_cast<unsigned char>(imageData[1]) != 0xDA)
    {
    imageData = QByteArray::fromBase64(imageData);
    bufferLength = imageData.size();
    }

  if (bufferLength != expectedLength &&
      static_cast<unsigned char>(imageData[0]) == 0x78 &&
      static_cast<unsigned char>(imageData[1]) == 0xDA)
    {
    imageData.prepend(static_cast<char>((expectedLength >> 0) & 0xFF));
    imageData.prepend(static_cast<char>((expectedLength >> 8) & 0xFF));
    imageData.prepend(static_cast<char>((expectedLength >> 16) & 0xFF));
    imageData.prepend(static_cast<char>((expectedLength >> 24) & 0xFF));
    imageData = qUncompress(imageData);
    }
  QImage image(reinterpret_cast<unsigned char*>(imageData.data()),
               width, height, width * pixelSize,
               pixelSize == 4 ? QImage::Format_ARGB32 : QImage::Format_RGB888);
  if (pixelSize == 4)
    {
    return image.rgbSwapped();
    }
  return image.copy();
}

//----------------------------------------------------------------------------
QIcon ctk::getColorizedIcon(const QString& hexColor, const QString& resourcePath)
{
  QIcon icon;
  if (hexColor == "#000000")
  {
    // resource icons are already colored black by default
    icon = QIcon(resourcePath);
  }
  else
  {
    QFile file(resourcePath);
    file.open(QIODevice::ReadOnly);
    QByteArray data = file.readAll();
    QString newHexString = "fill=\"" + hexColor;
    QByteArray newByteArray = newHexString.toLocal8Bit();
    const char *newHexChar = newByteArray.data();
    data.replace("fill=\"#000000", newHexChar);
    QPixmap pixmap;
    pixmap.loadFromData(data);
    icon = QIcon(pixmap);
  }
  return icon;
}
