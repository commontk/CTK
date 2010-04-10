#ifndef CTKPLUGINFRAMEWORKCONTEXTPRIVATE_P_H
#define CTKPLUGINFRAMEWORKCONTEXTPRIVATE_P_H

#include <QDebug>
#include <QMutex>

#include "ctkPluginFrameworkContext.h"

namespace ctk {

  class Plugin;

  class PluginFrameworkContextPrivate {

  private:


      /**
       * All plugins in this framework.
       */
      //Plugins plugins;

      /**
       * All registered services in this framework.
       */
      //Services services;

      /**
       * System bundle
       */
      //SystemPlugin systemPlugin;

      /**
       * Framework id.
       */
      int id;

  public:


      /**
       * global lock.
       */
      static QMutex globalFwLock;

      /**
       * Id to use for next instance of plugin framework.
       */
      static int globalId;

      PluginFrameworkContext::Properties props;

      /**
       * Contruct a framework context
       *
       */
      PluginFrameworkContextPrivate(const PluginFrameworkContext::Properties& initProps);


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
      int getId();


      /**
       * Check that the plugin belongs to this framework instance.
       *
       */
      void checkOurPlugin(Plugin* b);


      /**
       * Log message for debugging framework
       *
       */
      QDebug log();


  };

}

#endif // CTKPLUGINFRAMEWORKCONTEXTPRIVATE_P_H
