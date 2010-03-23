/*
 * ctkCLIPlugin.cxx
 *
 *  Created on: Mar 11, 2010
 *      Author: zelzer
 */

#include "ctkCLIPlugin.h"

#include <QtPlugin>

#include <iostream>

namespace ctk {

  void CLIPlugin::start(PluginContext::Pointer context)
  {
    std::cout << "Plugin A started\n";
  }

  void CLIPlugin::stop(PluginContext::Pointer context)
  {
    std::cout << "Plugin B stopped\n";
  }

  QObject* CLIPlugin::createInstance(const QServiceInterfaceDescriptor& descriptor,
                              QServiceContext* context,
                              QAbstractSecuritySession* session)
  {
    std::cout << "Creating service instance for " << descriptor.interfaceName().toStdString() << std::endl;
    return 0;
  }

Q_EXPORT_PLUGIN2(org_commontk_cli, CLIPlugin)

}
