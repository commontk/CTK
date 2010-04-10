#include "ctkPluginFrameworkContext.h"

#include "ctkPluginFrameworkContextPrivate_p.h"

namespace ctk {

  PluginFrameworkContext::PluginFrameworkContext(const Properties& initProps)
    : d_ptr(new PluginFrameworkContextPrivate(initProps))
  {

  }

  PluginFrameworkContext::~PluginFrameworkContext()
  {
    delete d_ptr;
  }

}
