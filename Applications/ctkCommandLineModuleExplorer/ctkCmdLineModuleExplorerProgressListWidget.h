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

#ifndef CTKCMDLINEMODULEEXPLORERPROGRESSLISTWIDGET_H
#define CTKCMDLINEMODULEEXPLORERPROGRESSLISTWIDGET_H

#include <QWidget>
#include <QHash>

class ctkCmdLineModuleFrontend;
class ctkCmdLineModuleFuture;
class ctkCmdLineModuleExplorerProgressWidget;

class ctkCmdLineModuleExplorerProgressListWidget : public QWidget
{
  Q_OBJECT

public:
  explicit ctkCmdLineModuleExplorerProgressListWidget(QWidget *parent = 0);
  
  void addProgressWidget(ctkCmdLineModuleFrontend* frontend, const ctkCmdLineModuleFuture& future);

signals:

  void progressWidgetClicked(ctkCmdLineModuleFrontend* frontend);

public slots:

  void removeProgressWidget(ctkCmdLineModuleFrontend* frontend);

  void setCurrentProgressWidget(ctkCmdLineModuleFrontend* frontend);

  void progressWidgetClicked();

  void clearList();
  
private slots:

  void progressWidgetDestroyed(QObject* progressWidget);

private:

  QHash<ctkCmdLineModuleFrontend*, ctkCmdLineModuleExplorerProgressWidget*> FrontendToProgressWidgetMap;
  QHash<ctkCmdLineModuleExplorerProgressWidget*, ctkCmdLineModuleFrontend*> ProgressWidgetToFrontendMap;

  ctkCmdLineModuleExplorerProgressWidget* CurrentWidget;
};

#endif // CTKCMDLINEMODULEEXPLORERPROGRESSLISTWIDGET_H
