/*
 * ctkPluginContext.cxx
 *
 *  Created on: Mar 10, 2010
 *      Author: zelzer
 */

#include "ctkPluginContext.h"

#include "ctkServiceRegistration.h"
#include "ctkServiceReference.h"

namespace ctk {

  class PluginContextPrivate
  {

  public:


  };


  PluginContext::PluginContext()
  : d_ptr(new PluginContextPrivate())
  {}

  PluginContext::~PluginContext()
  {
	  Q_D(PluginContext);
	  delete d;
  }

  ServiceRegistration PluginContext::registerService(const QStringList& clazzes, QObject* service, const ServiceProperties& properties)
  {

  }

  QList<ServiceReference> PluginContext::getServiceReferences(const QString& clazz, const QString& filter)
  {

  }

  ServiceReference PluginContext::getServiceReference(const QString& clazz)
  {

  }

  QObject* PluginContext::getService(const ServiceReference& reference)
  {

  }


}
