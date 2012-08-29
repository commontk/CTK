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

#include "ctkCmdLineModuleExplorerOutputText.h"

#include "ctkCmdLineModuleFrontend.h"
#include "ctkCmdLineModuleFuture.h"
#include "ctkCmdLineModuleFutureWatcher.h"

ctkCmdLineModuleExplorerOutputText::ctkCmdLineModuleExplorerOutputText(QWidget* parent)
  : QTextEdit(parent)
  , CurrentWatcher(NULL)
  , CurrentFrontend(NULL)
{
}

ctkCmdLineModuleExplorerOutputText::~ctkCmdLineModuleExplorerOutputText()
{
  qDeleteAll(this->FrontendToWatcherMap);
}

void ctkCmdLineModuleExplorerOutputText::setActiveFrontend(ctkCmdLineModuleFrontend* moduleFrontend)
{
  if (this->CurrentFrontend == moduleFrontend) return;

  if (this->CurrentFrontend)
  {
    if (this->CurrentWatcher)
    {
      this->CurrentWatcher->disconnect();
    }
    this->CurrentFrontend->disconnect();

    // save the current output text
    this->FrontendToOutputMap[this->CurrentFrontend] = this->toHtml();
    this->clear();
  }

  this->CurrentFrontend = moduleFrontend;
  if (moduleFrontend)
  {
    // restore previous content
    this->setHtml(this->FrontendToOutputMap[moduleFrontend]);
    QTextCursor endCursor = this->textCursor();
    endCursor.movePosition(QTextCursor::End);
    this->setTextCursor(endCursor);

    this->CurrentWatcher = FrontendToWatcherMap[moduleFrontend];
    if (this->CurrentWatcher == NULL)
    {
      this->CurrentWatcher = new ctkCmdLineModuleFutureWatcher;
      this->FrontendToWatcherMap[moduleFrontend] = this->CurrentWatcher;
    }

    connect(this->CurrentFrontend, SIGNAL(started()), SLOT(frontendStarted()));

    connect(this->CurrentWatcher, SIGNAL(outputDataReady()), SLOT(outputDataReady()));
    connect(this->CurrentWatcher, SIGNAL(errorDataReady()), SLOT(errorDataReady()));

    this->CurrentWatcher->setFuture(moduleFrontend->future());

    // if the frontend is already finished get any output we have not yet fetched
    if (moduleFrontend->future().isFinished())
    {
      this->outputDataReady();
      this->errorDataReady();
    }
  }
  else
  {
    if (this->CurrentWatcher)
    {
      this->CurrentWatcher->disconnect();
      this->CurrentWatcher = NULL;
    }
    if (this->CurrentFrontend)
    {
      this->CurrentFrontend->disconnect();
      this->CurrentFrontend = NULL;
    }
    this->clear();
  }
}

void ctkCmdLineModuleExplorerOutputText::frontendRemoved(ctkCmdLineModuleFrontend *frontend)
{
  delete this->FrontendToWatcherMap[frontend];
  this->FrontendToWatcherMap.remove(frontend);
  this->FrontendToOutputMap.remove(frontend);
}

void ctkCmdLineModuleExplorerOutputText::frontendStarted()
{
  this->clear();
  this->FrontendToOutputMap[this->CurrentFrontend].clear();
  this->CurrentWatcher->setFuture(this->CurrentFrontend->future());
}

void ctkCmdLineModuleExplorerOutputText::outputDataReady()
{
  QByteArray newOutput = this->CurrentWatcher->readPendingOutputData();
  this->setTextColor(QColor(Qt::black));
  this->insertPlainText(newOutput.data());
}

void ctkCmdLineModuleExplorerOutputText::errorDataReady()
{
  QByteArray newOutput = this->CurrentWatcher->readPendingErrorData();
  this->setTextColor(QColor(Qt::darkRed));
  this->insertPlainText(newOutput.data());
}
