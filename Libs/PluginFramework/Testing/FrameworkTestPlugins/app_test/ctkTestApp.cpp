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


#include "ctkTestApp_p.h"

#include <ctkPluginContext.h>
#include <service/application/ctkApplicationException.h>

#include <QDebug>
#include <QThread>
#include <QCoreApplication>


//----------------------------------------------------------------------------
MyAppDescriptor::MyAppDescriptor(const QString& id, MyAppContainer* container)
  : id(id)
  , container(container)
{
}

//----------------------------------------------------------------------------
QString MyAppDescriptor::getApplicationId() const
{
  return id;
}

//----------------------------------------------------------------------------
ctkProperties MyAppDescriptor::getProperties(const QLocale&) const
{
  return getProperties();
}

//----------------------------------------------------------------------------
ctkProperties MyAppDescriptor::getProperties() const
{
  ctkProperties props;
  props[ctkApplicationDescriptor::APPLICATION_PID] = getApplicationId();
  return props;
}

//----------------------------------------------------------------------------
ctkApplicationHandle* MyAppDescriptor::launch(const QHash<QString, QVariant>& arguments)
{
  MyAppHandle* appHandle = createAppHandle(arguments);
  container->launch(appHandle);
  return appHandle;
}

//----------------------------------------------------------------------------
MyAppHandle* MyAppDescriptor::createAppHandle(const ctkProperties& /*arguments*/)
{
  MyAppHandle* newAppHandle = new MyAppHandle(getInstanceId(), this);
  container->registerHandle(newAppHandle);
  return newAppHandle;
}

//----------------------------------------------------------------------------
QString MyAppDescriptor::getInstanceId() const
{
  static long instanceId = 0;
  return getApplicationId() + "." + QString::number(instanceId++);
}


//----------------------------------------------------------------------------
MyAppHandle::MyAppHandle(const QString& instanceId, ctkApplicationDescriptor* descriptor)
  : descriptor(descriptor)
  , instanceId(instanceId)
{

}

//----------------------------------------------------------------------------
ctkApplicationDescriptor*MyAppHandle::getApplicationDescriptor() const
{
  return descriptor;
}

//----------------------------------------------------------------------------
QString MyAppHandle::getState() const
{
  return "unknown";
}

//----------------------------------------------------------------------------
QVariant MyAppHandle::getExitValue(long /*timeout*/) const
{
  return QVariant();
}

//----------------------------------------------------------------------------
QString MyAppHandle::getInstanceId() const
{
  return instanceId;
}

//----------------------------------------------------------------------------
void MyAppHandle::destroy()
{
}

//----------------------------------------------------------------------------
QVariant MyAppHandle::run(const QVariant& /*context*/)
{
  Q_ASSERT_X(QThread::currentThread() == QCoreApplication::instance()->thread(), "MyAppHandle::run", "Not running in main thread");
  QCoreApplication::instance()->setProperty("app_test.success", true);
  return QVariant();
}

//----------------------------------------------------------------------------
void MyAppHandle::stop()
{
}

//----------------------------------------------------------------------------
MyAppContainer::MyAppContainer(ctkPluginContext* context)
  : context(context)
  , appLauncher(NULL)
  , launcherTracker(context, this)
  , defaultMainThreadAppHandle(NULL)
{
}

//----------------------------------------------------------------------------
MyAppContainer::~MyAppContainer()
{
}

//----------------------------------------------------------------------------
void MyAppContainer::start()
{
  qDebug() << "Starting app container";

  launcherTracker.open();

  // register a (default) test descriptor
  MyAppDescriptor* appDescr = new MyAppDescriptor("test-app", this);
  ctkServiceRegistration reg = context->registerService<ctkApplicationDescriptor>(appDescr, appDescr->getProperties());
  descriptorRegistrations.insert(appDescr, reg);
  descriptors.push_back(appDescr);

  try
  {
    appDescr->launch(ctkProperties());
  }
  catch (const ctkApplicationException& e)
  {
    qWarning() << "An error occurred whild starting the application:" << e;
  }
}

//----------------------------------------------------------------------------
void MyAppContainer::stop()
{
  for(QHash<MyAppHandle*, ctkServiceRegistration>::iterator iter = handleRegistrations.begin();
      iter != handleRegistrations.end(); ++iter)
  {
    iter.key()->destroy();
    iter.value().unregister();
  }
  qDeleteAll(handles);

  for(QHash<MyAppDescriptor*, ctkServiceRegistration>::iterator iter = descriptorRegistrations.begin();
      iter != descriptorRegistrations.end(); ++iter)
  {
    iter.value().unregister();
  }
  qDeleteAll(descriptors);

  launcherTracker.close();
}

//----------------------------------------------------------------------------
void MyAppContainer::launch(MyAppHandle* handle)
{
  // use the ApplicationLauncher provided by the framework to ensure it is launched on the main thread
  ctkApplicationLauncher* appLauncher = launcherTracker.getService();
  if (appLauncher == NULL)
  {
    // we need to wait to allow the ApplicationLauncher to get registered;
    // save the handle to be launched as soon as the ApplicationLauncher is available
    defaultMainThreadAppHandle = handle;
    qDebug() << "Waiting for AppLauncher to become available";
    return;
  }

  appLauncher->launch(handle, QVariant());
}

//----------------------------------------------------------------------------
void MyAppContainer::registerHandle(MyAppHandle* handle)
{
  ctkServiceRegistration reg = context->registerService<ctkApplicationHandle>(handle);
  handles.push_back(handle);
  handleRegistrations.insert(handle, reg);
}

ctkApplicationLauncher* MyAppContainer::addingService(const ctkServiceReference& reference)
{
  ctkApplicationLauncher* appLauncher = NULL;
  ctkApplicationRunnable* appRunnable = NULL;

  appLauncher = context->getService<ctkApplicationLauncher>(reference);
  // see if there is a default main threaded application waiting to run
  appRunnable = defaultMainThreadAppHandle;
  // null out so we do not attempt to start this handle again
  defaultMainThreadAppHandle = NULL;
  if (appRunnable != NULL)
  {
    // found a main threaded app; start it now that the app launcher is available
    appLauncher->launch(appRunnable, QVariant());
  }
  return appLauncher;
}

//----------------------------------------------------------------------------
void MyAppContainer::modifiedService(const ctkServiceReference&, ctkApplicationLauncher*)
{
}

//----------------------------------------------------------------------------
void MyAppContainer::removedService(const ctkServiceReference&, ctkApplicationLauncher*)
{
}
