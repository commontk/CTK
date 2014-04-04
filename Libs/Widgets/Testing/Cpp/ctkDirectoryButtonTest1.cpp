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
#include <QFormLayout>
#include <QSignalSpy>
#include <QStyle>
#include <QTimer>
#include <QVBoxLayout>

// CTK includes
#include "ctkDirectoryButton.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkDirectoryButtonTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);
  
  QWidget topLevel;
  ctkDirectoryButton button;
  
  QIcon defaultIcon = button.style()->standardIcon(QStyle::SP_DirIcon);
  QIcon icon = button.style()->standardIcon(QStyle::SP_MessageBoxQuestion);
  QIcon icon2 = button.style()->standardIcon(QStyle::SP_DesktopIcon);

  ctkDirectoryButton button2(".");
  ctkDirectoryButton button3(icon, "..");
  ctkDirectoryButton button4;
  button4.setAcceptMode(QFileDialog::AcceptSave);
  button4.setOptions(button4.options() | ctkDirectoryButton::DontUseNativeDialog);


  QFormLayout* layout = new QFormLayout;
  layout->addRow("Default button:", &button);
  layout->addRow("Current (.) directory: ", &button2);
  layout->addRow("Top (..) directory with icon:", &button3);
  layout->addRow("Writable directory only:", &button4);
  topLevel.setLayout(layout);
  
  button.setCaption("Select a directory");
  if (button.caption() != "Select a directory")
    {
    std::cerr << "ctkDirectoryButton::setCaption() failed." << std::endl;
    return EXIT_FAILURE;
    }

  button.setText("Click here");
  if (button.text() != "Click here")
    {
    std::cerr << "ctkDirectoryButton::setText() failed." << std::endl;
    return EXIT_FAILURE;
    }
  // Restore text to directory path
  button.setText(QString());
  if (button.text() != QString())
    {
    std::cerr << "ctkDirectoryButton::setText() failed." << std::endl;
    return EXIT_FAILURE;
    }

  if (button.icon().pixmap(20).toImage() !=
      defaultIcon.pixmap(20).toImage())
    {
    std::cerr << "ctkDirectoryButton::icon() failed." << std::endl;
    return EXIT_FAILURE;
    }

  button3.setIcon(icon2);
  if (button3.icon().pixmap(20).toImage() !=
      icon2.pixmap(20).toImage())
    {
    std::cerr << "ctkDirectoryButton::setIcon() failed." << std::endl;
    return EXIT_FAILURE;
    }
  
#ifdef USE_QFILEDIALOG_OPTIONS
  button.setOptions(QFileDialog::ShowDirsOnly | QFileDialog::ReadOnly);
  if (button.options() != (QFileDialog::ShowDirsOnly |
                           QFileDialog::ReadOnly))
#else
  button.setOptions(ctkDirectoryButton::ShowDirsOnly |
                    ctkDirectoryButton::ReadOnly);
  
  if (button.options() != (ctkDirectoryButton::ShowDirsOnly |
                           ctkDirectoryButton::ReadOnly))
#endif
    {
    std::cerr<< "ctkDirectoryButton::setOptions failed" << std::endl;
    return EXIT_FAILURE;
    }

  QSignalSpy spyDirectoryChanged(&button, SIGNAL(directoryChanged(QString)));
  QSignalSpy spyDirectorySelected(&button, SIGNAL(directorySelected(QString)));

  button.setDirectory(QDir::home().absolutePath());
  if ( QDir(button.directory()) != QDir::home() ||
       spyDirectoryChanged.count() != 1 || 
       spyDirectorySelected.count() != 1)
    {
    std::cerr<< "ctkDirectoryButton::setDirectory failed" << button.directory().toStdString() << std::endl;
    return EXIT_FAILURE;
    }

  spyDirectoryChanged.clear();
  spyDirectorySelected.clear();
  // set it again... just to check that it doesn't fire a new signal
  button.setDirectory(QDir::home().absolutePath());

  if ( QDir(button.directory()) != QDir::home() ||
       spyDirectoryChanged.count() != 0 || 
       spyDirectorySelected.count() != 1)
    {
    std::cerr<< "ctkDirectoryButton::setDirectory failed" << button.directory().toStdString() << std::endl;
    return EXIT_FAILURE;
    }

  topLevel.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(400, &app, SLOT(quit()));
    }

  // If Qt uses the default native dialog, a nested event loop won't
  // be created and app.quit() will have no effect (as it solely quits
  // event loops).
  button.setOptions(button.options() | ctkDirectoryButton::DontUseNativeDialog);
  QTimer::singleShot(100, &button, SLOT(browse()));

  return app.exec();
}
