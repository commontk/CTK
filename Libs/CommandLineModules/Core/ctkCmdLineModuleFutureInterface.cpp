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

#include "ctkCmdLineModuleFutureInterface.h"
#include "ctkCmdLineModuleFutureInterface_p.h"

const int ctkCmdLineModuleFutureCallOutEvent::TypeId = QEvent::registerEventType();

//----------------------------------------------------------------------------
// ctkCmdLineModuleFutureInterfacePrivate

//----------------------------------------------------------------------------
ctkCmdLineModuleFutureInterfacePrivate::ctkCmdLineModuleFutureInterfacePrivate(ctkCmdLineModuleFutureInterface* q)
  : RefCount(1)
  , CanCancel(false)
  , CanPause(false)
  , q(q)
{
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleFutureInterfacePrivate::sendCallOut(const ctkCmdLineModuleFutureCallOutEvent &callOutEvent)
{
  if (OutputConnections.isEmpty())
  {
    return;
  }

  for (int i = 0; i < OutputConnections.count(); ++i)
  {
    OutputConnections.at(i)->postCmdLineModuleCallOutEvent(callOutEvent);
  }
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleFutureInterfacePrivate::connectOutputInterface(ctkCmdLineModuleFutureCallOutInterface *iface)
{
  QMutexLocker locker(&Mutex);

  if (q->isStarted())
  {
    if (!this->OutputData.isEmpty())
    {
      iface->postCmdLineModuleCallOutEvent(ctkCmdLineModuleFutureCallOutEvent(ctkCmdLineModuleFutureCallOutEvent::OutputReady));
    }
    if (!this->ErrorData.isEmpty())
    {
      iface->postCmdLineModuleCallOutEvent(ctkCmdLineModuleFutureCallOutEvent(ctkCmdLineModuleFutureCallOutEvent::ErrorReady));
    }
  }

  OutputConnections.append(iface);
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleFutureInterfacePrivate::disconnectOutputInterface(ctkCmdLineModuleFutureCallOutInterface *iface)
{
  QMutexLocker lock(&Mutex);
  const int index = OutputConnections.indexOf(iface);
  if (index == -1)
    return;
  OutputConnections.removeAt(index);

  iface->cmdLineModuleCallOutInterfaceDisconnected();
}

//----------------------------------------------------------------------------
// QFutureInterface<ctkCmdLineModuleResult>

//----------------------------------------------------------------------------
QFutureInterface<ctkCmdLineModuleResult>::QFutureInterface(State initialState)
  : QFutureInterfaceBase(initialState)
  , d(new ctkCmdLineModuleFutureInterfacePrivate(this))
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
  refT();
#endif
}

//----------------------------------------------------------------------------
QFutureInterface<ctkCmdLineModuleResult>::QFutureInterface(const QFutureInterface& other)
  : QFutureInterfaceBase(other)
  , d(other.d)
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
  refT();
#endif
  d->RefCount.ref();
}

//----------------------------------------------------------------------------
QFutureInterface<ctkCmdLineModuleResult>::~QFutureInterface()
{
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
  if (referenceCountIsOne())
    resultStore().clear();
#elif QT_VERSION < QT_VERSION_CHECK(5,9,0)
  if (!derefT())
    resultStore().clear();
#else
  if (!derefT())
    resultStore().clear<ctkCmdLineModuleResult>();
#endif

  if (!d->RefCount.deref())
  {
    delete d;
  }
}

//----------------------------------------------------------------------------
QFutureInterface<ctkCmdLineModuleResult> QFutureInterface<ctkCmdLineModuleResult>::canceledResult()
{
  return QFutureInterface(State(Started | Finished | Canceled));
}

//----------------------------------------------------------------------------
QFutureInterface<ctkCmdLineModuleResult>&
QFutureInterface<ctkCmdLineModuleResult>::operator=(const QFutureInterface& other)
{
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
  if (referenceCountIsOne())
#else
  other.refT();
  if (!derefT())
#endif
#if QT_VERSION < QT_VERSION_CHECK(5,9,0)
    resultStore().clear();
#else
    resultStore().clear<ctkCmdLineModuleResult>();
#endif

  QFutureInterfaceBase::operator=(other);

  other.d->RefCount.ref();
  if (!d->RefCount.deref())
    delete d;
  d = other.d;

  // update the q pointer in the private implementation
  d->q = this;

  return *this;
}

//----------------------------------------------------------------------------
bool QFutureInterface<ctkCmdLineModuleResult>::canCancel() const
{
  return d->CanCancel;
}

//----------------------------------------------------------------------------
void QFutureInterface<ctkCmdLineModuleResult>::setCanCancel(bool canCancel)
{
  d->CanCancel = canCancel;
}

//----------------------------------------------------------------------------
bool QFutureInterface<ctkCmdLineModuleResult>::canPause() const
{
  return d->CanPause;
}

//----------------------------------------------------------------------------
void QFutureInterface<ctkCmdLineModuleResult>::setCanPause(bool canPause)
{
  d->CanPause = canPause;
}

//----------------------------------------------------------------------------
void QFutureInterface<ctkCmdLineModuleResult>::reportOutputData(const QByteArray& outputData)
{
  QMutexLocker l(&d->Mutex);

  if (isCanceled() || isFinished()) return;
  d->OutputData.append(outputData);
  d->sendCallOut(ctkCmdLineModuleFutureCallOutEvent(ctkCmdLineModuleFutureCallOutEvent::OutputReady));
}

//----------------------------------------------------------------------------
void QFutureInterface<ctkCmdLineModuleResult>::reportErrorData(const QByteArray& errorData)
{
  QMutexLocker l(&d->Mutex);

  if (isCanceled() || isFinished()) return;
  d->ErrorData.append(errorData);
  d->sendCallOut(ctkCmdLineModuleFutureCallOutEvent(ctkCmdLineModuleFutureCallOutEvent::ErrorReady));
}

//----------------------------------------------------------------------------
QByteArray QFutureInterface<ctkCmdLineModuleResult>::outputData(int position, int size) const
{
  QMutexLocker l(&d->Mutex);
  if (size < 0) size = d->OutputData.size();
  if (size > d->OutputData.size() - position) size = d->OutputData.size() - position;
  return QByteArray(d->OutputData.data() + position, size);
}

//----------------------------------------------------------------------------
QByteArray QFutureInterface<ctkCmdLineModuleResult>::errorData(int position, int size) const
{
  QMutexLocker l(&d->Mutex);
  if (size < 0) size = d->ErrorData.size();
  if (size > d->ErrorData.size() - position) size = d->ErrorData.size() - position;
  return QByteArray(d->ErrorData.data() + position, size);
}
