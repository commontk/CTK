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


#ifndef CTKEATOPICHANDLERFILTERS_P_H
#define CTKEATOPICHANDLERFILTERS_P_H

#include <QString>

/**
 * The factory for <tt>ctkEventHandler</tt> filters based on a certain topic.
 */
template<class Impl>
struct ctkEATopicHandlerFilters
{
  /**
   * Create a filter that will match all <tt>ctkEventHandler</tt> services that match
   * the given topic.
   *
   * @param topic The topic to match
   *
   * @return A filter that will match all <tt>ctkEventHandler</tt> services for
   *      the given topic.
   */
  QString createFilterForTopic(const QString& topic)
  {
    return static_cast<Impl*>(this)->createFilterForTopic(topic);
  }

  virtual ~ctkEATopicHandlerFilters() {}
};

#endif // CTKEATOPICHANDLERFILTERS_P_H
