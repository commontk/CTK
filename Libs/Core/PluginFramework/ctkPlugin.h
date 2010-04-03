#ifndef CTKPLUGIN_H
#define CTKPLUGIN_H

#include "ctkPluginContext.h"

#include "ctkVersion.h"

namespace ctk {

  class PluginPrivate;
  class Plugin {

    Q_DECLARE_PRIVATE(Plugin)

  public:

    enum State {
      UNINSTALLED,
      INSTALLED,
      RESOLVED,
      STARTING,
      STOPPING,
      ACTIVE
    };

    State state() const;

    virtual void start();

    virtual void stop();

    PluginContext* getPluginContext() const;

    long getPluginId() const;

    QString getSymbolicName() const;

    Version getVersion() const;

  protected:

    PluginPrivate * const d_ptr;

    Plugin();
    Plugin(PluginPrivate& dd);
  };

}

#endif // CTKPLUGIN_H
