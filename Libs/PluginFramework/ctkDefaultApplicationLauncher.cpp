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

#include "ctkDefaultApplicationLauncher_p.h"

#include <service/application/ctkApplicationDescriptor.h>

#include <ctkApplicationRunnable.h>
#include <ctkException.h>
#include <ctkPluginConstants.h>
#include <ctkPluginContext.h>
#include <ctkPluginFrameworkLauncher.h>

#include <ctkPluginFrameworkProperties_p.h>

#include <QVariant>
#include <QDebug>


//----------------------------------------------------------------------------
struct FreeResources
{
  ctkDefaultApplicationLauncher* launcher;

  FreeResources(ctkDefaultApplicationLauncher* launcher)
    : launcher(launcher)
  {}

  ~FreeResources()
  {
    //if (Profile.PROFILE && Profile.STARTUP)
    //  Profile.logExit("ctkPluginFrameworkLauncher.run(QVariant)()");

    // free the runnable application and release the lock to allow another app to be launched.
    launcher->runnable = NULL;
    launcher->appContext.clear();
    launcher->runningLock.release();
  }
};

//----------------------------------------------------------------------------
ctkDefaultApplicationLauncher::ctkDefaultApplicationLauncher(ctkPluginContext* context, bool relaunch, bool failOnNoDefault)
  : runnable(NULL)
  , runningLock(1)
  , waitForAppLock(0)
  , context(context)
  , relaunch(relaunch)
  , failOnNoDefault(failOnNoDefault)
{
}

//----------------------------------------------------------------------------
QVariant ctkDefaultApplicationLauncher::start(const QVariant& defaultContext)
{
  // here we assume that launch has been called by runtime before we started
  // TODO this may be a bad assumption but it works for now because we register the app launcher as a service and runtime synchronously calls launch on the service
  if (failOnNoDefault && runnable == NULL)
  {
    throw ctkIllegalStateException("Unable to acquire application service. Ensure that an application container is active");
  }
  QVariant result;
  bool doRelaunch = false;
  do
  {
    try
    {
      result = runApplication(defaultContext);
    }
    catch (const std::exception& e)
    {
      if (!relaunch || (context->getPlugin()->getState() != ctkPlugin::ACTIVE))
      {
        throw;
      }
      qWarning() << "Application error:" << e.what();
    }
    doRelaunch = (relaunch && context->getPlugin()->getState() == ctkPlugin::ACTIVE) ||
                  ctkPluginFrameworkProperties::getProperty(ctkPluginFrameworkLauncher::PROP_OSGI_RELAUNCH).toBool();
  }
  while (doRelaunch);

  return result;
}

//----------------------------------------------------------------------------
void ctkDefaultApplicationLauncher::launch(ctkApplicationRunnable* app, const QVariant& applicationContext)
{
  waitForAppLock.tryAcquire(); // clear out any pending apps notifications
  if (!runningLock.tryAcquire()) // check to see if an application is currently running
  {
    throw ctkIllegalStateException("An application is aready running.");
  }
  this->runnable = app;
  this->appContext = applicationContext;
  waitForAppLock.release(); // notify the main thread to launch an application.
  runningLock.release(); // release the running lock
}

//----------------------------------------------------------------------------
void ctkDefaultApplicationLauncher::shutdown()
{
  // this method will aquire and keep the runningLock to prevent
  // all future application launches.
  if (runningLock.tryAcquire())
  {
    return; // no application is currently running.
  }
  ctkApplicationRunnable* currentRunnable = runnable;
  currentRunnable->stop();
  runningLock.tryAcquire(1, 60000); // timeout after 1 minute.
}

//----------------------------------------------------------------------------
QVariant ctkDefaultApplicationLauncher::reStart(const QVariant& argument)
{
  QList<ctkServiceReference> refs;
  refs = context->getServiceReferences<ctkApplicationDescriptor>("(ctk.application.default=true)");
  if (!refs.isEmpty())
  {
    ctkApplicationDescriptor* defaultApp = context->getService<ctkApplicationDescriptor>(refs.front());
    defaultApp->launch(QHash<QString, QVariant>());
    return start(argument);
  }
  throw ctkIllegalStateException("Unable to acquire application service. Ensure that an application container is active");
}

//----------------------------------------------------------------------------
QVariant ctkDefaultApplicationLauncher::runApplication(const QVariant& defaultContext)
{

  // wait for an application to be launched.
  waitForAppLock.acquire();
  // an application is ready; acquire the running lock.
  // this must happen after we have acquired an application (by acquiring waitForAppLock above).
  runningLock.acquire();

  // run the actual application on the current thread (main).
  FreeResources cleanup(this);
  return runnable->run(appContext.isValid() ? appContext : defaultContext);
}
