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

#include "ctkPluginFramework.h"

#include "ctkPluginFrameworkPrivate_p.h"
#include "ctkPluginPrivate_p.h"
#include "ctkPluginFrameworkContext_p.h"
#include "ctkPluginConstants.h"
#include "ctkPluginArchive_p.h"


ctkPluginFramework::ctkPluginFramework()
  : ctkPlugin()
{
  qRegisterMetaType<ctkPluginFrameworkEvent>("ctkPluginFrameworkEvent");
  qRegisterMetaType<ctkPluginEvent>("ctkPluginEvent");
}

void ctkPluginFramework::init()
{
  Q_D(ctkPluginFramework);

  QMutexLocker sync(&d->lock);
  // waitOnActivation(d->lock, "Framework.init", true);
  switch (d->state)
  {
  case ctkPlugin::INSTALLED:
  case ctkPlugin::RESOLVED:
    break;
  case ctkPlugin::STARTING:
  case ctkPlugin::ACTIVE:
    return;
  default:
    throw std::logic_error("INTERNAL ERROR, Illegal state");
  }
  d->init();
}

void ctkPluginFramework::start(const ctkPlugin::StartOptions& options)
{
  Q_UNUSED(options);
  Q_D(ctkPluginFramework);

  QStringList pluginsToStart;
  {
    QMutexLocker sync(&d->lock);
    // TODO: parallel start
    //waitOnActivation(lock, "ctkPluginFramework::start", true);

    switch (d->state)
    {
    case INSTALLED:
    case RESOLVED:
      d->init();
    case STARTING:
      d->activating = true;
      break;
    case ACTIVE:
      return;
    default:
      throw std::logic_error("INTERNAL ERROR, Illegal state");
    }

    pluginsToStart = d->fwCtx->storage->getStartOnLaunchPlugins();
  }

  // Start plugins according to their autostart setting.
  QStringListIterator i(pluginsToStart);
  while (i.hasNext())
  {
    ctkPlugin* plugin = d->fwCtx->plugins->getPlugin(i.next());
    try {
      const int autostartSetting = plugin->d_func()->archive->getAutostartSetting();
      // Launch must not change the autostart setting of a plugin
      StartOptions option = ctkPlugin::START_TRANSIENT;
      if (ctkPlugin::START_ACTIVATION_POLICY == autostartSetting)
      {
        // Transient start according to the plugins activation policy.
        option |= ctkPlugin::START_ACTIVATION_POLICY;
      }
      plugin->start(option);
    }
    catch (const ctkPluginException& pe)
    {
      d->fwCtx->listeners.frameworkError(plugin, pe);
    }
  }

  {
    QMutexLocker sync(&d->lock);
    d->state = ACTIVE;
    d->activating = false;
    d->fwCtx->listeners.emitFrameworkEvent(
        ctkPluginFrameworkEvent(ctkPluginFrameworkEvent::STARTED, this));
  }
}

QStringList ctkPluginFramework::getResourceList(const QString& path) const
{
  QString resourcePath = QString(":/") + ctkPluginConstants::SYSTEM_PLUGIN_SYMBOLICNAME;
  if (path.startsWith('/'))
    resourcePath += path;
  else
    resourcePath += QString("/") + path;

  QStringList paths;
  QFileInfoList entryInfoList = QDir(resourcePath).entryInfoList();
  QListIterator<QFileInfo> infoIter(entryInfoList);
  while (infoIter.hasNext())
  {
    const QFileInfo& resInfo = infoIter.next();
    QString entry = resInfo.canonicalFilePath().mid(resourcePath.size());
    if (resInfo.isDir())
      entry += "/";

    paths << entry;
  }

  return paths;
}

QByteArray ctkPluginFramework::getResource(const QString& path) const
{
  QString resourcePath = QString(":/") + ctkPluginConstants::SYSTEM_PLUGIN_SYMBOLICNAME;
  if (path.startsWith('/'))
    resourcePath += path;
  else
    resourcePath += QString("/") + path;

  QFile resourceFile(resourcePath);
  resourceFile.open(QIODevice::ReadOnly);
  return resourceFile.readAll();
}

QHash<QString, QString> ctkPluginFramework::getHeaders()
{
  //TODO security
  Q_D(ctkPluginFramework);
  return d->systemHeaders;
}
