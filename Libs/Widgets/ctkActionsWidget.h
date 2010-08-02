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

#ifndef __ctkActionsWidget_h
#define __ctkActionsWidget_h

// Qt includes
#include <QStyledItemDelegate>
#include <QSortFilterProxyModel>
#include <QWidget>

// CTK includes
#include "ctkPimpl.h"
#include "CTKWidgetsExport.h"
class ctkActionsWidgetPrivate;
class ctkSortFilterActionsProxyModelPrivate;

class QAction;
class QStandardItemModel;
class QStandardItem;
class QTreeView;

//---------------------------------------------------------------------------
class CTK_WIDGETS_EXPORT ctkActionsWidget : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(bool actionsWithNoShortcutVisible READ areActionsWithNoShortcutVisible WRITE setActionsWithNoShortcutVisible)
  Q_PROPERTY(bool menuActionsVisible READ areMenuActionsVisible WRITE setMenuActionsVisible)
public:
  explicit ctkActionsWidget(QWidget* parent = 0);

  /// Add an action into a specified group (or at top level if group is empty)
  /// An action can be added multiple times (in a different group). Once added,
  /// ctkActionsWidget listens to the QAction and updates the action properties
  /// TODO: check that the action hasn't been already added into a group
  void addAction(QAction* action, const QString& group = QString());

  /// Convenient function to add a list of action at once
  void addActions(QList<QAction*> actions, const QString& group = QString());

  /// Remove all the actions and groups
  void clear();

  /// Return a pointer on a group item (you probably have no use for it)
  QStandardItem* groupItem(const QString& category);

  /// Show/hide QActions that have an empty shortcut
  void setActionsWithNoShortcutVisible(bool show);
  bool areActionsWithNoShortcutVisible()const;

  /// Show/hide QActions that have an empty shortcut
  void setMenuActionsVisible(bool show);
  bool areMenuActionsVisible()const;

  QStandardItemModel* model()const;
  QTreeView* view()const;

protected slots:
  void updateAction();

protected:
  enum ActionColumn{
    NameColumn = 0,
    ShortcutColumn,
    ContextColumn,
    DetailsColumn
  };
private:
  friend class ctkSortFilterActionsProxyModel;
  CTK_DECLARE_PRIVATE(ctkActionsWidget);
};

//---------------------------------------------------------------------------
class ctkSortFilterActionsProxyModel : public QSortFilterProxyModel
{
  Q_OBJECT
public:
  ctkSortFilterActionsProxyModel(QObject* parent=0);

  void setActionsWithNoShortcutVisible(bool);
  bool areActionsWithNoShortcutVisible()const;

  void setMenuActionsVisible(bool);
  bool areMenuActionsVisible()const;

protected:
  bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;
private:
  CTK_DECLARE_PRIVATE(ctkSortFilterActionsProxyModel);
};

//---------------------------------------------------------------------------
class ctkRichTextItemDelegate : public QStyledItemDelegate
{
  Q_OBJECT
protected:
  virtual void paint(QPainter * painter, const QStyleOptionViewItem & option,
             const QModelIndex & index) const;
  virtual QSize sizeHint(const QStyleOptionViewItem & option,
                         const QModelIndex & index)const;
};

#endif
