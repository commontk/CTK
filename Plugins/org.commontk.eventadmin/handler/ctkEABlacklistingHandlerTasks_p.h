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


#ifndef CTKEABLACKLISTINGHANDLERTASKS_P_H
#define CTKEABLACKLISTINGHANDLERTASKS_P_H

#include "ctkEAHandlerTasks_p.h"

#include <ctkPluginContext.h>
#include <ctkEventAdminActivator_p.h>
#include <service/event/ctkEventConstants.h>
#include <service/event/ctkEventHandler.h>

#include "ctkEATopicHandlerFilters_p.h"
#include "ctkEAFilters_p.h"
#include "ctkEABlackList_p.h"

/**
 * This class is an implementation of the ctkEAHandlerTasks interface that does provide
 * blacklisting of event handlers. Furthermore, handlers are determined from the
 * framework on any call to <tt>createHandlerTasks()</tt> hence, there is no
 * book-keeping of <tt>ctkEventHandler</tt> services while they come and go but a
 * query for each sent event. In order to do this, an ldap-filter is created that
 * will match applicable <tt>ctkEventHandler</tt> references. In order to ease some of
 * the overhead pains of this approach some light caching is going on.
 */
template<class BlackList, class TopicHandlerFilters, class Filters>
class ctkEABlacklistingHandlerTasks :
    public ctkEAHandlerTasks<
    ctkEABlacklistingHandlerTasks<BlackList, TopicHandlerFilters, Filters> >
{

private:

  typedef ctkEABlacklistingHandlerTasks<BlackList, TopicHandlerFilters, Filters> Self;

  // The blacklist that holds blacklisted event handler service references
  ctkEABlackList<BlackList>* const blackList;

  // The context of the plugin used to get the actual event handler services
  ctkPluginContext* const context;

  // Used to create the filters that can determine applicable event handlers for
  // a given event
  ctkEATopicHandlerFilters<TopicHandlerFilters>* topicHandlerFilters;

  // Used to create the filters that are used to determine whether an applicable
  // event handler is interested in a particular event
  ctkEAFilters<Filters>* filters;

public:

  /**
   * The constructor of the factory.
   *
   * @param context The context of the plugin
   * @param blackList The set to use for keeping track of blacklisted references
   * @param topicHandlerFilters The factory for topic handler filters
   * @param filters The factory for <tt>ctkLDAPSearchFilter</tt> objects
   */
  ctkEABlacklistingHandlerTasks(ctkPluginContext* context,
                                ctkEABlackList<BlackList>* blackList,
                                ctkEATopicHandlerFilters<TopicHandlerFilters>* topicHandlerFilters,
                                ctkEAFilters<Filters>* filters);

  ~ctkEABlacklistingHandlerTasks();

  /**
   * Create the handler tasks for the event. All matching event handlers are
   * determined and delivery tasks for them returned.
   *
   * @param event The event for which' handlers delivery tasks must be created
   *
   * @return A delivery task for each handler that matches the given event
   *
   * @see ctkHandlerTasks#createHandlerTasks(const ctkEvent&)
   */
  QList<ctkEAHandlerTask<Self> > createHandlerTasks(const ctkEvent& event);

  /**
   * Blacklist the given service reference. This is a private method and only
   * public due to its usage in a friend class.
   *
   * @param handlerRef The service reference to blacklist
   */
  void blackListRef(const ctkServiceReference& handlerRef);

  /**
   * Get the real ctkEventHandler service for the handlerRef from the context in case
   * the ref is not blacklisted and the service is not unregistered. The
   * NullEventHandler object is returned otherwise. This is a private method and
   * only public due to its usage in a friend class.
   *
   * @param handlerRef The service reference for which to get its service
   * @return The service of the reference or a null object if the service is
   *      unregistered
   */
  ctkEventHandler* getEventHandler(const ctkServiceReference& handlerRef);

  /**
   * Unget the service reference for the given event handler unless it is the
   * NullEventHandler. This is a private method and only public due to
   * its usage in a friend class.
   *
   * @param handler The event handler service to unget
   * @param handlerRef The service reference to unget
   */
  void ungetEventHandler(ctkEventHandler* handler,
                         const ctkServiceReference& handlerRef);

private:

  /*
   * This is a null object that is supposed to do nothing. This is used once an
   * EventHandler is requested for a service reference that is either stale
   * (i.e., unregistered) or blacklisted
   */
  struct NullEventHandler : public ctkEventHandler
  {
    /**
     * This is a null object that is supposed to do nothing at this point.
     *
     * @param event an event that is not used
     */
    void handleEvent(const ctkEvent& /*event*/)
    {
      // This is a null object that is supposed to do nothing at this
      // point. This is used once a ctkEventHandler is requested for a
      // servicereference that is either stale (i.e., unregistered) or
      // blacklisted.
    }
  };

  NullEventHandler nullEventHandler;

  /*
   * This is a utility method that will throw a <tt>ctkInvalidArgumentException</tt>
   * in case that the given object is null. The message will be of the form name +
   * may not be null.
   */
  void checkNull(void* object, const QString& name);
};

#include "ctkEABlacklistingHandlerTasks.tpp"

#endif // CTKEABLACKLISTINGHANDLERTASKS_P_H
