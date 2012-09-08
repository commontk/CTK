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

#ifndef CTKCMDLINEMODULEEXPLORERTABLIST_H
#define CTKCMDLINEMODULEEXPLORERTABLIST_H

class ctkCmdLineModuleFrontend;
class ctkCmdLineModuleManager;

#include <QObject>
#include <QHash>

class QTabWidget;
class QModelIndex;

/**
 * \class ctkCmdLineModuleExplorerTabList
 * \brief Example application tab list
 */
class ctkCmdLineModuleExplorerTabList : public QObject
{
  Q_OBJECT

public:

  ctkCmdLineModuleExplorerTabList(QTabWidget* tabWidget);
  ~ctkCmdLineModuleExplorerTabList();

  ctkCmdLineModuleFrontend* activeTab() const;

  QList<ctkCmdLineModuleFrontend*> tabs() const;

  Q_SLOT void setActiveTab(ctkCmdLineModuleFrontend* frontend);

  Q_SLOT void addTab(ctkCmdLineModuleFrontend* frontend);

  Q_SIGNAL void tabActivated(ctkCmdLineModuleFrontend* module);
  Q_SIGNAL void tabClosed(ctkCmdLineModuleFrontend* module);

private:

  Q_SLOT void tabIndexChanged(int index);
  Q_SLOT void tabCloseRequested(int index);

private:

  QTabWidget* TabWidget;
  QList<ctkCmdLineModuleFrontend*> TabIndexToFrontend;
};

#endif // CTKCMDLINEMODULEEXPLORERTABLIST_H
