#ifndef CTKPLUGINARCHIVEINTERFACE_P_H
#define CTKPLUGINARCHIVEINTERFACE_P_H

#include <QString>

typedef quint64 qtimestamp;

/**
 * Interface for managing plugin data.
 *
 */
struct PluginArchiveInterface {

  /**
   * Autostart setting stopped.
   * @see PluginArchiveInterface#setAutostartSetting(const QString&)
   */
  static const QString AUTOSTART_SETTING_STOPPED; // = "stopped"

  /**
   * Autostart setting eager.
   * @see PluginArchiveInterface#setAutostartSetting(const QString&)
   */
  static const QString AUTOSTART_SETTING_EAGER; // = "eager"

  /**
   * Autostart setting declared activation policy.
   * @see PluginArchiveInterface#setAutostartSetting(const QString&)
   */
  static const QString AUTOSTART_SETTING_ACTIVATION_POLICY; // = "activation_policy"


  /**
   * Get an attribute from the manifest of a plugin.
   *
   * Not localized
   *
   * @param key Name of attribute to get.
   * @return A string with result or null if the entry doesn't exists.
   */
  QString getAttribute(const QString& key) const = 0;

  /**
   * Gets all localization entries from this plugin. Will typically
   * read the file OSGI-INF/plugin_&lt;locale&gt;.properties.
   *
   * @param localeFile Filename within archive for localization properties.
   * @return null or a mapping of the entries.
   */
  QHash<QString,QString> getLocalizationEntries(const QString& localeFile) const = 0;


  /**
   * @returns the (raw/unlocalized) attributes
   */
  QHash<QString,QString> getUnlocalizedAttributes() const = 0;


  /**
   * Get plugin identifier for this plugin archive.
   *
   * @return Plugin identifier.
   */
  int getPluginId() const = 0;


  /**
   * Get bundle location for this plugin archive.
   *
   * @return Bundle location.
   */
  QString getPluginLocation() const = 0;


  /**
   * Get a QIODevice to a named entry inside a plugin.
   * Leading '/' is stripped.
   *
   * @param component Entry to get reference to.
   * @return QIODevice to entry or null if it doesn't exist.
   */
  QIODevice* getPluginResource(const QString& component) const = 0;


  /**
   * Returns a QStringList of all the paths
   * to entries within the plugin whose longest sub-path matches the supplied
   * path argument.
   *
   * @param name
   * @return
   */
  QStringList findResourcesPath(const QString& path) const = 0;


  /**
   * Get stored plugin start level.
   */
  int getStartLevel() const = 0;


  /**
   * Set stored plugin start level.
   */
  void setStartLevel(int level) = 0;


  /**
   * Get last modified timestamp.
   */
  qtimestamp getLastModified() const = 0;


  /**
   * Set stored last modified timestamp.
   */
  void setLastModified(qtimestamp timemillisecs) = 0;


  /**
   * Get auto-start setting.
   *
   * @return the autostart setting. "-1" if the plugin is not started.
   */
  int getAutostartSetting() const = 0;


  /**
   * Set the auto-start setting.
   *
   * @param setting the autostart setting to use.
   */
  void setAutostartSetting(int setting) = 0;


  /**
   * @return the location of the cached plugin.
   */
  QString getLibLocation() const = 0;


  /**
   * Get certificate chains associated with a plugin.
   *
   * @param onlyTrusted Only return trusted certificates.
   * @return All certificates or null if bundle is unsigned.
   */
  //QList<> getCertificateChains(bool onlyTrusted) const = 0;


  /**
   * Mark certificate associated with the plugin as trusted.
   *
   */
  //void trustCertificateChain(QList<> trustedChain) = 0;


  /**
   * Remove plugin from persistent storage.
   */
  void purge() = 0;


  /**
   * Close archive and all its open files.
   */
  void close() = 0;

}


#endif // CTKPLUGINARCHIVEINTERFACE_P_H
