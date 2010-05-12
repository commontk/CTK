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

#ifndef CTKPLUGINFRAMEWORKCONTEXT_P_H
#define CTKPLUGINFRAMEWORKCONTEXT_P_H

#include <QDebug>
#include <QMutex>

#include "ctkPluginFrameworkFactory.h"
#include "ctkPluginFramework.h"
#include "ctkPluginStorage_p.h"
#include "ctkPlugins_p.h"
#include "ctkPluginFrameworkListeners_p.h"

namespace ctk {

  class Plugin;

  class PluginFrameworkContext {

  public:

      /**
       * All plugins in this framework.
       */
      Plugins* plugins;

      /**
       * All listeners in this framework.
       */
      PluginFrameworkListeners listeners;

      /**
       * All registered services in this framework.
       */
      //Services services;

      /**
       * System plugin
       */
      PluginFramework systemPlugin;

      /**
       * Plugin storage
       */
      PluginStorage storage;

      /**
       * Framework id.
       */
      int id;

      /**
       * global lock.
       */
      static QMutex globalFwLock;

      /**
       * Id to use for next instance of plugin framework.
       */
      static int globalId;

      PluginFrameworkFactory::Properties props;

      /**
       * Contruct a framework context
       *
       */
      PluginFrameworkContext(const PluginFrameworkFactory::Properties& initProps);


      /**
       * Initialize the framework
       *
       */
      void init();


      /**
       * Undo as much as possible of what init() does.
       *
       */
      void uninit();


      /**
       *
       */
      int getId() const;


      /**
       * Check that the plugin belongs to this framework instance.
       *
       */
      void checkOurPlugin(Plugin* plugin) const;


      /**
       * Check that the plugin specified can resolve all its
       * Require-Plugin constraints.
       *
       * @param plugin Plugin to check, must be in INSTALLED state
       *
       * @throws PluginException
       */
      void resolvePlugin(PluginPrivate* plugin);


      /**
       * Log message for debugging framework
       *
       */
      QDebug log() const;

  private:

      QSet<PluginPrivate*> tempResolved;

      void checkRequirePlugin(PluginPrivate* plugin);
  };

}

#endif // CTKPLUGINFRAMEWORKCONTEXT_P_H
