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
#include "ctkCmdLineModuleObjectHierarchyReader.h"
#include "ctkCmdLineModuleProcessRunner_p.h"
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
  ctkCmdLineModuleInstanceFactory* InstanceFactory;

  QHash<QString, ctkCmdLineModuleReference> Cache;
};

ctkCmdLineModuleManager::ctkCmdLineModuleManager(ctkCmdLineModuleInstanceFactory *instanceFactory,
                                                 ValidationMode validationMode)
  : d(new ctkCmdLineModuleManagerPrivate)
{
  d->InstanceFactory = instanceFactory;
}

ctkCmdLineModuleManager::~ctkCmdLineModuleManager()
{
  delete d->InstanceFactory;
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
    qWarning() << "The executable at" << location << "could not be started:" << process.errorString();
    return ref;
  }

  process.waitForReadyRead();
  QByteArray xml = process.readAllStandardOutput();

  qDebug() << xml;

  // validate the outputted xml description
  QBuffer input(&xml);
  input.open(QIODevice::ReadOnly);

  ctkCmdLineModuleXmlValidator validator(&input);
  if (!validator.validateInput())
  {
    qCritical() << validator.errorString();
    return ref;
  }

  ref.d->RawXmlDescription = xml;
//  ref.d->objectRepresentation = descriptionFactory->createObjectRepresentationFromXML(ref.d->xml);
//  ref.d->setGUI(descriptionFactory->createGUIFromXML(ref.d->xml));

  d->Cache[location] = ref;
  return ref;
}

void ctkCmdLineModuleManager::unregisterModule(const ctkCmdLineModuleReference&)
{
  throw ctkException("not implemented yet");
}

ctkCmdLineModuleReference ctkCmdLineModuleManager::moduleReference(const QString& location) const
{
  throw ctkException("not implemented yet");
}

QList<ctkCmdLineModuleReference> ctkCmdLineModuleManager::moduleReferences() const
{
  throw ctkException("not implemented yet");
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
