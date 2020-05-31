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
#include <QDebug>
#include <QHeaderView>
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
  void setupHeaders();
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
  this->setupHeaders();

  this->SortFilterActionsProxyModel = new ctkSortFilterActionsProxyModel(q);
  this->SortFilterActionsProxyModel->setSourceModel(this->ActionsModel);

  this->ActionsTreeView = new QTreeView(q);
  QVBoxLayout* layout = new QVBoxLayout(q);
  layout->addWidget(this->ActionsTreeView);
  layout->setContentsMargins(0,0,0,0);
  q->setLayout(layout);
  this->ActionsTreeView->setModel(this->SortFilterActionsProxyModel);
  this->ActionsTreeView->setAlternatingRowColors(true);
  //this->ActionsTreeView->setItemDelegate(new ctkRichTextItemDelegate);
}

//-----------------------------------------------------------------------------
void ctkActionsWidgetPrivate::setupHeaders()
{
  this->ActionsModel->setColumnCount(4);
  QStringList headers;
  headers << "Action" << "Shortcut(s)" << "Context" << "Details";
  this->ActionsModel->setHorizontalHeaderLabels(headers);
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
  QStringList shortcuts;
  foreach(const QKeySequence& keySequence, action->shortcuts())
    {
    shortcuts << keySequence.toString(QKeySequence::NativeText);
    }
  items[ctkActionsWidget::ShortcutColumn]->setText(
    shortcuts.join("; "));
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
    #if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
    item->setData(QVariant::fromValue(qobject_cast<QObject*>(action)));
    #else
    item->setData(qVariantFromValue(qobject_cast<QObject*>(action)));
    #endif
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    actionItems << item;
    }

  d->updateItems(actionItems, action);

  bool expandGroupItem = (actionGroupItem->rowCount() == 0);
  actionGroupItem->appendRow(actionItems);
  // if the group didn't exist yet or was empty, then open/expand it
  // automatcally to show its contents. If the group was not empty, then let
  // it as is (maybe the user closed/collapsed it for a good reason...
  if (expandGroupItem)
    {
    d->ActionsTreeView->expand(
      d->SortFilterActionsProxyModel->mapFromSource(d->ActionsModel->indexFromItem(actionGroupItem)));
    }
  d->ActionsTreeView->resizeColumnToContents(ctkActionsWidget::NameColumn);
  d->ActionsTreeView->resizeColumnToContents(ctkActionsWidget::DetailsColumn);
  connect(action, SIGNAL(changed()), this, SLOT(updateAction()));
}

//-----------------------------------------------------------------------------
void ctkActionsWidget::addActions(QList<QAction*> actions, const QString& group)
{
  Q_D(ctkActionsWidget);
  bool wasSortinEnabled = d->ActionsTreeView->isSortingEnabled();
  d->ActionsTreeView->setSortingEnabled(false);
  foreach(QAction* action, actions)
    {
    this->addAction(action, group);
    }
  d->ActionsTreeView->setSortingEnabled(wasSortinEnabled);
}

//-----------------------------------------------------------------------------
void ctkActionsWidget::clear()
{
  Q_D(ctkActionsWidget);
  d->ActionsModel->clear();
  d->setupHeaders();
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
void ctkActionsWidget::setSortColumn(int column)
{
  Q_D(ctkActionsWidget);
  d->ActionsTreeView->sortByColumn(column, Qt::AscendingOrder);
  d->ActionsTreeView->setSortingEnabled(column != -1);
}

//-----------------------------------------------------------------------------
int ctkActionsWidget::sortColumn()const
{
  Q_D(const ctkActionsWidget);
  return d->ActionsTreeView->isSortingEnabled() ?
    d->ActionsTreeView->header()->sortIndicatorSection() : -1;
}

//-----------------------------------------------------------------------------
void ctkActionsWidget::updateAction()
{
  Q_D(ctkActionsWidget);
  QAction* action = qobject_cast<QAction*>(this->sender());
  Q_ASSERT(action);
  #if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
  QVariant variant = QVariant::fromValue(qobject_cast<QObject*>(action));
  #else
  QVariant variant = qVariantFromValue(qobject_cast<QObject*>(action));
  #endif
  QModelIndexList foundActions =
    d->ActionsModel->match(d->ActionsModel->index(0,0),
    Qt::UserRole + 1, variant,
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
ctkSortFilterActionsProxyModel::~ctkSortFilterActionsProxyModel()
{
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
  QModelIndex shortcutIndex = this->sourceModel()->index(source_row, ctkActionsWidget::ShortcutColumn, source_parent);
  QStandardItem* shortcutItem = qobject_cast<QStandardItemModel*>(
    this->sourceModel())->itemFromIndex(shortcutIndex);
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
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
  QStyleOptionViewItemV4 options = option;
#else
  QStyleOptionViewItem options = option;
#endif
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
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
  QStyleOptionViewItemV4 options = option;
#else
  QStyleOptionViewItem options = option;
#endif
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
