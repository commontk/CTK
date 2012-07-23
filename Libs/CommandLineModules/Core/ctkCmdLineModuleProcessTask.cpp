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
#include "ctkCmdLineModuleXmlProgressWatcher.h"
#include "ctkCmdLineModuleFuture.h"

#include <QDebug>
#include <QEventLoop>
#include <QThreadPool>
#include <QProcess>

ctkCmdLineModuleProcessProgressWatcher::ctkCmdLineModuleProcessProgressWatcher(QProcess& process, const QString& location,
                                                                               ctkCmdLineModuleFutureInterface &futureInterface)
  : process(process), location(location), futureInterface(futureInterface), processXmlWatcher(&process),
    progressValue(0)
{
  futureInterface.setProgressRange(0, 1000);

  connect(&processXmlWatcher, SIGNAL(filterStarted(QString,QString)), SLOT(filterStarted(QString,QString)));
  connect(&processXmlWatcher, SIGNAL(filterProgress(float)), SLOT(filterProgress(float)));
  connect(&processXmlWatcher, SIGNAL(filterFinished(QString)), SLOT(filterFinished(QString)));
  connect(&processXmlWatcher, SIGNAL(filterXmlError(QString)), SLOT(filterXmlError(QString)));
}

void ctkCmdLineModuleProcessProgressWatcher::filterStarted(const QString& name, const QString& comment)
{
  Q_UNUSED(comment)
  futureInterface.setProgressValueAndText(incrementProgress(), name);
}

void ctkCmdLineModuleProcessProgressWatcher::filterProgress(float progress)
{
  futureInterface.setProgressValue(updateProgress(progress));
}

void ctkCmdLineModuleProcessProgressWatcher::filterFinished(const QString& name)
{
  futureInterface.setProgressValueAndText(incrementProgress(), "Finished: " + name);
}

void ctkCmdLineModuleProcessProgressWatcher::filterXmlError(const QString &error)
{
  qDebug().nospace() << "[Module " << location << "]: " << error;
}

int ctkCmdLineModuleProcessProgressWatcher::updateProgress(float progress)
{
  progressValue = static_cast<int>(progress * 1000.0f);
  // normalize the value to lie between 0 and 1000.
  // 0 is reported when the process starts and 1000 is reserved for
  // reporting completion + standard output text
  if (progressValue < 1) progressValue = 1;
  if (progressValue > 999) progressValue = 999;
  return progressValue;
}

int ctkCmdLineModuleProcessProgressWatcher::incrementProgress()
{
  if (++progressValue > 999) progressValue = 999;
  return progressValue;
}

ctkCmdLineModuleProcessTask::ctkCmdLineModuleProcessTask(const QString& location, const QStringList& args)
  : location(location), args(args)
{
  this->setCanCancel(true);
}

ctkCmdLineModuleProcessTask::~ctkCmdLineModuleProcessTask()
{
}

ctkCmdLineModuleFuture ctkCmdLineModuleProcessTask::start()
{
  this->setRunnable(this);
  this->reportStarted();
  ctkCmdLineModuleFuture future = this->future();
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
  process.setReadChannel(QProcess::StandardOutput);

  QEventLoop localLoop;
  QObject::connect(&process, SIGNAL(finished(int)), &localLoop, SLOT(quit()));
  QObject::connect(&process, SIGNAL(error(QProcess::ProcessError)), &localLoop, SLOT(quit()));

  process.start(location, args);

  ctkCmdLineModuleProcessProgressWatcher progressWatcher(process, location, *this);
  Q_UNUSED(progressWatcher)

  localLoop.exec();

  if (process.error() != QProcess::UnknownError)
  {
    this->reportException(ctkCmdLineModuleRunException(location, process.exitCode(), process.errorString()));
  }
  else if (process.exitCode() != 0)
  {
    this->reportException(ctkCmdLineModuleRunException(location, process.exitCode(), process.readAllStandardError()));
  }

  this->setProgressValueAndText(1000, process.readAllStandardError());

  //this->reportResult(result);
  this->reportFinished();

}
