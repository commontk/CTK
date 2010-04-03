#ifndef CTKINTERNALSYSTEMPLUGIN_P_H
#define CTKINTERNALSYSTEMPLUGIN_P_H

#include "ctkPlugin.h"

namespace ctk {

  class InternalSystemPluginPrivate;
  class InternalSystemPlugin : public Plugin {

  public:

    void start();
    void stop();

  };

}

#endif // CTKINTERNALSYSTEMPLUGIN_P_H
