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

#include "ctkPluginFramework_global.h"

#include "ctkPluginEvent.h"

#include "CTKPluginFrameworkExport.h"



  // CTK class forward declarations
  class ctkPlugin;
  class ctkPluginPrivate;
  class ctkServiceRegistration;
  class ctkServiceReference;
  class ctkPluginContextPrivate;

  /**
   * A plugin's execution context within the Framework. The context is used to
   * grant access to other methods so that this plugin can interact with the
   * Framework.
   *
   * <p>
   * <code>ctkPluginContext</code> methods allow a plugin to:
   * <ul>
   * <li>Subscribe to events published by the Framework.
   * <li>Register service objects with the Framework service registry.
   * <li>Retrieve <code>ServiceReferences</code> from the Framework service
   * registry.
   * <li>Get and release service objects for a referenced service.
   * <li>Install new plugins in the Framework.
   * <li>Get the list of plugins installed in the Framework.
   * <li>Get the {@link ctkPlugin} object for a plugin.
   * <li>Create <code>QFile</code> objects for files in a persistent storage
   * area provided for the plugin by the Framework.
   * </ul>
   *
   * <p>
   * A <code>ctkPluginContext</code> object will be created and provided to the
   * plugin associated with this context when it is started using the
   * {@link ctkPluginActivator::start} method. The same <code>ctkPluginContext</code>
   * object will be passed to the plugin associated with this context when it is
   * stopped using the {@link ctkPluginActivator::stop} method. A
   * <code>ctkPluginContext</code> object is generally for the private use of its
   * associated plugin and is not meant to be shared with other plugins in the
   * plugin environment.
   *
   * <p>
   * The <code>ctkPlugin</code> object associated with a <code>ctkPluginContext</code>
   * object is called the <em>context plugin</em>.
   *
   * <p>
   * The <code>ctkPluginContext</code> object is only valid during the execution of
   * its context plugin; that is, during the period from when the context plugin
   * is in the <code>STARTING</code>, <code>STOPPING</code>, and
   * <code>ACTIVE</code> plugin states. If the <code>ctkPluginContext</code>
   * object is used subsequently, a <code>std::logic_error</code> must be
   * thrown. The <code>ctkPluginContext</code> object must never be reused after
   * its context plugin is stopped.
   *
   * <p>
   * The Framework is the only entity that can create <code>ctkPluginContext</code>
   * objects and they are only valid within the Framework that created them.
   *
   * @threadsafe
   */
  class CTK_PLUGINFW_EXPORT ctkPluginContext
  {
	  Q_DECLARE_PRIVATE(ctkPluginContext)

  public:

    ~ctkPluginContext();

    /**
     * Returns the <code>ctkPlugin</code> object associated with this
     * <code>ctkPluginContext</code>. This plugin is called the context plugin.
     *
     * @return The <code>ctkPlugin</code> object associated with this
     *         <code>ctkPluginContext</code>.
     * @throws std::logic_error If this ctkPluginContext is no
     *         longer valid.
     */
    ctkPlugin* getPlugin() const;

    /**
     * Returns the plugin with the specified identifier.
     *
     * @param id The identifier of the plugin to retrieve.
     * @return A <code>ctkPlugin</code> object or <code>0</code> if the
     *         identifier does not match any installed plugin.
     */
    ctkPlugin* getPlugin(long id) const;


    /**
     * Returns a list of all installed plugins.
     * <p>
     * This method returns a list of all plugins installed in the plugin
     * environment at the time of the call to this method. However, since the
     * Framework is a very dynamic environment, plugins can be installed or
     * uninstalled at anytime.
     *
     * @return A QList of <code>ctkPlugin</code> objects, one object per
     *         installed plugin.
     */
    QList<ctkPlugin*> getPlugins() const;

    /**
     * Registers the specified service object with the specified properties
     * under the specified class names into the Framework. A
     * <code>ctkServiceRegistration</code> object is returned. The
     * <code>ctkServiceRegistration</code> object is for the private use of the
     * plugin registering the service and should not be shared with other
     * plugins. The registering plugin is defined to be the context plugin.
     * Other plugins can locate the service by using either the
     * {@link #getServiceReferences} or {@link #getServiceReference} method.
     *
     * <p>
     * A plugin can register a service object that implements the
     * {@link ctkServiceFactory} interface to have more flexibility in providing
     * service objects to other plugins.
     *
     * <p>
     * The following steps are required to register a service:
     * <ol>
     * <li>If <code>service</code> is not a <code>ctkServiceFactory</code>, an
     * <code>std::invalid_argument</code> is thrown if <code>service</code>
     * is not an instance of all the specified class names.
     * <li>The Framework adds the following service properties to the service
     * properties from the specified <code>ServiceProperties</code> (which may be
     * omitted): <br/>
     * A property named {@link PluginConstants#SERVICE_ID} identifying the
     * registration number of the service <br/>
     * A property named {@link PluginConstants#OBJECTCLASS} containing all the
     * specified classes. <br/>
     * Properties with these names in the specified <code>ServiceProperties</code> will
     * be ignored.
     * <li>The service is added to the Framework service registry and may now be
     * used by other plugins.
     * <li>A service event of type {@link ServiceEvent#REGISTERED} is fired.
     * <li>A <code>ctkServiceRegistration</code> object for this registration is
     * returned.
     * </ol>
     *
     * @param clazzes The class names under which the service can be located.
     *        The class names will be stored in the service's
     *        properties under the key {@link PluginConstants#OBJECTCLASS}.
     * @param service The service object or a <code>ctkServiceFactory</code>
     *        object.
     * @param properties The properties for this service. The keys in the
     *        properties object must all be <code>QString</code> objects. See
     *        {@link PluginConstants} for a list of standard service property keys.
     *        Changes should not be made to this object after calling this
     *        method. To update the service's properties the
     *        {@link ctkServiceRegistration::setProperties} method must be called.
     *        The set of properties may be omitted if the service has
     *        no properties.
     * @return A <code>ctkServiceRegistration</code> object for use by the plugin
     *         registering the service to update the service's properties or to
     *         unregister the service.
     * @throws std::invalid_argument If one of the following is true:
     *         <ul>
     *         <li><code>service</code> is <code>0</code>. <li><code>service
     *         </code> is not a <code>ctkServiceFactory</code> object and is not an
     *         instance of all the named classes in <code>clazzes</code>. <li>
     *         <code>properties</code> contains case variants of the same key
     *         name.
     *         </ul>
     * @throws std::logic_error If this ctkPluginContext is no longer valid.
     * @see ctkServiceRegistration
     * @see ctkServiceFactory
     */
    ctkServiceRegistration* registerService(const QStringList& clazzes, QObject* service, const ServiceProperties& properties = ServiceProperties());

    /**
     * Returns a list of <code>ctkServiceReference</code> objects. The returned
     * list contains services that
     * were registered under the specified class and match the specified filter
     * expression.
     *
     * <p>
     * The list is valid at the time of the call to this method. However since
     * the Framework is a very dynamic environment, services can be modified or
     * unregistered at any time.
     *
     * <p>
     * The specified <code>filter</code> expression is used to select the
     * registered services whose service properties contain keys and values
     * which satisfy the filter expression. See {@link Filter} for a description
     * of the filter syntax. If the specified <code>filter</code> is
     * empty, all registered services are considered to match the
     * filter. If the specified <code>filter</code> expression cannot be parsed,
     * an {@link std::invalid_argument} will be thrown with a human readable
     * message where the filter became unparsable.
     *
     * <p>
     * The result is a list of <code>ctkServiceReference</code> objects for all
     * services that meet all of the following conditions:
     * <ul>
     * <li>If the specified class name, <code>clazz</code>, is not
     * empty, the service must have been registered with the
     * specified class name. The complete list of class names with which a
     * service was registered is available from the service's
     * {@link PlugincConstants::OBJECTCLASS objectClass} property.
     * <li>If the specified <code>filter</code> is not empty, the
     * filter expression must match the service.
     * </ul>
     *
     * @param clazz The class name with which the service was registered or
     *        an empty string for all services.
     * @param filter The filter expression or empty for all
     *        services.
     * @return A list of <code>ctkServiceReference</code> objects or
     *         an empty list if no services are registered which satisfy the
     *         search.
     * @throws std::invalid_argument If the specified <code>filter</code>
     *         contains an invalid filter expression that cannot be parsed.
     * @throws std::logic_error If this ctkPluginContext is no longer valid.
     */
    QList<ctkServiceReference*> getServiceReferences(const QString& clazz, const QString& filter = QString());

    /**
     * Returns a <code>ctkServiceReference</code> object for a service that
     * implements and was registered under the specified class.
     *
     * <p>
     * The returned <code>ctkServiceReference</code> object is valid at the time of
     * the call to this method. However as the Framework is a very dynamic
     * environment, services can be modified or unregistered at any time.
     *
     * <p>
     * This method is the same as calling
     * {@link ctkPluginContext::getServiceReferences(const QString&, const QString&)} with an
     * empty filter expression. It is provided as a convenience for
     * when the caller is interested in any service that implements the
     * specified class.
     * <p>
     * If multiple such services exist, the service with the highest ranking (as
     * specified in its {@link PluginConstants::SERVICE_RANKING} property) is returned.
     * <p>
     * If there is a tie in ranking, the service with the lowest service ID (as
     * specified in its {@link PluginConstants::SERVICE_ID} property); that is, the
     * service that was registered first is returned.
     *
     * @param clazz The class name with which the service was registered.
     * @return A <code>ctkServiceReference</code> object, or <code>0</code> if
     *         no services are registered which implement the named class.
     * @throws std::logic_error If this ctkPluginContext is no longer valid.
     * @see #getServiceReferences(const QString&, const QString&)
     */
    ctkServiceReference* getServiceReference(const QString& clazz);

    /**
     * Returns the service object referenced by the specified
     * <code>ctkServiceReference</code> object.
     * <p>
     * A plugin's use of a service is tracked by the plugin's use count of that
     * service. Each time a service's service object is returned by
     * {@link #getService(ctkServiceReference*)} the context plugin's use count for
     * that service is incremented by one. Each time the service is released by
     * {@link #ungetService(ctkServiceReference*)} the context plugin's use count
     * for that service is decremented by one.
     * <p>
     * When a plugin's use count for a service drops to zero, the plugin should
     * no longer use that service.
     *
     * <p>
     * This method will always return <code>0</code> when the service
     * associated with this <code>reference</code> has been unregistered.
     *
     * <p>
     * The following steps are required to get the service object:
     * <ol>
     * <li>If the service has been unregistered, <code>0</code> is returned.
     * <li>The context plugin's use count for this service is incremented by
     * one.
     * <li>If the context plugin's use count for the service is currently one
     * and the service was registered with an object implementing the
     * <code>ctkServiceFactory</code> interface, the
     * {@link ctkServiceFactory::getService(ctkPlugin*, ctkServiceRegistration*)} method is
     * called to create a service object for the context plugin. This service
     * object is cached by the Framework. While the context plugin's use count
     * for the service is greater than zero, subsequent calls to get the
     * services's service object for the context plugin will return the cached
     * service object. <br>
     * If the service object returned by the <code>ctkServiceFactory</code> object
     * is not an instance of all the classes named when the service
     * was registered or the <code>ctkServiceFactory</code> object throws an
     * exception, <code>0</code> is returned and a Framework event of type
     * {@link FrameworkEvent::ERROR} containing a {@link ctkServiceException}
     * describing the error is fired.
     * <li>The service object for the service is returned.
     * </ol>
     *
     * @param reference A reference to the service.
     * @return A service object for the service associated with
     *         <code>reference</code> or <code>0</code> if the service is not
     *         registered, the service object returned by a
     *         <code>ctkServiceFactory</code> does not implement the classes under
     *         which it was registered or the <code>ctkServiceFactory</code> threw
     *         an exception.
     * @throws std::logic_error If this ctkPluginContext is no
     *         longer valid.
     * @throws std::invalid_argument If the specified
     *         <code>ctkServiceReference</code> was not created by the same
     *         framework instance as this <code>ctkPluginContext</code>.
     * @see #ungetService(ctkServiceReference*)
     * @see ctkServiceFactory
     */
    QObject* getService(ctkServiceReference* reference);

    ctkPlugin* installPlugin(const QUrl& location, QIODevice* in = 0);


    bool connectPluginListener(const QObject* receiver, const char* method, Qt::ConnectionType type = Qt::QueuedConnection);

    bool connectFrameworkListener(const QObject* receiver, const char* method, Qt::ConnectionType type = Qt::QueuedConnection);

  protected:

    friend class ctkPluginFrameworkPrivate;
    friend class ctkPlugin;
    friend class ctkPluginPrivate;

    ctkPluginContext(ctkPluginPrivate* plugin);

    ctkPluginContextPrivate * const d_ptr;
  };


#endif /* CTKPLUGINCONTEXT_H_ */
