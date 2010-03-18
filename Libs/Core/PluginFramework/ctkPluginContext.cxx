/*
 * ctkPluginContext.cxx
 *
 *  Created on: Mar 10, 2010
 *      Author: zelzer
 */

#include "ctkPluginContext.h"

#include <QServiceManager>

namespace ctk {

  class PluginContextPrivate
  {

  public:

	  QServiceManager serviceManager;
  };


  PluginContext::PluginContext()
  : d_ptr(new PluginContextPrivate())
  {}

  PluginContext::~PluginContext()
  {
	  Q_D(PluginContext);
	  delete d;
  }

  QServiceInterfaceDescriptor PluginContext::getServiceDescriptor(const QString& interfaceName) const
  {
	Q_D(const PluginContext);
	return d->serviceManager.interfaceDefault(interfaceName);
  }

}
