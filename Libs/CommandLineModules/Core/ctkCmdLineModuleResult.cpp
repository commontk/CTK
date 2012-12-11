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

#include "ctkCmdLineModuleResult.h"

#include <QVariant>
#include <QDebug>

struct ctkCmdLineModuleResultPrivate : public QSharedData
{
  QString Parameter;
  QVariant Value;
};

ctkCmdLineModuleResult::ctkCmdLineModuleResult()
  : d(new ctkCmdLineModuleResultPrivate)
{}

ctkCmdLineModuleResult::~ctkCmdLineModuleResult()
{
}

ctkCmdLineModuleResult::ctkCmdLineModuleResult(const ctkCmdLineModuleResult &other)
  : d(other.d)
{
}

ctkCmdLineModuleResult::ctkCmdLineModuleResult(const QString& parameter, const QVariant& value)
  : d(new ctkCmdLineModuleResultPrivate)
{
  d->Parameter = parameter;
  d->Value = value;
}

bool ctkCmdLineModuleResult::operator==(const ctkCmdLineModuleResult& other) const
{
  return d->Parameter == other.d->Parameter && d->Value == other.d->Value;
}

QString ctkCmdLineModuleResult::parameter() const
{
  return d->Parameter;
}

QVariant ctkCmdLineModuleResult::value() const
{
  return d->Value;
}

QDebug operator<<(QDebug debug, const ctkCmdLineModuleResult& result)
{
  debug.nospace() << result.parameter() << "=" << result.value();
  return debug;
}


ctkCmdLineModuleResult &ctkCmdLineModuleResult::operator =(const ctkCmdLineModuleResult &other)
{
  d->Parameter = other.d->Parameter;
  d->Value = other.d->Value;
  return *this;
}
