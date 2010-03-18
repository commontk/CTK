/*
 * ctkPluginManager.h
 *
 *  Created on: Mar 10, 2010
 *      Author: zelzer
 */

#ifndef CTKPLUGINMANAGER_H_
#define CTKPLUGINMANAGER_H_

#include <QString>

namespace ctk {

  class PluginManagerPrivate;

  class PluginManager
  {
    Q_DECLARE_PRIVATE(PluginManager)

  public:

    PluginManager();
    virtual ~PluginManager();

	  void addSearchPath(const QString& searchPath);

	  void startAllPlugins();

  protected:

	  PluginManagerPrivate* const d_ptr;

  };

}


#endif /* CTKPLUGINMANAGER_H_ */
