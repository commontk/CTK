#ifndef CTKSERVICEREGISTRATION_H
#define CTKSERVICEREGISTRATION_H

#include "ctkPluginContext.h"

#include "ctkServiceReference.h"

namespace ctk {

  class ServiceRegistration {

  public:

    ServiceReference getReference() const;

    void setProperties(const PluginContext::ServiceProperties& properties);

    void unregister() const;

  };

}

#endif // CTKSERVICEREGISTRATION_H
