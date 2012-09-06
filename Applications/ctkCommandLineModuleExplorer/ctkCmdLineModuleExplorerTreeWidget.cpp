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

#include <ctkCmdLineModuleFrontend.h>
#include <ctkCmdLineModuleBackend.h>
#include <ctkCmdLineModuleFrontendFactory.h>
#include <ctkCmdLineModuleDescription.h>

#include <QContextMenuEvent>
#include <QMenu>
#include <QDebug>
#include <QUrl>
#include <QApplication>

QString ctkCmdLineModuleExplorerTreeWidget::CATEGORY_UNKNOWN = "Uncategorized";

class ctkCmdLineModuleTreeWidgetItem : public QTreeWidgetItem
{
public:

  static const int CmdLineModuleType = 1001;

  ctkCmdLineModuleTreeWidgetItem(const ctkCmdLineModuleReference& moduleRef)
    : QTreeWidgetItem(CmdLineModuleType), ModuleRef(moduleRef)
  {
    init();
  }

  ctkCmdLineModuleTreeWidgetItem(QTreeWidget* parent, const ctkCmdLineModuleReference& moduleRef)
    : QTreeWidgetItem(parent, CmdLineModuleType), ModuleRef(moduleRef)
  {
    init();
  }

  ctkCmdLineModuleTreeWidgetItem(QTreeWidgetItem* parent, const ctkCmdLineModuleReference& moduleRef)
    : QTreeWidgetItem(parent, CmdLineModuleType), ModuleRef(moduleRef)
  {
    init();
  }

  ctkCmdLineModuleReference moduleReference() const
  {
    return ModuleRef;
  }

private:

  void init()
  {
    this->setText(0, ModuleRef.description().title() + " [" + ModuleRef.backend()->name() + "]");
    this->setData(0, Qt::UserRole, QVariant::fromValue(ModuleRef));
    QString toolTip = ModuleRef.location().toString();
    if (!ModuleRef.xmlValidationErrorString().isEmpty())
    {
      this->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_MessageBoxWarning));
      toolTip += "\n\nWarning:\n\n" + ModuleRef.xmlValidationErrorString();
    }
    this->setToolTip(0, toolTip);
  }

  ctkCmdLineModuleReference ModuleRef;

};

ctkCmdLineModuleExplorerTreeWidget::ctkCmdLineModuleExplorerTreeWidget(QWidget *parent)
  : QTreeWidget(parent)
{
  this->ContextMenu = new QMenu(this);
  this->ShowFrontendMenu = this->ContextMenu->addMenu("Create Frontend");

  this->ContextMenu->addAction("Properties");

  connect(this, SIGNAL(doubleClicked(QModelIndex)), SLOT(moduleDoubleClicked(QModelIndex)));
}

void ctkCmdLineModuleExplorerTreeWidget::setModuleFrontendFactories(const QList<ctkCmdLineModuleFrontendFactory *> &frontendFactories)
{
  this->FrontendFactories = frontendFactories;

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
  QString category = moduleRef.description().category();

  QTreeWidgetItem* rootItem = NULL;

  if (category.isEmpty())
  {
    category = CATEGORY_UNKNOWN;
  }

  rootItem = TreeWidgetCategories[category];
  if (rootItem == NULL)
  {
    // lazily create the root item for the category
    rootItem = new QTreeWidgetItem(this);
    rootItem->setText(0, category);
    TreeWidgetCategories[category] = rootItem;
  }
  TreeWidgetItems[moduleRef] = new ctkCmdLineModuleTreeWidgetItem(rootItem, moduleRef);
}

void ctkCmdLineModuleExplorerTreeWidget::removeModuleItem(const ctkCmdLineModuleReference &moduleRef)
{
  QString category = moduleRef.description().category();
  if (category.isEmpty())
  {
    category = CATEGORY_UNKNOWN;
  }


  QTreeWidgetItem* treeWidgetItem = TreeWidgetItems.take(moduleRef);
  if (treeWidgetItem == NULL) return;

  this->removeItemWidget(treeWidgetItem, 0);
  delete treeWidgetItem;

  QTreeWidgetItem* rootItem = TreeWidgetCategories[category];
  if (rootItem && rootItem->childCount() == 0)
  {
    this->removeItemWidget(rootItem, 0);
    TreeWidgetCategories.remove(category);
    delete rootItem;
  }
}

void ctkCmdLineModuleExplorerTreeWidget::contextMenuEvent(QContextMenuEvent *event)
{
  QTreeWidgetItem* item = this->itemAt(this->viewport()->mapFromGlobal(event->globalPos()));
  if (item != NULL && item->type() == ctkCmdLineModuleTreeWidgetItem::CmdLineModuleType)
  {
    this->ContextReference = item->data(0, Qt::UserRole).value<ctkCmdLineModuleReference>();
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
  QVariant data = index.data(Qt::UserRole);
  if (!data.isValid()) return;

  ctkCmdLineModuleReference moduleRef = data.value<ctkCmdLineModuleReference>();
  if (!moduleRef) return;

  emit moduleDoubleClicked(moduleRef);
}

void ctkCmdLineModuleExplorerTreeWidget::frontendFactoryActionTriggered()
{
  ctkCmdLineModuleFrontendFactory* frontendFactory = this->ActionsToFrontendFactoryMap[static_cast<QAction*>(this->sender())];
  ctkCmdLineModuleFrontend* frontend = frontendFactory->create(this->ContextReference);
  emit moduleFrontendCreated(frontend);
}
