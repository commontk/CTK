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
#include <QStyle>
#include <QTimer>

// ctkDICOMCore includes
#include "ctkThumbnailLabel.h"

// STD includes
#include <iostream>

int ctkThumbnailLabelTest1( int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkThumbnailLabel widget;

  widget.setText("Test");
  if (widget.text() != "Test")
    {
    std::cerr << "ctkThumbnailLabel::setText failed."
              << " text: " << qPrintable(widget.text())
              << " expected: Test" << std::endl;
    return EXIT_FAILURE;
    }

  widget.setPixmap(QPixmap());
  if (widget.pixmap() != 0)
    {
    std::cerr << "ctkThumbnailLabel::setPixmap failed:"
              << widget.pixmap() << std::endl;
    return EXIT_FAILURE;
    }

  QPixmap standardPixmap = widget.style()->standardPixmap(QStyle::SP_DriveNetIcon);

  widget.setPixmap(standardPixmap);
  if (widget.pixmap() == 0)
    {
    std::cerr << "ctkThumbnailLabel::setPixmap failed:"
              << widget.pixmap() << std::endl;
    return EXIT_FAILURE;
    }
  widget.setTextPosition(Qt::AlignRight);
  widget.setTextPosition(Qt::AlignBottom);

  widget.setTransformationMode(Qt::SmoothTransformation);

  widget.show();

  if (argc <= 1 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}
