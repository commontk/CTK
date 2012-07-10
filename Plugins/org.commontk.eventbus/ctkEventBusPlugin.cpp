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

/*
 * ctkEventBusPlugin.cxx
 *
 *  Created on: Mar 29, 2010
 *      Author: zelzer
 */

#include "ctkEventBusPlugin_p.h"

#include <QtPlugin>

#include "ctkEventBusImpl_p.h"

#include <iostream>

ctkEventBusPlugin* ctkEventBusPlugin::instance = 0;

void ctkEventBusPlugin::start(ctkPluginContext* context)
{
  instance = this;
  this->context = context;
  qDebug() << "ctkEventBus Plugin starting";
  m_Bus = new ctkEventBusImpl;
  qDebug() << "ctkEventBus created";
  context->registerService(QStringList("ctkEventAdminBus"),m_Bus);
  qDebug() << "ctkEventBus Plugin started";
}

void ctkEventBusPlugin::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)
  std::cout << "org.commontk.eventbus Plugin stopped\n";
  delete m_Bus;
}

ctkEventBusPlugin* ctkEventBusPlugin::getInstance()
{
  return instance;
}

ctkPluginContext* ctkEventBusPlugin::getPluginContext() const
{
  return context;
}

Q_EXPORT_PLUGIN2(org_commontk_eventbus, ctkEventBusPlugin)
