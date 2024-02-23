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

  QStringList arguments = app.arguments();
  QString testName = arguments.takeFirst();

  bool interactive = arguments.removeOne("-I");

  if (arguments.count() != 2)
  {
    std::cerr << "Warning, no sql file given. Test stops" << std::endl;
    std::cerr << "Usage: " << qPrintable(testName)
              << " <scratch.db> <dumpfile.sql>" << std::endl;
    return EXIT_FAILURE;
  }

  QString databaseFile(arguments.at(0));
  QString sqlFileName(arguments.at(1));

  try
  {
    QFileInfo databasePath;
    databasePath.setFile(databaseFile);
    ctkDICOMDatabase myCTK( databasePath.absoluteFilePath() );

    if (!myCTK.initializeDatabase(sqlFileName.toUtf8()))
    {
      std::cerr << "Error when initializing the data base: " << qPrintable(sqlFileName)
                << " error: " << qPrintable(myCTK.lastError()) << std::endl;
      return EXIT_FAILURE;
    }

    ctkDICOMModel model;
    model.setDatabase(myCTK.database());

    ctkDICOMThumbnailListWidget widget;
    widget.setDatabaseDirectory(databasePath.absolutePath());
    widget.addThumbnails(model.index(0,0));
    widget.show();

    if (!interactive)
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
