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

#ifndef CTKPLUGIN_H
#define CTKPLUGIN_H

#include "ctkPluginContext.h"

#include "ctkVersion.h"

namespace ctk {

  class PluginArchive;
  class PluginFrameworkContextPrivate;
  class PluginPrivate;

  class CTK_CORE_EXPORT Plugin {

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

    /**
     * Returns this plugin's current state.
     *
     * <p>
     * A plugin can be in only one state at any time.
     *
     * @return An element of <code>UNINSTALLED</code>,<code>INSTALLED</code>,
     *         <code>RESOLVED</code>,<code>STARTING</code>,
     *         <code>STOPPING</code>,<code>ACTIVE</code>.
     */
    State getState() const;

    virtual void start();

    virtual void stop();

    PluginContext* getPluginContext() const;

    int getPluginId() const;

    /**
     * Returns this plugin's location identifier.
     *
     * <p>
     * The location identifier is the location passed to
     * <code>PluginContext::installPlugin</code> when a plugin is installed.
     * The location identifier does not change while this plugin remains
     * installed, even if this plugin is updated.
     *
     * <p>
     * This method must continue to return this plugin's location identifier
     * while this plugin is in the <code>UNINSTALLED</code> state.
     *
     * @return The string representation of this plugin's location identifier.
     */
    QString getLocation() const;

    QString getSymbolicName() const;

    /**
     * Returns a list of all the files and directories
     * within this plugin whose longest sub-path matches the
     * specified path.
     * <p>
     * The specified path is always relative to the root of this plugin
     * (the plugins symbolic name) and may begin with a &quot;/&quot;.
     * A path value of &quot;/&quot; indicates the root of this plugin.
     * <p>
     * Returned paths indicating subdirectory paths end with a &quot;/&quot;.
     * The returned paths are all relative to the root of this plugin and must
     * not begin with &quot;/&quot;.
     * <p>
     *
     * @param path The path name for which to return resource paths.
     * @return A list of the resource paths (<code>QString</code>
     *         objects) or an empty list if no entry could be found.

     * @throws std::logic_error If this plugin has been
     *         uninstalled.
     */
    virtual QStringList getResourceList(const QString& path) const;

    /**
     * Returns a QByteArray containing a Qt resource located at the
     * specified path in this plugin.
     * <p>
     * The specified path is always relative to the root of this plugin
     * (the plugins symbolic name) and may
     * begin with &quot;/&quot;. A path value of &quot;/&quot; indicates the
     * root of this plugin.
     * <p>
     *
     * @param path The path name of the resource.
     * @return A QString to the resource, or a null QString if no resource could be
     *         found.
     * @throws std::logic_error If this plugin has been
     *         uninstalled.
     */
    virtual QByteArray getResource(const QString& path) const;

    Version getVersion() const;

  protected:

    friend class PluginFrameworkContextPrivate;
    friend class Plugins;

    PluginPrivate * const d_ptr;

    Plugin(PluginFrameworkContextPrivate* fw, PluginArchive* ba);
    Plugin(PluginPrivate& dd);
  };

}

Q_DECLARE_OPERATORS_FOR_FLAGS(ctk::Plugin::States)

#endif // CTKPLUGIN_H
