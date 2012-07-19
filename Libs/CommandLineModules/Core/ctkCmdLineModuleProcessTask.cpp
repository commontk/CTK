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

#include "ctkCmdLineModuleProcessTask.h"
#include "ctkCmdLineModuleRunException.h"

#include <QDebug>
#include <QEventLoop>
#include <QThreadPool>
#include <QProcess>
#include <QFuture>


ctkCmdLineModuleProcessTask::ctkCmdLineModuleProcessTask(const QString& location, const QStringList& args)
  : location(location), args(args)
{
}

ctkCmdLineModuleProcessTask::~ctkCmdLineModuleProcessTask()
{
}

QFuture<QString> ctkCmdLineModuleProcessTask::start()
{
  this->setRunnable(this);
  this->reportStarted();
  QFuture<QString> future = this->future();
  QThreadPool::globalInstance()->start(this, /*m_priority*/ 0);
  return future;
}

void ctkCmdLineModuleProcessTask::run()
{
  if (this->isCanceled())
  {
    this->reportFinished();
    return;
  }

  QProcess process;
  QEventLoop localLoop;
  connect(&process, SIGNAL(finished(int)), &localLoop, SLOT(quit()));
  connect(&process, SIGNAL(error(QProcess::ProcessError)), SLOT(error(QProcess::ProcessError)));
  connect(&process, SIGNAL(readyReadStandardError()), SLOT(readyReadStandardError()));
  connect(&process, SIGNAL(readyReadStandardOutput()), SLOT(readyReadStandardOutput()));

  process.start(location, args);

  localLoop.exec();

  if (process.exitCode() != 0 || process.exitStatus() == QProcess::CrashExit)
  {
    QString msg = "The process running \"%1\" ";
    msg += process.exitStatus() == QProcess::CrashExit ? QString("crashed: ")
                                                     : QString("exited with code %1: ").arg(process.exitCode());
    msg += process.readAllStandardError();
    this->reportException(ctkCmdLineModuleRunException(msg));
  }

  this->setProgressValueAndText(100, process.readAllStandardOutput());

  //this->reportResult(result);
  this->reportFinished();
}

void ctkCmdLineModuleProcessTask::error(QProcess::ProcessError error)
{
}

void ctkCmdLineModuleProcessTask::readyReadStandardError()
{
}

void ctkCmdLineModuleProcessTask::readyReadStandardOutput()
{
}
