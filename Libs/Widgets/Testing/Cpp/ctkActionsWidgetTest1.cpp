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
#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QIcon>
#include <QMenu>
#include <QStandardItem>
#include <QStyle>
#include <QTimer>
#include <QTreeView>

// CTK includes
#include "ctkActionsWidget.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkActionsWidgetTest1(int argc, char* argv[])
{
  QApplication app(argc, argv);

  ctkActionsWidget actionsWidget(0);
  actionsWidget.show();

  QWidget widget;
  QIcon informationIcon = actionsWidget.style()->standardIcon(QStyle::SP_MessageBoxInformation);

  actionsWidget.addAction(new QAction(0));
  actionsWidget.addAction(new QAction(qApp));
  actionsWidget.clear();
  actionsWidget.addAction(new QAction("Action Text", qApp));
  actionsWidget.addAction(new QAction(informationIcon, "Action Text2", qApp));

  actionsWidget.addAction(new QAction(0), "category 1");
  actionsWidget.addAction(new QAction(qApp), "category 1");
  actionsWidget.addAction(new QAction("Action Text3", &widget), "category 1");
  actionsWidget.addAction(new QAction(informationIcon, "Action Text4", qApp), "category 1");

  actionsWidget.addAction(new QAction(0), "category 2");
  actionsWidget.addAction(new QAction(qApp), "category 3");
  actionsWidget.addAction(new QAction("Action Text5", &widget), "category 4");
  actionsWidget.addAction(new QAction(informationIcon, "Action Text6", qApp), "category 5");

  if (actionsWidget.groupItem("category 1") == 0 || 
      actionsWidget.groupItem("category 1")->rowCount() != 4)
    {
    qDebug() << "Invalid Category 1";
    return EXIT_FAILURE;
    }

  // check shortcut
  QAction* action = new QAction("custom action", 0);
  action->setShortcut(Qt::Key_F1);
  action->setToolTip("custom tooltip");
  actionsWidget.addAction(action);
  QStandardItem* actionItem = actionsWidget.model()->item(7);
  if (!actionItem || actionItem->text() != "custom action")
    {
    qDebug() << "Invalid custom action" << (actionItem ? actionItem->text() : "NaN");
    return EXIT_FAILURE;
    }
  // check update on change 
  action->setText("new custom action");
  QStandardItem* changedActionItem = actionsWidget.model()->item(7);
  if (changedActionItem != actionItem ||
      changedActionItem->text() != "new custom action")
    {
    qDebug() << "Invalid action update" << changedActionItem->text();
    return EXIT_FAILURE;
    }
  widget.addAction(action);
  
  QList<QAction*> actions;
  actions << new QAction("group action 1",qApp);
  actions << new QAction("group action 2",qApp);
  actions << new QAction("group action 3",qApp);
  actions << new QAction("group action 4",qApp);
  actions << new QAction("group action 5",qApp);
  actionsWidget.addActions(actions,"category 6");
  
  QMenu menu;
  actionsWidget.addAction(menu.addAction("&menu action"), "menu category");
  actionsWidget.addAction(menu.addSeparator(), "menu category");
  actionsWidget.addAction(menu.addMenu("submenu action")->menuAction(), "menu category");

  actionsWidget.setActionsWithNoShortcutVisible(false);
  
  QModelIndexList actionTextActions = actionsWidget.view()->model()->match(
    QModelIndex(), Qt::DisplayRole, QString("Action Text"), -1,
    Qt::MatchStartsWith | Qt::MatchWrap |Qt::MatchRecursive);

  if (actionsWidget.areActionsWithNoShortcutVisible() != false ||
      actionTextActions.count() != 0)
    {
    qDebug() << "ctkActionsWidget::setActionsWithNoShortcutVisible failed: actionTextActions.count()";
    return EXIT_FAILURE;
    }

  actionsWidget.setActionsWithNoShortcutVisible(true);

  actionsWidget.setMenuActionsVisible(false);

  // make sure the submenu action is hidden
  QModelIndexList submenuActions = actionsWidget.view()->model()->match(
    QModelIndex(), Qt::DisplayRole, QString("submenu action"), -1,
    Qt::MatchExactly | Qt::MatchWrap |Qt::MatchRecursive);
  if (actionsWidget.areMenuActionsVisible() != false ||
      submenuActions.count() != 0)
    {
    qDebug() << "ctkActionsWidget search failed" << submenuActions.count();
    return EXIT_FAILURE;
    }
  
  actionsWidget.setMenuActionsVisible(true);
  
  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}
