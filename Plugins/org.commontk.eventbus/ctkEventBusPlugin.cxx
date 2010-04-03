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

namespace ctk {

  void EventBusPlugin::start(PluginContext* context)
  {
    std::cout << "ctkCore Plugin started\n";
  }

  void EventBusPlugin::stop(PluginContext* context)
  {
    std::cout << "ctkCore Plugin stopped\n";
  }

  QObject* EventBusPlugin::createInstance(const QServiceInterfaceDescriptor& descriptor,
                              QServiceContext* context,
                              QAbstractSecuritySession* session)
  {
    std::cout << "Creating service instance for " << descriptor.interfaceName().toStdString() << std::endl;
    if (descriptor.interfaceName() == "org.commontk.core.EventBus")
    {
      return EventBusImpl::instance();
    }
  }

Q_EXPORT_PLUGIN2(org_commontk_eventbus, EventBusPlugin)

}
