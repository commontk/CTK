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

#ifndef __ctkActionsWidget_h
#define __ctkActionsWidget_h

// Qt includes
#include <QStyledItemDelegate>
#include <QSortFilterProxyModel>
#include <QWidget>

// CTK includes
#include "ctkPimpl.h"
#include "ctkWidgetsExport.h"
class ctkActionsWidgetPrivate;
class ctkSortFilterActionsProxyModelPrivate;

class QAction;
class QStandardItemModel;
class QStandardItem;
class QTreeView;

/// \ingroup Widgets
/// ctkActionsWidget presents a list of QAction to the user. The QAction's are
/// displayed in a multi column tree view. The columns contain the QAction's
/// text, shortcut, context and tooltip in that order.
/// The typical use is to show what shortcuts are associated to what commands
/// in an application.
/// ctkActionsWidget internally uses a QStandardItemModel where each item data
/// (QStandardItem::data) contain a pointer to the QAction.
/// QActions can optionally be ordered by group
/// TODO: Add "hide empty group" property to hide empty groups
class CTK_WIDGETS_EXPORT ctkActionsWidget : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(bool actionsWithNoShortcutVisible READ areActionsWithNoShortcutVisible WRITE setActionsWithNoShortcutVisible)
  Q_PROPERTY(bool menuActionsVisible READ areMenuActionsVisible WRITE setMenuActionsVisible)
  /// This property controls whether the actions are sorted by column (>=0) or
  /// not (-1). Not sorted by default.
  /// \sa sortColumn(), setSortColumn()
  Q_PROPERTY(int sortColumn READ sortColumn WRITE setSortColumn)
public:
  explicit ctkActionsWidget(QWidget* parent = 0);
  virtual ~ctkActionsWidget();

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

  /// If true, shows QActions that have an empty shortcut, otherwise hide them.
  /// True by default
  void setActionsWithNoShortcutVisible(bool show);
  bool areActionsWithNoShortcutVisible()const;

  /// If true, shows QMenus, otherwise hide them.
  /// True by default
  void setMenuActionsVisible(bool show);
  bool areMenuActionsVisible()const;

  /// Set the sortColumn property value.
  /// \sa sortColumn
  void setSortColumn(int column);
  /// Return the sortColumn property value.
  /// \sa sortColumn
  int sortColumn()const;

  /// Return the unsorted/unfiltered model of all the actions
  QStandardItemModel* model()const;

  /// return the view used to display the action model
  QTreeView* view()const;

protected Q_SLOTS:
  void updateAction();

protected:
  enum ActionColumn{
    NameColumn = 0,
    ShortcutColumn,
    ContextColumn,
    DetailsColumn
  };
protected:
  QScopedPointer<ctkActionsWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkActionsWidget);
  Q_DISABLE_COPY(ctkActionsWidget);
  friend class ctkSortFilterActionsProxyModel;
};

/// \ingroup Widgets
/// ctkSortFilterActionsProxyModel is a utility class that is needed by
/// ctkActionsWidget. It's a specialization of a QSortFilterProxyModel and
/// control what action is visible to the tree view.
class ctkSortFilterActionsProxyModel : public QSortFilterProxyModel
{
  Q_OBJECT
public:
  explicit ctkSortFilterActionsProxyModel(QObject* parent=0);
  virtual ~ctkSortFilterActionsProxyModel();

  void setActionsWithNoShortcutVisible(bool);
  bool areActionsWithNoShortcutVisible()const;

  void setMenuActionsVisible(bool);
  bool areMenuActionsVisible()const;

protected:
  bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;
  QScopedPointer<ctkSortFilterActionsProxyModelPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkSortFilterActionsProxyModel);
  Q_DISABLE_COPY(ctkSortFilterActionsProxyModel);
};

/// \ingroup Widgets
/// ctkRichTextItemDelegate is a utility class that is needed by
/// ctkActionsWidget. It control how QAction tree items are displayed when the
/// text is written in HTML.
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
