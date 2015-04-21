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

#include "ctkPluginFrameworkProperties_p.h"

#include "ctkPluginFrameworkLauncher.h"

#include <QDir>
#include <QCoreApplication>

QMutex ctkPluginFrameworkProperties::lock;
ctkProperties ctkPluginFrameworkProperties::properties;

//----------------------------------------------------------------------------
ctkProperties& ctkPluginFrameworkProperties::getProperties()
{
  return properties;
}

//----------------------------------------------------------------------------
QVariant ctkPluginFrameworkProperties::getProperty(const QString& key)
{
  return getProperty(key, QVariant());
}

//----------------------------------------------------------------------------
QVariant ctkPluginFrameworkProperties::getProperty(const QString& key, const QVariant& defaultValue)
{
  QMutexLocker l(&lock);
  ctkProperties::iterator iter = properties.find(key);
  if (iter != properties.end()) return iter.value();
  return defaultValue;
}

//----------------------------------------------------------------------------
QVariant ctkPluginFrameworkProperties::setProperty(const QString& key, const QVariant& value)
{
  QMutexLocker l(&lock);
  QVariant oldVal = properties[key];
  properties[key] = value;
  return oldVal;
}

//----------------------------------------------------------------------------
QVariant ctkPluginFrameworkProperties::clearProperty(const QString& key)
{
  QMutexLocker l(&lock);
  return properties.take(key);
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkProperties::setProperties(const ctkProperties& input)
{
  QMutexLocker l(&lock);
  properties = input;
}

//----------------------------------------------------------------------------
void ctkPluginFrameworkProperties::initializeProperties()
{
  // initialize some framework properties that must always be set

  setProperty(ctkPluginFrameworkLauncher::PROP_USER_HOME, QDir::homePath());
  setProperty(ctkPluginFrameworkLauncher::PROP_USER_DIR, QDir::currentPath());

  if (getProperty(ctkPluginFrameworkLauncher::PROP_INSTALL_AREA).isNull())
  {
    setProperty(ctkPluginFrameworkLauncher::PROP_INSTALL_AREA, QCoreApplication::applicationDirPath());
  }
}
