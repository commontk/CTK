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


#include "ctkEAInterruptibleThread_p.h"

#include "ctkEAInterruptedException_p.h"

#include <QRunnable>
#include <QDebug>

ctkEAInterruptibleThread::ctkEAInterruptibleThread(ctkEARunnable* command, QObject* parent)
  : QThread(parent), command(command)
{
  this->setObjectName(QString("ctkEAInterruptibleThread") + QString::number(reinterpret_cast<qint64>(command)));
}

void ctkEAInterruptibleThread::run()
{
  if (command)
  {
    const bool autoDelete = command->autoDelete();
    command->run();
    if (autoDelete && !--command->ref) delete command;
  }
  else
  {
    QThread::run();
  }
}

ctkEAInterruptibleThread* ctkEAInterruptibleThread::currentThread()
{
  return qobject_cast<ctkEAInterruptibleThread*>(QThread::currentThread());
}

void ctkEAInterruptibleThread::wait(QMutex* mutex, QWaitCondition* waitCond, unsigned long time)
{
  currWaitCond_.testAndSetOrdered(0, waitCond);
  isWaiting_.testAndSetOrdered(0,1);
  waitCond->wait(mutex, time == 0 ? ULONG_MAX : time);
  isWaiting_.testAndSetOrdered(1,0);
  currWaitCond_.fetchAndStoreOrdered(0);

  if (interrupted_.fetchAndAddOrdered(0))
  {
    interrupted_.testAndSetOrdered(1, 0);
    throw ctkEAInterruptedException();
  }
}

void ctkEAInterruptibleThread::join()
{
  this->QThread::wait();
}

void ctkEAInterruptibleThread::interrupt()
{
  if (isWaiting_.fetchAndAddOrdered(0) && this->isRunning())
  {
    interrupted_.testAndSetOrdered(0, 1);
    if (QWaitCondition* waitCond = currWaitCond_.fetchAndStoreOrdered(0))
      waitCond->wakeAll();
  }
  else
  {
    if (this->isRunning())
    {
      interrupted_.testAndSetOrdered(0, 1);
    }
  }
}

bool ctkEAInterruptibleThread::interrupted()
{
  if (ctkEAInterruptibleThread* t = qobject_cast<ctkEAInterruptibleThread*>(QThread::currentThread()))
  {
    return t->interrupted_.fetchAndStoreOrdered(0);
  }
  return false;
}

bool ctkEAInterruptibleThread::isInterrupted() const
{
  return interrupted_.fetchAndAddOrdered(0);
}
