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


#ifndef CTKEACACHETOPICHANDLERFILTERS_P_H
#define CTKEACACHETOPICHANDLERFILTERS_P_H

#include "ctkEATopicHandlerFilters_p.h"
#include <util/ctkEACacheMap_p.h>

/**
 * The factory for <tt>ctkEventHandler</tt> filters based on a certain topic. This
 * implementation uses a cache to speed-up filter creation.
 */
template<class CacheMap>
class ctkEACacheTopicHandlerFilters : public ctkEATopicHandlerFilters<ctkEACacheTopicHandlerFilters<CacheMap> >
{
private:

  // The cache
  typedef ctkEACacheMap<typename CacheMap::KeyType, typename CacheMap::ValueType, CacheMap> CacheMapInterface;
  CacheMapInterface* cache;

  QString filterStart;

public:

  /**
   * The constructor of the filter factory.
   *
   * @param cache The cache to use in order to speed-up filter creation.
   *
   * @param requireTopic Include handlers that do not provide a topic
   */
  ctkEACacheTopicHandlerFilters(CacheMapInterface* cache, bool requireTopic);

  ~ctkEACacheTopicHandlerFilters();

  /**
   * Create a filter that will match all <tt>ctkEventHandler</tt> services that match
   * the given topic.
   *
   * @param topic The topic to match
   *
   * @return A filter that will match all <tt>ctkEventHandler</tt> services for
   *      the given topic.
   *
   * @see ctkEATopicHandlerFilters#createFilterForTopic(const QString&)
   */
  QString createFilterForTopic(const QString& topic);
};

#include "ctkEACacheTopicHandlerFilters.tpp"

#endif // CTKEACACHETOPICHANDLERFILTERS_P_H
