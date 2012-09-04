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

#ifndef CTKCMDLINEMODULEPROCESSTASK_H
#define CTKCMDLINEMODULEPROCESSTASK_H

#include "ctkCmdLineModuleFutureInterface.h"

#include "ctkCommandLineModulesBackendLocalProcessExport.h"

#include <QObject>
#include <QRunnable>
#include <QStringList>
#include <QBuffer>
#include <QFutureWatcher>
#include <QTimer>

class QProcess;

struct ctkCmdLineModuleProcessTaskPrivate;

/**
 * \class ctkCmdLineModuleProcessTask
 * \brief Implements ctkCmdLineModuleFutureInterface to enabling
 * running a command line application asynchronously.
 * \ingroup CommandLineModulesBackendLocalProcess_API
 */
class CTK_CMDLINEMODULEBACKENDLP_EXPORT ctkCmdLineModuleProcessTask
    : public ctkCmdLineModuleFutureInterface, public QRunnable
{

public:

  ctkCmdLineModuleProcessTask(const QString& location, const QStringList& args);
  ~ctkCmdLineModuleProcessTask();

  ctkCmdLineModuleFuture start();

  void run();

private:

  QScopedPointer<ctkCmdLineModuleProcessTaskPrivate> d;

};



#endif // CTKCMDLINEMODULEPROCESSTASK_H
