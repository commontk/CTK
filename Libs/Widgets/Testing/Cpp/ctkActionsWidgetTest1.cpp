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
#include <QAction>
#include <QDebug>
#include <QIcon>
#include <QStandardItem>
#include <QStyle>

// CTK includes
#include "ctkActionsWidget.h"
#include "ctkTestApplication.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
QCTK_DECLARE_TEST(ctkActionsWidgetTest1)
{
  ctkActionsWidget* actionsWidget = new ctkActionsWidget(0);
  actionsWidget->show();
  QWidget widget;
  QIcon informationIcon = actionsWidget->style()->standardIcon(QStyle::SP_MessageBoxInformation);

  actionsWidget->addAction(new QAction(0));
  actionsWidget->addAction(new QAction(qApp));
  actionsWidget->addAction(new QAction("Action Text", qApp));
  actionsWidget->addAction(new QAction(informationIcon, "Action Text", qApp));

  actionsWidget->addAction(new QAction(0), "category 1");
  actionsWidget->addAction(new QAction(qApp), "category 1");
  actionsWidget->addAction(new QAction("Action Text", &widget), "category 1");
  actionsWidget->addAction(new QAction(informationIcon, "Action Text", qApp), "category 1");

  actionsWidget->addAction(new QAction(0), "category 2");
  actionsWidget->addAction(new QAction(qApp), "category 3");
  actionsWidget->addAction(new QAction("Action Text", &widget), "category 4");
  actionsWidget->addAction(new QAction(informationIcon, "Action Text", qApp), "category 5");

  if (actionsWidget->groupItem("category 1") == 0 || 
      actionsWidget->groupItem("category 1")->rowCount() != 4)
    {
    qDebug() << "Invalid Category 1";
    QCTK_EXIT_TEST(EXIT_FAILURE);
    }

  // check shortcut
  QAction* action = new QAction("custom action", 0);
  action->setShortcut(Qt::Key_F1);
  action->setToolTip("custom tooltip");
  actionsWidget->addAction(action);
  QStandardItem* actionItem = actionsWidget->model()->item(9);
  if (!actionItem || actionItem->text() != "custom action")
    {
    qDebug() << "Invalid custom action" << actionItem->text();
    QCTK_EXIT_TEST(EXIT_FAILURE);
    }
  // check update on change 
  action->setText("new custom action");
  QStandardItem* changedActionItem = actionsWidget->model()->item(9);
  if (changedActionItem != actionItem ||
      changedActionItem->text() != "new custom action")
    {
    qDebug() << "Invalid action update" << changedActionItem->text();
    QCTK_EXIT_TEST(EXIT_FAILURE);
    }
  widget.addAction(action);
  QCTK_EXIT_TEST(EXIT_SUCCESS);
  //QTimer::singleShot(500, QApplication::instance(), SLOT(quit()));
}

QCTK_RUN_TEST(ctkActionsWidgetTest1);
