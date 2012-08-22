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

#include <QString>
#include <QVariant>

/**
 * \class ctkCmdLineModuleResult
 * \brief Describes the output of a command line module.
 * \ingroup CommandLineModulesCore
 */
class ctkCmdLineModuleResult
{
public:

  ctkCmdLineModuleResult() {}

  ctkCmdLineModuleResult(const QString& parameter, const QVariant& value)
    : Parameter(parameter), Value(value)
  {}

  inline QString parameter() const { return Parameter; }
  inline QVariant value() const { return Value; }

private:

  QString Parameter;
  QVariant Value;
};

#endif // CTKCMDLINEMODULERESULT_H
