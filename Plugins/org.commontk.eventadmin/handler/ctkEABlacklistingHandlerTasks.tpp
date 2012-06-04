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


template<class BlackList, class TopicHandlerFilters, class Filters>
ctkEABlacklistingHandlerTasks<BlackList, TopicHandlerFilters, Filters>::
ctkEABlacklistingHandlerTasks(ctkPluginContext* context,
                              ctkEABlackList<BlackList>* blackList,
                              ctkEATopicHandlerFilters<TopicHandlerFilters>* topicHandlerFilters,
                              ctkEAFilters<Filters>* filters)
  : blackList(blackList), context(context),
    topicHandlerFilters(topicHandlerFilters), filters(filters)
{
  checkNull(context, "Context");
  checkNull(blackList, "BlackList");
  checkNull(topicHandlerFilters, "TopicHandlerFilters");
  checkNull(filters, "Filters");
}

template<class BlackList, class TopicHandlerFilters, class Filters>
ctkEABlacklistingHandlerTasks<BlackList, TopicHandlerFilters, Filters>::
~ctkEABlacklistingHandlerTasks()
{
  delete filters;
  delete topicHandlerFilters;
  delete blackList;
}

template<class BlackList, class TopicHandlerFilters, class Filters>
QList<ctkEAHandlerTask<ctkEABlacklistingHandlerTasks<BlackList, TopicHandlerFilters, Filters> > >
ctkEABlacklistingHandlerTasks<BlackList, TopicHandlerFilters, Filters>::
createHandlerTasks(const ctkEvent& event)
{
  QList<ctkEAHandlerTask<Self> > result;
  QList<ctkServiceReference> handlerRefs;

  try
  {
    handlerRefs = context->getServiceReferences<ctkEventHandler>(
          topicHandlerFilters->createFilterForTopic(event.getTopic()));
  }
  catch (const ctkInvalidArgumentException& e)
  {
    CTK_WARN_EXC(ctkEventAdminActivator::getLogService(), &e)
        << "Invalid EVENT_TOPIC [" << event.getTopic() << "]";
  }

  for (int i = 0; i < handlerRefs.size(); ++i)
  {
    const ctkServiceReference& ref = handlerRefs.at(i);
    if (!blackList->contains(ref)
        //TODO security
        //&& ref.getPlugin()->hasPermission(
        //  PermissionsUtil.createSubscribePermission(event.getTopic()))
        )
    {
      try
      {
        if (event.matches(filters->createFilter(
                            ref.getProperty(ctkEventConstants::EVENT_FILTER).toString())))
        {
          result.push_back(ctkEAHandlerTask<Self>(ref, event, this));
        }
      }
      catch (const ctkInvalidArgumentException& e)
      {
        CTK_WARN_SR_EXC(ctkEventAdminActivator::getLogService(), ref, &e)
            << "Invalid EVENT_FILTER - Blacklisting ServiceReference ["
            << ref << " | Plugin(" << ref.getPlugin() << ")]";

        blackList->add(ref);
      }
    }
  }

  return result;
}

template<class BlackList, class TopicHandlerFilters, class Filters>
void
ctkEABlacklistingHandlerTasks<BlackList, TopicHandlerFilters, Filters>::
blackListRef(const ctkServiceReference& handlerRef)
{
  blackList->add(handlerRef);

  CTK_WARN(ctkEventAdminActivator::getLogService())
      << "Blacklisting ServiceReference [" << handlerRef << " | Plugin("
      << handlerRef.getPlugin() << ")] due to timeout!";
}

template<class BlackList, class TopicHandlerFilters, class Filters>
ctkEventHandler*
ctkEABlacklistingHandlerTasks<BlackList, TopicHandlerFilters, Filters>::
getEventHandler(const ctkServiceReference& handlerRef)
{
  ctkEventHandler* result = (blackList->contains(handlerRef)) ? 0
                                                             : context->getService<ctkEventHandler>(handlerRef);

  return (result ? result : &nullEventHandler);
}

template<class BlackList, class TopicHandlerFilters, class Filters>
void
ctkEABlacklistingHandlerTasks<BlackList, TopicHandlerFilters, Filters>::
ungetEventHandler(ctkEventHandler* handler,
                       const ctkServiceReference& handlerRef)
{
  if(&nullEventHandler != handler)
  {
    // Is the handler not unregistered or blacklisted?
    if(!blackList->contains(handlerRef) &&
        (handlerRef.getPlugin()))
    {
      context->ungetService(handlerRef);
    }
  }
}

template<class BlackList, class TopicHandlerFilters, class Filters>
void
ctkEABlacklistingHandlerTasks<BlackList, TopicHandlerFilters, Filters>::
checkNull(void* object, const QString& name)
{
  if(object == 0)
  {
    throw ctkInvalidArgumentException(qPrintable(name + " may not be null"));
  }
}

