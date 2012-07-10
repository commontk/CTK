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
ctkEACacheFilters<CacheMap>::
ctkEACacheFilters(CacheMap* cache,
                  ctkPluginContext* context)
  : cache(cache), context(context)
{
  if(cache == 0)
  {
    throw ctkInvalidArgumentException("Cache may not be null");
  }

  if(context == 0)
  {
    throw ctkInvalidArgumentException("Context may not be null");
  }
}

template<class CacheMap>
ctkEACacheFilters<CacheMap>::
~ctkEACacheFilters()
{
  delete cache;
}

template<class CacheMap>
ctkLDAPSearchFilter
ctkEACacheFilters<CacheMap>::
createFilter(const QString& filter)
{
  ctkLDAPSearchFilter result = filter.isEmpty() ? this->TRUE_FILTER : cache->value(filter);
  if (!result)
  {
    result = ctkLDAPSearchFilter(filter);
    cache->insert(filter, result);
  }
  return result;
}

