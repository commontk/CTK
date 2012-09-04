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

#ifndef CTKCMDLINEMODULERESULT_H
#define CTKCMDLINEMODULERESULT_H

#include "ctkCommandLineModulesCoreExport.h"

#include <QSharedPointer>

struct ctkCmdLineModuleResultPrivate;

/**
 * @ingroup CommandLineModulesCore_API
 *
 * @brief Describes a reported result of a command line module.
 *
 * This class wraps a specific result reported by a running module via a
 * ctkCmdLineModuleFuture instance.
 *
 * @see ctkCmdLineModuleFuture
 * @see ctkCmdLineModuleFutureWatcher
 */
class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleResult
{
public:

  ctkCmdLineModuleResult();
  ~ctkCmdLineModuleResult();

  ctkCmdLineModuleResult(const ctkCmdLineModuleResult& other);
  ctkCmdLineModuleResult& operator=(const ctkCmdLineModuleResult& other);

  ctkCmdLineModuleResult(const QString& parameter, const QVariant& value);

  bool operator==(const ctkCmdLineModuleResult& other) const;

  /**
   * @brief Get the name of the output parameter for which this result was reported.
   * @return The output parameter name.
   */
  QString parameter() const;

  /**
   * @brief Get the result value.
   * @return The result value.
   */
  QVariant value() const;

private:

  QSharedPointer<ctkCmdLineModuleResultPrivate> d;
};

CTK_CMDLINEMODULECORE_EXPORT QDebug operator<<(QDebug debug, const ctkCmdLineModuleResult& result);

#endif // CTKCMDLINEMODULERESULT_H
