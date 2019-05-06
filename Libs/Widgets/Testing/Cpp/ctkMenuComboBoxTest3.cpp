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
#include <QList>
#include <QMenu>
#include <QTimer>

// CTK includes
#include "ctkCoreTestingMacros.h"
#include "ctkMenuComboBox.h"

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
int ctkMenuComboBoxTest3(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkMenuComboBox menu;
  CHECK_NULL(menu.menu());

  menu.setEditableBehavior(ctkMenuComboBox::Editable);
  QList<QComboBox* > comboBox = menu.findChildren<QComboBox *>();
  CHECK_BOOL(comboBox[0]->isEditable(), true); // Check default value

  menu.setEditableBehavior(ctkMenuComboBox::EditableOnFocus);
  CHECK_BOOL(comboBox[0]->isEditable(), false);

  menu.setEditableBehavior(ctkMenuComboBox::Editable);
  CHECK_BOOL(comboBox[0]->isEditable(), true);

  menu.setEditableBehavior(ctkMenuComboBox::NotEditable);
  CHECK_BOOL(comboBox[0]->isEditable(), false);

  menu.show();
  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}

