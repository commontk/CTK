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

#include "ctkCmdLineModuleFutureWatcher.h"

#include "ctkCmdLineModuleFuture.h"
#include "ctkCmdLineModuleFutureInterface_p.h"

#include <QThread>
#include <QCoreApplication>

//----------------------------------------------------------------------------
struct ctkCmdLineModuleFutureWatcherPrivate : public ctkCmdLineModuleFutureCallOutInterface
{
  ctkCmdLineModuleFutureWatcherPrivate(ctkCmdLineModuleFutureWatcher* q)
    : q(q)
    , pendingOutputReadyEvent(NULL)
    , pendingErrorReadyEvent(NULL)
    , outputPos(0)
    , errorPos(0)
  {}

  void connectOutputInterface()
  {
    q->futureInterface().d->connectOutputInterface(this);
  }

  void disconnectOutputInterface(bool pendingAssignment = false)
  {
    if (pendingAssignment)
    {
      delete this->pendingOutputReadyEvent;
      this->pendingOutputReadyEvent = NULL;
      delete this->pendingErrorReadyEvent;
      this->pendingErrorReadyEvent = NULL;
    }

    q->futureInterface().d->disconnectOutputInterface(this);
  }

  void postCmdLineModuleCallOutEvent(const ctkCmdLineModuleFutureCallOutEvent& callOutEvent)
  {
    QCoreApplication::postEvent(q, callOutEvent.clone());
  }

  void cmdLineModuleCallOutInterfaceDisconnected()
  {
    QCoreApplication::removePostedEvents(q, ctkCmdLineModuleFutureCallOutEvent::TypeId);
  }

  void sendCmdLineModuleCallOutEvent(ctkCmdLineModuleFutureCallOutEvent* event)
  {
    if (q->futureInterface().isCanceled()) return;

    switch (event->callOutType)
    {
    case ctkCmdLineModuleFutureCallOutEvent::OutputReady:
      emit q->outputDataReady();
      break;
    case ctkCmdLineModuleFutureCallOutEvent::ErrorReady:
      emit q->errorDataReady();
      break;
    default: break;
    }
  }

  ctkCmdLineModuleFutureWatcher* q;

  ctkCmdLineModuleFuture Future;

  ctkCmdLineModuleFutureCallOutEvent* pendingOutputReadyEvent;
  ctkCmdLineModuleFutureCallOutEvent* pendingErrorReadyEvent;
  int outputPos;
  int errorPos;
};

//----------------------------------------------------------------------------
ctkCmdLineModuleFutureWatcher::ctkCmdLineModuleFutureWatcher(QObject* parent)
  : QFutureWatcher<ctkCmdLineModuleResult>(parent)
  , d(new ctkCmdLineModuleFutureWatcherPrivate(this))
{
}

//----------------------------------------------------------------------------
ctkCmdLineModuleFutureWatcher::~ctkCmdLineModuleFutureWatcher()
{
  disconnectOutputInterface();
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleFutureWatcher::setFuture(const ctkCmdLineModuleFuture& future)
{
  if (d->Future == future) return;

  d->outputPos = 0;
  d->errorPos = 0;

  d->disconnectOutputInterface(true);
  d->Future = future;
  QFutureWatcher<ctkCmdLineModuleResult>::setFuture(future);
  d->connectOutputInterface();
}

//----------------------------------------------------------------------------
ctkCmdLineModuleFuture ctkCmdLineModuleFutureWatcher::future() const
{
  return d->Future;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleFutureWatcher::event(QEvent *event)
{
  if (event->type() == ctkCmdLineModuleFutureCallOutEvent::TypeId)
  {
    ctkCmdLineModuleFutureCallOutEvent* callOutEvent = static_cast<ctkCmdLineModuleFutureCallOutEvent*>(event);

    if (futureInterface().isPaused())
    {
      if (callOutEvent->callOutType == ctkCmdLineModuleFutureCallOutEvent::OutputReady &&
          d->pendingOutputReadyEvent == NULL)
      {
        d->pendingOutputReadyEvent = callOutEvent->clone();
      }
      if (callOutEvent->callOutType == ctkCmdLineModuleFutureCallOutEvent::ErrorReady &&
          d->pendingErrorReadyEvent == NULL)
      {
        d->pendingErrorReadyEvent = callOutEvent->clone();
      }
      return true;
    }

    d->sendCmdLineModuleCallOutEvent(callOutEvent);
    return true;
  }
  else if (event->type() == QEvent::FutureCallOut)
  {
    bool result = QFutureWatcher<ctkCmdLineModuleResult>::event(event);

    if (futureInterface().isRunning())
    {
      // send all pending call outs
      if (d->pendingOutputReadyEvent)
      {
        d->sendCmdLineModuleCallOutEvent(d->pendingOutputReadyEvent);
        delete d->pendingOutputReadyEvent;
        d->pendingOutputReadyEvent = NULL;
      }
      if (d->pendingErrorReadyEvent)
      {
        d->sendCmdLineModuleCallOutEvent(d->pendingErrorReadyEvent);
        delete d->pendingErrorReadyEvent;
        d->pendingErrorReadyEvent = NULL;
      }
    }
    return result;
  }
  return QFutureWatcher<ctkCmdLineModuleResult>::event(event);
}

//----------------------------------------------------------------------------
QByteArray ctkCmdLineModuleFutureWatcher::readPendingOutputData() const
{
  QByteArray output = futureInterface().outputData(d->outputPos);
  d->outputPos += output.size();
  return output;
}

//----------------------------------------------------------------------------
QByteArray ctkCmdLineModuleFutureWatcher::readPendingErrorData() const
{
  QByteArray errorOutput = futureInterface().errorData(d->errorPos);
  d->errorPos += errorOutput.size();
  return errorOutput;
}

//----------------------------------------------------------------------------
QByteArray ctkCmdLineModuleFutureWatcher::readAllOutputData() const
{
  return d->Future.readAllOutputData();
}

//----------------------------------------------------------------------------
QByteArray ctkCmdLineModuleFutureWatcher::readAllErrorData() const
{
  return d->Future.readAllErrorData();
}

//----------------------------------------------------------------------------
const ctkCmdLineModuleFutureInterface& ctkCmdLineModuleFutureWatcher::futureInterface() const
{
  return d->Future.d;
}

//----------------------------------------------------------------------------
ctkCmdLineModuleFutureInterface& ctkCmdLineModuleFutureWatcher::futureInterface()
{
  return d->Future.d;
}
