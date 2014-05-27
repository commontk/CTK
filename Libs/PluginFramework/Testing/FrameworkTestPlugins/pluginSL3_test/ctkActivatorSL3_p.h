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


#ifndef CTKACTIVATORSL3_P_H
#define CTKACTIVATORSL3_P_H

#include <ctkPluginActivator.h>
#include <ctkServiceTrackerCustomizer.h>
#include <ctkServiceTracker.h>

#include <QScopedPointer>

class ctkFooService;

class ctkActivatorSL3 :
    public QObject, public ctkPluginActivator,
    public ctkServiceTrackerCustomizer<ctkFooService*>
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator)
#ifdef HAVE_QT5
  Q_PLUGIN_METADATA(IID "pluginSL3_test")
#endif
  Q_PROPERTY(bool serviceAdded READ serviceAdded)
  Q_PROPERTY(bool serviceRemoved READ serviceRemoved)

public:

  ctkActivatorSL3();
  ~ctkActivatorSL3();

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

  bool serviceAdded() const;
  bool serviceRemoved() const;

  ctkFooService* addingService(const ctkServiceReference& reference);
  void modifiedService(const ctkServiceReference& reference, ctkFooService* service);
  void removedService(const ctkServiceReference& reference, ctkFooService* service);

private:

  bool _serviceAdded;
  bool _serviceRemoved;

  typedef ctkServiceTracker<ctkFooService*> FooTracker;
  QScopedPointer<FooTracker> tracker;
  ctkPluginContext* context;


}; // ctkActivatorSL3

#endif // CTKACTIVATORSL3_P_H
