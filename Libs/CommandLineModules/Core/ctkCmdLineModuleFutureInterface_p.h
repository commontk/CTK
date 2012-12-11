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

#ifndef CTKCMDLINEMODULEFUTUREINTERFACE_P_H
#define CTKCMDLINEMODULEFUTUREINTERFACE_P_H

#include <QEvent>
#include <QAtomicInt>
#include <QMutex>

class ctkCmdLineModuleFutureCallOutEvent : public QEvent
{
public:

  static const int TypeId;

  enum CallOutType {
    OutputReady,
    ErrorReady
  };

  ctkCmdLineModuleFutureCallOutEvent()
    : QEvent(static_cast<QEvent::Type>(TypeId))
    , callOutType(CallOutType(0))
  {}

  ctkCmdLineModuleFutureCallOutEvent(CallOutType callOutType)
    : QEvent(static_cast<QEvent::Type>(TypeId))
    , callOutType(callOutType)
  {}

  ctkCmdLineModuleFutureCallOutEvent* clone() const
  {
    return new ctkCmdLineModuleFutureCallOutEvent(callOutType);
  }

  CallOutType callOutType;
};

class ctkCmdLineModuleFutureCallOutInterface
{
public:
  virtual ~ctkCmdLineModuleFutureCallOutInterface() {}
  virtual void postCmdLineModuleCallOutEvent(const ctkCmdLineModuleFutureCallOutEvent &) = 0;
  virtual void cmdLineModuleCallOutInterfaceDisconnected() = 0;
};

class ctkCmdLineModuleFutureInterfacePrivate
{
public:

  ctkCmdLineModuleFutureInterfacePrivate(ctkCmdLineModuleFutureInterface* q);

  QAtomicInt RefCount;
  mutable QMutex Mutex;

  QList<ctkCmdLineModuleFutureCallOutInterface *> OutputConnections;

  bool CanCancel;
  bool CanPause;

  QByteArray OutputData;
  QByteArray ErrorData;

  ctkCmdLineModuleFutureInterface* q;

  void sendCallOut(const ctkCmdLineModuleFutureCallOutEvent &callOut);
  void connectOutputInterface(ctkCmdLineModuleFutureCallOutInterface *iface);
  void disconnectOutputInterface(ctkCmdLineModuleFutureCallOutInterface *iface);
};

#endif // CTKCMDLINEMODULEFUTUREINTERFACE_P_H
