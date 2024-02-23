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
#include <QLineEdit>
#include <QStringListModel>
#include <QTimer>

// CTK includes
#include "ctkCompleter.h"

//-----------------------------------------------------------------------------
int ctkCompleterTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QStringList list;
  list << "toto tutu";
  list << "tutu, tata";
  list << "titi, tata";
  list << "tititi";

  QStringListModel* model = new QStringListModel(list);
  ctkCompleter* completer = new ctkCompleter(model);
  completer->setCaseSensitivity(Qt::CaseInsensitive);

  QLineEdit lineEdit;
  lineEdit.setCompleter(completer);

  completer->setModelFiltering(ctkCompleter::FilterStartsWith);
  completer->setModelFiltering(ctkCompleter::FilterWordStartsWith);
  completer->setModelFiltering(ctkCompleter::FilterContains);

  lineEdit.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
  {
    QTimer::singleShot(200, &app, SLOT(quit()));
  }

  return app.exec();
}
