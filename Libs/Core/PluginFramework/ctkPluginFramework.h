#ifndef CTKPLUGINFRAMEWORK_H
#define CTKPLUGINFRAMEWORK_H

#include "ctkPlugin.h"

#include "ctkPluginRepository_p.h"

namespace ctk {

  class PluginFramework : public Plugin
  {

  public:

    PluginFramework();

    void init();

    // TODO return info about the reason why this
    // method returned
    void waitForStop(int timeout);

  protected:

    bool active;
    PluginRepository plugins;

    ServiceRegistry serviceRegistry;

    InternalSystemPlugin systemPlugin;
  };

}

#endif // CTKPLUGINFRAMEWORK_H
