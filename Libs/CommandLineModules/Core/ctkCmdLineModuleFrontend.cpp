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

#include "ctkCmdLineModuleFrontend.h"

#include "ctkCmdLineModuleDescription.h"
#include "ctkCmdLineModuleParameter.h"
#include "ctkCmdLineModuleParameterGroup.h"
#include "ctkCmdLineModuleReference.h"
#include "ctkCmdLineModuleFuture.h"

#include <QUrl>

struct ctkCmdLineModuleFrontendPrivate
{
  ctkCmdLineModuleFrontendPrivate(const ctkCmdLineModuleReference& moduleRef)
    : ModuleReference(moduleRef)
  {
  }

  ctkCmdLineModuleReference ModuleReference;

  QList<QString> ParameterNames;

  ctkCmdLineModuleFuture Future;
};


ctkCmdLineModuleFrontend::ctkCmdLineModuleFrontend(const ctkCmdLineModuleReference& moduleRef)
  : d(new ctkCmdLineModuleFrontendPrivate(moduleRef))
{
}

void ctkCmdLineModuleFrontend::setFuture(const ctkCmdLineModuleFuture &future)
{
  d->Future = future;
}

ctkCmdLineModuleFrontend::~ctkCmdLineModuleFrontend()
{
}

QList<QString> ctkCmdLineModuleFrontend::parameterNames() const
{
  if (!d->ParameterNames.isEmpty()) return d->ParameterNames;

  foreach (ctkCmdLineModuleParameterGroup paramGroup,
           moduleReference().description().parameterGroups())
  {
    foreach (ctkCmdLineModuleParameter param, paramGroup.parameters())
    {
      d->ParameterNames.push_back(param.name());
    }
  }
  return d->ParameterNames;
}

ctkCmdLineModuleReference ctkCmdLineModuleFrontend::moduleReference() const
{
  return d->ModuleReference;
}

QUrl ctkCmdLineModuleFrontend::location() const
{
  return d->ModuleReference.location();
}

ctkCmdLineModuleFuture ctkCmdLineModuleFrontend::future() const
{
  return d->Future;
}

bool ctkCmdLineModuleFrontend::isRunning() const
{
  return d->Future.isRunning();
}

bool ctkCmdLineModuleFrontend::isPaused() const
{
  return d->Future.isPaused();
}

QHash<QString, QVariant> ctkCmdLineModuleFrontend::values() const
{
  QHash<QString,QVariant> result;
  foreach(QString parameterName, parameterNames())
  {
    result.insert(parameterName, value(parameterName));
  }
  return result;
}

void ctkCmdLineModuleFrontend::setValues(const QHash<QString, QVariant> &values)
{
  QHashIterator<QString,QVariant> iter(values);
  while(iter.hasNext())
  {
    iter.next();
    setValue(iter.key(), iter.value());
  }
}
