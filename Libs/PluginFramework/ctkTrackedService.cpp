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


#include "ctkTrackedService_p.h"

#include "ctkServiceTracker.h"
#include "ctkServiceTrackerPrivate.h"

ctkTrackedService::ctkTrackedService(ctkServiceTracker* serviceTracker,
                  ctkServiceTrackerCustomizer* customizer)
  : serviceTracker(serviceTracker), customizer(customizer)
{

}

void ctkTrackedService::serviceChanged(const ctkServiceEvent& event)
{
  /*
   * Check if we had a delayed call (which could happen when we
   * close).
   */
  if (closed)
  {
    return;
  }

  ctkServiceReference reference = event.getServiceReference();
  if (serviceTracker->d_func()->DEBUG)
  {
    qDebug() << "ctkTrackedService::serviceChanged["
        << event.getType() << "]: " << reference;
  }

  switch (event.getType())
  {
  case ctkServiceEvent::REGISTERED :
  case ctkServiceEvent::MODIFIED :
    {
      if (!serviceTracker->d_func()->listenerFilter.isNull())
      { // service listener added with filter
        track(reference, event);
        /*
       * If the customizer throws an unchecked exception, it
       * is safe to let it propagate
       */
      }
      else
      { // service listener added without filter
        if (serviceTracker->d_func()->filter.match(reference))
        {
          track(reference, event);
          /*
         * If the customizer throws an unchecked exception,
         * it is safe to let it propagate
         */
        }
        else
        {
          untrack(reference, event);
          /*
         * If the customizer throws an unchecked exception,
         * it is safe to let it propagate
         */
        }
      }
      break;
    }
  case ctkServiceEvent::MODIFIED_ENDMATCH :
  case ctkServiceEvent::UNREGISTERING :
    untrack(reference, event);
    /*
     * If the customizer throws an unchecked exception, it is
     * safe to let it propagate
     */
    break;
  }
}

void ctkTrackedService::modified()
{
  Superclass::modified(); /* increment the modification count */
  serviceTracker->d_func()->modified();
}

QVariant ctkTrackedService::customizerAdding(ctkServiceReference item, ctkServiceEvent related)
{
  Q_UNUSED(related)
  QVariant var;
  var.setValue(customizer->addingService(item));
  return var;
}

void ctkTrackedService::customizerModified(ctkServiceReference item,
                        ctkServiceEvent related, QVariant object)
{
  Q_UNUSED(related)
  customizer->modifiedService(item, object.value<QObject*>());
}

void ctkTrackedService::customizerRemoved(ctkServiceReference item,
                       ctkServiceEvent related, QVariant object)
{
  Q_UNUSED(related)
  customizer->removedService(item, object.value<QObject*>());
}
