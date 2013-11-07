/*==========================================================================

  Library: CTK

  Copyright (c) Brigham and Women's Hospital (BWH).
  Copyright (c) University of Sheffield.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==========================================================================*/

// STD includes
#include <iostream>
#include <algorithm>

// Qt includes
#include <QApplication>
#include <QFileDialog>
#include <QHeaderView>
#include <QString>
#include <QTimer>
#include <QTreeView>

// CTK Core
#include "ctkDICOMObjectModel.h"

int ctkDICOMObjectModelTest1( int argc, char * argv [] )
{
  QApplication app(argc, argv);
  QString fileName;
  //TODO: Add the option for reading the test file from argv
  fileName = QFileDialog::getOpenFileName( 0,
    "Choose a DCM File", ".","DCM (*)" );
  ctkDICOMObjectModel dcmObjModel;
  dcmObjModel.setFile(fileName);

  QTreeView *viewer = new QTreeView();
  viewer->setModel( &dcmObjModel);
  viewer->expandAll();
  viewer->resizeColumnToContents(0);
  viewer->resizeColumnToContents(1);
  viewer->resizeColumnToContents(2);
  viewer->resizeColumnToContents(3);
  viewer->resizeColumnToContents(4);
  //viewer->header()->setResizeMode( QHeaderView::Stretch);
  viewer->show();
  viewer->raise();

  return app.exec();
}
