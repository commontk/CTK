/*==========================================================================

  Library: CTK

  Copyright (c) University of Sheffield

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
#include <limits>

// CTK Core
#include "ctkDICOMModelObject.h"

// Qt includes
#include <QApplication>
#include <QFileDialog>
#include <QString>
#include <QTreeView>
#include <QHeaderView>

int main(int argv, char** argc)
{
  QApplication app(argv, argc);

  QString fileName;

  if( QApplication::argc() > 1)
    {
      fileName = QApplication::argv()[1];
    }
  else
    {
      fileName = QFileDialog::getOpenFileName( 0,
       "Choose an image file", ".",
       "DCM (*)" 
       );
      if( fileName.size() == 0 )
	{
	  return EXIT_SUCCESS;
	}
    }
  

  ctkDICOMModelObject dcmInfoModel;
  dcmInfoModel.setFile(fileName);
    

  QTreeView *viewer = new QTreeView();
  viewer->setModel( &dcmInfoModel);
  viewer->expandAll();
  viewer->resizeColumnToContents(1);
  viewer->resizeColumnToContents(2);
  viewer->header()->setResizeMode( QHeaderView::Stretch);
  viewer->show();
  viewer->raise();
  return app.exec();
}
