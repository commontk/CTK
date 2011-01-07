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


#include "ctkEACleanBlackList_p.h"

void ctkEACleanBlackList::add(const ctkServiceReference& ref)
{
  QMutexLocker lock(&mutex);
  blackList.insert(ref);
}

bool ctkEACleanBlackList::contains(const ctkServiceReference& ref) const
{
  QMutexLocker lock(&mutex);

  // This removes stale (i.e., unregistered) references on any call to implContains
  foreach (ctkServiceReference ref, blackList)
  {
    if (!ref)
    {
      blackList.remove(ref);
    }
  }

  return blackList.contains(ref);
}
