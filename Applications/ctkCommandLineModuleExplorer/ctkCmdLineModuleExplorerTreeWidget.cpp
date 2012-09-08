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

#include "ctkCmdLineModuleExplorerTreeWidget.h"
#include "ctkCmdLineModuleExplorerShowXmlAction.h"

#include <ctkCmdLineModuleFrontend.h>
#include <ctkCmdLineModuleBackend.h>
#include <ctkCmdLineModuleFrontendFactory.h>
#include <ctkCmdLineModuleDescription.h>
#include <ctkCmdLineModuleXmlException.h>

#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QContextMenuEvent>
#include <QMenu>
#include <QDebug>
#include <QUrl>
#include <QApplication>
#include <QMessageBox>

QString ctkCmdLineModuleExplorerTreeWidget::CATEGORY_UNKNOWN = "Uncategorized";

class ctkCmdLineModuleTreeWidgetItem : public QStandardItem
{
public:

  ctkCmdLineModuleTreeWidgetItem(const ctkCmdLineModuleReference& moduleRef)
    : QStandardItem()
    , ModuleRef(moduleRef)
  {
    QString title;
    try
    {
      title = ModuleRef.description().title();
    }
    catch (const ctkCmdLineModuleXmlException&)
    {
      title = ModuleRef.location().toString();
    }

    this->setText(title + " [" + ModuleRef.backend()->name() + "]");
    this->setData(QVariant::fromValue(ModuleRef));
    QString toolTip = ModuleRef.location().toString();
    if (!ModuleRef.xmlValidationErrorString().isEmpty())
    {
      this->setIcon(QApplication::style()->standardIcon(QStyle::SP_MessageBoxWarning));
      toolTip += "\n\nWarning:\n\n" + ModuleRef.xmlValidationErrorString();
    }
    this->setToolTip(toolTip);
  }

  ctkCmdLineModuleReference moduleReference() const
  {
    return ModuleRef;
  }

private:

  ctkCmdLineModuleReference ModuleRef;

};

ctkCmdLineModuleExplorerTreeWidget::ctkCmdLineModuleExplorerTreeWidget(QWidget *parent)
  : QTreeView(parent)
  , DefaultFrontendFactory(NULL)
{
  this->ContextMenu = new QMenu(this);
  this->ShowFrontendMenu = this->ContextMenu->addMenu("Create Frontend");

  this->ShowXmlAction = new ctkCmdLineModuleExplorerShowXmlAction(this);
  this->ContextMenu->addAction(ShowXmlAction);

  connect(this, SIGNAL(doubleClicked(QModelIndex)), SLOT(moduleDoubleClicked(QModelIndex)));

  TreeModel = new QStandardItemModel(this);

  FilterProxyModel = new ModuleSortFilterProxyModel(this);
  FilterProxyModel->setSourceModel(TreeModel);
  FilterProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
  FilterProxyModel->setDynamicSortFilter(true);
  this->setModel(FilterProxyModel);
}

void ctkCmdLineModuleExplorerTreeWidget::setModuleFrontendFactories(const QList<ctkCmdLineModuleFrontendFactory *> &frontendFactories,
                                                                    ctkCmdLineModuleFrontendFactory* defaultFactory)
{
  this->FrontendFactories = frontendFactories;
  this->DefaultFrontendFactory = defaultFactory;

  this->ShowFrontendMenu->clear();
  foreach(ctkCmdLineModuleFrontendFactory* factory, this->FrontendFactories)
  {
    QAction* action = this->ShowFrontendMenu->addAction(factory->name(), this, SLOT(frontendFactoryActionTriggered()));
    this->ActionsToFrontendFactoryMap[action] = factory;
    action->setToolTip(factory->description());
  }
}

void ctkCmdLineModuleExplorerTreeWidget::addModuleItem(const ctkCmdLineModuleReference &moduleRef)
{
  QString categories;
  try
  {
    categories = moduleRef.description().category();
  }
  catch (const ctkCmdLineModuleXmlException&)
  {
    categories = CATEGORY_UNKNOWN;
  }

  if (categories.isEmpty())
  {
    categories = CATEGORY_UNKNOWN;
  }

  QString currentCategories;
  QStandardItem* oldRootItem = NULL;
  foreach (const QString& category, categories.split('.', QString::SkipEmptyParts))
  {
    currentCategories += (currentCategories.isEmpty() ? QString() : QString(".")) + category;
    QStandardItem* rootItem = TreeWidgetCategories[currentCategories];
    if (rootItem == NULL)
    {
      rootItem = new QStandardItem(category);
      TreeWidgetCategories[currentCategories] = rootItem;
      if (oldRootItem != NULL)
      {
        oldRootItem->appendRow(rootItem);
      }
      else
      {
        TreeModel->appendRow(rootItem);
      }
    }
    oldRootItem = rootItem;
  }

  QStandardItem* moduleItem =  new ctkCmdLineModuleTreeWidgetItem(moduleRef);
  TreeWidgetItems[moduleRef] = moduleItem;
  oldRootItem->appendRow(moduleItem);
}

void ctkCmdLineModuleExplorerTreeWidget::removeModuleItem(const ctkCmdLineModuleReference &moduleRef)
{
  QStandardItem* treeWidgetItem = TreeWidgetItems.take(moduleRef);
  if (treeWidgetItem == NULL) return;

  QString categories;
  try
  {
    categories = moduleRef.description().category();
  }
  catch (const ctkCmdLineModuleXmlException&)
  {
    categories = CATEGORY_UNKNOWN;
  }

  if (categories.isEmpty())
  {
    categories = CATEGORY_UNKNOWN;
  }

  QStringList categoryList = categories.split('.', QString::SkipEmptyParts);
  while (!categoryList.isEmpty())
  {
    QStandardItem* rootItem = TreeWidgetCategories[categoryList.join(".")];
    Q_ASSERT(rootItem);

    rootItem->removeRow(treeWidgetItem->row());

    if (rootItem->rowCount() == 0)
    {
      treeWidgetItem = rootItem;
      TreeWidgetCategories.remove(categoryList.join("."));
      categoryList.pop_back();
    }
    else
    {
      break;
    }
  }
  if (categoryList.isEmpty())
  {
    TreeModel->removeRow(treeWidgetItem->row());
  }
}

void ctkCmdLineModuleExplorerTreeWidget::contextMenuEvent(QContextMenuEvent *event)
{
  QModelIndex index = this->indexAt(this->viewport()->mapFromGlobal(event->globalPos()));
  if (index.isValid() && index.data(Qt::UserRole+1).isValid())
  {
    this->ContextReference = index.data(Qt::UserRole+1).value<ctkCmdLineModuleReference>();
    this->ShowXmlAction->setModuleReference(this->ContextReference);
    this->ContextMenu->exec(event->globalPos());
    event->accept();
  }
  else
  {
    this->ContextReference = ctkCmdLineModuleReference();
    event->ignore();
  }
}

void ctkCmdLineModuleExplorerTreeWidget::moduleDoubleClicked(const QModelIndex &index)
{
  if (this->DefaultFrontendFactory == NULL) return;

  QVariant data = index.data(Qt::UserRole+1);
  if (!data.isValid()) return;

  ctkCmdLineModuleReference moduleRef = data.value<ctkCmdLineModuleReference>();
  if (!moduleRef) return;

  this->createFrontend(moduleRef, this->DefaultFrontendFactory);
}

void ctkCmdLineModuleExplorerTreeWidget::frontendFactoryActionTriggered()
{
  ctkCmdLineModuleFrontendFactory* frontendFactory = this->ActionsToFrontendFactoryMap[static_cast<QAction*>(this->sender())];
  this->createFrontend(this->ContextReference, frontendFactory);
}


void ctkCmdLineModuleExplorerTreeWidget::setFilter(const QString &filter)
{
  this->FilterProxyModel->setFilterWildcard(filter);
}

ctkCmdLineModuleFrontend* ctkCmdLineModuleExplorerTreeWidget::createFrontend(const ctkCmdLineModuleReference &moduleRef,
                                                                             ctkCmdLineModuleFrontendFactory* frontendFactory)
{
  try
  {
    moduleRef.description();
    ctkCmdLineModuleFrontend* moduleFrontend = frontendFactory->create(moduleRef);
    emit moduleFrontendCreated(moduleFrontend);
    return moduleFrontend;
  }
  catch (const ctkException& e)
  {
    QMessageBox::information(this, "Frontend creation failed", "Creating a " + frontendFactory->name()
                             + " frontend failed:\n\n" + e.what());
    return NULL;
  }
}


bool ModuleSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
  QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

  QModelIndex childIndex = index.child(0, 0);
  if (childIndex.isValid())
  {
    int i = 0;
    bool accept = false;
    while(childIndex.isValid())
    {
      accept = this->filterAcceptsRow(childIndex.row(), index);
      if (accept) return true;

      childIndex = index.child(++i, 0);
    }
    return false;
  }

  return (sourceModel()->data(index).toString().contains(filterRegExp()));
}

bool ModuleSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QVariant l = (left.model() ? left.model()->data(left, this->sortRole()) : QVariant());
    QVariant r = (right.model() ? right.model()->data(right, this->sortRole()) : QVariant());
    return l.toString().compare(r.toString(), this->sortCaseSensitivity()) > 0;
}


ModuleSortFilterProxyModel::ModuleSortFilterProxyModel(QObject *parent)
  : QSortFilterProxyModel(parent)
{
}
