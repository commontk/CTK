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
#include <QSignalSpy>
#include <QStyle>
#include <QTimer>
#include <QVBoxLayout>

#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
#include <QDesktopServices>
#else
#include <QStandardPaths>
#endif 

// CTK includes
#include "ctkPathLineEdit.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkPathLineEditTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QWidget topLevel;

  ctkPathLineEdit button;

  ctkPathLineEdit button2("Files",
                          QStringList() << "Images (*.png *.jpg)" << "Text (*.txt)",
                          ctkPathLineEdit::Files);

  ctkPathLineEdit button3("Dirs", QStringList(), ctkPathLineEdit::Dirs);
  button3.setShowHistoryButton(false);

#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
  QString documentsFolder = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
#else
  QString documentsFolder = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#endif 
  button3.setCurrentPath(documentsFolder);

  QVBoxLayout* layout = new QVBoxLayout;
  layout->addWidget(&button);
  layout->addWidget(&button2);
  layout->addWidget(&button3);
  topLevel.setLayout(layout);

  button.setCurrentPath(QDir::tempPath());
  if (button.currentPath() != QDir::tempPath())
    {
    std::cerr << "ctkPathLineEdit::setCurrentPath() failed"
              << qPrintable(button.currentPath()) << std::endl;
    return EXIT_FAILURE;
    }

  button.setLabel("Default");

  if (button.label() != "Default")
    {
    std::cerr << "ctkPathLineEdit::setLabel() failed"
              << qPrintable(button.label()) << std::endl;
    return EXIT_FAILURE;
    }

  QStringList nameFilters= button2.nameFilters();
  nameFilters <<  "*.conf";
  button2.setNameFilters(nameFilters);

  if (button2.nameFilters() != nameFilters)
    {
    std::cerr << "ctkPathLineEdit::setNameFilters() failed" << std::endl;
    return EXIT_FAILURE;
    }

  button2.setFilters(button2.filters() | ctkPathLineEdit::Readable);
  if (!(button2.filters() & ctkPathLineEdit::Readable))
    {
    std::cerr << "ctkPathLineEdit::setFilters() failed"
              << button2.filters() << std::endl;
    return EXIT_FAILURE;
    }

  button2.setCurrentFileExtension("jpg");

  if (!button2.currentPath().endsWith(".jpg"))
    {
    std::cerr << "ctkPathLineEdit::setCurrentFileExtension() failed"
              << qPrintable(button2.currentPath()) << std::endl;
    return EXIT_FAILURE;
    }

  topLevel.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(500, &app, SLOT(quit()));
    }

  QTimer::singleShot(100, &button, SLOT(retrieveHistory()));
  QTimer::singleShot(115, &button2, SLOT(addCurrentPathToHistory()));
  // The open dialog blocks QTimers (to quit the app).
  button3.setOptions(button3.options() | ctkPathLineEdit::DontUseNativeDialog);
  QTimer::singleShot(120, &button3, SLOT(browse()));

  return app.exec();
}
