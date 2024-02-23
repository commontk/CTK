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
#include <QFileInfo>
#include <QHBoxLayout>
#include <QTimer>
#include <QTreeView>

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMModel.h"
#include "ctkModelTester.h"

// CTK includes
#include "ctkCheckableHeaderView.h"
#include <ctkCheckableModelHelper.h>

// STD includes
#include <iostream>

/* Test from build directory:
 ./CTK-build/bin/CTKDICOMCoreCxxTests ctkDICOMModelTest1 test.db ../CTK/Libs/DICOM/Core/Resources/dicom-sample.sql
*/

int ctkDICOMModelTest2( int argc, char * argv [] )
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
    ctkDICOMDatabase myCTK(databaseFile);

    if (!myCTK.initializeDatabase(sqlFileName.toUtf8()))
    {
      std::cerr << "Error when initializing the data base: " << qPrintable(sqlFileName)
                << " error: " << qPrintable(myCTK.lastError()) << std::endl;
      return EXIT_FAILURE;
    }

    ctkDICOMModel model;
    model.setDatabase(myCTK.database());

    QWidget topLevel;
    QTreeView viewer;
    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(&viewer);
    topLevel.setLayout(layout);
    viewer.setModel(&model);

    QHeaderView* previousHeaderView = viewer.header();
    qDebug() << "previous: " << previousHeaderView->isHidden();
    ctkCheckableHeaderView* headerView = new ctkCheckableHeaderView(Qt::Horizontal, &viewer);
    headerView->setSectionsClickable(previousHeaderView->sectionsClickable());
    headerView->setSectionsMovable(previousHeaderView->sectionsMovable());
    headerView->setHighlightSections(previousHeaderView->highlightSections());
    headerView->checkableModelHelper()->setPropagateDepth(-1);
    headerView->checkableModelHelper()->setForceCheckability(true);
    viewer.setHeader(headerView);
    model.setHeaderData(0, Qt::Horizontal, static_cast<int>(Qt::Checked), Qt::CheckStateRole);
    qDebug() << "new: " << headerView->isHidden();
    topLevel.show();

    if (!interactive)
    {
      QTimer::singleShot(200, &app, SLOT(quit()));
    }

    return app.exec();
  }
  catch (const std::exception& e)
  {
    std::cerr << "Error when opening the data base file: " << qPrintable(databaseFile)
              << " error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
