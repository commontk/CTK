/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QApplication>
#include <QSignalSpy>
#include <QTimer>
#include <QVBoxLayout>

// CTK includes
#include "ctkFontButton.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkFontButtonTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);
  
  QFont customFont(QFont("Helvetica", 8));
  QFont customFont2(QFont("Arial", 12));
  
  QWidget topLevel;
  ctkFontButton button;
  ctkFontButton button2(customFont);
  
  QVBoxLayout* layout = new QVBoxLayout(&topLevel);
  layout->addWidget(&button);
  layout->addWidget(&button2);
  
  button.setCurrentFont(customFont);
  if (button.currentFont() != customFont)
    {
    std::cerr << "ctkFontButton::setFont() failed." << std::endl;
    return EXIT_FAILURE;
    }

  QSignalSpy spyFontChanged(&button, SIGNAL(currentFontChanged(const QFont&)));

  button.setCurrentFont(customFont2);
  if ( button.currentFont() != customFont2 ||
       spyFontChanged.count() != 1)
    {
    std::cerr<< "ctkFontButton::setCurrentFont failed" << button.currentFont().toString().toStdString() << std::endl;
    return EXIT_FAILURE;
    }

  topLevel.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  QTimer::singleShot(100, &button, SLOT(browseFont()));

  return app.exec();
}

