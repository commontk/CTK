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
#include "ctkCmdLineModuleReference.h"
#include "ctkCmdLineModuleObjectHierarchyReader.h"
#include "ctkCmdLineModuleObjectHierarchyReader.h"
#include "ctkCmdLineModuleProcessRunner_p.h"

#include "ctkException.h"

#include <QStringList>
#include <QDebug>

namespace {

QString normalizeFlag(const QString& flag)
{
  return flag.trimmed().remove(QRegExp("^-*"));
}

}

struct ctkCmdLineModuleInstancePrivate
{
  ctkCmdLineModuleInstancePrivate(ctkCmdLineModuleInstance* qq,
                                  const ctkCmdLineModuleReference& moduleRef)
    : ModuleReference(moduleRef), q(qq)
  {

  }

  QStringList createCommandLineArgs()
  {
    ctkCmdLineModuleObjectHierarchyReader reader(q->parameterValueModel());

    QStringList cmdLineArgs;
    QHash<int, QString> indexedArgs;
    while(reader.readNextParameter())
    {
      if (reader.index() > -1)
      {
        indexedArgs.insert(reader.index(), reader.value().toString());
      }
      else
      {
        QString argFlag;
        if (reader.longFlag().isEmpty())
        {
          argFlag = QString("-") + normalizeFlag(reader.flag());
        }
        else
        {
          argFlag = QString("--") + normalizeFlag(reader.longFlag());
        }

        QStringList args;
        if (reader.isMultiple())
        {
          args = reader.value().toString().split(',', QString::SkipEmptyParts);
        }
        else
        {
          args.push_back(reader.value().toString());
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

  ctkCmdLineModuleReference ModuleReference;

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

QObject *ctkCmdLineModuleInstance::parameterValueModel() const
{
  return guiHandle();
}

QVariant ctkCmdLineModuleInstance::value(const QString &parameter) const
{
  throw ctkException("not implemented yet");
}

void ctkCmdLineModuleInstance::setValue(const QString &parameter, const QVariant &value)
{
  ctkCmdLineModuleObjectHierarchyReader reader(parameterValueModel());
  while(reader.readNextParameter())
  {
    if(reader.name() == parameter && reader.value() != value)
    {
      reader.setValue(value);
      emit valueChanged(parameter, value);
    }
  }
}

ctkCmdLineModuleReference ctkCmdLineModuleInstance::moduleReference() const
{
  return d->ModuleReference;
}

ctkCmdLineModuleProcessFuture ctkCmdLineModuleInstance::run() const
{
  // TODO: manage memory
  QStringList args = d->createCommandLineArgs();
  qDebug() << args;
  ctkCmdLineModuleProcessRunner* moduleProcess =
      new ctkCmdLineModuleProcessRunner(d->ModuleReference.location(), args);
  return moduleProcess->start();
}
