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

#include <service/event/ctkEventHandler.h>

#include <ctkEventAdminActivator_p.h>

#include <handler/ctkEABlacklistingHandlerTasks_p.h>

template<class BlacklistingHandlerTasks>
class ctkEAHandlerTask<BlacklistingHandlerTasks>::_GetAndUngetEventHandler
{
public:

  _GetAndUngetEventHandler(BlacklistingHandlerTasks* handlerTasks, const ctkServiceReference& ref)
    : handlerTasks(handlerTasks), currHandler(0), currRef(ref)
  {
    currHandler = handlerTasks->getEventHandler(currRef);
  }

  ~_GetAndUngetEventHandler()
  {
    handlerTasks->ungetEventHandler(currHandler, currRef);
  }

  QObject* getObject() const
  {
    return dynamic_cast<QObject*>(currHandler);
  }

  ctkEventHandler* getHandler() const
  {
    return currHandler;
  }

private:

  BlacklistingHandlerTasks* handlerTasks;
  ctkEventHandler* currHandler;
  ctkServiceReference currRef;
};

template<class BlacklistingHandlerTasks>
ctkEAHandlerTask<BlacklistingHandlerTasks>::ctkEAHandlerTask(const ctkServiceReference& eventHandlerRef,
                                                             const ctkEvent& event, BlacklistingHandlerTasks* handlerTasks)
  : eventHandlerRef(eventHandlerRef), event(event), handlerTasks(handlerTasks)
{

}

template<class BlacklistingHandlerTasks>
ctkEAHandlerTask<BlacklistingHandlerTasks>::ctkEAHandlerTask(const Self& task)
  : eventHandlerRef(task.eventHandlerRef), event(task.event),
    handlerTasks(task.handlerTasks)
{

}

template<class BlacklistingHandlerTasks>
ctkEAHandlerTask<BlacklistingHandlerTasks>&
ctkEAHandlerTask<BlacklistingHandlerTasks>::operator=(const Self& task)
{
  eventHandlerRef = task.eventHandlerRef;
  event = task.event;
  handlerTasks = task.handlerTasks;
  return *this;
}

template<class BlacklistingHandlerTasks>
QString ctkEAHandlerTask<BlacklistingHandlerTasks>::getHandlerClassName() const
{
  QObject* handler = _GetAndUngetEventHandler(handlerTasks, eventHandlerRef).getObject();
  return handler->metaObject()->className();
}

template<class BlacklistingHandlerTasks>
void ctkEAHandlerTask<BlacklistingHandlerTasks>::execute()
{
  // Get the service object
  ctkEventHandler* const handler = _GetAndUngetEventHandler(handlerTasks, eventHandlerRef).getHandler();

  try
  {
    handler->handleEvent(event);
  }
  catch (const std::exception& e)
  {
    // The spec says that we must catch exceptions and log them:
    CTK_WARN_SR_EXC(ctkEventAdminActivator::getLogService(), eventHandlerRef, &e)
        << "Exception during event dispatch [" << event.getTopic() << "| Plugin("
        << eventHandlerRef.getPlugin()->getSymbolicName() << ")]";
  }
}

template<class BlacklistingHandlerTasks>
void ctkEAHandlerTask<BlacklistingHandlerTasks>::blackListHandler()
{
  handlerTasks->blackListRef(eventHandlerRef);
}
