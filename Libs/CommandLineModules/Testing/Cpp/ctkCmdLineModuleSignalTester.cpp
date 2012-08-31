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

#include "ctkCmdLineModuleSignalTester.h"

#include <QDebug>

//-----------------------------------------------------------------------------
ctkCmdLineModuleSignalTester::ctkCmdLineModuleSignalTester()
{
  connect(&Watcher, SIGNAL(started()), SLOT(moduleStarted()));
  connect(&Watcher, SIGNAL(canceled()), SLOT(moduleCanceled()));
  connect(&Watcher, SIGNAL(finished()), SLOT(moduleFinished()));

  connect(&Watcher, SIGNAL(paused()), SLOT(modulePaused()));
  connect(&Watcher, SIGNAL(resumed()), SLOT(moduleResumed()));

  connect(&Watcher, SIGNAL(resultReadyAt(int)), SLOT(resultReadyAt(int)));
  connect(&Watcher, SIGNAL(resultsReadyAt(int,int)), SLOT(resultReadyAt(int,int)));

  connect(&Watcher, SIGNAL(progressRangeChanged(int,int)), SLOT(progressRangeChanged(int,int)));
  connect(&Watcher, SIGNAL(progressTextChanged(QString)), SLOT(progressTextChanged(QString)));
  connect(&Watcher, SIGNAL(progressValueChanged(int)), SLOT(progressValueChanged(int)));

  connect(&Watcher, SIGNAL(outputDataReady()), SLOT(outputDataReady()));
  connect(&Watcher, SIGNAL(errorDataReady()), SLOT(errorDataReady()));
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleSignalTester::setFuture(const ctkCmdLineModuleFuture &future)
{
  this->Watcher.setFuture(future);
}

//-----------------------------------------------------------------------------
ctkCmdLineModuleFutureWatcher *ctkCmdLineModuleSignalTester::watcher()
{
  return &this->Watcher;
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleSignalTester::moduleStarted()
{
  Events.push_back("module.started");
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleSignalTester::moduleFinished()
{
  Events.push_back("module.finished");
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleSignalTester::moduleProgressValueChanged(int progress)
{
  Events.push_back(QString("module.progressValueChanged(%1)").arg(progress));
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleSignalTester::moduleProgressTextChanged(const QString& text)
{
  Events.push_back(QString("module.progressTextChanged(\"%1\")").arg(text));
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleSignalTester::modulePaused()
{
  Events.push_back("module.paused");
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleSignalTester::moduleResumed()
{
  Events.push_back("module.resumed");
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleSignalTester::moduleCanceled()
{
  Events.push_back("module.canceled");
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleSignalTester::resultReadyAt(int resultIndex)
{
  Events.push_back(QString("module.resultReadyAt(%1)").arg(resultIndex));
  Results.push_back(Watcher.resultAt(resultIndex));
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleSignalTester::resultReadyAt(int beginIndex, int endIndex)
{
  Events.push_back(QString("module.resultReadyAt(%1,%2)").arg(beginIndex).arg(endIndex));
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleSignalTester::progressRangeChanged(int minimum, int maximum)
{
  Events.push_back(QString("module.progressRangeChanged(%1,%2)").arg(minimum).arg(maximum));
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleSignalTester::progressValueChanged(int progressValue)
{
  Events.push_back(QString("module.progressValueChanged(%1)").arg(progressValue));
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleSignalTester::progressTextChanged(const QString &progressText)
{
  Events.push_back(QString("module.progressTextChanged(%1)").arg(progressText));
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleSignalTester::outputDataReady()
{
  Events.push_back("module.outputReady");
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleSignalTester::errorDataReady()
{
  Events.push_back("module.errorReady");
}

//-----------------------------------------------------------------------------
bool ctkCmdLineModuleSignalTester::checkSignals(const QList<QString>& expectedSignals)
{
  if (Events.size() != expectedSignals.size())
  {
    dumpSignals(expectedSignals);
    return false;
  }

  for (int i=0; i < expectedSignals.size(); ++i)
  {
    if (expectedSignals[i] != Events[i])
    {
      dumpSignals(expectedSignals);
      return false;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleSignalTester::dumpSignals(const QList<QString>& expectedSignals)
{
  int max = Events.size() > expectedSignals.size() ? Events.size() : expectedSignals.size();
  qDebug() << "Expected signal --  Actual signal";
  for (int i = 0; i < max; ++i)
  {
    QString sig = i < Events.size() ? Events[i] : QString();
    if (i < expectedSignals.size())
    {
      qDebug() << " " << expectedSignals[i] << "--" << sig;
    }
    else
    {
      qDebug() << " " << "- NONE - " << "--" << sig;
    }
  }
}

//-----------------------------------------------------------------------------
QList<ctkCmdLineModuleResult> ctkCmdLineModuleSignalTester::results() const
{
  return Results;
}
