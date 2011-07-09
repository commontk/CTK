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
#include <QDebug>
#include <QApplication>
#include <QCleanlooksStyle>
#include <QTimer>

// CTK includes
#include <ctkCheckableComboBox.h>

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkCheckableComboBoxTest1(int argc, char * argv [] )
{
  // QCleanlooksStyle is the only style that doesn't show the checkboxes by
  // default. Test it with it
  QApplication::setStyle(new QCleanlooksStyle);
  QApplication app(argc, argv);

  ctkCheckableComboBox comboBox;
  comboBox.addItem("toto");
  comboBox.addItem("tata");
  comboBox.addItem("titi");
  comboBox.addItem(comboBox.style()->standardIcon(QStyle::SP_FileIcon),"tutu");
  
  comboBox.show();
  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }
  
  return app.exec();
}
