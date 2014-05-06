/*=============================================================================

  Library: CTK

  Copyright (c) University College London

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

#ifndef CTKCMDLINEMODULEXMLCHECKERTASK_P_H
#define CTKCMDLINEMODULEXMLCHECKERTASK_P_H

#include <ctkCmdLineModuleFutureInterface.h>
#include <QRunnable>

/**
 * \class ctkCmdLineModuleXMLCheckerTask
 * \brief Provides a ctkCmdLineModuleFutureInterface implementation specifically to
 * run an XML Checker asynchronously.
 * \ingroup CommandLineModulesBackendXMLChecker_API
 */
class ctkCmdLineModuleXMLCheckerTask : public ctkCmdLineModuleFutureInterface, public QRunnable
{
public:

  ctkCmdLineModuleXMLCheckerTask();
  ctkCmdLineModuleFuture start();

  void run();

private:
};

#endif // CTKCMDLINEMODULEXMLCHECKERTASK_P_H
