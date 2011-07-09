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
#include <QServiceInterfaceDescriptor>

#include "ctkEventBusImpl_p.h"

#include <iostream>

using namespace QtMobility;

void ctkEventBusPlugin::start(ctkPluginContext* context)
{
  Q_UNUSED(context)
  std::cout << "org.commontk.eventbus Plugin started\n";
}

void ctkEventBusPlugin::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)
  std::cout << "org.commontk.eventbus Plugin stopped\n";
}

QObject* ctkEventBusPlugin::createInstance(const QServiceInterfaceDescriptor& descriptor,
                            QServiceContext* context,
                            QAbstractSecuritySession* session)
{
  Q_UNUSED(context)
  Q_UNUSED(session)

  std::cout << "Creating service instance for " << descriptor.interfaceName().toStdString() << std::endl;
  if (descriptor.interfaceName() == "org.commontk.core.EventBus")
  {
    return ctkEventBusImpl::instance();
  }

  return 0;
}

Q_EXPORT_PLUGIN2(org_commontk_eventbus, ctkEventBusPlugin)
