/*
 * ctkCLIPlugin.cxx
 *
 *  Created on: Mar 11, 2010
 *      Author: zelzer
 */

#include "ctkCLIPlugin.h"

#include "ctkCLIRegistry.h"

#include <QtPlugin>
#include <QServiceInterfaceDescriptor>
#include <QStringList>

#include <iostream>

using namespace QtMobility;

void ctkCLIPlugin::start(ctkPluginContext* context)
{
  ctkCLIRegistry* registry = new ctkCLIRegistry();
  context->registerService(QStringList("ctkCLIRegistry"), registry);

  std::cout << "Plugin A started\n";
}

void ctkCLIPlugin::stop(ctkPluginContext* context)
{
  std::cout << "Plugin B stopped\n";
}

QObject* ctkCLIPlugin::createInstance(const QServiceInterfaceDescriptor& descriptor,
                            QServiceContext* context,
                            QAbstractSecuritySession* session)
{
  std::cout << "Creating service instance for " << descriptor.interfaceName().toStdString() << std::endl;
  return 0;
}

Q_EXPORT_PLUGIN2(org_commontk_cli, ctkCLIPlugin)

