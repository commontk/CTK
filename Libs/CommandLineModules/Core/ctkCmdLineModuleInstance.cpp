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

#include "ctkCmdLineModuleInstance.h"
#include "ctkCmdLineModuleDescription.h"
#include "ctkCmdLineModuleParameter.h"
#include "ctkCmdLineModuleParameterGroup.h"
#include "ctkCmdLineModuleReference.h"
#include "ctkCmdLineModuleProcess_p.h"

#include "ctkException.h"

#include <QStringList>
#include <QDebug>


struct ctkCmdLineModuleInstancePrivate
{
  ctkCmdLineModuleInstancePrivate(ctkCmdLineModuleInstance* qq,
                                  const ctkCmdLineModuleReference& moduleRef)
    : ModuleReference(moduleRef), q(qq)
  {

  }

  QString normalizeFlag(const QString& flag)
  {
    return flag.trimmed().remove(QRegExp("^-*"));
  }

  ctkCmdLineModuleReference ModuleReference;

  QList<QString> ParameterNames;

private:

  ctkCmdLineModuleInstance* q;

};


ctkCmdLineModuleInstance::ctkCmdLineModuleInstance(const ctkCmdLineModuleReference& moduleRef)
  : d(new ctkCmdLineModuleInstancePrivate(this, moduleRef))
{
}

ctkCmdLineModuleInstance::~ctkCmdLineModuleInstance()
{
}

QList<QString> ctkCmdLineModuleInstance::parameterNames() const
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

ctkCmdLineModuleReference ctkCmdLineModuleInstance::moduleReference() const
{
  return d->ModuleReference;
}

QString ctkCmdLineModuleInstance::location() const
{
  return d->ModuleReference.location();
}

QStringList ctkCmdLineModuleInstance::commandLineArguments() const
{
  QStringList cmdLineArgs;
  QHash<int, QString> indexedArgs;

  QHash<QString,QVariant> currentValues = values();
  ctkCmdLineModuleDescription description = moduleReference().description();
  QHashIterator<QString,QVariant> valuesIter(currentValues);
  while(valuesIter.hasNext())
  {
    valuesIter.next();
    ctkCmdLineModuleParameter parameter = description.parameter(valuesIter.key());
    if (parameter.index() > -1)
    {
      indexedArgs.insert(parameter.index(), valuesIter.value().toString());
    }
    else
    {
      QString argFlag;
      if (parameter.longFlag().isEmpty())
      {
        argFlag = QString("-") + d->normalizeFlag(parameter.flag());
      }
      else
      {
        argFlag = QString("--") + d->normalizeFlag(parameter.longFlag());
      }

      QStringList args;
      if (parameter.multiple())
      {
        args = valuesIter.value().toString().split(',', QString::SkipEmptyParts);
      }
      else
      {
        args.push_back(valuesIter.value().toString());
      }

      foreach(QString arg, args)
      {
        cmdLineArgs << argFlag << arg;
      }
    }
  }

  QList<int> indexes = indexedArgs.keys();
  qSort(indexes.begin(), indexes.end());
  foreach(int index, indexes)
  {
    cmdLineArgs << indexedArgs[index];
  }

  return cmdLineArgs;
}

struct ctkCmdLineModuleFuture {};

ctkCmdLineModuleFuture ctkCmdLineModuleInstance::run() const
{
//  // TODO: manage memory
  QStringList args = commandLineArguments();
  qDebug() << args;
//  ctkCmdLineModuleProcessRunner* moduleProcess =
//      new ctkCmdLineModuleProcessRunner(d->ModuleReference.location(), args);
//  return moduleProcess->start();
}


QHash<QString, QVariant> ctkCmdLineModuleInstance::values() const
{
  QHash<QString,QVariant> result;
  foreach(QString parameterName, parameterNames())
  {
    result.insert(parameterName, value(parameterName));
  }
  return result;
}

void ctkCmdLineModuleInstance::setValues(const QHash<QString, QVariant> &values)
{
  QHashIterator<QString,QVariant> iter(values);
  while(iter.hasNext())
  {
    iter.next();
    setValue(iter.key(), iter.value());
  }
}
