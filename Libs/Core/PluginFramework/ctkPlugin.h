#ifndef CTKPLUGIN_H
#define CTKPLUGIN_H

#include "ctkPluginContext.h"

#include "ctkVersion.h"

namespace ctk {

  class PluginArchiveInterface;
  class PluginFrameworkContext;
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

    Q_DECLARE_FLAGS(States, State)

    virtual ~Plugin();

    State state() const;

    virtual void start();

    virtual void stop();

    PluginContext* getPluginContext() const;

    long getPluginId() const;

    QString getSymbolicName() const;

    Version getVersion() const;

  protected:

    PluginPrivate * const d_ptr;

    Plugin(PluginFrameworkContext* fw, PluginArchiveInterface* ba);
    Plugin(PluginPrivate& dd);
  };

}

Q_DECLARE_OPERATORS_FOR_FLAGS(ctk::Plugin::States)

#endif // CTKPLUGIN_H
