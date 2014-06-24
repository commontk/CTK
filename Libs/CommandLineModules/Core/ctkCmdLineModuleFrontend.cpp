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
#include "ctkException.h"

#include <QVariant>
#include <QUrl>
#include <QFutureWatcher>

//----------------------------------------------------------------------------
struct ctkCmdLineModuleFrontendPrivate
{
  ctkCmdLineModuleFrontendPrivate(const ctkCmdLineModuleReference& moduleRef, ctkCmdLineModuleFrontend* q)
    : q(q)
    , ModuleReference(moduleRef)
  {
  }

  void _q_resultReadyAt(int index)
  {
    q->resultReady(Future.resultAt(index));
  }

  ctkCmdLineModuleFrontend* q;

  ctkCmdLineModuleReference ModuleReference;

  QList<QString> ParameterNames;

  ctkCmdLineModuleFuture Future;
  QFutureWatcher<ctkCmdLineModuleResult> FutureWatcher;
};


//----------------------------------------------------------------------------
ctkCmdLineModuleFrontend::ctkCmdLineModuleFrontend(const ctkCmdLineModuleReference& moduleRef)
  : d(new ctkCmdLineModuleFrontendPrivate(moduleRef, this))
{
  connect(&d->FutureWatcher, SIGNAL(resultReadyAt(int)), SLOT(_q_resultReadyAt(int)));
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleFrontend::setFuture(const ctkCmdLineModuleFuture &future)
{
  d->Future = future;
  // Reset all simple output (return) parameter values
  foreach(const ctkCmdLineModuleParameter& param, this->parameters(QString(), Output))
  {
    if (param.index() == 1000)
    {
      this->setValue(param.name(), param.defaultValue());
    }
  }

  d->FutureWatcher.setFuture(d->Future);
}

//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
ctkCmdLineModuleReference ctkCmdLineModuleFrontend::moduleReference() const
{
  return d->ModuleReference;
}

//----------------------------------------------------------------------------
QUrl ctkCmdLineModuleFrontend::location() const
{
  return d->ModuleReference.location();
}

//----------------------------------------------------------------------------
ctkCmdLineModuleFuture ctkCmdLineModuleFrontend::future() const
{
  return d->Future;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleFrontend::isRunning() const
{
  return d->Future.isRunning();
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleFrontend::isPaused() const
{
  return d->Future.isPaused();
}

//----------------------------------------------------------------------------
QHash<QString, QVariant> ctkCmdLineModuleFrontend::values() const
{
  QHash<QString,QVariant> result;
  foreach(QString parameterName, parameterNames())
  {
    result.insert(parameterName, value(parameterName));
  }
  return result;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleFrontend::setValues(const QHash<QString, QVariant> &values)
{
  QHashIterator<QString,QVariant> iter(values);
  while(iter.hasNext())
  {
    iter.next();
    setValue(iter.key(), iter.value());
  }
}

//----------------------------------------------------------------------------
QList<ctkCmdLineModuleParameter> ctkCmdLineModuleFrontend::parameters(const QString &type, ParameterFilters filters)
{
  ctkCmdLineModuleDescription description = this->moduleReference().description();
  QList<ctkCmdLineModuleParameter> parameters;
  foreach(ctkCmdLineModuleParameterGroup group, description.parameterGroups())
  {
    foreach(ctkCmdLineModuleParameter param, group.parameters())
    {
      if (filters.testFlag(Input) &&
          (param.channel().isEmpty() || param.channel().compare("input", Qt::CaseInsensitive) == 0))
      {
        if (type.isEmpty() || param.tag().compare(type, Qt::CaseInsensitive) == 0)
        {
          parameters << param;
        }
      }
      if (filters.testFlag(Output) && param.channel().compare("output", Qt::CaseInsensitive) == 0)
      {
        if (type.isEmpty() || param.tag().compare(type, Qt::CaseInsensitive) == 0)
        {
          parameters << param;
        }
      }
    }
  }
  return parameters;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleFrontend::resetValues()
{
  foreach(ctkCmdLineModuleParameter param, this->parameters())
  {
    this->setValue(param.name(), param.defaultValue());
  }
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleFrontend::resultReady(const ctkCmdLineModuleResult &result)
{
  try
  {
    if (this->moduleReference().description().parameter(result.parameter()).channel() != "output")
    {
      qWarning() << "Module" << this->moduleReference().location() << "is reporting results for non-output parameter"
                 << result.parameter() << ". Report ignored.";
      return;
    }
    this->setValue(result.parameter(), result.value());
  }
  catch (const ctkInvalidArgumentException&)
  {}
}

#include "moc_ctkCmdLineModuleFrontend.cpp"
