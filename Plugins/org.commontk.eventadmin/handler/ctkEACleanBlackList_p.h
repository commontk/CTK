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


#ifndef CTKEACLEANBLACKLIST_P_H
#define CTKEACLEANBLACKLIST_P_H

#include <QMutex>
#include <QSet>

#include <ctkServiceReference.h>

#include "ctkEABlackList_p.h"

/**
 * This class implements a <tt>ctkEABlackList</tt> that removes references to unregistered
 * services automatically.
 *
 * @see ctkEABlackList
 */
class ctkEACleanBlackList : public ctkEABlackList<ctkEACleanBlackList>
{
private:

  mutable QMutex mutex;

  mutable QSet<ctkServiceReference> blackList;

public:

  /**
   * Add a service to this blacklist.
   *
   * @param ref The reference of the service that is blacklisted
   */
  void add(const ctkServiceReference& ref);

  /**
  * Lookup whether a given service is blacklisted.
  *
  * @param ref The reference of the service
  *
  * @return <tt>true</tt> in case that the service reference has been blacklisted,
  *      <tt>false</tt> otherwise.
  */
  bool contains(const ctkServiceReference& ref) const;

};

#endif // CTKEACLEANBLACKLIST_P_H
