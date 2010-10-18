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

#ifndef CTKPLUGINMANAGER_H_
#define CTKPLUGINMANAGER_H_

#include <QString>
#include <QServiceManager>

#include <CTKPluginFrameworkExport.h>


  using namespace QtMobility;

  class ctkPluginManagerPrivate;

  class CTK_PLUGINFW_EXPORT ctkPluginManager
  {
    Q_DECLARE_PRIVATE(ctkPluginManager)

  public:

    ctkPluginManager();
    virtual ~ctkPluginManager();

    QServiceManager* serviceManager();

	  void addSearchPath(const QString& searchPath);

	  void startAllPlugins();

  protected:

	  ctkPluginManagerPrivate* const d_ptr;

  };



#endif /* CTKPLUGINMANAGER_H_ */
