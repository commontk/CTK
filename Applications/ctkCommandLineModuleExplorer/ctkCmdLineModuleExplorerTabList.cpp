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

#include "ctkCmdLineModuleExplorerTabList.h"

#include <ctkCmdLineModuleFrontend.h>
#include <ctkCmdLineModuleFrontendFactory.h>
#include <ctkCmdLineModuleDescription.h>
#include <ctkCmdLineModuleManager.h>
#include <ctkCmdLineModuleFuture.h>

#include <QTabWidget>
#include <QModelIndex>
#include <QMessageBox>
#include <QApplication>

Q_DECLARE_METATYPE(ctkCmdLineModuleFrontendFactory*)

ctkCmdLineModuleExplorerTabList::ctkCmdLineModuleExplorerTabList(QTabWidget *tabWidget)
  : TabWidget(tabWidget)
{
  Q_ASSERT(TabWidget != NULL);

  connect(TabWidget, SIGNAL(currentChanged(int)), SLOT(tabIndexChanged(int)));
  connect(TabWidget, SIGNAL(tabCloseRequested(int)), SLOT(tabCloseRequested(int)));
}

ctkCmdLineModuleExplorerTabList::~ctkCmdLineModuleExplorerTabList()
{
  qDeleteAll(this->TabIndexToFrontend);
}

ctkCmdLineModuleFrontend* ctkCmdLineModuleExplorerTabList::activeTab() const
{
  int index = this->TabWidget->currentIndex();
  if (index < 0) return NULL;
  return this->TabIndexToFrontend[index];
}

QList<ctkCmdLineModuleFrontend *> ctkCmdLineModuleExplorerTabList::tabs() const
{
  return this->TabIndexToFrontend;
}

void ctkCmdLineModuleExplorerTabList::setActiveTab(ctkCmdLineModuleFrontend *frontend)
{
  this->TabWidget->setCurrentIndex(this->TabIndexToFrontend.indexOf(frontend));
}

void ctkCmdLineModuleExplorerTabList::addTab(ctkCmdLineModuleFrontend* moduleFrontend)
{
  QWidget* widget = qobject_cast<QWidget*>(moduleFrontend->guiHandle());
  this->TabIndexToFrontend.push_back(moduleFrontend);
  int index = this->TabWidget->addTab(widget, moduleFrontend->moduleReference().description().title());
  this->TabWidget->setCurrentIndex(index);
}

void ctkCmdLineModuleExplorerTabList::tabIndexChanged(int index)
{
  if (index < 0)
  {
    emit tabActivated(NULL);
    return;
  }
  emit tabActivated(this->TabIndexToFrontend[index]);
}

void ctkCmdLineModuleExplorerTabList::tabCloseRequested(int index)
{
  ctkCmdLineModuleFrontend* frontend = this->TabIndexToFrontend[index];
  bool removeTab = false;

  if (frontend->isRunning())
  {
    if (frontend->future().canCancel())
    {
      QMessageBox::StandardButton button =
          QMessageBox::warning(QApplication::topLevelWidgets().front(),
                               "Closing a running module",
                               "The module '" + frontend->moduleReference().description().title() + "' is still running.\n"
                               "Closing the tab will cancel the current computation.",
                               QMessageBox::Ok | QMessageBox::Cancel);
      if (button == QMessageBox::Ok)
      {
        frontend->future().cancel();
        removeTab = true;
      }
    }
    else
    {
      QMessageBox::information(QApplication::topLevelWidgets().front(),
                               "Closing not possible",
                               "The module '" + frontend->moduleReference().description().title() + "' is still running "
                               "and does not support being canceled.");
    }
  }
  else
  {
    removeTab = true;
  }

  if (removeTab)
  {
    this->TabIndexToFrontend.removeAt(index);
    this->TabWidget->removeTab(index);
    emit this->tabClosed(frontend);
    delete frontend;
  }
}
