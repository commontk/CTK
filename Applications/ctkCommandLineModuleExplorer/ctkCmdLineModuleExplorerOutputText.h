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

#ifndef CTKCMDLINEMODULEEXPLOREROUTPUTTEXT_H
#define CTKCMDLINEMODULEEXPLOREROUTPUTTEXT_H


#include <QTextEdit>

class ctkCmdLineModuleFrontend;
class ctkCmdLineModuleFutureWatcher;

class ctkCmdLineModuleExplorerOutputText : public QTextEdit
{
  Q_OBJECT

public:

  ctkCmdLineModuleExplorerOutputText(QWidget* parent = 0);
  ~ctkCmdLineModuleExplorerOutputText();

public Q_SLOTS:

  void setActiveFrontend(ctkCmdLineModuleFrontend* frontend);

  void frontendRemoved(ctkCmdLineModuleFrontend* frontend);

private Q_SLOTS:

  void frontendStarted();

  void outputDataReady();
  void errorDataReady();

private:

  ctkCmdLineModuleFutureWatcher* CurrentWatcher;
  ctkCmdLineModuleFrontend* CurrentFrontend;
  QHash<ctkCmdLineModuleFrontend*,ctkCmdLineModuleFutureWatcher*> FrontendToWatcherMap;
  QHash<ctkCmdLineModuleFrontend*,QString> FrontendToOutputMap;
};

#endif // CTKCMDLINEMODULEEXPLOREROUTPUTTEXT_H
