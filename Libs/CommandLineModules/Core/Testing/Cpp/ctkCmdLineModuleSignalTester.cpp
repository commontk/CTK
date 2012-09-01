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

void ctkCmdLineModuleSignalTester::moduleStarted()
{
  events.push_back("module.started");
}

void ctkCmdLineModuleSignalTester::moduleFinished()
{
  events.push_back("module.finished");
}

void ctkCmdLineModuleSignalTester::moduleProgressValueChanged(int /*progress*/)
{
  events.push_back("module.progressValueChanged");
}

void ctkCmdLineModuleSignalTester::moduleProgressTextChanged(const QString &/*text*/)
{
  events.push_back("module.progressTextChanged");
}

void ctkCmdLineModuleSignalTester::modulePaused()
{
  events.push_back("module.paused");
}

void ctkCmdLineModuleSignalTester::moduleResumed()
{
  events.push_back("module.resumed");
}

void ctkCmdLineModuleSignalTester::moduleCanceled()
{
  events.push_back("module.canceled");
}

void ctkCmdLineModuleSignalTester::filterStarted(const QString &/*name*/, const QString &/*comment*/)
{
  events.push_back("filter.started");
}

void ctkCmdLineModuleSignalTester::filterProgress(float /*progress*/, const QString &/*comment*/)
{
  events.push_back("filter.progress");
}

void ctkCmdLineModuleSignalTester::filterFinished(const QString &/*name*/, const QString &/*comment*/)
{
  events.push_back("filter.finished");
}

void ctkCmdLineModuleSignalTester::filterXmlError(const QString &/*error*/)
{
  events.push_back("filter.xmlError");
}

bool ctkCmdLineModuleSignalTester::checkSignals(const QList<QString>& expectedSignals)
{
  if (events.size() != expectedSignals.size())
  {
    dumpSignals(expectedSignals);
    return false;
  }

  for (int i=0; i < expectedSignals.size(); ++i)
  {
    if (expectedSignals[i] != events[i])
    {
      dumpSignals(expectedSignals);
      return false;
    }
  }
  return true;
}

void ctkCmdLineModuleSignalTester::dumpSignals(const QList<QString>& expectedSignals)
{
  int max = events.size() > expectedSignals.size() ? events.size() : expectedSignals.size();
  qDebug() << "Expected signal --  Actual signal";
  for (int i = 0; i < max; ++i)
  {
    QString sig = i < events.size() ? events[i] : QString();
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
