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

#ifndef CTKDEFAULTAPPLICATIONLAUNCHER_H
#define CTKDEFAULTAPPLICATIONLAUNCHER_H

#include <service/application/ctkApplicationLauncher.h>

#include <QSemaphore>
#include <QVariant>

class ctkPluginContext;

/**
 * A ctkApplicationLauncher is used to launch ctkApplicationRunnable objects using
 * the main thread.
 * <p>
 * This class is for use by platform-related plug-ins which implement application containers.
 * Clients outside of the base platform should not reference or subclass this class.
 * </p>
 */
class ctkDefaultApplicationLauncher : public QObject, public ctkApplicationLauncher
{
  Q_OBJECT
  Q_INTERFACES(ctkApplicationLauncher)

private:

  ctkApplicationRunnable* runnable;
  QVariant appContext;
  QSemaphore runningLock;
  QSemaphore waitForAppLock;
  ctkPluginContext* context;
  bool relaunch;
  bool failOnNoDefault;
  // FrameworkLog log;

public:

  ctkDefaultApplicationLauncher(ctkPluginContext* context, bool relaunch, bool failOnNoDefault);

  /**
   * Starts this application launcher on the current thread.  This method
   * should be called by the main thread to ensure that applications are
   * launched in the main thread.
   */
  QVariant start(const QVariant& defaultContext);

  /**
     * Launches the specified runnable using the main thread.
     * @param runnable a ParameterizedRunnalbe to run on the main thread.
     * @param context the context to launch the runnable with
     */
  void launch(ctkApplicationRunnable* app, const QVariant& applicationContext);

  /**
   * Forces the current runnable which is running to be stopped.
   * This method will return after the currently running ctkApplicationRunnable
   * has completely stopped.
   * <p>
   * After this method returns this ctkApplicationLauncher will no longer allow
   * applications to be launched.
   */
  void shutdown();

  /**
   * Similar to the start method this method will restart the default method on the current thread.
   * This method assumes that the default application was launched at least once and that a ctkApplicationDescriptor
   * exists that can be used to relaunch the default application.
   * @param argument the new context to re-start the default application with
   */
  QVariant reStart(const QVariant& argument);


private:

  /*
   * Waits for an application to be launched and the runs the application on the
   * current thread (main).
   */
  QVariant runApplication(const QVariant& defaultContext);

  friend struct FreeResources;

};

#endif // CTKDEFAULTAPPLICATIONLAUNCHER_H
