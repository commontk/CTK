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
#include "ctkCmdLineModuleProcessWatcher_p.h"
#include "ctkCmdLineModuleRunException.h"
#include "ctkCmdLineModuleXmlProgressWatcher.h"
#include "ctkCmdLineModuleFuture.h"

#include <QDebug>
#include <QEventLoop>
#include <QThreadPool>
#include <QProcess>

//----------------------------------------------------------------------------
struct ctkCmdLineModuleProcessTaskPrivate
{
  ctkCmdLineModuleProcessTaskPrivate(const QString& location, const QStringList& args)
    : Location(location)
    , Args(args)
  {}

  const QString Location;
  const QStringList Args;
};

//----------------------------------------------------------------------------
ctkCmdLineModuleProcessTask::ctkCmdLineModuleProcessTask(const QString& location, const QStringList& args)
  : d(new ctkCmdLineModuleProcessTaskPrivate(location, args))
{
  this->setCanCancel(true);
#ifdef Q_OS_UNIX
  this->setCanPause(true);
#endif
}

//----------------------------------------------------------------------------
ctkCmdLineModuleProcessTask::~ctkCmdLineModuleProcessTask()
{
}

//----------------------------------------------------------------------------
ctkCmdLineModuleFuture ctkCmdLineModuleProcessTask::start()
{
  this->setRunnable(this);
  this->reportStarted();
  ctkCmdLineModuleFuture future = this->future();
  QThreadPool::globalInstance()->start(this, /*m_priority*/ 0);
  return future;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleProcessTask::run()
{
  if (this->isCanceled())
  {
    this->reportFinished();
    return;
  }

  QProcess process;
  process.setReadChannel(QProcess::StandardOutput);

  QEventLoop localLoop;
  QObject::connect(&process, SIGNAL(finished(int)), &localLoop, SLOT(quit()));
  QObject::connect(&process, SIGNAL(error(QProcess::ProcessError)), &localLoop, SLOT(quit()));

  process.start(d->Location, d->Args);

  ctkCmdLineModuleProcessWatcher progressWatcher(process, d->Location, *this);
  Q_UNUSED(progressWatcher)

  localLoop.exec();

  if (process.error() != QProcess::UnknownError)
  {
    this->reportException(ctkCmdLineModuleRunException(d->Location, process.exitCode(), process.errorString()));
  }
  else if (process.exitCode() != 0)
  {
    this->reportException(ctkCmdLineModuleRunException(d->Location, process.exitCode(), process.readAllStandardError()));
  }

  this->setProgressValueAndText(1000, process.readAllStandardError());

  //this->reportResult(result);
  this->reportFinished();

}
