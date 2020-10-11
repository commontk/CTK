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
#include <QTimer>

// CTK includes
#include "ctkCommandLineParser.h"
#include "ctkVTKObjectEventsObserver.h"
#include "ctkVTKSliceView.h"
#include "ctkVTKWidgetsUtils.h"

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
int ctkVTKSliceViewTest1(int argc, char * argv [] )
{
  ctk::vtkSetSurfaceDefaultFormat();

  QApplication app(argc, argv);

  // Command line parser
  ctkCommandLineParser parser;
  parser.addArgument("", "-I", QVariant::Bool);
  QHash<QString, QVariant> parsedArgs = parser.parseArguments(app.arguments());
  bool interactive = parsedArgs["-I"].toBool();

  ctkVTKSliceView sliceView;
  sliceView.setHighlightedBoxColor(QColor(Qt::yellow));
  sliceView.setCornerAnnotationText("CTK");
  sliceView.show();

  if (!interactive)
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }
  return app.exec();
}
