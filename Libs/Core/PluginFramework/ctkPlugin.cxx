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

#include "ctkPlugin.h"

#include "ctkPluginPrivate_p.h"
#include "ctkPluginArchive_p.h"

#include <QStringList>

namespace ctk {

  Plugin::Plugin(PluginFrameworkContextPrivate* fw,
                 PluginArchive* pa)
    : d_ptr(new PluginPrivate(*this, fw, pa))
  {

  }

  Plugin::Plugin(PluginPrivate& dd)
    : d_ptr(&dd)
  {

  }

  Plugin::~Plugin()
  {
    delete d_ptr;
  }

  Plugin::State Plugin::getState() const
  {
    Q_D(const Plugin);
    return d->state;
  }

  void Plugin::start()
  {

  }

  void Plugin::stop()
  {

  }

  PluginContext* Plugin::getPluginContext() const
  {
    //TODO security checks
    Q_D(const Plugin);
    return d->pluginContext;
  }

  int Plugin::getPluginId() const
  {
    Q_D(const Plugin);
    return d->id;
  }

  QString Plugin::getLocation() const
  {
    //TODO security
    Q_D(const Plugin);
    return d->location;
  }

  QString Plugin::getSymbolicName() const
  {
    Q_D(const Plugin);
    return d->symbolicName;
  }

  QStringList Plugin::getResourceList(const QString& path) const
  {
    Q_D(const Plugin);
    return d->archive->findResourcesPath(path);
  }

  QByteArray Plugin::getResource(const QString& path) const
  {
    Q_D(const Plugin);
    return d->archive->getPluginResource(path);
  }

  Version Plugin::getVersion() const
  {
    Q_D(const Plugin);
    return d->version;
  }

}
