/*
 * ctkPluginActivator.h
 *
 *  Created on: Mar 10, 2010
 *      Author: zelzer
 */

#ifndef CTKPLUGINACTIVATOR_H_
#define CTKPLUGINACTIVATOR_H_

#include "ctkPluginContext.h"

#include "CTKCoreExport.h"

namespace ctk {

  class PluginActivator
  {
  public:

    virtual ~PluginActivator() {};

    virtual void start(PluginContext* context) = 0;
    virtual void stop(PluginContext* context) = 0;

  };

}

Q_DECLARE_INTERFACE(ctk::PluginActivator, "org.commontk.core.pluginactivator")

#endif /* CTKPLUGINACTIVATOR_H_ */
