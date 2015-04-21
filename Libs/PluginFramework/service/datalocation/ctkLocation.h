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

#ifndef CTKLOCATION_H
#define CTKLOCATION_H

#include <ctkPluginFrameworkExport.h>

#include <QObject>

class QUrl;

/**
 * A ctkLocation represents a QUrl which may have a default value, may be read only, may
 * or may not have a current value and may be cascaded on to a parent location.
 */
struct CTK_PLUGINFW_EXPORT ctkLocation
{

  /**
   * Constant which defines the filter string for acquiring the service which
   * specifies the instance location.
   */
  static const QString INSTANCE_FILTER;

  /**
   * Constant which defines the filter string for acquiring the service which
   * specifies the install location.
   */
  static const QString INSTALL_FILTER;

  /**
   * Constant which defines the filter string for acquiring the service which
   * specifies the configuration location.
   */
  static const QString CONFIGURATION_FILTER;

  /**
   * Constant which defines the filter string for acquiring the service which
   * specifies the user location.
   */
  static const QString USER_FILTER;

  /**
   * Constant which defines the filter string for acquiring the service which
   * specifies the eclipse home location.
   */
  static const QString CTK_HOME_FILTER;


  virtual ~ctkLocation() {}

  /**
   * Returns <code>true</code> if this location allows a default value to be assigned
   * and <code>false</code> otherwise.
   *
   * @return whether or not this location can have a default value assigned
   */
  virtual bool allowsDefault() const = 0;

  /**
   * Returns the default value of this location if any.  If no default is available then
   * <code>null</code> is returned. Note that even locations which allow defaults may still
   * return <code>null</code>.
   *
   * @return the default value for this location or <code>null</code>
   */
  virtual QUrl getDefault() const = 0;

  /**
   * Returns the parent of this location or <code>null</code> if none is available.
   *
   * @return the parent of this location or <code>null</code>
   */
  virtual ctkLocation* getParentLocation() const = 0;

  /**
   * Returns the actual QUrl of this location.  If the location's value has been set,
   * that value is returned.  If the value is not set and the location allows defaults,
   * the value is set to the default and returned.  In all other cases <code>null</code>
   * is returned.
   *
   * @return the URL for this location or <code>null</code> if none
   */
  virtual QUrl getUrl() const = 0;

  /**
   * Returns <code>true</code> if this location has a value and <code>false</code>
   * otherwise.
   *
   * @return boolean value indicating whether or not the value is set
   */
  virtual bool isSet() const = 0;

  /**
   * Returns <code>true</code> if this location represents a read only location and
   * <code>false</code> otherwise.  The read only character
   * of a location is not in enforced in any way but rather expresses the intention of the
   * location's creator.
   *
   * @return boolean value indicating whether the location is read only
   */
  virtual bool isReadOnly() const = 0;

  /**
   * Sets and optionally locks the location's value to the given QUrl.  If the location
   * already has a value an exception is thrown.  If locking is requested and fails, <code>false</code>
   * is returned and the QUrl of this location is not set.
   *
   * @param value the value of this location
   * @param lock whether or not to lock this location
   * @return whether or not the location was successfully set and, if requested, locked.
   * @throws ctkIllegalStateException if the location's value is already set
   * @throws ctkRuntimeException if there was an unexpected problem while acquiring the lock
   */
  virtual bool set(const QUrl& value, bool lock) = 0;

  /**
   * Sets and optionally locks the location's value to the given QUrl using the given lock file.  If the location
   * already has a value an exception is thrown.  If locking is requested and fails, <code>false</code>
   * is returned and the QUrl of this location is not set.
   *
   * @param value the value of this location
   * @param lock whether or not to lock this location
   * @param lockFilePath the path to the lock file.  This path will be used to establish locks on this location.
   * The path may be an absolute path or it may be relative to the given URL.  If a <code>null</code>
   * value is used then a default lock path will be used for this location.
   * @return whether or not the location was successfully set and, if requested, locked.
   * @throws ctkIllegalStateException if the location's value is already set
   * @throws ctkRuntimeException if there was an unexpected problem while acquiring the lock
   */
  virtual bool set(const QUrl& value, bool lock, const QString& lockFilePath) = 0;

  /**
   * Attempts to lock this location with a canonical locking mechanism and return
   * <code>true</code> if the lock could be acquired.  Not all locations can be
   * locked.
   * <p>
   * Locking a location is advisory only.  That is, it does not prevent other applications from
   * modifying the same location
   * </p>
   * @return true if the lock could be acquired; otherwise false is returned
   *
   * @exception ctkRuntimeException if there was an unexpected problem while acquiring the lock
   */
  virtual bool lock() = 0;

  /**
   * Releases the lock on this location.  If the location is not already locked, no action
   * is taken.
   */
  virtual void release() = 0;

  /**
   * Returns <code>true</code> if this location is locked and <code>false</code>
   * otherwise.
   * @return boolean value indicating whether or not this location is locked
   * @throws ctkRuntimeException if there was an unexpected problem reading the lock
   */
  virtual bool isLocked() const = 0;

  /**
   * Constructs a new location.
   * @param parent the parent location.  A <code>null</code> value is allowed.
   * @param defaultValue the default value of the location. A <code>null</code> value is allowed.
   * @param readonly true if the location is read-only.
   * @return a new location.
   */
  virtual ctkLocation* createLocation(ctkLocation* parent, const QUrl& defaultValue, bool readonly) = 0;

  /**
   * Returns a URL to the specified path within this location.  The path
   * of the returned URL may not exist yet.  It is the responsibility of the
   * client to create the content of the data area returned if it does not exist.
   * <p>
   * This method can be used to obtain a private area within the given location.
   * For example use the symbolic name of a plugin to obtain a data area specific
   * to that plugin.
   * </p>
   * <p>
   * Clients should check if the location is read only before writing anything
   * to the returned data area.  An <code>ctkRuntimeException</code> will be thrown if
   * this method is called and the location URL has not been set and there is
   * no default value for this location.
   * </p>
   *
   * @param path the name of the path to get from this location
   * @return the URL to the data area with the specified path.
   * @throws ctkRuntimeException if the location URL is not already set
   */
  virtual QUrl getDataArea(const QString& path) const = 0;

};

Q_DECLARE_INTERFACE(ctkLocation, "org.commontk.service.datalocation.Location")

#endif // CTKLOCATION_H
