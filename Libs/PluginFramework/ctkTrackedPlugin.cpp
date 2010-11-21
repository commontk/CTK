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


#include "ctkTrackedPlugin_p.h"

#include "ctkPluginTracker.h"
#include "ctkPluginTrackerPrivate.h"

ctkTrackedPlugin::ctkTrackedPlugin(ctkPluginTracker* pluginTracker,
                 ctkPluginTrackerCustomizer* customizer)
  : pluginTracker(pluginTracker), customizer(customizer)
{

}

void ctkTrackedPlugin::pluginChanged(const ctkPluginEvent& event)
{
  /*
   * Check if we had a delayed call (which could happen when we
   * close).
   */
  if (closed)
  {
    return;
  }

  QSharedPointer<ctkPlugin> plugin = event.getPlugin();
  ctkPlugin::State state = plugin->getState();
  if (pluginTracker->d_func()->DEBUG)
  {
    qDebug() << "ctkTrackedPlugin::pluginChanged[" << state << "]: " << *plugin;
  }

  if (pluginTracker->d_func()->mask & state)
  {
    track(plugin, event);
    /*
     * If the customizer throws an exception, it is safe
     * to let it propagate
     */
  }
  else
  {
    untrack(plugin, event);
    /*
     * If the customizer throws an exception, it is safe
     * to let it propagate
     */
  }
}

QVariant ctkTrackedPlugin::customizerAdding(QSharedPointer<ctkPlugin> item,
    ctkPluginEvent related)
{
  return customizer->addingPlugin(item, related);
}

void ctkTrackedPlugin::customizerModified(QSharedPointer<ctkPlugin> item,
    ctkPluginEvent related, QVariant object)
{
  customizer->modifiedPlugin(item, related, object);
}

void ctkTrackedPlugin::customizerRemoved(QSharedPointer<ctkPlugin> item,
    ctkPluginEvent related, QVariant object)
{
  customizer->removedPlugin(item, related, object);
}
