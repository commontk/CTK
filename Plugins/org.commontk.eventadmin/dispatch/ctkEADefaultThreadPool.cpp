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


#include "ctkEADefaultThreadPool_p.h"

#include "ctkEALinkedQueue_p.h"
#include "ctkEAInterruptedException_p.h"

#include <ctkEventAdminActivator_p.h>
#include <tasks/ctkEASyncThread_p.h>

struct _SyncThreadFactory : public ctkEAThreadFactory
{
  ctkEAInterruptibleThread* newThread(ctkEARunnable* command)
  {
    ctkEAInterruptibleThread* thread = new ctkEASyncThread(command);
    //thread->setPriority(QThread::NormalPriority);
    //thread->setDaemon(true);

    return thread;
  }
};

struct _AsyncThreadFactory : public ctkEAThreadFactory
{
  ctkEAInterruptibleThread* newThread(ctkEARunnable* command)
  {
    ctkEAInterruptibleThread* thread = new ctkEAInterruptibleThread(command);
    //thread->setPriority(QThread::NormalPriority);
    //thread->setDaemon(false);

    return thread;
  }
};

ctkEADefaultThreadPool::ctkEADefaultThreadPool(int poolSize, bool syncThreads)
  : ctkEAPooledExecutor(new ctkEALinkedQueue())
{
  if (syncThreads)
  {
    delete this->setThreadFactory(new _SyncThreadFactory());
  }
  else
  {
    delete this->setThreadFactory(new _AsyncThreadFactory());
  }

  configure(poolSize);
  setKeepAliveTime(60000);
  runWhenBlocked();
}

void ctkEADefaultThreadPool::configure(int poolSize)
{
  setMinimumPoolSize(poolSize);
  setMaximumPoolSize(poolSize + 10);
}

void ctkEADefaultThreadPool::close()
{
  shutdownNow();

  try
  {
    awaitTerminationAfterShutdown();
  }
  catch (const ctkEAInterruptedException& )
  {
    // ignore this
  }
}

void ctkEADefaultThreadPool::executeTask(ctkEARunnable* task)
{
  try
  {
    this->execute(task);
  }
  catch (const std::exception& e)
  {
    CTK_WARN_EXC(ctkEventAdminActivator::getLogService(), &e)
        << "Exception: " << e.what();
    // ignore this
  }
}
