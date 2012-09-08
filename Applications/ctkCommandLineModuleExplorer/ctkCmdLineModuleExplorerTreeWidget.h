/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

#ifndef CTKCMDLINEMODULEEXPLORERTREEWIDGET_H
#define CTKCMDLINEMODULEEXPLORERTREEWIDGET_H

#include <ctkCmdLineModuleReference.h>

#include <QSortFilterProxyModel>
#include <QTreeView>

class ctkCmdLineModuleFrontend;
struct ctkCmdLineModuleFrontendFactory;
class ctkCmdLineModuleExplorerShowXmlAction;

class QStandardItem;
class QStandardItemModel;

class ModuleSortFilterProxyModel : public QSortFilterProxyModel
{
  Q_OBJECT

public:

  ModuleSortFilterProxyModel(QObject *parent = 0);

protected:

  bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

  bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
};

/**
 * \class ctkCmdLineModuleExplorerTreeWidget
 * \brief Example application tree widget.
 */
class ctkCmdLineModuleExplorerTreeWidget : public QTreeView
{
  Q_OBJECT

public:

  explicit ctkCmdLineModuleExplorerTreeWidget(QWidget* parent = 0);

  void setModuleFrontendFactories(const QList<ctkCmdLineModuleFrontendFactory*>& frontendFactories, ctkCmdLineModuleFrontendFactory *defaultFactory);

  Q_SLOT void addModuleItem(const ctkCmdLineModuleReference& moduleRef);
  Q_SLOT void removeModuleItem(const ctkCmdLineModuleReference& moduleRef);

  Q_SIGNAL void moduleFrontendCreated(ctkCmdLineModuleFrontend* moduleFrontend);

  Q_SLOT void setFilter(const QString& filter);

protected:

  void contextMenuEvent(QContextMenuEvent* event);

private:

  ctkCmdLineModuleFrontend* createFrontend(const ctkCmdLineModuleReference &moduleRef,
                                           ctkCmdLineModuleFrontendFactory* frontendFactory);

  Q_SLOT void moduleDoubleClicked(const QModelIndex& index);
  Q_SLOT void frontendFactoryActionTriggered();

  static QString CATEGORY_UNKNOWN;

  QMenu* ContextMenu;
  QMenu* ShowFrontendMenu;

  ctkCmdLineModuleExplorerShowXmlAction* ShowXmlAction;

  ctkCmdLineModuleReference ContextReference;
  QList<ctkCmdLineModuleFrontendFactory*> FrontendFactories;
  ctkCmdLineModuleFrontendFactory* DefaultFrontendFactory;
  QHash<QString, QStandardItem*> TreeWidgetCategories;
  QHash<ctkCmdLineModuleReference, QStandardItem*> TreeWidgetItems;
  QHash<QAction*, ctkCmdLineModuleFrontendFactory*> ActionsToFrontendFactoryMap;

  QStandardItemModel* TreeModel;
  QSortFilterProxyModel* FilterProxyModel;
};

#endif // CTKCMDLINEMODULEEXPLORERTREEWIDGET_H
