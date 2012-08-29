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

#include "ctkCmdLineModuleProcessWatcher_p.h"

#include "ctkCmdLineModuleFuture.h"

#include <QProcess>

#ifdef Q_OS_UNIX
#include <signal.h>
#endif

//----------------------------------------------------------------------------
ctkCmdLineModuleProcessWatcher::ctkCmdLineModuleProcessWatcher(QProcess& process, const QString& location,
                                                               ctkCmdLineModuleFutureInterface &futureInterface)
  : process(process), location(location), futureInterface(futureInterface), processXmlWatcher(&process),
    processPaused(false), progressValue(0)
{
  futureInterface.setProgressRange(0, 1000);

  connect(&processXmlWatcher, SIGNAL(filterStarted(QString,QString)), SLOT(filterStarted(QString,QString)));
  connect(&processXmlWatcher, SIGNAL(filterProgress(float)), SLOT(filterProgress(float)));
  connect(&processXmlWatcher, SIGNAL(filterFinished(QString)), SLOT(filterFinished(QString)));
  connect(&processXmlWatcher, SIGNAL(filterXmlError(QString)), SLOT(filterXmlError(QString)));

  connect(&processXmlWatcher, SIGNAL(outputDataAvailable(QByteArray)), SLOT(outputDataAvailable(QByteArray)));
  connect(&processXmlWatcher, SIGNAL(errorDataAvailable(QByteArray)), SLOT(errorDataAvailable(QByteArray)));

  connect(&futureWatcher, SIGNAL(canceled()), SLOT(cancelProcess()));
#ifdef Q_OS_UNIX
  connect(&futureWatcher, SIGNAL(resumed()), SLOT(resumeProcess()));
  // Due to Qt bug 12152, we cannot listen to the "paused" signal because it is
  // not emitted directly when the QFuture is paused. Instead, it is emitted after
  // resuming the future, after the "resume" signal has been emitted...
  //connect(&futureWatcher, SIGNAL(paused()), SLOT(pauseProcess()));
  connect(&pollPauseTimer, SIGNAL(timeout()), this, SLOT(pauseProcess()));
  pollPauseTimer.start(500);
#endif
  futureWatcher.setFuture(futureInterface.future());
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleProcessWatcher::filterStarted(const QString& name, const QString& comment)
{
  Q_UNUSED(comment)
  futureInterface.setProgressValueAndText(incrementProgress(), name);
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleProcessWatcher::filterProgress(float progress)
{
  futureInterface.setProgressValue(updateProgress(progress));
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleProcessWatcher::filterFinished(const QString& name)
{
  futureInterface.setProgressValueAndText(incrementProgress(), "Finished: " + name);
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleProcessWatcher::filterXmlError(const QString &error)
{
  qDebug().nospace() << "[Module " << location << "]: " << error;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleProcessWatcher::pauseProcess()
{
  if (processPaused || !futureInterface.isPaused()) return;

#ifdef Q_OS_UNIX
  if (::kill(process.pid(), SIGSTOP))
  {
    // error
    futureInterface.setPaused(false);
  }
  else
  {
    processPaused = true;
  }
#endif
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleProcessWatcher::resumeProcess()
{
  if (!processPaused) return;

#ifdef Q_OS_UNIX
  if(::kill(process.pid(), SIGCONT))
  {
    // error
    futureInterface.setPaused(true);
  }
  else
  {
    processPaused = false;
  }
#endif
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleProcessWatcher::cancelProcess()
{
  process.kill();
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleProcessWatcher::outputDataAvailable(const QByteArray &outputData)
{
  futureInterface.reportOutputData(outputData);
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleProcessWatcher::errorDataAvailable(const QByteArray &errorData)
{
  futureInterface.reportErrorData(errorData);
}

//----------------------------------------------------------------------------
int ctkCmdLineModuleProcessWatcher::updateProgress(float progress)
{
  progressValue = static_cast<int>(progress * 1000.0f);
  // normalize the value to lie between 0 and 1000.
  // 0 is reported when the process starts and 1000 is reserved for
  // reporting completion + standard output text
  if (progressValue < 1) progressValue = 1;
  if (progressValue > 999) progressValue = 999;
  return progressValue;
}

//----------------------------------------------------------------------------
int ctkCmdLineModuleProcessWatcher::incrementProgress()
{
  if (++progressValue > 999) progressValue = 999;
  return progressValue;
}
