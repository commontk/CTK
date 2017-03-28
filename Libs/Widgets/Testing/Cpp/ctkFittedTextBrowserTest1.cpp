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

  ctkFittedTextBrowser textBrowserWidgetCollapsibleText(&widget);
  textBrowserWidgetCollapsibleText.setCollapsibleText(
    "This is the teaser for auto-text.\n More details are here.\n"
    "This is a very very, very very very, very very, very very very, very very, very very very long line\n"
    "Some more lines 1.\n"
    "Some more lines 2.\n"
    "Some more, some more.");
  textBrowserWidgetCollapsibleText.setShowDetailsText("&gt;&gt;&gt;");
  textBrowserWidgetCollapsibleText.setHideDetailsText("&lt;&lt;&lt;");
  layout->addWidget(&textBrowserWidgetCollapsibleText);

  ctkFittedTextBrowser textBrowserWidgetCollapsibleHtml(&widget);
  textBrowserWidgetCollapsibleHtml.setCollapsibleHtml(
    "This is the teaser for html.<br>"
    "More details are here."
    "This is a very very, very very very, very very, very very very, very very, very very very long line\n"
    "Some more lines 1."
    "Some more lines 2."
    "Some more, some more.");
  layout->addWidget(&textBrowserWidgetCollapsibleHtml);

  ctkFittedTextBrowser textBrowserWidgetCollapsibleComplexHtml(&widget);
  textBrowserWidgetCollapsibleComplexHtml.setCollapsibleHtml(
    "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"><html>"
    "<head><meta name=\"qrichtext\" content=\"1\" /> <style type=\"text/css\"> p, li { white-space: pre-wrap; } </style></head>"
    "<body style=\" font-family:'MS Shell Dlg 2'; font-size:12.25pt; font-weight:400; font-style:normal;\">"
    "<p>This is the teaser for complex html.<br></p>"
    "<p>More details are here.</p>"
    "<p>This is a very very, very very very, very very, very very very, very very, very very very long line</p>"
    "<p>Some more lines 1."
    "Some more lines 2."
    "Some more, some more.</p>"
    "</body></html>");
  layout->addWidget(&textBrowserWidgetCollapsibleComplexHtml);
  textBrowserWidgetCollapsibleHtml.setCollapsed(false);

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
