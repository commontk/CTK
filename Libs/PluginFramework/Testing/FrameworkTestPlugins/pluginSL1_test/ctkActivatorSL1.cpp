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


#include "ctkActivatorSL1_p.h"
#include "ctkFooService.h"

#include <QtPlugin>
#include <QStringList>

//----------------------------------------------------------------------------
ctkActivatorSL1::ctkActivatorSL1()
  : _serviceAdded(false), _serviceRemoved(false),
    context(0)
{
}

//----------------------------------------------------------------------------
ctkActivatorSL1::~ctkActivatorSL1()
{

}

//----------------------------------------------------------------------------
void ctkActivatorSL1::start(ctkPluginContext* context)
{
  this->context = context;

  context->registerService(this->metaObject()->className(), this);

  tracker.reset(new FooTracker(context, this));
  tracker->open();
}

//----------------------------------------------------------------------------
void ctkActivatorSL1::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)
  tracker->close();
}

//----------------------------------------------------------------------------
bool ctkActivatorSL1::serviceAdded() const
{
  return _serviceAdded;
}

//----------------------------------------------------------------------------
bool ctkActivatorSL1::serviceRemoved() const
{
  return _serviceRemoved;
}

//----------------------------------------------------------------------------
ctkFooService* ctkActivatorSL1::addingService(const ctkServiceReference& reference)
{
  _serviceAdded = true;
  qDebug() << "Adding reference =" << reference;

  ctkFooService* fooService = context->getService<ctkFooService>(reference);
  fooService->foo();
  return fooService;
}

//----------------------------------------------------------------------------
void ctkActivatorSL1::modifiedService(const ctkServiceReference& reference, ctkFooService* service)
{
  Q_UNUSED(reference)
  Q_UNUSED(service)
}

//----------------------------------------------------------------------------
void ctkActivatorSL1::removedService(const ctkServiceReference& reference, ctkFooService* service)
{
  Q_UNUSED(service)
  _serviceRemoved = true;
  qDebug() << "Removing reference =" << reference;
}

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
Q_EXPORT_PLUGIN2(pluginSL1_test, ctkActivatorSL1)
#endif
