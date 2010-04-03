#ifndef CTKPLUGINFRAMEWORK_H
#define CTKPLUGINFRAMEWORK_H

#include "ctkPlugin.h"

#include "ctkPluginRepository_p.h"

namespace ctk {

  class PluginFramework
  {

  public:

    void launch();
    void shutdown();

  protected:

    bool active;
    PluginRepository plugins;

    ServiceRegistry serviceRegistry;

    InternalSystemPlugin systemPlugin;
  };

}

#endif // CTKPLUGINFRAMEWORK_H
