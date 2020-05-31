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

#include "ctkLocationManager_p.h"

#include "ctkPluginFrameworkProperties_p.h"
#include "ctkBasicLocation_p.h"

#include "ctkException.h"
#include "ctkPluginFrameworkLauncher.h"
#include "ctkPluginConstants.h"

#include <QCoreApplication>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <QUrl>

namespace {

// Constants for configuration location discovery
static const QString CTK = "commontk";
static const QString PRODUCT_SITE_MARKER = ".commontkproduct";
static const QString PRODUCT_SITE_ID = "id";
static const QString PRODUCT_SITE_VERSION = "version";

static const QString CONFIG_DIR = "configuration";

// Data mode constants for user, configuration and data locations.
static const QString NONE = "@none";
static const QString NO_DEFAULT = "@noDefault";
static const QString USER_HOME = "@user.home";
static const QString USER_DIR = "@user.dir";
// Placeholder for hashcode of installation directory
static const QString INSTALL_HASH_PLACEHOLDER = "@install.hash";

static const QString INSTANCE_DATA_AREA_PREFIX = ".metadata/.plugins/";

static QScopedPointer<ctkBasicLocation> installLocation;
static QScopedPointer<ctkBasicLocation> configurationLocation;
static QScopedPointer<ctkBasicLocation> userLocation;
static QScopedPointer<ctkBasicLocation> instanceLocation;
static QScopedPointer<ctkBasicLocation> ctkHomeLocation;

static bool CanWrite(const QUrl& location);
static bool CanWrite(const QFileInfo& location);
static QString SubstituteVar(const QString& source, const QString& var, const QString& prop);
static QString GetInstallDirHash();
static QString ComputeDefaultUserAreaLocation(const QString& pathAppendage);

static bool isInitialized = false;

//----------------------------------------------------------------------------
static QUrl BuildUrl(const QString& location, bool trailingSlash)
{
  QUrl result(location,QUrl::StrictMode);

  if (!result.isValid() || result.scheme().compare("file", Qt::CaseInsensitive) != 0)
  {
    result = QUrl::fromLocalFile(location);
  }

  bool isFile = result.isValid() && result.scheme().compare("file", Qt::CaseInsensitive) == 0;
  if (isFile)
  {
    QString path = result.path();
    if (trailingSlash == path.endsWith('/'))
    {
      return result;
    }
    path = trailingSlash ? path + '/' : path.left(path.size()-1);
    result.setPath(path);
  }
  return result;
}

//----------------------------------------------------------------------------
static QString ComputeCTKHomeLocation()
{
  // TODO MacOS do we need to walk out of the application bundle?
  return QCoreApplication::applicationDirPath();
}

//----------------------------------------------------------------------------
ctkBasicLocation* BuildLocation(const QString& property, const QUrl& defaultLocation,
                           const QString& userDefaultAppendage, bool readOnlyDefault,
                           bool computeReadOnly, const QString& dataAreaPrefix)
{
  QString location = ctkPluginFrameworkProperties::clearProperty(property).toString();
  // the user/product may specify a non-default readOnly setting
  QVariant userReadOnlySetting = ctkPluginFrameworkProperties::getProperty(property + ctkLocationManager::READ_ONLY_AREA_SUFFIX);
  bool readOnly = (userReadOnlySetting.isNull() ? readOnlyDefault : userReadOnlySetting.toBool());
  // if the instance location is not set, predict where the workspace will be and
  // put the instance area inside the workspace meta area.
  if (location.isNull())
  {
    return new ctkBasicLocation(property, defaultLocation,
                                !userReadOnlySetting.isNull() || !computeReadOnly ? readOnly : !CanWrite(defaultLocation),
                                dataAreaPrefix);
  }
  QString trimmedLocation = location.trimmed();
  if (trimmedLocation.compare(NONE, Qt::CaseInsensitive) == 0)
  {
    return NULL;
  }
  if (trimmedLocation.compare(NO_DEFAULT, Qt::CaseInsensitive) == 0)
  {
    return new ctkBasicLocation(property, QUrl(), readOnly, dataAreaPrefix);
  }
  if (trimmedLocation.startsWith(USER_HOME))
  {
    QString base = SubstituteVar(location, USER_HOME, ctkPluginFrameworkLauncher::PROP_USER_HOME);
    location = QFileInfo(QDir(base), userDefaultAppendage).absoluteFilePath();
  }
  else if (trimmedLocation.startsWith(USER_DIR))
  {
    QString base = SubstituteVar(location, USER_DIR, ctkPluginFrameworkLauncher::PROP_USER_DIR);
    location = QFileInfo(QDir(base), userDefaultAppendage).absoluteFilePath();
  }
  int idx = location.indexOf(INSTALL_HASH_PLACEHOLDER);
  if (idx == 0)
  {
    throw ctkRuntimeException("The location cannot start with '" + INSTALL_HASH_PLACEHOLDER + "': " + location);
  }
  else if (idx > 0)
  {
    location = location.left(idx) + GetInstallDirHash() + location.mid(idx + INSTALL_HASH_PLACEHOLDER.size());
  }
  QUrl url = BuildUrl(location, true);
  ctkBasicLocation* result = NULL;
  if (url.isValid())
  {
    result = new ctkBasicLocation(property, QUrl(), !userReadOnlySetting.isNull() || !computeReadOnly ? readOnly : !CanWrite(url), dataAreaPrefix);
    result->set(url, false);
  }
  return result;
}

//----------------------------------------------------------------------------
static QString SubstituteVar(const QString& source, const QString& var, const QString& prop)
{
  QString value = ctkPluginFrameworkProperties::getProperty(prop).toString();
  return value + source.mid(var.size());
}

//----------------------------------------------------------------------------
static QUrl ComputeInstallConfigurationLocation()
{
  QString property = ctkPluginFrameworkProperties::getProperty(ctkPluginFrameworkLauncher::PROP_INSTALL_AREA).toString();
  if (!property.isEmpty())
  {
    return BuildUrl(property, true);
  }
  return QUrl();
}

//----------------------------------------------------------------------------
static QUrl ComputeSharedConfigurationLocation()
{
  QVariant property = ctkPluginFrameworkProperties::getProperty(ctkPluginFrameworkLauncher::PROP_SHARED_CONFIG_AREA);
  if (property.isNull())
  {
    return QUrl();
  }

  QUrl sharedConfigurationURL = BuildUrl(property.toString(), true);
  if (!sharedConfigurationURL.isValid())
  {
    return QUrl();
  }
  if (sharedConfigurationURL.path().startsWith("/"))
  {
    // absolute
    return sharedConfigurationURL;
  }
  QUrl installURL = installLocation->getUrl();
  if (sharedConfigurationURL.scheme() != installURL.scheme())
  {
    // different protocol
    return sharedConfigurationURL;
  }
  QString sharedConfigPath = QDir::cleanPath(installURL.path() + '/' + sharedConfigurationURL.path());
  sharedConfigurationURL = installURL;
  sharedConfigurationURL.setPath(sharedConfigPath);
  ctkPluginFrameworkProperties::setProperty(ctkPluginFrameworkLauncher::PROP_SHARED_CONFIG_AREA, sharedConfigurationURL.toString());
  return QUrl();
}

//----------------------------------------------------------------------------
static QString ComputeDefaultConfigurationLocation()
{
  // 1) We store the config state relative to the 'ctk' directory if possible
  // 2) If this directory is read-only
  //    we store the state in <user.home>/.commontk/<application-id>_<version> where <user.home>
  //    is unique for each local user, and <application-id> is the one
  //    defined in .eclipseproduct marker file. If .eclipseproduct does not
  //    exist, use "eclipse" as the application-id.

  QUrl installURL = ComputeInstallConfigurationLocation();
  if (installURL.isValid() && installURL.scheme() == "file")
  {
    QDir installDir(installURL.toLocalFile());
    QFileInfo defaultConfigDir(installDir, CONFIG_DIR);
    if (!defaultConfigDir.exists())
    {
      installDir.mkpath(defaultConfigDir.absoluteFilePath());
    }
    if (defaultConfigDir.exists() && CanWrite(defaultConfigDir))
    {
      return defaultConfigDir.absoluteFilePath();
    }
  }
  // We can't write in the CTK install dir so try for some place in the user's home dir
  return ComputeDefaultUserAreaLocation(CONFIG_DIR);
}

//----------------------------------------------------------------------------
static bool CanWrite(const QFileInfo& location)
{
  if (location.isWritable() == false)
  {
    return false;
  }

  if (!location.isDir())
  {
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------
static bool CanWrite(const QUrl& location)
{
  if (location.isValid() && location.scheme() == "file")
  {
    QFileInfo locationDir(location.toLocalFile());
    if (!locationDir.exists())
    {
      QDir().mkpath(locationDir.absoluteFilePath());
    }
    if (locationDir.exists() && CanWrite(locationDir))
      return true;
  }
  return false;
}

//----------------------------------------------------------------------------
static QString ComputeDefaultUserAreaLocation(const QString& pathAppendage)
{
  //    we store the state in <user.home>/.commontk/<application-id>_<version> where <user.home>
  //    is unique for each local user, and <application-id> is the one
  //    defined in .commontkproduct marker file. If .commontkproduct does not
  //    exist, use "commontk" as the application-id.
  QString installProperty = ctkPluginFrameworkProperties::getProperty(ctkPluginFrameworkLauncher::PROP_INSTALL_AREA).toString();
  QUrl installURL = BuildUrl(installProperty, true);
  if (!installURL.isValid())
  {
    return QString();
  }
  QFileInfo installDir(installURL.toLocalFile());
  QString installDirHash = GetInstallDirHash();

  QString appName = "." + CTK;
  QFileInfo ctkProduct(QDir(installDir.absoluteFilePath()), PRODUCT_SITE_MARKER);
  if (ctkProduct.exists())
  {
    QSettings props(ctkProduct.absoluteFilePath(), QSettings::IniFormat);

    QString appId = props.value(PRODUCT_SITE_ID).toString();
    if (appId.trimmed().isEmpty())
    {
      appId = CTK;
    }
    QString appVersion = props.value(PRODUCT_SITE_VERSION).toString();
    if (appVersion.trimmed().isEmpty())
    {
      appVersion = "";
    }
    appName += "/" + appId + "_" + appVersion + "_" + installDirHash;
  }
  else
  {
    // add the hash to help prevent collisions
    appName += "/" + installDirHash;
  }
  QString userHome = ctkPluginFrameworkProperties::getProperty(ctkPluginFrameworkLauncher::PROP_USER_HOME).toString();
  return QFileInfo(QDir(userHome), appName + "/" + pathAppendage).absoluteFilePath();
}

/**
 * Return hash code identifying an absolute installation path
 * @return hash code as String
 */
//----------------------------------------------------------------------------
static QString GetInstallDirHash()
{
  // compute an install dir hash to prevent configuration area collisions with other eclipse installs
  QString installProperty = ctkPluginFrameworkProperties::getProperty(ctkPluginFrameworkLauncher::PROP_INSTALL_AREA).toString();
  QUrl installURL = BuildUrl(installProperty, true);
  if (!installURL.isValid())
  {
    return "";
  }
  QFileInfo installDir(installURL.toLocalFile());
  QString stringToHash = installDir.canonicalFilePath();
  if (stringToHash.isEmpty())
  {
    // fall back to absolute path
    stringToHash = installDir.absoluteFilePath();
  }
  uint hashCode = qHash(stringToHash);
  return QString::number(hashCode);
}

}

const QString ctkLocationManager::READ_ONLY_AREA_SUFFIX = ".readOnly";

// configuration area file/dir names
const QString ctkLocationManager::CONFIG_FILE = "config.ini";
const QString ctkLocationManager::CTK_PROPERTIES = "ctk.properties";


//----------------------------------------------------------------------------
void ctkLocationManager::initializeLocations()
{
  if (isInitialized) return;

  // set the ctk storage area if it exists
  QVariant osgiStorage = ctkPluginFrameworkProperties::getProperty(ctkPluginConstants::FRAMEWORK_STORAGE);
  if (!osgiStorage.isNull())
  {
    ctkPluginFrameworkProperties::setProperty(ctkPluginFrameworkLauncher::PROP_CONFIG_AREA, osgiStorage);
  }
  // do install location initialization first since others may depend on it
  // assumes that the property is already set
  installLocation.reset(BuildLocation(ctkPluginFrameworkLauncher::PROP_INSTALL_AREA, QUrl(), "", true, false, QString()));

  // TODO not sure what the data area prefix should be here for the user area
  QScopedPointer<ctkBasicLocation> temp(BuildLocation(ctkPluginFrameworkLauncher::PROP_USER_AREA_DEFAULT,
                                                      QUrl(), "", false, false, QString()));
  QUrl defaultLocation = temp ? temp->getUrl() : QUrl();
  if (!defaultLocation.isValid())
  {
    defaultLocation = BuildUrl(QFileInfo(QDir(ctkPluginFrameworkProperties::getProperty(ctkPluginFrameworkLauncher::PROP_USER_HOME).toString()), "user").absoluteFilePath(), true);
  }
  userLocation.reset(BuildLocation(ctkPluginFrameworkLauncher::PROP_USER_AREA, defaultLocation, "", false, false, QString()));

  temp.reset(BuildLocation(ctkPluginFrameworkLauncher::PROP_INSTANCE_AREA_DEFAULT, QUrl(), "", false, false, INSTANCE_DATA_AREA_PREFIX));
  defaultLocation = temp ? temp->getUrl() : QUrl();
  if (!defaultLocation.isValid())
  {
    defaultLocation = BuildUrl(QFileInfo(QDir(ctkPluginFrameworkProperties::getProperty(ctkPluginFrameworkLauncher::PROP_USER_DIR).toString()), "workspace").absoluteFilePath(), true);
  }
  instanceLocation.reset(BuildLocation(ctkPluginFrameworkLauncher::PROP_INSTANCE_AREA, defaultLocation, "", false, false, INSTANCE_DATA_AREA_PREFIX));

  //mungeConfigurationLocation();

  // compute a default but it is very unlikely to be used since main will have computed everything
  temp.reset(BuildLocation(ctkPluginFrameworkLauncher::PROP_CONFIG_AREA_DEFAULT, QUrl(), "", false, false, QString()));
  defaultLocation = temp ? temp->getUrl() : QUrl();
  if (!defaultLocation.isValid() && ctkPluginFrameworkProperties::getProperty(ctkPluginFrameworkLauncher::PROP_CONFIG_AREA).isNull())
  {
    // only compute the default if the configuration area property is not set
    defaultLocation = BuildUrl(ComputeDefaultConfigurationLocation(), true);
  }
  configurationLocation.reset(BuildLocation(ctkPluginFrameworkLauncher::PROP_CONFIG_AREA, defaultLocation, "", false, false, QString()));
  // get the parent location based on the system property. This will have been set on the
  // way in either by the caller/user or by main.  There will be no parent location if we are not
  // cascaded.
  QUrl parentLocation = ComputeSharedConfigurationLocation();
  if (parentLocation.isValid() && parentLocation != configurationLocation->getUrl())
  {
    ctkBasicLocation* parent = new ctkBasicLocation(QString(), parentLocation, true, QString());
    configurationLocation->setParent(parent);
  }
  //initializeDerivedConfigurationLocations();

  if (ctkPluginFrameworkProperties::getProperty(ctkPluginFrameworkLauncher::PROP_HOME_LOCATION_AREA).isNull())
  {
    QString blueberryHomeLocationPath = ComputeCTKHomeLocation();
    if (!blueberryHomeLocationPath.isEmpty())
    {
      ctkPluginFrameworkProperties::setProperty(ctkPluginFrameworkLauncher::PROP_HOME_LOCATION_AREA, blueberryHomeLocationPath);
    }
  }
  // if ctk.home.location is not set then default to ctk.install.area
  if (ctkPluginFrameworkProperties::getProperty(ctkPluginFrameworkLauncher::PROP_HOME_LOCATION_AREA).isNull() &&
      !ctkPluginFrameworkProperties::getProperty(ctkPluginFrameworkLauncher::PROP_INSTALL_AREA).isNull())
  {
    ctkPluginFrameworkProperties::setProperty(ctkPluginFrameworkLauncher::PROP_HOME_LOCATION_AREA,
                                              ctkPluginFrameworkProperties::getProperty(ctkPluginFrameworkLauncher::PROP_INSTALL_AREA));
  }
  ctkHomeLocation.reset(BuildLocation(ctkPluginFrameworkLauncher::PROP_HOME_LOCATION_AREA, QUrl(), "", true, true, QString()));

  isInitialized = true;
}

//----------------------------------------------------------------------------
ctkBasicLocation* ctkLocationManager::getUserLocation()
{
  return userLocation.data();
}

//----------------------------------------------------------------------------
ctkBasicLocation* ctkLocationManager::getConfigurationLocation()
{
  return configurationLocation.data();
}

//----------------------------------------------------------------------------
ctkBasicLocation* ctkLocationManager::getInstallLocation()
{
  return installLocation.data();
}

//----------------------------------------------------------------------------
ctkBasicLocation* ctkLocationManager::getInstanceLocation()
{
  return instanceLocation.data();
}

//----------------------------------------------------------------------------
ctkBasicLocation* ctkLocationManager::getCTKHomeLocation()
{
  return ctkHomeLocation.data();
}
