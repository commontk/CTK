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
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

// CTK includes
#include "ctkFittedTextBrowser.h"

// STD includes
#include <cstdlib>
#include <iostream>

int ctkFittedTextBrowserTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QWidget widget;
  QVBoxLayout* layout = new QVBoxLayout;
  widget.setLayout(layout);

  ctkFittedTextBrowser textBrowserWidget(&widget);
  textBrowserWidget.setText(
    "<pre>"
    "This is a short line.\n"
    "This is a very very, very very very, very very, very very very, very very, very very very long line\n"
    "Some more lines 1."
    "Some more lines 2."
    "Some more, some more."
    "</pre>");
  layout->addWidget(&textBrowserWidget);

  QPushButton expandingButton(&widget);
  QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  sizePolicy.setHorizontalStretch(1);
  sizePolicy.setVerticalStretch(1);
  expandingButton.setSizePolicy(sizePolicy);
  layout->addWidget(&expandingButton);

  widget.show();

  if (argc < 2 || QString(argv[1]) != "-I")
  {
    QTimer::singleShot(200, &app, SLOT(quit()));
  }

  return app.exec();
}
