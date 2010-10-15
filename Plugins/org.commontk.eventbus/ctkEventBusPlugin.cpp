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
