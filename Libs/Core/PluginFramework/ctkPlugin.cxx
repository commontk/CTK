#include "ctkPlugin.h"

#include "ctkPluginPrivate_p.h"

namespace ctk {

  Plugin::Plugin(PluginFrameworkContext* fw,
                 PluginArchiveInterface* ba)
    : d_ptr(new PluginPrivate(fw, ba))
  {

  }

  Plugin::Plugin(PluginPrivate& dd)
    : d_ptr(&dd)
  {

  }

  Plugin::~Plugin()
  {
    delete d_ptr;
  }

  Plugin::State Plugin::state() const
  {
    return ACTIVE;
  }

  void Plugin::start()
  {

  }

  void Plugin::stop()
  {

  }

  PluginContext* Plugin::getPluginContext() const
  {

  }

  long Plugin::getPluginId() const
  {
    return 0;
  }

  QString Plugin::getSymbolicName() const
  {

  }

  Version Plugin::getVersion() const
  {

  }

}
