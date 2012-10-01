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
#include <QDebug>
#include <QIcon>
#include <QLineEdit>
#include <QMenu>
#include <QTimer>
#include <QToolBar>
#include <QVBoxLayout>

// CTK includes
#include "ctkMenuComboBox.h"

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
int ctkMenuComboBoxTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);
  QIcon plus(":Icons/plus.png");

  QMenu* menu = new QMenu("Menu");
  menu->addAction("First item");
  QMenu* subMenu = new QMenu("SubMenu");
  menu->addMenu(subMenu);

  QMenu* subMenu2 = new QMenu("SubMenu2");
  menu->addMenu(subMenu2);

  ctkMenuComboBox* menuComboBox = new ctkMenuComboBox(0);
  menuComboBox->setMenu(menu);
  menuComboBox->setDefaultText("Search");
  menuComboBox->setAutoFillBackground(true);
  menuComboBox->setMinimumContentsLength(25);
  if (menuComboBox->isSearchIconVisible() != true)
    {
    std::cerr << "Wrong default isSearchIconVisible: "
              << menuComboBox->isSearchIconVisible()
              << std::endl;
    return EXIT_FAILURE;
    }
  menuComboBox->setSearchIconVisible(true);
  if (menuComboBox->isSearchIconVisible() != true)
    {
    std::cerr << "Failed to set searchIconVisible: "
              << menuComboBox->isSearchIconVisible()
              << std::endl;
    return EXIT_FAILURE;
    }
  menuComboBox->setSearchIconVisible(false);
  if (menuComboBox->toolButtonStyle() != Qt::ToolButtonIconOnly)
    {
    std::cerr << "Wrong default toolButtonStyle: "
              << static_cast<unsigned int>(menuComboBox->toolButtonStyle())
              << std::endl;
    return EXIT_FAILURE;
    }
  menuComboBox->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  if (menuComboBox->toolButtonStyle() != Qt::ToolButtonTextUnderIcon)
    {
    std::cerr << "Failed to set toolButtonStyle: "
              << static_cast<unsigned int>(menuComboBox->toolButtonStyle())
              << std::endl;
    return EXIT_FAILURE;
    }
  menuComboBox->setSearchIconVisible(true);
  menuComboBox->setEditableBehavior(ctkMenuComboBox::EditableOnPopup);

  QWidget topLevelWidget(0);
  QVBoxLayout* layout = new QVBoxLayout;
  topLevelWidget.setLayout(layout);

  QToolBar bar;
  bar.addWidget(menuComboBox);
  layout->addWidget(&bar);

  menu->addAction(plus, "Item with icon");
  menu->addAction("Long item to test the width of the combobox...");
  subMenu->addAction("Submenu item");
  /// subMenu2 is added twice in the menu, once at the toplevel and once
  /// in the subMenu level.
  subMenu->addMenu(subMenu2);
  subMenu2->addAction("Subsubmenu item");

  QAction* actionEnd = subMenu->addAction("Quit");
  actionEnd->setShortcut(QKeySequence("Ctrl+Q"));
  QObject::connect(actionEnd, SIGNAL(triggered()), qApp, SLOT(quit()));

  topLevelWidget.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}
