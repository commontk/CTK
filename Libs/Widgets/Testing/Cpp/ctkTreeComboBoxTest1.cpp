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
#include <QStandardItemModel>
#include <QTimer>
#include <QTreeView>

// CTK includes
#include "ctkTreeComboBox.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkTreeComboBoxTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkTreeComboBox combo;
  QStandardItemModel model;
  model.appendRow(new QStandardItem("Test1"));
  model.item(0)->appendRow(new QStandardItem("Test1.1"));
  model.item(0)->appendRow(new QStandardItem("Test1.2"));
  model.item(0)->appendRow(new QStandardItem("Test1.3"));
  model.appendRow(new QStandardItem("Test2"));
  model.appendRow(new QStandardItem("Test3"));
  combo.setModel(&model);
  
  if (combo.treeView() == 0)
    {
    std::cerr << "No tree view" << std::endl;
    return EXIT_FAILURE;
    }

  combo.show();
  QApplication::processEvents();

  // if the effect UI_AnimateCombo is enabled, the popup doesn't have time
  // to be visible when hidePopup() is called wich doesn't hide the popup.
  bool oldEnabled = QApplication::isEffectEnabled(Qt::UI_AnimateCombo);
  QApplication::setEffectEnabled(Qt::UI_AnimateCombo, false);
  combo.showPopup();
  QApplication::processEvents();

  combo.treeView()->setExpanded(model.item(0)->index(), true);
  QApplication::processEvents();

  combo.hidePopup();
  QApplication::processEvents();

  QApplication::setEffectEnabled(Qt::UI_AnimateCombo, oldEnabled);

  combo.showPopup();
  
  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }
  return app.exec();
}

