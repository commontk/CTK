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

#ifndef CTKAPPLICATIONDESCRIPTOR_H
#define CTKAPPLICATIONDESCRIPTOR_H

#include <ctkPluginFrameworkExport.h>

#include <ctkPluginFramework_global.h>

#include <QHash>
#include <QVariant>
#include <QString>
#include <QLocale>


struct ctkApplicationHandle;

/**
 * A service that represents an installed application and stores
 * information about it. The application descriptor can be used for instance
 * creation.
 */
struct CTK_PLUGINFW_EXPORT ctkApplicationDescriptor
{

  /**
   * The property key for the localized name of the application.
   */
  static const QString APPLICATION_NAME; // = "application.name";

  /**
   * The property key for the localized icon of the application.
   */
  static const QString APPLICATION_ICON; // = "application.icon";

  /**
   * The property key for the unique identifier (PID) of the application.
   */
  static const QString APPLICATION_PID; // = ctkPluginConstants::SERVICE_PID;

  /**
   * The property key for the version of the application.
   */
  static const QString APPLICATION_VERSION; // = "application.version";

  /**
   * The property key for the name of the application vendor.
   */
  static const QString APPLICATION_VENDOR; // = ctkPluginConstants::SERVICE_VENDOR;

  /**
   * The property key for the visibility property of the application.
   */
  static const QString APPLICATION_VISIBLE; // = "application.visible";

  /**
   * The property key for the launchable property of the application.
   */
  static const QString APPLICATION_LAUNCHABLE; // = "application.launchable";

  /**
   * The property key for the locked property of the application.
   */
  static const QString APPLICATION_LOCKED; // = "application.locked";

  /**
   * The property key for the localized description of the application.
   */
  static const QString APPLICATION_DESCRIPTION; // = "application.description";

  /**
   * The property key for the localized documentation of the application.
   */
  static const QString APPLICATION_DOCUMENTATION; // = "application.documentation";

  /**
   * The property key for the localized copyright notice of the application.
   */
  static const QString APPLICATION_COPYRIGHT; // = "application.copyright";

  /**
   * The property key for the localized license of the application.
   */
  static const QString APPLICATION_LICENSE; // = "application.license";

  /**
   * The property key for the application container of the application.
   */
  static const QString APPLICATION_CONTAINER; // = "application.container";

  /**
   * The property key for the location of the application.
   */
  static const QString APPLICATION_LOCATION; // = "application.location";


  virtual ~ctkApplicationDescriptor() {}

  /**
   * Returns the identifier of the represented application.
   *
   * @return the identifier of the represented application
   */
  virtual QString getApplicationId() const = 0;

  /**
   * Returns the properties of the application descriptor as key-value pairs.
   * The return value contains the locale aware and unaware properties as
   * well. The returned <code>QHash</code> will include the service
   * properties of this <code>ctkApplicationDescriptor</code> as well.
   * <p>
   * The returned <code>QHash</code> will contain the standard CTK service
   * properties as well
   * (e.g. service.id, service.vendor etc.) and specialized application
   * descriptors may offer further service properties. The returned Map contains
   * a snapshot of the properties. It will not reflect further changes in the
   * property values nor will the update of the Map change the corresponding
   * service property.
   *
   * @param locale
   *            the locale, defaults to the default constructed <code>QLocale()</code>.
   *
   * @return copy of the service properties of this application descriptor service,
   *         according to the specified locale.
   *
   * @throws ctkIllegalStateException
   *             if the application descriptor is unregistered
   */
  virtual ctkProperties getProperties(const QLocale& locale) const = 0;

  /**
   * Returns the properties of the application descriptor as key-value pairs.
   * The return value contains the raw (non-localized) property values.
   * The returned <code>QHash</code> will include the service
   * properties of this <code>ctkApplicationDescriptor</code> as well.
   * <p>
   * The returned <code>QHash</code> will contain the standard CTK service
   * properties as well
   * (e.g. service.id, service.vendor etc.) and specialized application
   * descriptors may offer further service properties. The returned QHash contains
   * a snapshot of the properties. It will not reflect further changes in the
   * property values nor will the update of the Map change the corresponding
   * service property.
   *
   * @return copy of the non-localized service properties of this application
   *         descriptor service.
   *
   * @throws ctkIllegalStateException
   *             if the application descriptor is unregistered
   */
  virtual ctkProperties getProperties() const = 0;

  /**
   * Launches a new instance of an application. The <code>arguments</code> parameter
   * specifies the startup parameters for the instance to be launched, it may
   * be empty.
   * <p>
   * The following steps are made:
   * <ul>
   * <li>Check the locking state of the application. If locked then throw an
   * {@link ctkApplicationException} with the reason code
   * {@link ctkApplicationException#APPLICATION_LOCKED}.
   * <li>Create and start an application instance.
   * <li>Returns the <code>ctkApplicationHandle</code>.
   * </ul>
   * <p>
   * The <code>QHash</code> argument of the launch method contains startup arguments
   * for the application. The keys used in the QHash must be non-empty
   * <code>QString</code> objects. They can be standard or application specific.
   * CTK defines the <code>org.commontk.triggeringevent</code> key to be used to pass
   * the triggering event to a scheduled application, however in the future it
   * is possible that other well-known keys will be defined. To avoid unwanted
   * clashes of keys, the following rules should be applied:
   * <ul>
   * <li>The keys starting with the dash (-) character are application
   * specific, no well-known meaning should be associated with them.</li>
   * <li>Well-known keys should follow the reverse domain name based naming.
   * In particular, the keys standardized in CTK should start with
   * <code>org.commontk.</code>.</li>
   * </ul>
   * <P>
   * The method is synchronous, it returns only when the application instance
   * was successfully started or the attempt to start it failed.
   * <P>
   * This method never returns <code>null</code>. If launching an application
   * fails, the appropriate exception is thrown.
   *
   * @param arguments Arguments for the newly launched application, may be
   *        null
   *
   * @return the registered ctkApplicationHandle, which represents the newly
   *         launched application instance. Never returns <code>null</code>.
   *
   * @throws ctkApplicationException if starting the application failed
   * @throws ctkIllegalStateException if the application descriptor is
   *         unregistered
   * @throws std::invalid_argument if the specified <code>QHash</code> contains
   *         invalid keys (empty <code>QString</code>)
   */
  virtual ctkApplicationHandle* launch(const QHash<QString, QVariant>& arguments) = 0;

};

Q_DECLARE_INTERFACE(ctkApplicationDescriptor, "org.commontk.service.application.ApplicationDescriptor")

#endif // CTKAPPLICATIONDESCRIPTOR_H
