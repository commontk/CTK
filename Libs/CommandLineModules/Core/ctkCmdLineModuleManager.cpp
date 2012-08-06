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
#include "ctkCmdLineModuleReference_p.h"
#include "ctkCmdLineModuleFactory.h"

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

  ctkCmdLineModuleFactory* InstanceFactory;

  QHash<QString, ctkCmdLineModuleReference> LocationToRef;

  bool Verbose;
};

ctkCmdLineModuleManager::ctkCmdLineModuleManager(ctkCmdLineModuleFactory *instanceFactory,
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

  d->LocationToRef[location] = ref;

  emit moduleRegistered(ref);
  return ref;
}

void ctkCmdLineModuleManager::unregisterModule(const ctkCmdLineModuleReference& ref)
{
  d->LocationToRef.remove(ref.location());
  emit moduleUnregistered(ref);
}

ctkCmdLineModuleReference ctkCmdLineModuleManager::moduleReference(const QString& location) const
{
  return d->LocationToRef[location];
}

QList<ctkCmdLineModuleReference> ctkCmdLineModuleManager::moduleReferences() const
{
  return d->LocationToRef.values();
}

ctkCmdLineModule*
ctkCmdLineModuleManager::createModule(const ctkCmdLineModuleReference& moduleRef)
{
  return d->InstanceFactory->create(moduleRef);
}
