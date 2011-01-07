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


#include "ctkEASyncMasterThread_p.h"

#include <QRunnable>
#include <QDebug>

ctkEASyncMasterThread::ctkEASyncMasterThread()
  : command(0)
{
  thread.setObjectName("ctkEASyncMasterThread");
  moveToThread(&thread);
  thread.start();
}

void ctkEASyncMasterThread::syncRun(QRunnable* command)
{
  if (ctkEAInterruptibleThread::currentThread() != &thread)
  {
    QMutexLocker l(&mutex);
    this->command = command;
    QMetaObject::invokeMethod(this, "runCommand", Qt::QueuedConnection);
    waitCond.wait(&mutex);
  }
  else
  {
    qWarning() << "This method is not intended to be called from thread" << thread.objectName();
  }
}

void ctkEASyncMasterThread::runCommand()
{
  if (command)
  {
    QMutexLocker l(&mutex);
    command->run();
    waitCond.wakeOne();
  }
}

void ctkEASyncMasterThread::stop()
{
  thread.quit();
  thread.join();
}
