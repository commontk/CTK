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


#include "ctkEASyncThread_p.h"

ctkEASyncThread::ctkEASyncThread(ctkEARunnable* target, QObject* parent)
  : ctkEAInterruptibleThread(target, parent), counter(0)
{
  this->setObjectName(QString("ctkEASyncThread") + QString::number(reinterpret_cast<qint64>(target)));
}

void ctkEASyncThread::init(ctkEARendezvous* timerBarrier, ctkEARendezvous* cascadingBarrier)
{
  this->timerBarrier.testAndSetOrdered(0, timerBarrier);
  this->cascadingBarrier.testAndSetOrdered(0, cascadingBarrier);
}

void ctkEASyncThread::uninit()
{
  this->timerBarrier.testAndSetOrdered(timerBarrier, 0);
  this->cascadingBarrier.testAndSetOrdered(cascadingBarrier, 0);
}

ctkEARendezvous* ctkEASyncThread::getTimerBarrier() const
{
  return timerBarrier.fetchAndAddOrdered(0);
}

ctkEARendezvous* ctkEASyncThread::getCascadingBarrier() const
{
  return cascadingBarrier.fetchAndAddOrdered(0);
}

bool ctkEASyncThread::isTopMostHandler() const
{
  return counter.fetchAndAddOrdered(0) == 0;
}

void ctkEASyncThread::innerEventHandlingStart()
{
  counter.ref();
}

void ctkEASyncThread::innerEventHandlingStopped()
{
  counter.deref();
}
