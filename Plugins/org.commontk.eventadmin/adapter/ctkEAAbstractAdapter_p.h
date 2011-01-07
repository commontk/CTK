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


#ifndef CTKEAABSTRACTADAPTER_P_H
#define CTKEAABSTRACTADAPTER_P_H

#include <QAtomicPointer>

#include <service/event/ctkEventAdmin.h>

class ctkPluginContext;

/**
 * Abstract base class for all adapters.
 * This class allows to exchange the event admin at runtime
 */
class ctkEAAbstractAdapter
{

private:

  mutable QAtomicPointer<ctkEventAdmin> admin_;

public:

  virtual ~ctkEAAbstractAdapter() {}

  /**
   * The constructor of the adapter.
   *
   * @param admin The <tt>ctkEventAdmin</tt> to use for posting events.
   */
  ctkEAAbstractAdapter(ctkEventAdmin* admin);

  virtual void destroy(ctkPluginContext* pluginContext) = 0;

protected:

  ctkEventAdmin* getEventAdmin() const;

};

#endif // CTKEAABSTRACTADAPTER_P_H
