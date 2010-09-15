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
#include <QPainter>
#include <QSortFilterProxyModel>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTextDocument>
#include <QTreeView>
#include <QVBoxLayout>

// CTK includes
#include "ctkActionsWidget.h"

//-----------------------------------------------------------------------------
class ctkActionsWidgetPrivate
{
  Q_DECLARE_PUBLIC(ctkActionsWidget);
protected:
  ctkActionsWidget* const q_ptr;
public:
  ctkActionsWidgetPrivate(ctkActionsWidget& object);
  void setupUI();
  void updateItems(QList<QStandardItem*>& items, QAction* action);

  QStandardItemModel*    ActionsModel;
  ctkSortFilterActionsProxyModel* SortFilterActionsProxyModel;
  QTreeView*             ActionsTreeView;
};

//-----------------------------------------------------------------------------
ctkActionsWidgetPrivate::ctkActionsWidgetPrivate(ctkActionsWidget& object)
  :q_ptr(&object)
{
  this->ActionsModel = 0;
  this->SortFilterActionsProxyModel = 0;
  this->ActionsTreeView = 0;
}

//-----------------------------------------------------------------------------
void ctkActionsWidgetPrivate::setupUI()
{
  Q_Q(ctkActionsWidget);

  this->ActionsModel = new QStandardItemModel(q);
  this->ActionsModel->setColumnCount(4);
  QStringList headers;
  headers << "Action" << "Shortcut" << "Context" << "Details";
  this->ActionsModel->setHorizontalHeaderLabels(headers);

  this->SortFilterActionsProxyModel = new ctkSortFilterActionsProxyModel(q);
  this->SortFilterActionsProxyModel->setSourceModel(this->ActionsModel);

  this->ActionsTreeView = new QTreeView(q);
  QVBoxLayout* layout = new QVBoxLayout(q);
  layout->addWidget(this->ActionsTreeView);
  q->setLayout(layout);
  this->ActionsTreeView->setModel(this->SortFilterActionsProxyModel);
  this->ActionsTreeView->setAlternatingRowColors(true);
  //this->ActionsTreeView->setItemDelegate(new ctkRichTextItemDelegate);
}

//-----------------------------------------------------------------------------
void ctkActionsWidgetPrivate
::updateItems(QList<QStandardItem*>& items, QAction* action)
{
  Q_ASSERT(items.size() == 4);
  // Name
  QString actionText = action->text();
  if (actionText.indexOf('&') != -1)
    {
    actionText = actionText.remove(actionText.indexOf('&'),1);  // remove mnemonic
    }
  items[ctkActionsWidget::NameColumn]->setText(actionText);
  items[ctkActionsWidget::NameColumn]->setIcon(action->icon());
  // Shortcut
  items[ctkActionsWidget::ShortcutColumn]->setText(
    action->shortcut().toString(QKeySequence::NativeText));
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
  items[ctkActionsWidget::ContextColumn]->setText(shortcutContext);
  items[ctkActionsWidget::DetailsColumn]->setText(action->toolTip() != actionText
    ? action->toolTip() : QString(""));
}

//-----------------------------------------------------------------------------
ctkActionsWidget::ctkActionsWidget(QWidget* parentWidget)
  :QWidget(parentWidget)
  , d_ptr(new ctkActionsWidgetPrivate(*this))
{
  Q_D(ctkActionsWidget);
  d->setupUI();
}

//-----------------------------------------------------------------------------
ctkActionsWidget::~ctkActionsWidget()
{
}

//-----------------------------------------------------------------------------
void ctkActionsWidget::addAction(QAction* action, const QString& group)
{
  Q_D(ctkActionsWidget);
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
    qDebug() << d->ActionsModel->indexFromItem(actionGroupItem);
    d->ActionsTreeView->expand(
      d->SortFilterActionsProxyModel->mapFromSource(d->ActionsModel->indexFromItem(actionGroupItem)));
    }
  d->ActionsTreeView->resizeColumnToContents(0);
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
void ctkActionsWidget::clear()
{
  Q_D(ctkActionsWidget);
  d->ActionsModel->clear();
}

//-----------------------------------------------------------------------------
QStandardItem* ctkActionsWidget::groupItem(const QString& group)
{
  Q_D(ctkActionsWidget);
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
  groupItem->setFlags(Qt::ItemIsEnabled);
  d->ActionsModel->appendRow(groupItem);
  return groupItem;
}

//-----------------------------------------------------------------------------
QStandardItemModel* ctkActionsWidget::model()const
{
  Q_D(const ctkActionsWidget);
  return d->ActionsModel;
}

//-----------------------------------------------------------------------------
void ctkActionsWidget::setActionsWithNoShortcutVisible(bool show)
{
  Q_D(ctkActionsWidget);
  d->SortFilterActionsProxyModel->setActionsWithNoShortcutVisible(show);
}

//-----------------------------------------------------------------------------
bool ctkActionsWidget::areActionsWithNoShortcutVisible()const
{
  Q_D(const ctkActionsWidget);
  return d->SortFilterActionsProxyModel->areActionsWithNoShortcutVisible();
}

//-----------------------------------------------------------------------------
void ctkActionsWidget::setMenuActionsVisible(bool show)
{
  Q_D(ctkActionsWidget);
  d->SortFilterActionsProxyModel->setMenuActionsVisible(show);
}

//-----------------------------------------------------------------------------
bool ctkActionsWidget::areMenuActionsVisible()const
{
  Q_D(const ctkActionsWidget);
  return d->SortFilterActionsProxyModel->areMenuActionsVisible();
}

//-----------------------------------------------------------------------------
void ctkActionsWidget::updateAction()
{
  Q_D(ctkActionsWidget);
  QAction* action = qobject_cast<QAction*>(this->sender());
  Q_ASSERT(action);
  QModelIndexList foundActions =
    d->ActionsModel->match(d->ActionsModel->index(0,0),
    Qt::UserRole + 1, qVariantFromValue(qobject_cast<QObject*>(action)),
    -1, Qt::MatchExactly | Qt::MatchRecursive);
  Q_ASSERT(foundActions.size());
  foreach (QModelIndex actionIndex, foundActions)
    {
    QModelIndex parentIndex = actionIndex.parent();
    QStandardItem* parent = parentIndex.isValid()
      ? d->ActionsModel->itemFromIndex(parentIndex)
      : d->ActionsModel->invisibleRootItem();
    int actionRow = actionIndex.row();
    Q_ASSERT(actionRow >= 0);
    QList<QStandardItem*> actionItems;
    for(int i = 0; i < 4; ++i)
      {
      actionItems << parent->child(actionRow, i);
      }
    d->updateItems(actionItems, action);
    }
}

//-----------------------------------------------------------------------------
QTreeView* ctkActionsWidget::view()const
{
  Q_D(const ctkActionsWidget);
  return d->ActionsTreeView;
}

//-----------------------------------------------------------------------------
class ctkSortFilterActionsProxyModelPrivate
{
protected:
  ctkSortFilterActionsProxyModel* const q_ptr;
public:
  ctkSortFilterActionsProxyModelPrivate(ctkSortFilterActionsProxyModel& object);
  bool                   ActionsWithNoShortcutVisible;
  bool                   MenuActionsVisible;
};

//-----------------------------------------------------------------------------
ctkSortFilterActionsProxyModelPrivate::ctkSortFilterActionsProxyModelPrivate(ctkSortFilterActionsProxyModel& object)
  :q_ptr(&object)
{
  this->ActionsWithNoShortcutVisible = true;
  this->MenuActionsVisible = true;
}

//-----------------------------------------------------------------------------
ctkSortFilterActionsProxyModel::ctkSortFilterActionsProxyModel(QObject* parentObject)
 :QSortFilterProxyModel(parentObject)
  , d_ptr(new ctkSortFilterActionsProxyModelPrivate(*this))
{
}

//-----------------------------------------------------------------------------
void ctkSortFilterActionsProxyModel::setActionsWithNoShortcutVisible(bool visible)
{
  Q_D(ctkSortFilterActionsProxyModel);
  d->ActionsWithNoShortcutVisible = visible;
  this->invalidateFilter();
}

//-----------------------------------------------------------------------------
bool ctkSortFilterActionsProxyModel::areActionsWithNoShortcutVisible()const
{
  Q_D(const ctkSortFilterActionsProxyModel);
  return d->ActionsWithNoShortcutVisible;
}

//-----------------------------------------------------------------------------
void ctkSortFilterActionsProxyModel::setMenuActionsVisible(bool visible)
{
  Q_D(ctkSortFilterActionsProxyModel);
  d->MenuActionsVisible = visible;
  this->invalidateFilter();
}

//-----------------------------------------------------------------------------
bool ctkSortFilterActionsProxyModel::areMenuActionsVisible()const
{
  Q_D(const ctkSortFilterActionsProxyModel);
  return d->MenuActionsVisible;
}

//-----------------------------------------------------------------------------
bool ctkSortFilterActionsProxyModel::filterAcceptsRow(int source_row, const QModelIndex & source_parent) const
{
  Q_D(const ctkSortFilterActionsProxyModel);
  QStandardItem* shortcutItem = qobject_cast<QStandardItemModel*>(
    this->sourceModel())->itemFromIndex(
      source_parent.child(source_row, ctkActionsWidget::ShortcutColumn));
  QAction* action = shortcutItem ?
    qobject_cast<QAction*>(shortcutItem->data().value<QObject*>()) : 0;
  if (!action)
    {
    return true;
    }
  if (action->isSeparator())
    {
    return false;
    }
  if (action->text().isEmpty())
    {// not sure what the empty text actions are
    return false;
    }
  if (!d->ActionsWithNoShortcutVisible && shortcutItem->text().isEmpty())
    {
    return false;
    }
  if (!d->MenuActionsVisible && action->menu())
    {
    return false;
    }
  return true;
}

//---------------------------------------------------------------------------
void ctkRichTextItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem & option,
                                    const QModelIndex &index) const
{
  QStyleOptionViewItemV4 options = option;
  initStyleOption(&options, index);
  if (! Qt::mightBeRichText(options.text))
    {
    this->QStyledItemDelegate::paint(painter, option, index);
    return;
    }

  painter->save();

  QTextDocument doc;
  doc.setHtml(options.text);

  /* Call this to get the focus rect and selection background. */
  options.text = "";
  options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter, options.widget);

  /* Draw using our rich text document. */
  painter->translate(options.rect.left(), options.rect.top());
  QRect clip(0, 0, options.rect.width(), options.rect.height());
  doc.drawContents(painter, clip);

  painter->restore();
}

//---------------------------------------------------------------------------
QSize ctkRichTextItemDelegate::sizeHint(const QStyleOptionViewItem & option,
                                        const QModelIndex & index)const
{
  QStyleOptionViewItemV4 options = option;
  initStyleOption(&options, index);
  if (! Qt::mightBeRichText(options.text))
    {
    return this->QStyledItemDelegate::sizeHint(option, index);;
    }

  QTextDocument doc;
  doc.setHtml(options.text);
  doc.setTextWidth(options.rect.width());
  return QSize(doc.idealWidth(), doc.size().height());
}
