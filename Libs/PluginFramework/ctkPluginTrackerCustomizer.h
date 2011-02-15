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


#ifndef CTKPLUGINTRACKERCUSTOMIZER_H
#define CTKPLUGINTRACKERCUSTOMIZER_H

#include <QVariant>

#include "ctkPluginEvent.h"

class QObject;
class ctkPlugin;

/**
 * \ingroup PluginFramework
 *
 * The <code>ctkPluginTrackerCustomizer</code> interface allows a
 * <code>ctkPluginTracker</code> to customize the <code>ctkPlugin</code>s that are
 * tracked. A <code>ctkPluginTrackerCustomizer</code> is called when a plugin is
 * being added to a <code>ctkPluginTracker</code>. The
 * <code>ctkPluginTrackerCustomizer</code> can then return an object for the
 * tracked plugin. A <code>ctkPluginTrackerCustomizer</code> is also called when a
 * tracked plugin is modified or has been removed from a
 * <code>ctkPluginTracker</code>.
 *
 * <p>
 * The methods in this interface may be called as the result of a
 * <code>ctkPluginEvent</code> being received by a <code>ctkPluginTracker</code>.
 * Since <code>ctkPluginEvent</code>s are received synchronously by the
 * <code>ctkPluginTracker</code>, it is highly recommended that implementations of
 * these methods do not alter plugin states while being synchronized on any
 * object.
 *
 * <p>
 * The <code>ctkPluginTracker</code> class is thread-safe. It does not call a
 * <code>ctkPluginTrackerCustomizer</code> while holding any locks.
 * <code>ctkPluginTrackerCustomizer</code> implementations must also be
 * thread-safe.
 *
 * \tparam T The type of the tracked object.
 * \remarks This class is thread safe.
 */
template<class T>
struct ctkPluginTrackerCustomizer {

  virtual ~ctkPluginTrackerCustomizer() {}

  /**
   * A plugin is being added to the <code>ctkPluginTracker</code>.
   *
   * <p>
   * This method is called before a plugin which matched the search parameters
   * of the <code>ctkPluginTracker</code> is added to the
   * <code>ctkPluginTracker</code>. This method should return the object to be
   * tracked for the specified <code>ctkPlugin</code>. The returned object is
   * stored in the <code>ctkPluginTracker</code> and is available from the
   * ctkPluginTracker::getObject(ctkPlugin*) method.
   *
   * @param plugin The <code>ctkPlugin</code> being added to the
   *        <code>ctkPluginTracker</code>.
   * @param event The plugin event which caused this customizer method to be
   *        called or an invalid event (ctkPluginEvent::isNull() returns <code>true</code>)
   *        if there is no plugin event associated
   *        with the call to this method.
   * @return The object to be tracked for the specified <code>ctkPlugin</code>
   *         object or <code>null</code> if the specified <code>ctkPlugin</code>
   *         object should not be tracked.
   */
  virtual T addingPlugin(QSharedPointer<ctkPlugin> plugin, const ctkPluginEvent& event) = 0;

  /**
   * A plugin tracked by the <code>ctkPluginTracker</code> has been modified.
   *
   * <p>
   * This method is called when a plugin being tracked by the
   * <code>ctkPluginTracker</code> has had its state modified.
   *
   * @param plugin The <code>ctkPlugin</code> whose state has been modified.
   * @param event The plugin event which caused this customizer method to be
   *        called or an invalid event (ctkPluginEvent::isNull() returns <code>true</code>)
   *        if there is no plugin event associated
   *        with the call to this method.
   * @param object The tracked object for the specified plugin.
   */
  virtual void modifiedPlugin(QSharedPointer<ctkPlugin> plugin, const ctkPluginEvent& event,
      T object) = 0;

  /**
   * A plugin tracked by the <code>ctkPluginTracker</code> has been removed.
   *
   * <p>
   * This method is called after a plugin is no longer being tracked by the
   * <code>ctkPluginTracker</code>.
   *
   * @param plugin The <code>ctkPlugin</code> that has been removed.
   * @param event The plugin event which caused this customizer method to be
   *        called or an invalid event (ctkPluginEvent::isNull() returns <code>true</code>)
   *        if there is no plugin event associated
   *        with the call to this method.
   * @param object The tracked object for the specified plugin.
   */
  virtual void removedPlugin(QSharedPointer<ctkPlugin> plugin, const ctkPluginEvent& event,
      T object) = 0;
};

#endif // CTKPLUGINTRACKERCUSTOMIZER_H
