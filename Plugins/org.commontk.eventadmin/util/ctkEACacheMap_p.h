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


#ifndef CTKEACACHEMAP_P_H
#define CTKEACACHEMAP_P_H

/**
 * This is the interface of a simple cache map.
*/
template<class K, class V, class Impl>
struct ctkEACacheMap
{
  virtual ~ctkEACacheMap() {}

  /**
   * Return the value for the key in case there is one in the cache.
   *
   * @param key The key to look-up
   *
   * @return The value for the given key in case there is one in the cache,
   *         <tt>null</tt> otherwise
   */
  const V value(const K& key) const
  {
    return static_cast<const Impl*>(this)->value(key);
  }

  const V value(const K& key, const V& defaultValue) const
  {
    return static_cast<const Impl*>(this)->value(key, defaultValue);
    }

  /**
   * Add a value for the key to this cache.
   *
   * @param key The key for the value
   * @param value The value to add to the cache
   */
  void insert(const K& key, const V& value)
  {
    static_cast<Impl*>(this)->insert(key, value);
  }

  /**
   * Remove a key and its value from the cache.
   *
   * @param key The key to remove
   *
   * @return The value of the key in case there is one in the cache, <tt>null</tt>
   *         otherwise
   */
  const V remove(const K& key)
  {
    return static_cast<Impl*>(this)->remove(key);
  }

  /**
   * Returns the number of key-value pairs in this cache.
   *
   * @return The number of key-value pairs in this cache
   */
  int size() const
  {
    return static_cast<const Impl*>(this)->size();
  }

  /**
   * Remove all entries of the cache.
   */
  void clear()
  {
    static_cast<Impl*>(this)->clear();
  }
};


#endif // CTKEACACHEMAP_P_H
