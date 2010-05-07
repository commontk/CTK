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

#ifndef CTKPLUGINCONTEXT_H_
#define CTKPLUGINCONTEXT_H_

#include <QHash>
#include <QString>
#include <QVariant>
#include <QUrl>

#include "ctkPluginEvent.h"

#include "CTKPluginFrameworkExport.h"


namespace ctk {

  // CTK class forward declarations
  class Plugin;
  class PluginPrivate;
  class ServiceRegistration;
  class ServiceReference;
  class PluginContextPrivate;

  /**
   * A plugin's execution context within the Framework. The context is used to
   * grant access to other methods so that this plugin can interact with the
   * Framework.
   *
   * <p>
   * <code>PluginContext</code> methods allow a plugin to:
   * <ul>
   * <li>Subscribe to events published by the Framework.
   * <li>Register service objects with the Framework service registry.
   * <li>Retrieve <code>ServiceReferences</code> from the Framework service
   * registry.
   * <li>Get and release service objects for a referenced service.
   * <li>Install new plugins in the Framework.
   * <li>Get the list of plugins installed in the Framework.
   * <li>Get the {@link Plugin} object for a plugin.
   * <li>Create <code>QFile</code> objects for files in a persistent storage
   * area provided for the plugin by the Framework.
   * </ul>
   *
   * <p>
   * A <code>PluginContext</code> object will be created and provided to the
   * plugin associated with this context when it is started using the
   * {@link PluginActivator#start} method. The same <code>PluginContext</code>
   * object will be passed to the plugin associated with this context when it is
   * stopped using the {@link PluginActivator#stop} method. A
   * <code>PluginContext</code> object is generally for the private use of its
   * associated plugin and is not meant to be shared with other plugins in the
   * plugin environment.
   *
   * <p>
   * The <code>Plugin</code> object associated with a <code>PluginContext</code>
   * object is called the <em>context plugin</em>.
   *
   * <p>
   * The <code>PluginContext</code> object is only valid during the execution of
   * its context plugin; that is, during the period from when the context plugin
   * is in the <code>STARTING</code>, <code>STOPPING</code>, and
   * <code>ACTIVE</code> plugin states. If the <code>PluginContext</code>
   * object is used subsequently, a <code>std::logic_error</code> must be
   * thrown. The <code>PluginContext</code> object must never be reused after
   * its context plugin is stopped.
   *
   * <p>
   * The Framework is the only entity that can create <code>PluginContext</code>
   * objects and they are only valid within the Framework that created them.
   *
   * @threadsafe
   */
  class CTK_PLUGINFW_EXPORT PluginContext : public QObject
  {
    Q_OBJECT
	  Q_DECLARE_PRIVATE(PluginContext)

  public:

    typedef QHash<QString, QVariant> ServiceProperties;

    ~PluginContext();

    /**
     * Returns the <code>Plugin</code> object associated with this
     * <code>PluginContext</code>. This plugin is called the context plugin.
     *
     * @return The <code>Plugin</code> object associated with this
     *         <code>PluginContext</code>.
     * @throws std::logic_error If this PluginContext is no
     *         longer valid.
     */
    Plugin* getPlugin(int id) const;

    /**
     * Returns a list of all installed plugins.
     * <p>
     * This method returns a list of all plugins installed in the plugin
     * environment at the time of the call to this method. However, since the
     * Framework is a very dynamic environment, plugins can be installed or
     * uninstalled at anytime.
     *
     * @return A QList of <code>Plugin</code> objects, one object per
     *         installed plugin.
     */
    QList<Plugin*> getPlugins() const;

    ServiceRegistration registerService(const QStringList& clazzes, QObject* service, const ServiceProperties& properties = ServiceProperties());

    QList<ServiceReference> getServiceReferences(const QString& clazz, const QString& filter = QString());

    ServiceReference getServiceReference(const QString& clazz);

    QObject* getService(const ServiceReference& reference);

    Plugin* installPlugin(const QUrl& location, QIODevice* in = 0);


  signals:

    /**
     *
     */
    void pluginChanged(const PluginEvent& event);

    /**
     *
     */
    //void frameworkEvent(const FrameworkEvent& event);

    /**
     *
     */
    //void serviceChanged(const ServiceEvent& event);

  protected:

    friend class PluginFrameworkPrivate;

    PluginContext(PluginPrivate* plugin);

    PluginContextPrivate * const d_ptr;
  };

}

#endif /* CTKPLUGINCONTEXT_H_ */
