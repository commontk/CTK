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
#include "ctkDICOMItemView.h"

// DCMTK includes
#include <dcmtk/dcmimgle/dcmimage.h>


// Qt includes
#include <QApplication>
#include <QFileDialog>
#include <QString>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  qApp->setOrganizationName("CTK");
  qApp->setOrganizationDomain("commontk.org");
  qApp->setApplicationName("ctkDICOMObjectViewer");

  QString s;
  if( argc > 1 )
    {
    s = argv[1];
    }
  else
    {
    s = QFileDialog::getOpenFileName( 0,
     "Choose an image file", ".",
     "DCM (*)"
     );
    if( s.size() == 0 )
      {
      return EXIT_SUCCESS;
      }
    }

  DicomImage dcmImage( s.toStdString().c_str() );

  ctkDICOMItemView imageView;
  imageView.addImage( dcmImage );
  imageView.show();
  imageView.raise();

  return app.exec();
}
