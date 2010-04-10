#ifndef CTKPLUGINPRIVATE_P_H
#define CTKPLUGINPRIVATE_P_H

#include "ctkPlugin.h"
#include "ctkPluginException.h"

#include <QHash>

namespace ctk {

  class PluginActivator;
  class PluginArchiveInterface;
  class PluginFrameworkContext;

  class PluginPrivate {

  public:

    /**
     * Construct a new plugin based on a PluginArchive.
     *
     * @param fw PluginFrameworkContext for this plugin.
     * @param ba Plugin archive representing the shared library and cached data
     * @param checkContext AccessConrolContext to do permission checks against.
     * @exception std::invalid_argument Faulty manifest for bundle
     */
    PluginPrivate(PluginFrameworkContext* fw,
               PluginArchiveInterface* ba /*, Object checkContext*/);

    /**
     * Construct a new empty Plugin.
     *
     * Only called for the system plugin
     *
     * @param fw Framework for this plugin.
     */
    PluginPrivate(PluginFrameworkContext* fw,
                  int id,
                  QString loc,
                  QString sym,
                  const Version& ver);

    virtual ~PluginPrivate();

    QHash<QString, QString> getHeaders(const QString& locale);


  protected:

    /**
     * Union of flags allowing plugin class access
     */
    static const Plugin::States RESOLVED_FLAGS;

    PluginFrameworkContext * const fwCtx;

    /**
     * Plugin identifier
     */
    const int id;

    /**
     * Plugin symbolic name
     */
    QString symbolicName;

    /**
     * Plugin version
     */
    Version version;

    /**
     * State of the plugin
     */
    Plugin::States state;

    /**
     * PluginContext for the plugin
     */
    PluginContext* pluginContext;

    /**
     * PluginActivator for the plugin
     */
    PluginActivator* pluginActivator;

    /**
     * Stores the default locale entries when uninstalled
     */
    QHash<QString, QString> cachedHeaders;

    /**
     * Stores the raw manifest headers
     */
    QHash<QString, QString> cachedRawHeaders;

    bool lazyActivation;

    /** True during the finalization of an activation. */
    bool activating;

    /** True during the state change from active to resolved. */
    bool deactivating;

    /** Saved exception of resolve failure */
    PluginException resolveFailException;


  };

}

#endif // CTKPLUGINPRIVATE_P_H
