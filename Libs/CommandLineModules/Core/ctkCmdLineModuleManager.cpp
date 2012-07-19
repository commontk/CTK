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

#include "ctkCmdLineModuleManager.h"

#include "ctkCmdLineModuleXmlValidator.h"
#include "ctkCmdLineModuleReference.h"
#include "ctkCmdLineModuleReferencePrivate.h"
#include "ctkCmdLineModuleInstanceFactory.h"

#include <ctkException.h>

#include <QStringList>
#include <QBuffer>

#include <QProcess>
#include <QFuture>

struct ctkCmdLineModuleManagerPrivate
{
  ctkCmdLineModuleManagerPrivate()
    : Verbose(false)
  {}

  ctkCmdLineModuleInstanceFactory* InstanceFactory;

  QHash<QString, ctkCmdLineModuleReference> Cache;
  bool Verbose;
};

ctkCmdLineModuleManager::ctkCmdLineModuleManager(ctkCmdLineModuleInstanceFactory *instanceFactory,
                                                 ValidationMode validationMode)
  : d(new ctkCmdLineModuleManagerPrivate)
{
  d->InstanceFactory = instanceFactory;
}

ctkCmdLineModuleManager::~ctkCmdLineModuleManager()
{
}

void ctkCmdLineModuleManager::setVerboseOutput(bool verbose)
{
  d->Verbose = verbose;
}

bool ctkCmdLineModuleManager::verboseOutput() const
{
  return d->Verbose;
}

ctkCmdLineModuleReference
ctkCmdLineModuleManager::registerModule(const QString& location)
{
  QProcess process;
  process.setReadChannel(QProcess::StandardOutput);
  process.start(location, QStringList("--xml"));

  ctkCmdLineModuleReference ref;
  ref.d->Location = location;
  if (!process.waitForFinished() || process.exitStatus() == QProcess::CrashExit ||
      process.error() != QProcess::UnknownError)
  {
    if(d->Verbose)
    {
      qWarning() << "The executable at" << location << "could not be started:" << process.errorString();
    }
    return ref;
  }

  process.waitForReadyRead();
  QByteArray xml = process.readAllStandardOutput();

  // validate the outputted xml description
  QBuffer input(&xml);
  input.open(QIODevice::ReadOnly);

  ctkCmdLineModuleXmlValidator validator(&input);
  if (!validator.validateInput())
  {
    if(d->Verbose)
    {
      qWarning() << validator.errorString();
    }
    return ref;
  }

  ref.d->RawXmlDescription = xml;

  d->Cache[location] = ref;

  emit moduleAdded(ref);
  return ref;
}

void ctkCmdLineModuleManager::unregisterModule(const ctkCmdLineModuleReference& ref)
{
  d->Cache.remove(ref.location());
  emit moduleRemoved(ref);
}

ctkCmdLineModuleReference ctkCmdLineModuleManager::moduleReference(const QString& location) const
{
  return d->Cache[location];
}

QList<ctkCmdLineModuleReference> ctkCmdLineModuleManager::moduleReferences() const
{
  return d->Cache.values();
}

ctkCmdLineModuleInstance*
ctkCmdLineModuleManager::createModuleInstance(const ctkCmdLineModuleReference& moduleRef)
{
  return d->InstanceFactory->create(moduleRef);
}

QList<ctkCmdLineModuleInstance*>
ctkCmdLineModuleManager::moduleInstances(const ctkCmdLineModuleReference& moduleRef) const
{
  throw ctkException("not implemented yet");
}
