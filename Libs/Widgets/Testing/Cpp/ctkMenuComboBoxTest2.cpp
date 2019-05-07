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
int ctkMenuComboBoxTest2(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkMenuComboBox* menu = new ctkMenuComboBox();

  /// ------ Test setMenu ------
  menu->setMenu(new QMenu(0));
  CHECK_BOOL(menu->menu()->isEmpty(), true);

  QMenu* file = new QMenu("File");
  file->addAction("first");
  menu->setMenu(file);
  // File [Menu]
  //  |-- first [Action]
  QList<QCompleter* > completer = menu->findChildren<QCompleter *>();
  CHECK_BOOL(menu->menu()->isEmpty(), false);
  CHECK_INT(completer[0]->model()->rowCount(), 1);

  /// ------- Test delete Menu -----
  //delete menu->menu();
  //file = new QMenu("File");
  //menu->show();
  //return app.exec();

  /// ------- Test addAction -------
  menu->setMenu(file);
  QMenu* informatics = new QMenu("Informatics");
  QAction* titi = informatics->addAction("titi");
  file->addMenu(informatics);
  // File [Menu]
  //  |-- first [Action]
  //  |
  //  |-- Informatics [Menu]
  //        |-- titi [Action]
  CHECK_BOOL(menu->menu()->isEmpty(), false);
  CHECK_INT(completer[0]->model()->rowCount(), 2);

  /// ------- Test removeAction -------
  informatics->removeAction(titi);
  // File [Menu]
  //  |-- first [Action]
  //  |
  //  |-- Informatics [Menu]
  CHECK_BOOL(menu->menu()->isEmpty(), false);
  CHECK_INT(completer[0]->model()->rowCount(), 1);

  /// ------- Test add 2 same actions -> only 1 add on the completer --------
  informatics->addAction(titi);
  file->addAction(titi);
  // File [Menu]
  //  |-- first [Action]
  //  |-- titi [Action]
  //  |
  //  |-- Informatics [Menu]
  //        |-- titi [Action]
  CHECK_BOOL(menu->menu()->isEmpty(), false);
  CHECK_INT(completer[0]->model()->rowCount(), 2);

  /// ------- Test remove one of the two --------
  informatics->removeAction(titi);
  //file->removeAction(first);

  // File [Menu]
  //  |-- first [Action]
  //  |-- titi [Action]
  //  |
  //  |-- Informatics [Menu]
  CHECK_BOOL(menu->menu()->isEmpty(), false);
  CHECK_INT(completer[0]->model()->rowCount(), 2);

  /// ------- Test remove the second one -------
  file->removeAction(titi);
  // File [Menu]
  //  |-- first [Action]
  //  |
  //  |-- Informatics [Menu]
  CHECK_BOOL(menu->menu()->isEmpty(), false);
  CHECK_INT(completer[0]->model()->rowCount(), 1);

  menu->show();
  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}

