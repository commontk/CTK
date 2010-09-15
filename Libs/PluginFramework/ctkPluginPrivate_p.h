/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

#ifndef CTKPLUGINPRIVATE_P_H
#define CTKPLUGINPRIVATE_P_H

#include "ctkPlugin.h"
#include "ctkPluginException.h"
#include "ctkRequirePlugin_p.h"

#include <QHash>
#include <QPluginLoader>


  class ctkPluginActivator;
  class ctkPluginArchive;
  class ctkPluginFrameworkContext;

  class ctkPluginPrivate {
  public:
    Q_DECLARE_PUBLIC(ctkPlugin);
    ctkPlugin * const q_ptr;

    /**
     * Construct a new plugin based on a ctkPluginArchive.
     *
     * @param fw ctkPluginFrameworkContext for this plugin.
     * @param ba ctkPlugin archive representing the shared library and cached data
     * @param checkContext AccessConrolContext to do permission checks against.
     * @exception std::invalid_argument Faulty manifest for bundle
     */
    ctkPluginPrivate(ctkPlugin& qq, ctkPluginFrameworkContext* fw,
               ctkPluginArchive* pa /*, Object checkContext*/);

    /**
     * Construct a new empty ctkPlugin.
     *
     * Only called for the system plugin
     *
     * @param fw Framework for this plugin.
     */
    ctkPluginPrivate(ctkPlugin& qq,
                  ctkPluginFrameworkContext* fw,
                  long id,
                  const QString& loc,
                  const QString& sym,
                  const ctkVersion& ver);

    virtual ~ctkPluginPrivate();

    /**
     * Get updated plugin state. That means check if an installed
     * plugin has been resolved.
     *
     * @return ctkPlugin state
     */
    ctkPlugin::State getUpdatedState();

    /**
     * Save the autostart setting to the persistent plugin storage.
     *
     * @param setting The autostart options to save.
     */
    void setAutostartSetting(const ctkPlugin::StartOptions& setting);

    /**
     * Performs the actual activation.
     */
    void finalizeActivation();

    /**
     * Union of flags allowing plugin class access
     */
    static const ctkPlugin::States RESOLVED_FLAGS;

    ctkPluginFrameworkContext * const fwCtx;

    /**
     * ctkPlugin identifier
     */
    const long id;

    /**
     * ctkPlugin location identifier
     */
    const QString location;

    /**
     * ctkPlugin symbolic name
     */
    QString symbolicName;

    /**
     * ctkPlugin version
     */
    ctkVersion version;

    /**
     * State of the plugin
     */
    ctkPlugin::State state;

    /**
     * ctkPlugin archive
     */
    ctkPluginArchive* archive;

    /**
     * ctkPluginContext for the plugin
     */
    ctkPluginContext* pluginContext;

    /**
     * ctkPluginActivator for the plugin
     */
    ctkPluginActivator* pluginActivator;

    /**
     * The Qt plugin loader for the plugin
     */
    QPluginLoader pluginLoader;

    /**
     * Time when the plugin was last modified
     */
    long lastModified;

    /**
     * Stores the default locale entries when uninstalled
     */
    QHash<QString, QString> cachedHeaders;

    /**
     * Stores the raw manifest headers
     */
    QHash<QString, QString> cachedRawHeaders;

    /**
     * True when this plugin has its activation policy
     * set to "eager"
     */
    bool eagerActivation;

    /** True during the finalization of an activation. */
    bool activating;

    /** True during the state change from active to resolved. */
    bool deactivating;

    /** Saved exception of resolve failure */
    //ctkPluginException resolveFailException;

    /** List of ctkRequirePlugin entries. */
    QList<ctkRequirePlugin*> require;

  private:

    /**
     * Check manifest and cache certain manifest headers as variables.
     */
    void checkManifestHeaders();

    // This could potentially be run in its own thread,
    // parallelizing plugin activations
    void start0();

    /**
     * Remove a plugins registered listeners, registered services and
     * used services.
     *
     */
    void removePluginResources();


  };


#endif // CTKPLUGINPRIVATE_P_H
