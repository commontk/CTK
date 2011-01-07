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


#ifndef CTKEALEASTRECENTLYUSEDCACHEMAP_P_H
#define CTKEALEASTRECENTLYUSEDCACHEMAP_P_H

#include <QHash>
#include <QList>
#include <QMutex>

#include "ctkEACacheMap_p.h"

/**
 * This class implements a least recently used cache map. It will hold
 * a given size of key-value pairs and drop the least recently used entry once this
 * size is reached. This class is thread safe.
 */
template<typename K, typename V>
class ctkEALeastRecentlyUsedCacheMap : public ctkEACacheMap<K,V, ctkEALeastRecentlyUsedCacheMap<K,V> >
{

private:

  // The internal lock for this object
  mutable QMutex mutex;

  // The max number of entries in the cache. Once reached entries are replaced
  const int maxSize;

  // The cache
  QHash<K,V> cache;

  // The history used to determine the least recently used entries. The end of the
  // list is the most recently used key. In other words history.front() returns
  // the least recently used key.
  mutable QList<K> history;

public:

  typedef K KeyType;
  typedef V ValueType;

  /**
   * The constructor of the cache. The given max size will be used to determine the
   * size of the cache that triggers replacing least recently used entries with
   * new ones.
   *
   * @param maxSize The max number of entries in the cache
   */
  ctkEALeastRecentlyUsedCacheMap(int maxSize);

  /**
   * Returns the value for the key in case there is one. Additionally, the
   * LRU counter for the key is updated.
   *
   * @param key The key for the value to return
   *
   * @return The value of the key in case there is one, a default constructed value otherwise
   *
   */
  const V value(const K& key) const;

  /**
   * Returns the value for the key in case there is one. Additionally, the
   * LRU counter for the key is updated.
   *
   * @param key The key for the value to return
   * @param defaultValue The value to return in case the key does not exist.
   *
   * @return The value of the key in case there is one, the defaultValue otherwise.
   *
   */
  const V value(const K& key, const V& defaultValue) const;

  /**
   * Add the key-value pair to the cache. The key will be come the most recently
   * used entry. In case max size is (or has been) reached this will remove the
   * least recently used entry in the cache. In case that the cache already
   * contains this specific key-value pair it LRU counter is updated only.
   *
   * @param key The key for the value
   * @param value The value for the key
   */
  void insert(const K& key, const V& value);

  /**
   * Remove the entry denoted by key from the cache and return its value.
   *
   * @param key The key of the entry to be removed
   *
   * @return The value of the entry removed, a default-constructed value if none
   */
  const V remove(const K& key);

  /**
   * Return the current size of the cache.
   *
   * @return The number of entries currently in the cache.
   */
  int size() const;

  /**
   * Remove all entries from the cache.
   */
  void clear();

};

#include "ctkEALeastRecentlyUsedCacheMap.tpp"

#endif // CTKEALEASTRECENTLYUSEDCACHEMAP_P_H
