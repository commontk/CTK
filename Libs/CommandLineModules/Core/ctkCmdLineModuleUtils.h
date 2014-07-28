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

#ifndef CTKCOMMANDLINEMODULEUTILS_H
#define CTKCOMMANDLINEMODULEUTILS_H

#include "ctkCommandLineModulesCoreExport.h"

#include "ctkCmdLineModuleManager.h"
#include "ctkCmdLineModuleReferenceResult.h"

#include <QFuture>
#include <QString>

struct CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleUtils
{
  static QString errorMessagesFromModuleRegistration(
      const QList<ctkCmdLineModuleReferenceResult>& moduleRefs,
      ctkCmdLineModuleManager::ValidationMode validationMode
      );

  static QString errorMessagesFromModuleRegistration(
      const QFuture<ctkCmdLineModuleReferenceResult>& moduleRefsFuture,
      ctkCmdLineModuleManager::ValidationMode validationMode
      );

  static void messageBoxForModuleRegistration(
      const QString& errorMessages
      );

  static void messageBoxModuleRegistration(
      const QFuture<ctkCmdLineModuleReferenceResult>& moduleRefsFuture,
      ctkCmdLineModuleManager::ValidationMode validationMode
      );
};

#endif // CTKCOMMANDLINEMODULEUTILS_H
