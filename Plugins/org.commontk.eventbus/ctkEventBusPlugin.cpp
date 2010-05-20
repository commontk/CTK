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
  std::cout << "ctkCore Plugin started\n";
}

void ctkEventBusPlugin::stop(ctkPluginContext* context)
{
  std::cout << "ctkCore Plugin stopped\n";
}

QObject* ctkEventBusPlugin::createInstance(const QServiceInterfaceDescriptor& descriptor,
                            QServiceContext* context,
                            QAbstractSecuritySession* session)
{
  std::cout << "Creating service instance for " << descriptor.interfaceName().toStdString() << std::endl;
  if (descriptor.interfaceName() == "org.commontk.core.EventBus")
  {
    return ctkEventBusImpl::instance();
  }
}

Q_EXPORT_PLUGIN2(org_commontk_eventbus, ctkEventBusPlugin)
