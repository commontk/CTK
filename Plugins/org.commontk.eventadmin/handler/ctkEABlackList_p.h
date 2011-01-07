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


#ifndef CTKEABLACKLIST_H
#define CTKEABLACKLIST_H

/**
 * This interface represents a simple set that allows to add service references
 * and lookup whether a given reference is in the list. Note that implementations
 * of this interface may do additional service reference life-cycle related
 * clean-up actions like removing references that point to unregistered services.
 */
template<class Impl>
struct ctkEABlackList
{
  /**
   * Add a service to this blacklist.
   *
   * @param ref The reference of the service that is blacklisted
   */
  void add(const ctkServiceReference& ref)
  {
    static_cast<Impl*>(this)->add(ref);
  }

  /**
   * Lookup whether a given service is blacklisted.
   *
   * @param ref The reference of the service
   *
   * @return <tt>true</tt> in case that the service reference has been blacklisted,
   *      <tt>false</tt> otherwise.
   */
  bool contains(const ctkServiceReference& ref) const
  {
    return static_cast<const Impl*>(this)->contains(ref);
  }

  virtual ~ctkEABlackList() {}
};

#endif // CTKEABLACKLIST_H
