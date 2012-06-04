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


template<typename K, typename V>
ctkEALeastRecentlyUsedCacheMap<K,V>::
ctkEALeastRecentlyUsedCacheMap(int maxSize)
  : maxSize(maxSize)
{
  if(0 >= maxSize)
  {
    throw ctkInvalidArgumentException("Size must be positive");
  }

  // We need one more entry then m_maxSize in the cache and a HashMap is
  // expanded when it reaches 3/4 of its size hence, the funny numbers.
  cache.reserve(((maxSize + 1) * 4)/3);
}

template<typename K, typename V>
const V
ctkEALeastRecentlyUsedCacheMap<K,V>::
value(const K& key) const
{
  QMutexLocker lock(&mutex);
  if (cache.contains(key))
  {
    history.removeAll(key);
    history.push_back(key);
    return cache.value(key);
  }
  else
  {
    return V();
  }
}

template<typename K, typename V>
const V
ctkEALeastRecentlyUsedCacheMap<K,V>::
value(const K& key, const V& defaultValue) const
{
  QMutexLocker lock(&mutex);
  if (cache.contains(key))
  {
    history.removeAll(key);
    history.push_back(key);
    return cache.value(key);
  }
  else
  {
    return defaultValue;
  }
}

template<typename K, typename V>
void
ctkEALeastRecentlyUsedCacheMap<K,V>::
insert(const K& key, const V& value)
{
  QMutexLocker lock(&mutex);

  if (cache.contains(key))
  {
    history.removeAll(key);
  }

  cache.insert(key, value);
  history.push_back(key);

  if(maxSize < cache.size())
  {
    cache.remove(history.takeFirst());
  }
}

template<typename K, typename V>
const V
ctkEALeastRecentlyUsedCacheMap<K,V>::
remove(const K& key)
{
  QMutexLocker lock(&mutex);
  if (cache.contains(key))
  {
    history.removeAll(key);
  }

  return cache.take(key);
}

template<typename K, typename V>
int
ctkEALeastRecentlyUsedCacheMap<K,V>::
size() const
{
  QMutexLocker lock(&mutex);
  return cache.size();
}

template<typename K, typename V>
void
ctkEALeastRecentlyUsedCacheMap<K,V>::
clear()
{
  QMutexLocker lock(&mutex);
  cache.clear();
  history.clear();
}

