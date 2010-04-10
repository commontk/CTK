#include "ctkPluginFrameworkContextPrivate_p.h"

namespace ctk {

  QMutex PluginFrameworkContextPrivate::globalFwLock;
  int PluginFrameworkContextPrivate::globalId = 1;


  PluginFrameworkContextPrivate::PluginFrameworkContextPrivate(
      const PluginFrameworkContext::Properties& initProps)
        : /*plugins(this), services(this), systemPlugin(this)*/
        props(initProps)
  {

    {
      QMutexLocker lock(&globalFwLock);
      id = globalId++;
    }

    log() << "created";
  }

  void PluginFrameworkContextPrivate::init()
  {
    log() << "initializing";

//    if (Constants.FRAMEWORK_STORAGE_CLEAN_ONFIRSTINIT
//        .equals(props.getProperty(Constants.FRAMEWORK_STORAGE_CLEAN))) {
//      deleteFWDir();
//      // Must remove the storage clean property since it should not be
//      // used more than once!
//      props.removeProperty(Constants.FRAMEWORK_STORAGE_CLEAN);
//    }
//    props.save();


//    systemPlugin.initSystemBundle();

//    plugins.load();

    log() << "inited";

    log() << "Installed bundles:";
//    // Use the ordering in the bundle storage to get a sorted list of bundles.
//    final BundleArchive [] allBAs = storage.getAllBundleArchives();
//    for (int i = 0; i<allBAs.length; i++) {
//      final BundleArchive ba = allBAs[i];
//      final Bundle b = bundles.getBundle(ba.getBundleLocation());
//      log(" #" +b.getBundleId() +" " +b.getSymbolicName() +":"
//          +b.getVersion() +" location:" +b.getLocation());
//    }
  }

  void PluginFrameworkContextPrivate::uninit()
  {
    log() << "uninit";

    //systemBundle.uninitSystemBundle();

    //storage.close();

  }

  int PluginFrameworkContextPrivate::getId() {
    return id;
  }

  void PluginFrameworkContextPrivate::checkOurPlugin(Plugin* b) {
//    if (this != ((BundleImpl)b).fwCtx) {
//      throw new IllegalArgumentException("Bundle does not belong to this framework: " + b);
//    }
  }

  QDebug PluginFrameworkContextPrivate::log() {
    QDebug dbg(qDebug());
    dbg << "Framework instance " << getId() << ": ";
    return dbg;
  }

}
