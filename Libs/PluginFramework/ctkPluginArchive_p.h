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

#ifndef CTKPLUGINARCHIVE_P_H
#define CTKPLUGINARCHIVE_P_H

#include <QHash>
#include <QUrl>
#include <QDateTime>

/**
 * \ingroup PluginFramework
 *
 * Interface for managing plugin data.
 *
 */
class ctkPluginArchive
{

public:

  /**
   * Autostart setting stopped.
   * @see ctkPluginArchive#setAutostartSetting(const QString&)
   */
  static const QString AUTOSTART_SETTING_STOPPED; // = "stopped"

  /**
   * Autostart setting eager.
   * @see ctkPluginArchive#setAutostartSetting(const QString&)
   */
  static const QString AUTOSTART_SETTING_EAGER; // = "eager"

  /**
   * Autostart setting declared activation policy.
   * @see ctkPluginArchive#setAutostartSetting(const QString&)
   */
  static const QString AUTOSTART_SETTING_ACTIVATION_POLICY; // = "activation_policy"

  virtual ~ctkPluginArchive() {}

  /**
   * Get an attribute from the manifest of a plugin.
   *
   * Not localized
   *
   * @param key Name of attribute to get.
   * @return A string with result or null if the entry doesn't exists.
   */
  virtual QString getAttribute(const QString& key) const = 0;


  /**
   * @returns the (raw/unlocalized) attributes
   */
  virtual QHash<QString,QString> getUnlocalizedAttributes() const = 0;


  /**
   * Get the plugin generation associated with this plugin archive.
   *
   * @return A integer representing the generation.
   */
  virtual int getPluginGeneration() const = 0;


  /**
   * Get plugin identifier for this plugin archive.
   *
   * @return ctkPlugin identifier.
   */
  virtual int getPluginId() const = 0;


  /**
   * Get plugin location for this plugin archive.
   *
   * @return Bundle location.
   */
  virtual QUrl getPluginLocation() const = 0;


  /**
   * Get the path to the plugin library on the local
   * file system
   *
   * @return Path to the plugin library
   */
  virtual QString getLibLocation() const = 0;


  /**
   * Get a Qt resource as a byte array from a plugin. The resource
   * is cached and may be aquired even if the plugin is not active.
   *
   * @param component Resource to get the byte array from.
   * @return QByteArray to the entry (empty if it doesn't exist).
   */
  virtual QByteArray getPluginResource(const QString& component) const = 0;


  /**
   * Returns a QStringList of all the paths
   * to entries within the plugin whose longest sub-path matches the supplied
   * path argument.
   *
   * @param name
   * @return
   */
  virtual QStringList findResourcesPath(const QString& path) const = 0;


  /**
   * Get stored plugin start level.
   */
  virtual int getStartLevel() const = 0;


  /**
   * Set stored plugin start level.
   */
  virtual void setStartLevel(int level) = 0;


  /**
   * Get last modified timestamp.
   */
  virtual QDateTime getLastModified() const = 0;


  /**
   * Set stored last modified timestamp.
   */
  virtual void setLastModified(const QDateTime& timemillisecs) = 0;


  /**
   * Get auto-start setting.
   *
   * @return the autostart setting. "-1" if the plugin is not started.
   */
  virtual int getAutostartSetting() const = 0;


  /**
   * Set the auto-start setting.
   *
   * @param setting the autostart setting to use.
   */
  virtual void setAutostartSetting(int setting) = 0;


  /**
   * Get certificate chains associated with a plugin.
   *
   * @param onlyTrusted Only return trusted certificates.
   * @return All certificates or null if bundle is unsigned.
   */
  //QList<> getCertificateChains(bool onlyTrusted) const;


  /**
   * Mark certificate associated with the plugin as trusted.
   *
   */
  //void trustCertificateChain(QList<> trustedChain);


  /**
   * Remove plugin from persistent storage.
   */
  virtual void purge() = 0;

  /**
   * Close archive and all its open files.
   */
  virtual void close() = 0;

};



#endif // CTKPLUGINARCHIVE_P_H
