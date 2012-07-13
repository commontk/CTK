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

#ifndef CTKPLUGINFRAMEWORKPRIVATE_P_H
#define CTKPLUGINFRAMEWORKPRIVATE_P_H

#include <QMutex>

#include "ctkPlugin_p.h"
#include "ctkPluginFramework.h"

class ctkPluginFrameworkContext;

/**
 * \ingroup PluginFramework
 */
class ctkPluginFrameworkPrivate : public ctkPluginPrivate
{
public:

  /**
   * Holds all information for constructing a ctkPluginFrameworkEvent instance,
   * except for holding a QSharedPointer to the framework plug-in. This avoids
   * cyclic references in ctkPluginFramework.
   */
  struct FWEventWrapper
  {
    FWEventWrapper() : isNull(true) {}
    bool isNull;
    ctkPluginFrameworkEvent::Type type;
  };

  /**
   * The event to return to callers waiting in ctkPluginFramework::waitForStop()
   * when the framework has been stopped.
   */
  FWEventWrapper stopEvent;

  /**
   * The flag indicating that the thread that performs shutdown of this
   * framework instance is running.
   */
  QAtomicInt shuttingDown;

  /**
   * Lock object
   */
  LockObject lock;

  ctkPluginFrameworkPrivate(QWeakPointer<ctkPlugin> qq, ctkPluginFrameworkContext* fw);

  void init();

  void initSystemPlugin();

  void uninitSystemPlugin();

  /**
   * This method starts a thread that stops this Framework,
   * stopping all started plug-ins.
   *
   * <p>If the framework is not started, this method does nothing.
   * If the framework is started, this method will:
   * <ol>
   * <li>Set the state of the ctkPluginFrameworkContext to <i>inactive</i>.</li>
   * <li>Suspended all started plug-ins as described in the
   * {@link ctkPlugin#stop()} method except that the persistent
   * state of the plug-in will continue to be started.
   * Reports any exceptions that occur during stopping using
   * <code>ctkPluginFramework</code>s.</li>
   * <li>Disable event handling.</li>
   * </ol></p>
   *
   */
  void shutdown(bool restart);

  QHash<QString, QString> systemHeaders;

private:

  /**
   * Stop this FrameworkContext, suspending all started contexts.
   * This method suspends all started contexts so that they can be
   * automatically restarted when this FrameworkContext is next launched.
   *
   * <p>If the framework is not started, this method does nothing.
   * If the framework is started, this method will:
   * <ol>
   * <li>Set the state of the FrameworkContext to <i>inactive</i>.</li>
   * <li>Stop all started bundles as described in the
   * {@link Bundle#stop(int)} method except that the persistent
   * state of the bundle will continue to be started.
   * Reports any exceptions that occur during stopping using
   * <code>FrameworkErrorEvents</code>.</li>
   * <li>Disable event handling.</li>
   * </ol>
   * </p>
   *
   */
  void shutdown0(bool restart, bool wasActive);

  /**
   * Tell system plugin shutdown finished.
   */
  void shutdownDone_unlocked(bool restart);

  /**
   * Stop and unresolve all plug-ins.
   */
  void stopAllPlugins();

  /**
   * Shutting down is done.
   */
  void systemShuttingdownDone(const ctkPluginFrameworkEvent& fe);

  void systemShuttingdownDone_unlocked(const ctkPluginFrameworkEvent& fe);

};


#endif // CTKPLUGINFRAMEWORKPRIVATE_P_H
