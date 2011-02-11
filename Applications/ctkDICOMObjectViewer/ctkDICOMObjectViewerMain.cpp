/*==========================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

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
#include "iostream"

// CTK Widgets
#include "ctkDICOMDatasetView.h"

// Qt includes
#include <QApplication>
#include <QFileDialog>
#include <QString>

int main(int argv, char** argc)
{
  QApplication app(argv, argc);

  qApp->setOrganizationName("CTK");
  qApp->setOrganizationDomain("commontk.org");
  qApp->setApplicationName("ctkDICOMObjectViewer");

  QString s;
  if( QApplication::argc() > 1 )
    {
    s = QApplication::argv()[1];
    }
  else
    {
    s = QFileDialog::getOpenFileName( 0,
     "Choose an image file", ".",
     "JPG (*.jpg *.jpep);; PNG (*.png);; BMP (*.bmp);; TIFF (*.tif *.tiff)" 
     );
    }

  QImage image( s );
  std::cout << "Loading image _" << s.toStdString() << "_" << std::endl;

  ctkDICOMDatasetView imageView;
  imageView.addImage( & image );
  imageView.show();
  imageView.raise();

  return app.exec();
}
