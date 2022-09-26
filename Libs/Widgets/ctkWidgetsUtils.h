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

#ifndef __ctkWidgetsUtils_h
#define __ctkWidgetsUtils_h

// Qt includes
#include <QString>
#include <QRect>
class QImage;

#include "ctkWidgetsExport.h"

namespace ctk {
///
/// \ingroup Widgets
/// Create a base 64 image tag. Can be used that way:
/// QString("<img src=\"%1\">").arg(base64HTMLImageTagSrc(myImage);
CTK_WIDGETS_EXPORT QString base64HTMLImageTagSrc(const QImage& image);

///
/// \ingroup Widgets
/// Grab the contents of a QWidget and all its children.
/// Handle correctly the case of QGLWidgets.
/// \sa QWidget::grab
CTK_WIDGETS_EXPORT QImage grabWidget(QWidget* widget, QRect rectangle = QRect());

///
/// \ingroup Widgets
/// Convert an KWidget encoded image into a QImage
/// The data can be base64 encoded and/or zlib compressed.
CTK_WIDGETS_EXPORT QImage kwIconToQImage(const unsigned char *data, int width, int height, int pixelSize, unsigned int bufferLength, int options = 0);

///
/// \ingroup Widgets
/// Get a colorized version of a SVG image
CTK_WIDGETS_EXPORT QIcon getColorizedIcon(const QString& hexColor, const QString& resourcePath);
}

#endif
