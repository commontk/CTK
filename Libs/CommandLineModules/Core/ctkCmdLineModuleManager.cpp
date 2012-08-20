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

#include "ctkCmdLineModuleBackend.h"
#include "ctkCmdLineModuleFrontend.h"
#include "ctkCmdLineModuleFuture.h"
#include "ctkCmdLineModuleXmlValidator.h"
#include "ctkCmdLineModuleReference.h"
#include "ctkCmdLineModuleReference_p.h"

#include <ctkException.h>

#include <QStringList>
#include <QBuffer>
#include <QUrl>
#include <QHash>
#include <QList>

#include <QFuture>

struct ctkCmdLineModuleManagerPrivate
{
  ctkCmdLineModuleManagerPrivate(ctkCmdLineModuleManager::ValidationMode mode)
    : ValidationMode(mode)
  {}

  void checkBackends(const QUrl& location)
  {
    if (!this->SchemeToBackend.contains(location.scheme()))
    {
      throw ctkInvalidArgumentException(QString("No suitable backend registered for module at ") + location.toString());
    }
  }

  QHash<QString, ctkCmdLineModuleBackend*> SchemeToBackend;
  QHash<QUrl, ctkCmdLineModuleReference> LocationToRef;

  ctkCmdLineModuleManager::ValidationMode ValidationMode;
};

ctkCmdLineModuleManager::ctkCmdLineModuleManager(ValidationMode validationMode)
  : d(new ctkCmdLineModuleManagerPrivate(validationMode))
{
}

ctkCmdLineModuleManager::~ctkCmdLineModuleManager()
{
}

void ctkCmdLineModuleManager::registerBackend(ctkCmdLineModuleBackend *backend)
{
  QList<QString> supportedSchemes = backend->schemes();

  // Check if there is already a backound registerd for any of the
  // supported schemes. We only supported one backend per scheme.
  foreach (QString scheme, supportedSchemes)
  {
    if (d->SchemeToBackend.contains(scheme))
    {
      throw ctkInvalidArgumentException(QString("A backend for scheme %1 is already registered.").arg(scheme));
    }
  }

  // All good
  foreach (QString scheme, supportedSchemes)
  {
    d->SchemeToBackend[scheme] = backend;
  }
}

ctkCmdLineModuleReference
ctkCmdLineModuleManager::registerModule(const QUrl &location)
{
  d->checkBackends(location);

  QByteArray xml = d->SchemeToBackend[location.scheme()]->rawXmlDescription(location);

  if (xml.isEmpty())
  {
    throw ctkInvalidArgumentException(QString("No XML output available from ") + location.toString());
  }

  ctkCmdLineModuleReference ref;
  ref.d->Location = location;
  ref.d->RawXmlDescription = xml;
  ref.d->Backend = d->SchemeToBackend[location.scheme()];

  if (d->ValidationMode != SKIP_VALIDATION)
  {
    // validate the outputted xml description
    QBuffer input(&xml);
    input.open(QIODevice::ReadOnly);

    ctkCmdLineModuleXmlValidator validator(&input);
    if (!validator.validateInput())
    {
      if (d->ValidationMode == STRICT_VALIDATION)
      {
        throw ctkInvalidArgumentException(QString("Validating module at %1 failed: %2")
                                          .arg(location.toString()).arg(validator.errorString()));
      }
      else
      {
        ref.d->XmlValidationErrorString = validator.errorString();
      }
    }
  }

  d->LocationToRef[location] = ref;

  emit moduleRegistered(ref);
  return ref;
}

void ctkCmdLineModuleManager::unregisterModule(const ctkCmdLineModuleReference& ref)
{
  d->LocationToRef.remove(ref.location());
  emit moduleUnregistered(ref);
}

ctkCmdLineModuleReference ctkCmdLineModuleManager::moduleReference(const QUrl &location) const
{
  return d->LocationToRef[location];
}

QList<ctkCmdLineModuleReference> ctkCmdLineModuleManager::moduleReferences() const
{
  return d->LocationToRef.values();
}

ctkCmdLineModuleFuture ctkCmdLineModuleManager::run(ctkCmdLineModuleFrontend *frontend)
{
  d->checkBackends(frontend->location());

  ctkCmdLineModuleFuture future = d->SchemeToBackend[frontend->location().scheme()]->run(frontend);
  frontend->setFuture(future);
  return future;
}
