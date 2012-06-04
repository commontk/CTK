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


template<class CacheMap>
ctkEACacheTopicHandlerFilters<CacheMap>::
ctkEACacheTopicHandlerFilters(CacheMapInterface* cache, bool requireTopic)
  : cache(cache)
{
  if(cache == 0)
  {
    throw ctkInvalidArgumentException("Cache may not be null");
  }

  filterStart = QString("(|")
      + (requireTopic ? QString("") : QString("(!(") + ctkEventConstants::EVENT_TOPIC + "=*))")
      + "(" + ctkEventConstants::EVENT_TOPIC + "=\\*)(" + ctkEventConstants::EVENT_TOPIC + "=";
}

template<class CacheMap>
ctkEACacheTopicHandlerFilters<CacheMap>::
~ctkEACacheTopicHandlerFilters()
{
  delete cache;
}

template<class CacheMap>
QString
ctkEACacheTopicHandlerFilters<CacheMap>::
createFilterForTopic(const QString& topic)
{
  // build the ldap-query - as a simple example:
  // topic=org/commontk/TEST
  // result = (|(topic=\*)(topic=org/\*)(topic=org/commontk/\*)
  //           (topic=org/commontk/TEST))
  QString result = cache->value(topic);

  if(result.isEmpty())
  {
    QString filter = filterStart;

    for (int i = 0; i < topic.size(); ++i)
    {
      if (QChar('/') == topic.at(i))
      {
        filter.append('/').append('\\').append('*').append(')');
        filter.append('(').append(ctkEventConstants::EVENT_TOPIC).append('=').append(
              topic.left(i+1));
      }
      else
      {
        filter.append(topic.at(i));
      }
    }

    filter.append(')').append(')');

    result = filter;

    cache->insert(topic, result);
  }

  return result;
}

