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
#include <QHBoxLayout>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QWidget>

// CTK includes
#include "ctkButton.h"

// STD includes

//-----------------------------------------------------------------------------
int ctkButtonTest1(int argc, char* argv[])
{
  QApplication app(argc, argv);

  QWidget widget(0);
  widget.show();
  QHBoxLayout* layout = new QHBoxLayout(&widget);
  ctkButton* button = new ctkButton(&widget);
  layout->addWidget(button);

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(500, &app, SLOT(quit()));
    }

  return app.exec();
}
