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

#ifndef CTKCMDLINEMODULEREFERENCERESULT_H
#define CTKCMDLINEMODULEREFERENCERESULT_H

#include "ctkCommandLineModulesCoreExport.h"
#include <ctkCmdLineModuleReference.h>

#include <QString>
#include <QUrl>

struct CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleReferenceResult
{
  ctkCmdLineModuleReferenceResult() {}
  ctkCmdLineModuleReferenceResult(const QUrl& moduleUrl) { m_Url = moduleUrl;}
  ctkCmdLineModuleReferenceResult(const QUrl& moduleUrl, const QString& errorMessage) { m_Url = moduleUrl; m_RuntimeError = errorMessage; }
  ctkCmdLineModuleReferenceResult(ctkCmdLineModuleReference& ref) { m_Reference = ref; m_Url = ref.location(); }

  QUrl m_Url;
  ctkCmdLineModuleReference m_Reference;
  QString m_RuntimeError;
};

#endif // CTKCMDLINEMODULEREFERENCERESULT_H
