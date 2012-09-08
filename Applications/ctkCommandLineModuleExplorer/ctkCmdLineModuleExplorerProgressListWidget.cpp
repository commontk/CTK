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

#include "ctkCmdLineModuleExplorerProgressListWidget.h"
#include "ctkCmdLineModuleExplorerProgressWidget.h"

#include "ctkCmdLineModuleFrontend.h"
#include "ctkCmdLineModuleReference.h"
#include "ctkCmdLineModuleDescription.h"

#include <QVBoxLayout>

ctkCmdLineModuleExplorerProgressListWidget::ctkCmdLineModuleExplorerProgressListWidget(QWidget *parent)
  : QWidget(parent)
  , CurrentWidget(NULL)
{
  QVBoxLayout* progressLayout = new QVBoxLayout();
  progressLayout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(progressLayout);
}

void ctkCmdLineModuleExplorerProgressListWidget::addProgressWidget(ctkCmdLineModuleFrontend *frontend,
                                                                   const ctkCmdLineModuleFuture &future)
{
  ctkCmdLineModuleExplorerProgressWidget* progressWidget = FrontendToProgressWidgetMap[frontend];
  if (progressWidget == NULL)
  {
    progressWidget = new ctkCmdLineModuleExplorerProgressWidget();
    FrontendToProgressWidgetMap[frontend] = progressWidget;
    ProgressWidgetToFrontendMap[progressWidget] = frontend;

    connect(progressWidget, SIGNAL(clicked()), SLOT(progressWidgetClicked()));
    connect(progressWidget, SIGNAL(destroyed(QObject*)), SLOT(progressWidgetDestroyed(QObject*)));

    this->layout()->addWidget(progressWidget);
  }

  progressWidget->setHighlightStyle(false);
  progressWidget->setTitle(frontend->moduleReference().description().title());
  progressWidget->setFuture(future);
}


void ctkCmdLineModuleExplorerProgressListWidget::progressWidgetDestroyed(QObject* widget)
{
  ctkCmdLineModuleExplorerProgressWidget* progressWidget = static_cast<ctkCmdLineModuleExplorerProgressWidget*>(widget);
  if (CurrentWidget == progressWidget)
  {
    CurrentWidget = NULL;
  }
  ctkCmdLineModuleFrontend* frontend = ProgressWidgetToFrontendMap.take(progressWidget);
  FrontendToProgressWidgetMap.remove(frontend);
}


void ctkCmdLineModuleExplorerProgressListWidget::removeProgressWidget(ctkCmdLineModuleFrontend *frontend)
{
  if (FrontendToProgressWidgetMap.contains(frontend))
  {
    FrontendToProgressWidgetMap[frontend]->deleteLater();
  }
}

void ctkCmdLineModuleExplorerProgressListWidget::setCurrentProgressWidget(ctkCmdLineModuleFrontend *frontend)
{
  if (frontend == NULL && CurrentWidget != NULL)
  {
    CurrentWidget->setHighlightStyle(false);
    CurrentWidget = NULL;
    return;
  }

  if (CurrentWidget != NULL)
  {
    CurrentWidget->setHighlightStyle(false);
    CurrentWidget = NULL;
  }

  ctkCmdLineModuleExplorerProgressWidget* progressWidget = FrontendToProgressWidgetMap[frontend];
  if (progressWidget)
  {
    progressWidget->setHighlightStyle(true);
    CurrentWidget = progressWidget;
  }
}

void ctkCmdLineModuleExplorerProgressListWidget::progressWidgetClicked()
{
  ctkCmdLineModuleExplorerProgressWidget* progressWidget =
      static_cast<ctkCmdLineModuleExplorerProgressWidget*>(this->sender());

  ctkCmdLineModuleFrontend* frontend = ProgressWidgetToFrontendMap[progressWidget];
  Q_ASSERT(frontend);

  this->setCurrentProgressWidget(frontend);
  emit progressWidgetClicked(frontend);
}

void ctkCmdLineModuleExplorerProgressListWidget::clearList()
{
  QList<ctkCmdLineModuleExplorerProgressWidget*> widgetsToRemove;
  QHashIterator<ctkCmdLineModuleFrontend*, ctkCmdLineModuleExplorerProgressWidget*> iter(FrontendToProgressWidgetMap);
  while (iter.hasNext())
  {
    iter.next();
    if (!iter.key()->isRunning())
    {
      widgetsToRemove.push_back(iter.value());
    }
  }

  foreach(ctkCmdLineModuleExplorerProgressWidget* widget, widgetsToRemove)
  {
    widget->deleteLater();
  }
}
