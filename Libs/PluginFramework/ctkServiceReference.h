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

#ifndef CTKSERVICEREFERENCE_H
#define CTKSERVICEREFERENCE_H

#include <QVariant>

#include "ctkPlugin.h"

#include "CTKPluginFrameworkExport.h"


  class ServiceRegistrationPrivate;
  class ServiceReferencePrivate;

  /**
   * A reference to a service.
   *
   * <p>
   * The Framework returns <code>ServiceReference</code> objects from the
   * <code>PluginContext::getServiceReference</code> and
   * <code>PluginContext::getServiceReferences</code> methods.
   * <p>
   * A <code>ServiceReference</code> object may be shared between plugins and
   * can be used to examine the properties of the service and to get the service
   * object.
   * <p>
   * Every service registered in the Framework has a unique
   * <code>ServiceRegistration</code> object and may have multiple, distinct
   * <code>ServiceReference</code> objects referring to it.
   * <code>ServiceReference</code> objects associated with a
   * <code>ServiceRegistration</code> are considered equal
   * (more specifically, their <code>operator==()</code>
   * method will return <code>true</code> when compared).
   * <p>
   * If the same service object is registered multiple times,
   * <code>ServiceReference</code> objects associated with different
   * <code>ServiceRegistration</code> objects are not equal.
   *
   * @see PluginContext::getServiceReference
   * @see PluginContext::getServiceReferences
   * @see PluginContext::getService
   * @threadsafe
   */
  class CTK_PLUGINFW_EXPORT ServiceReference {

    Q_DECLARE_PRIVATE(ServiceReference)

  public:

    ~ServiceReference();

    /**
     * Returns the property value to which the specified property key is mapped
     * in the properties <code>ServiceProperties</code> object of the service
     * referenced by this <code>ServiceReference</code> object.
     *
     * <p>
     * Property keys are case-insensitive.
     *
     * <p>
     * This method must continue to return property values after the service has
     * been unregistered. This is so references to unregistered services can
     * still be interrogated.
     *
     * @param key The property key.
     * @return The property value to which the key is mapped; an invalid QVariant
     *         if there is no property named after the key.
     */
    QVariant getProperty(const QString& key) const;

    /**
     * Returns a list of the keys in the <code>ServiceProperties</code>
     * object of the service referenced by this <code>ServiceReference</code>
     * object.
     *
     * <p>
     * This method will continue to return the keys after the service has been
     * unregistered. This is so references to unregistered services can
     * still be interrogated.
     *
     * <p>
     * This method is not <i>case-preserving</i>; this means that every key in the
     * returned array is in lower case, which may not be the case for the corresponding key in the
     * properties <code>ServiceProperties</code> that was passed to the
     * {@link PluginContext::registerService(const QStringList&, QObject*, const ServiceProperties&)} or
     * {@link ServiceRegistration::setProperties} methods.
     *
     * @return A list of property keys.
     */
    QStringList getPropertyKeys() const;

    /**
     * Returns the plugin that registered the service referenced by this
     * <code>ServiceReference</code> object.
     *
     * <p>
     * This method must return <code>0</code> when the service has been
     * unregistered. This can be used to determine if the service has been
     * unregistered.
     *
     * @return The plugin that registered the service referenced by this
     *         <code>ServiceReference</code> object; <code>0</code> if that
     *         service has already been unregistered.
     * @see PluginContext::registerService(const QStringList&, QObject* , const ServiceProperties&)
     */
    Plugin* getPlugin() const;

    /**
     * Returns the plugins that are using the service referenced by this
     * <code>ServiceReference</code> object. Specifically, this method returns
     * the plugins whose usage count for that service is greater than zero.
     *
     * @return A list of plugins whose usage count for the service referenced
     *         by this <code>ServiceReference</code> object is greater than
     *         zero.
     */
    QList<Plugin*> getUsingPlugins() const;

    /**
     * Compares this <code>ServiceReference</code> with the specified
     * <code>ServiceReference</code> for order.
     *
     * <p>
     * If this <code>ServiceReference</code> and the specified
     * <code>ServiceReference</code> have the same {@link PluginConstants::SERVICE_ID
     * service id} they are equal. This <code>ServiceReference</code> is less
     * than the specified <code>ServiceReference</code> if it has a lower
     * {@link PluginConstants::SERVICE_RANKING service ranking} and greater if it has a
     * higher service ranking. Otherwise, if this <code>ServiceReference</code>
     * and the specified <code>ServiceReference</code> have the same
     * {@link PluginConstants::SERVICE_RANKING service ranking}, this
     * <code>ServiceReference</code> is less than the specified
     * <code>ServiceReference</code> if it has a higher
     * {@link PluginConstants::SERVICE_ID service id} and greater if it has a lower
     * service id.
     *
     * @param reference The <code>ServiceReference</code> to be compared.
     * @return Returns a false or true if this
     *         <code>ServiceReference</code> is less than or greater
     *         than the specified <code>ServiceReference</code>.
     * @throws std::invalid_argument If the specified
     *         <code>ServiceReference</code> was not created by the same
     *         framework instance as this <code>ServiceReference</code>.
     */
    bool operator<(const ServiceReference& reference) const;

    bool operator==(const ServiceReference& reference) const;


  protected:

    friend class ServiceRegistrationPrivate;
    friend class PluginContext;

    ServiceReference(ServiceRegistrationPrivate* reg);

    ServiceReferencePrivate * const d_ptr;

  };


#endif // CTKSERVICEREFERENCE_H
