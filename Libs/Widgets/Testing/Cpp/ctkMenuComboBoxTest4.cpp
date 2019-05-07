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
#include <QComboBox>
#include <QCompleter>
#include <QDebug>
#include <QIcon>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QTimer>
#include <QToolBar>
#include <QVBoxLayout>

// CTK includes
#include "ctkCoreTestingMacros.h"
#include "ctkMenuComboBox.h"

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
int ctkMenuComboBoxTest4(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  // File [Menu]
  //  |-- file 1 [Action]
  //  |-- file 2 [Action]
  //  |
  //  |-- Informatics [Menu]
  //        |-- info 1 [Action]
  //        |-- info 2 [Action]
  QMenu* file = new QMenu("File");
  QAction* file_file_1 = file->addAction("file 1");
  QAction* file_file_2 = file->addAction("file 2");
  QMenu* informatics = new QMenu("Informatics");
  QAction* file_informatics_info_1 = informatics->addAction("info 1");
  QAction* file_informatics_info_2 = informatics->addAction("info 2");
  file->addMenu(informatics);

  // Completer [Menu]
  //  |-- file 1 [Action]
  //  |-- file 2 [Action]
  //  |-- info 1 [Action]
  //  |-- info 2 [Action]
  //  |-- completer 1 [Action]
  //  |-- completer 2 [Action]
  QMenu* completerMenu = new QMenu("Completer");
  completerMenu->addAction(file_file_1);
  completerMenu->addAction(file_file_2);
  completerMenu->addAction(file_informatics_info_1);
  completerMenu->addAction(file_informatics_info_2);
  /*QAction* completer_1 = */completerMenu->addAction("completer 1");
  /*QAction* completer_2 = */completerMenu->addAction("completer 2");

  ctkMenuComboBox* menu = new ctkMenuComboBox();

  /// ------ Test setMenu ------
  menu->setMenu(file);
  CHECK_POINTER(menu->menu(), file);
  CHECK_POINTER(menu->completerMenu(), file);
  CHECK_BOOL(menu->menu()->isEmpty(), false);
  QList<QCompleter* > completer = menu->findChildren<QCompleter *>();
  CHECK_INT(completer[0]->model()->rowCount(), 4);

  /// ------ Test setMenu(0) ------
  menu->setMenu(0);
  CHECK_POINTER(menu->menu(), 0);
  CHECK_POINTER(menu->completerMenu(), 0);
  CHECK_INT(completer[0]->model()->rowCount(), 0);

  /// ------ Test setCompleterMenu ------
  menu->setMenu(file);
  menu->setCompleterMenu(completerMenu);
  CHECK_POINTER(menu->menu(), file);
  CHECK_POINTER(menu->completerMenu(), completerMenu);
  CHECK_INT(completer[0]->model()->rowCount(), 6);

  /// ------ Test removeAction ------
  file->removeAction(file_file_2);
  informatics->removeAction(file_informatics_info_1);
  CHECK_INT(completer[0]->model()->rowCount(), 6); // Completer should still include all actions

  /// ------ Test setCompleterMenu ------
  menu->setCompleterMenu(file);
  CHECK_POINTER(menu->menu(), file);
  CHECK_POINTER(menu->completerMenu(), file);
  CHECK_INT(completer[0]->model()->rowCount(), 2);

  menu->show();
  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}

