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
#include <QDir>
#include <QTimer>

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMModel.h"
#include "ctkDICOMThumbnailListWidget.h"

// STD includes
#include <iostream>

int ctkDICOMThumbnailListWidgetTest1( int argc, char * argv [] )
{
  QApplication app(argc, argv);

  if (argc <= 2)
    {
    std::cerr << "Warning, no sql file given. Test stops" << std::endl;
    std::cerr << "Usage: qctkDICOMModelTest1 <scratch.db> <dumpfile.sql>" << std::endl;
    return EXIT_FAILURE;
    }

  try
    {
    QFileInfo databasePath;
    databasePath.setFile(argv[1]);
    ctkDICOMDatabase myCTK( databasePath.absoluteFilePath() );

    if (!myCTK.initializeDatabase(argv[2]))
      {
      std::cerr << "Error when initializing the data base: " << argv[2]
                << " error: " << myCTK.lastError().toStdString();
      }

    ctkDICOMModel model;
    model.setDatabase(myCTK.database());

    ctkDICOMThumbnailListWidget widget;
    widget.setDatabaseDirectory(databasePath.absolutePath());
    widget.addThumbnails(model.index(0,0));
    widget.show();

    if (argc <= 3 || QString(argv[3]) != "-I")
      {
      QTimer::singleShot(200, &app, SLOT(quit()));
      }
    return app.exec();
    }
  catch (...)
    {
    std::cerr << "Error" << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_FAILURE;
}
