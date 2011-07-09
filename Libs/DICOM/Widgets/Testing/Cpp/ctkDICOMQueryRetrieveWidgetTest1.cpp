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
#include <QDebug>
#include <QTimer>

// ctkDICOMCore includes
#include "ctkDICOMQueryRetrieveWidget.h"

// STD includes
#include <iostream>

int ctkDICOMQueryRetrieveWidgetTest1( int argc, char * argv [] )
{
  QApplication app(argc, argv);
 
  QSharedPointer<ctkDICOMDatabase> dicomDatabase;
  ctkDICOMQueryRetrieveWidget widget;
  widget.setRetrieveDatabase(dicomDatabase);
  if (widget.retrieveDatabase() != dicomDatabase)
    {
    std::cerr << "ctkDICOMQueryRetrieveDatabase::setRetrieveDatabase failed."
              << std::endl;
    }

  widget.query();
  widget.retrieve();
  widget.show();

  if (argc <= 1 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}
