/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QApplication>
#include <QTimer>

// ctkDICOMCore includes
#include "ctkDICOMThumbnailWidget.h"

// STD includes
#include <iostream>

int ctkDICOMThumbnailWidgetTest1( int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkDICOMThumbnailWidget widget;

  widget.setText("Test");
  if (widget.text() != "Test")
    {
    std::cerr << "ctkDICOMThumbnailWidget::setText failed."
              << " text: " << qPrintable(widget.text())
              << " expected: Test" << std::endl;
    return EXIT_FAILURE;
    }

  widget.setPixmap(QPixmap());
  if (widget.pixmap())
    {
    std::cerr << "ctkDICOMThumbnailWidget::setPixmap failed:"
              << widget.pixmap() << std::endl;
    return EXIT_FAILURE;
    }

  widget.show();

  if (argc <= 1 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}
