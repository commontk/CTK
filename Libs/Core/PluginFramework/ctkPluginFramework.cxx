#include "ctkPluginFramework.h"

namespace ctk {

  class PluginFrameworkPrivate : public PluginPrivate
  {

  };

  PluginFramework::PluginFramework()
    : Plugin(*new PluginFrameworkPrivate())
  {

  }

}
