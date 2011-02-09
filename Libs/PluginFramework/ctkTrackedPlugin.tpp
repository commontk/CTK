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

//----------------------------------------------------------------------------
template<class T>
ctkTrackedPlugin<T>::ctkTrackedPlugin(ctkPluginTracker<T>* pluginTracker,
                 ctkPluginTrackerCustomizer<T>* customizer)
  : pluginTracker(pluginTracker), customizer(customizer)
{

}

//----------------------------------------------------------------------------
template<class T>
void ctkTrackedPlugin<T>::pluginChanged(const ctkPluginEvent& event)
{
  /*
   * Check if we had a delayed call (which could happen when we
   * close).
   */
  if (this->closed)
  {
    return;
  }

  QSharedPointer<ctkPlugin> plugin = event.getPlugin();
  ctkPlugin::State state = plugin->getState();
  if (pluginTracker->d_func()->DEBUG)
  {
    qDebug() << "ctkTrackedPlugin<T>::pluginChanged[" << state << "]: " << *plugin;
  }

  if (pluginTracker->d_func()->mask & state)
  {
    this->track(plugin, event);
    /*
     * If the customizer throws an exception, it is safe
     * to let it propagate
     */
  }
  else
  {
    this->untrack(plugin, event);
    /*
     * If the customizer throws an exception, it is safe
     * to let it propagate
     */
  }
}

//----------------------------------------------------------------------------
template<class T>
T ctkTrackedPlugin<T>::customizerAdding(QSharedPointer<ctkPlugin> item,
                                     const ctkPluginEvent& related)
{
  return customizer->addingPlugin(item, related);
}

//----------------------------------------------------------------------------
template<class T>
void ctkTrackedPlugin<T>::customizerModified(QSharedPointer<ctkPlugin> item,
                                          const ctkPluginEvent& related,
                                          T object)
{
  customizer->modifiedPlugin(item, related, object);
}

//----------------------------------------------------------------------------
template<class T>
void ctkTrackedPlugin<T>::customizerRemoved(QSharedPointer<ctkPlugin> item,
                                         const ctkPluginEvent& related,
                                         T object)
{
  customizer->removedPlugin(item, related, object);
}
