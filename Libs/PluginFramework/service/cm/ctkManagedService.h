/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
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


#ifndef CTKMANAGEDSERVICE_H
#define CTKMANAGEDSERVICE_H

#include <QObject>
#include <ctkDictionary.h>

/**
 * \ingroup ConfigAdmin
 *
 * A service that can receive configuration data from a Configuration Admin
 * service.
 *
 * <p>
 * A Managed Service is a service that needs configuration data. Such an object
 * should be registered with the Framework registry with the
 * <code>service.pid</code> property set to some unique identifier called a
 * PID.
 *
 * <p>
 * If the Configuration Admin service has a <code>ctkConfiguration</code> object
 * corresponding to this PID, it will callback the <code>updated()</code>
 * method of the <code>ctkManagedService</code> object, passing the properties of
 * that <code>ctkConfiguration</code> object.
 *
 * <p>
 * If it has no such <code>ctkConfiguration</code> object, then it calls back
 * with an empty properties argument. Registering a Managed Service
 * will always result in a callback to the <code>updated()</code> method
 * provided the Configuration Admin service is, or becomes active. This callback
 * must always be done asynchronously.
 *
 * <p>
 * Else, every time that either of the <code>updated()</code> methods is
 * called on that <code>ctkConfiguration</code> object, the
 * <code>ctkManagedService#updated()</code> method with the new properties is
 * called. If the <code>remove()</code> method is called on that
 * <code>ctkConfiguration</code> object, <code>ctkManagedService#updated()</code>
 * is called with an empty map for the properties parameter. All these
 * callbacks must be done asynchronously.
 *
 * <p>
 * The following example shows the code of a serial port that will create a port
 * depending on configuration information.
 *
 * \code
 *
 *   class SerialPort : public QObject, public ctkManagedService
 *   {
 *
 *     QMutex mutex;
 *     ctkServiceRegistration registration;
 *     Hashtable configuration;
 *     CommPortIdentifier id;
 *
 *     void open(CommPortIdentifier id, ctkPluginContext* context)
 *     {
 *       QMutexLocker lock(&mutex);
 *       this->id = id;
 *       registration = context->registerService<ctkManagedService>(
 *         this, getDefaults());
 *     }
 *
 *     ctkDictionary getDefaults()
 *     {
 *       ctkDictionary defaults;
 *       defaults.insert("port", id.getName());
 *       defaults.insert("product", "unknown");
 *       defaults.insert("baud", 9600);
 *       defaults.insert(ctkPluginConstants::SERVICE_PID,
 *         QString("com.acme.serialport.") + id.getName());
 *       return defaults;
 *     }
 *
 *   public:
 *
 *     void updated(const ctkDictionary& configuration)
 *     {
 *       QMutexLocker lock(&mutex);
 *       if (configuration.isEmpty())
 *       {
 *         registration.setProperties(getDefaults());
 *       }
 *       else
 *       {
 *         setSpeed(configuration["baud"].toInt());
 *         registration.setProperties(configuration);
 *       }
 *     }
 *     ...
 *   };
 *
 * \endcode
 *
 * <p>
 * As a convention, it is recommended that when a Managed Service is updated, it
 * should copy all the properties it does not recognize into the service
 * registration properties. This will allow the Configuration Admin service to
 * set properties on services which can then be used by other applications.
 */
struct ctkManagedService
{
  virtual ~ctkManagedService() {}

  /**
   * Update the configuration for a Managed Service.
   *
   * <p>
   * When the implementation of <code>updated(const ctkDictionary&)</code> detects any
   * kind of error in the configuration properties, it should create a new
   * <code>ctkConfigurationException</code> which describes the problem. This
   * can allow a management system to provide useful information to a human
   * administrator.
   *
   * <p>
   * If this method throws any other <code>exception</code>, the
   * Configuration Admin service must catch it and should log it.
   * <p>
   * The Configuration Admin service must call this method asynchronously
   * which initiated the callback. This implies that implementors of Managed
   * Service can be assured that the callback will not take place during
   * registration when they execute the registration in a synchronized method.
   *
   * @param properties A copy of the ctkConfiguration properties.
   *        This argument must not contain the
   *        "service.pluginLocation" property. The value of this property may
   *        be obtained from the <code>ctkConfiguration#getPluginLocation()</code>
   *        method.
   * @throws ctkConfigurationException when the update fails
   */
  virtual void updated(const ctkDictionary& properties) = 0;
};

Q_DECLARE_INTERFACE(ctkManagedService, "org.commontk.service.cm.ManagedService")

#endif // CTKMANAGEDSERVICE_H
