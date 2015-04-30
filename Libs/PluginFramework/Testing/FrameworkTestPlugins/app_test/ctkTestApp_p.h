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


#ifndef CTKTESTAPP_P_H
#define CTKTESTAPP_P_H


#include <service/application/ctkApplicationDescriptor.h>
#include <service/application/ctkApplicationHandle.h>

#include <ctkApplicationRunnable.h>
#include <ctkServiceRegistration.h>

#include <service/application/ctkApplicationLauncher.h>

#include <ctkServiceTracker.h>
#include <ctkServiceTrackerCustomizer.h>

#include <QObject>

class ctkPluginContext;

class MyAppHandle;
class MyAppDescriptor;

class MyAppContainer : private ctkServiceTrackerCustomizer<ctkApplicationLauncher*>
{
public:

  MyAppContainer(ctkPluginContext* context);
  ~MyAppContainer();

  void start();
  void stop();

  void launch(MyAppHandle* handle);

  void registerHandle(MyAppHandle* handle);

private:

  Q_DISABLE_COPY(MyAppContainer)

  virtual ctkApplicationLauncher* addingService(const ctkServiceReference& reference);
  virtual void modifiedService(const ctkServiceReference& reference, ctkApplicationLauncher* service);
  virtual void removedService(const ctkServiceReference& reference, ctkApplicationLauncher* service);

  ctkPluginContext* context;

  ctkApplicationLauncher* appLauncher;
  ctkServiceTracker<ctkApplicationLauncher*> launcherTracker;

  ctkApplicationRunnable* defaultMainThreadAppHandle;

  QHash<MyAppHandle*, ctkServiceRegistration> handleRegistrations;
  QList<MyAppHandle*> handles;

  QList<MyAppDescriptor*> descriptors;
  QHash<MyAppDescriptor*, ctkServiceRegistration> descriptorRegistrations;
};

class MyAppHandle : public QObject, public ctkApplicationRunnable, public ctkApplicationHandle
{
  Q_OBJECT
  Q_INTERFACES(ctkApplicationHandle)

public:

  MyAppHandle(const QString& instanceId, ctkApplicationDescriptor* descriptor);

  virtual ctkApplicationDescriptor* getApplicationDescriptor() const;
  virtual QString getState() const;
  virtual QVariant getExitValue(long timeout) const;
  virtual QString getInstanceId() const;
  virtual void destroy();

  virtual QVariant run(const QVariant &context);
  virtual void stop();

private:

  ctkApplicationDescriptor* descriptor;
  QString instanceId;

};

class MyAppDescriptor : public QObject, public ctkApplicationDescriptor
{
  Q_OBJECT
  Q_INTERFACES(ctkApplicationDescriptor)

public:

  MyAppDescriptor(const QString& id, MyAppContainer* container);

  virtual QString getApplicationId() const;

  virtual ctkProperties getProperties(const QLocale& locale) const;

  virtual ctkProperties getProperties() const;

  virtual ctkApplicationHandle* launch(const QHash<QString, QVariant>& arguments);

private:

  MyAppHandle* createAppHandle(const ctkProperties& arguments);

  QString getInstanceId() const;

  const QString id;
  MyAppContainer* container;
};

#endif // CTKTESTAPP_P_H
