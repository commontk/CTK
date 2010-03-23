/*
 * ctkPluginManager.h
 *
 *  Created on: Mar 10, 2010
 *      Author: zelzer
 */

#ifndef CTKPLUGINMANAGER_H_
#define CTKPLUGINMANAGER_H_

#include <QString>
#include <QServiceManager>

#include <CTKCoreExport.h>

namespace ctk {

  using namespace QtMobility;

  class PluginManagerPrivate;

  class CTK_CORE_EXPORT PluginManager
  {
    Q_DECLARE_PRIVATE(PluginManager)

  public:

    PluginManager();
    virtual ~PluginManager();

    QServiceManager* serviceManager();

	  void addSearchPath(const QString& searchPath);

	  void startAllPlugins();

  protected:

	  PluginManagerPrivate* const d_ptr;

  };

}


#endif /* CTKPLUGINMANAGER_H_ */
