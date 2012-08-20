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

#ifndef CTKCMDLINEMODULEBACKENDLOCALPROCESS_H
#define CTKCMDLINEMODULEBACKENDLOCALPROCESS_H

#include "ctkCmdLineModuleBackend.h"

#include "ctkCommandLineModulesBackendLocalProcessExport.h"

#include <QScopedPointer>

struct ctkCmdLineModuleBackendLocalProcessPrivate;

class CTK_CMDLINEMODULEBACKENDLP_EXPORT ctkCmdLineModuleBackendLocalProcess : public ctkCmdLineModuleBackend
{

public:

  ctkCmdLineModuleBackendLocalProcess();
  ~ctkCmdLineModuleBackendLocalProcess();

  virtual QString name() const;
  virtual QString description() const;

  virtual QList<QString> schemes() const;

  virtual QByteArray rawXmlDescription(const QUrl& location);

  virtual ctkCmdLineModuleFuture run(ctkCmdLineModuleFrontend *frontend);

private:

  QScopedPointer<ctkCmdLineModuleBackendLocalProcessPrivate> d;

};

#endif // CTKCMDLINEMODULEBACKENDLOCALPROCESS_H
