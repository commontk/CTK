/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

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
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTreeView>
#include <QVBoxLayout>

// CTK includes
#include "ctkActionsWidget.h"

//-----------------------------------------------------------------------------
class ctkActionsWidgetPrivate:public ctkPrivate<ctkActionsWidget>
{
public:
  ctkActionsWidgetPrivate();
  void setupUI();
  void updateItems(QList<QStandardItem*>& items, QAction* action);

  QStandardItemModel* ActionsModel;
  QTreeView*          ActionsTreeView;
  bool                ShowActionsWithNoShortcut;
};

//-----------------------------------------------------------------------------
ctkActionsWidgetPrivate::ctkActionsWidgetPrivate()
{
  this->ActionsTreeView = 0;
  this->ShowActionsWithNoShortcut = false;
}

//-----------------------------------------------------------------------------
void ctkActionsWidgetPrivate::setupUI()
{
  CTK_P(ctkActionsWidget);

  this->ActionsModel = new QStandardItemModel(p);
  this->ActionsModel->setColumnCount(4);
  QStringList headers;
  headers << "Action" << "Shortcut" << "Context" << "Details";
  this->ActionsModel->setHorizontalHeaderLabels(headers);

  this->ActionsTreeView = new QTreeView(p);
  QVBoxLayout* layout = new QVBoxLayout(p);
  layout->addWidget(this->ActionsTreeView);
  p->setLayout(layout);
  this->ActionsTreeView->setModel(this->ActionsModel);
  this->ActionsTreeView->setAlternatingRowColors(true);
}

//-----------------------------------------------------------------------------
void ctkActionsWidgetPrivate
::updateItems(QList<QStandardItem*>& items, QAction* action)
{
  Q_ASSERT(items.size() == 4);
  // Name
  items[0]->setText(action->text());
  items[0]->setIcon(action->icon());
  // Shortcut
  items[1]->setText(action->shortcut().toString(QKeySequence::NativeText));
  // Context
  QString shortcutContext;
  switch (action->shortcutContext())
    {
    case Qt::WidgetShortcut:
    case Qt::WidgetWithChildrenShortcut:
      shortcutContext = "Widget";
      break;
    case Qt::WindowShortcut:
    case Qt::ApplicationShortcut:
    default:
      shortcutContext = "Application";
    }
  items[2]->setText(shortcutContext);
  items[3]->setText(action->toolTip() != action->text()
    ? action->toolTip() : QString(""));
}

//-----------------------------------------------------------------------------
ctkActionsWidget::ctkActionsWidget(QWidget* parentWidget)
  :QWidget(parentWidget)
{
  CTK_INIT_PRIVATE(ctkActionsWidget);
  CTK_D(ctkActionsWidget);
  d->setupUI();
}

//-----------------------------------------------------------------------------
void ctkActionsWidget::addAction(QAction* action, const QString& group)
{
  CTK_D(ctkActionsWidget);
  QStandardItem* actionGroupItem = this->groupItem(group);
  Q_ASSERT(actionGroupItem);
  QList<QStandardItem*> actionItems;
  for (int i = 0; i < 4; ++i)
    {
    QStandardItem* item = new QStandardItem;
    item->setData(qVariantFromValue(qobject_cast<QObject*>(action)));
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    actionItems << item;
    }

  d->updateItems(actionItems, action);
  bool expandGroupItem = (actionGroupItem->rowCount() == 0);
  actionGroupItem->appendRow(actionItems);
  if (expandGroupItem)
    {
    d->ActionsTreeView->expand(
      d->ActionsModel->indexFromItem(actionGroupItem));
    }
  connect(action, SIGNAL(changed()), this, SLOT(updateAction()));
}

//-----------------------------------------------------------------------------
void ctkActionsWidget::addActions(QList<QAction*> actions, const QString& group)
{
  foreach(QAction* action, actions)
    {
    this->addAction(action, group);
    }
}

//-----------------------------------------------------------------------------
QStandardItem* ctkActionsWidget::groupItem(const QString& group)
{
  CTK_D(ctkActionsWidget);
  if (group.isEmpty())
    {
    return d->ActionsModel->invisibleRootItem();
    }
  // check if the group already exists
  QList<QStandardItem *> foundGroup =
    d->ActionsModel->findItems(group);
  if (foundGroup.size() > 0)
    {
    return foundGroup[0];
    }
  QStandardItem* groupItem = new QStandardItem(group);
  d->ActionsModel->appendRow(groupItem);
  return groupItem;
}

//-----------------------------------------------------------------------------
QStandardItemModel* ctkActionsWidget::model()const
{
  CTK_D(const ctkActionsWidget);
  return d->ActionsModel;
}

//-----------------------------------------------------------------------------
void ctkActionsWidget::updateAction()
{
  CTK_D(ctkActionsWidget);
  QAction* action = qobject_cast<QAction*>(this->sender());
  Q_ASSERT(action);
  QModelIndexList foundAction = 
    d->ActionsModel->match(d->ActionsModel->index(0,0),
    Qt::UserRole + 1, qVariantFromValue(qobject_cast<QObject*>(action)),
    1, Qt::MatchExactly | Qt::MatchRecursive);
  Q_ASSERT(foundAction.size());
  QModelIndex parentIndex = foundAction[0].parent();
  QStandardItem* parent = parentIndex.isValid()
    ? d->ActionsModel->itemFromIndex(parentIndex)
    : d->ActionsModel->invisibleRootItem();
  int actionRow = foundAction[0].row();
  Q_ASSERT(actionRow >= 0);
  QList<QStandardItem*> actionItems;
  for(int i = 0; i < 4; ++i)
    {
    actionItems << parent->child(actionRow, i);
    }
  d->updateItems(actionItems, action);
}

//-----------------------------------------------------------------------------
QTreeView* ctkActionsWidget::view()const
{
  CTK_D(const ctkActionsWidget);
  return d->ActionsTreeView;
}
