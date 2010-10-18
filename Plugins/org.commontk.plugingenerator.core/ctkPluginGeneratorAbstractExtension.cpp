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


#include "ctkPluginGeneratorAbstractExtension.h"

#include "ctkPluginGeneratorCorePlugin_p.h"

#include <QVariant>

class ctkPluginGeneratorAbstractExtensionPrivate
{
public:

  ctkPluginGeneratorAbstractExtensionPrivate()
    : valid(true)
  {}

  bool valid;
  QString errorMessage;
  QHash<QString, QVariant> parameters;
};

ctkPluginGeneratorAbstractExtension::ctkPluginGeneratorAbstractExtension()
  : d_ptr(new ctkPluginGeneratorAbstractExtensionPrivate())
{
}

ctkPluginGeneratorAbstractExtension::~ctkPluginGeneratorAbstractExtension()
{
}

void ctkPluginGeneratorAbstractExtension::setParameter(const QHash<QString, QVariant>& params)
{
  Q_D(ctkPluginGeneratorAbstractExtension);
  d->parameters = params;
}

void ctkPluginGeneratorAbstractExtension::setParameter(const QString& name, const QVariant& value)
{
  Q_D(ctkPluginGeneratorAbstractExtension);
  d->parameters[name] = value;
}

QHash<QString, QVariant> ctkPluginGeneratorAbstractExtension::getParameter() const
{
  Q_D(const ctkPluginGeneratorAbstractExtension);
  return d->parameters;
}

bool ctkPluginGeneratorAbstractExtension::isValid() const
{
  Q_D(const ctkPluginGeneratorAbstractExtension);
  return d->valid;
}

void ctkPluginGeneratorAbstractExtension::validate()
{
  Q_D(ctkPluginGeneratorAbstractExtension);
  d->valid = verifyParameters(d->parameters);
}

void ctkPluginGeneratorAbstractExtension::updateCodeModel()
{
  Q_D(ctkPluginGeneratorAbstractExtension);
  this->updateCodeModel(d->parameters);
}

QString ctkPluginGeneratorAbstractExtension::getErrorMessage() const
{
  Q_D(const ctkPluginGeneratorAbstractExtension);
  return d->errorMessage;
}

ctkPluginGeneratorCodeModel* ctkPluginGeneratorAbstractExtension::getCodeModel() const
{
  return ctkPluginGeneratorCorePlugin::getInstance()->getCodeModel();
}

void ctkPluginGeneratorAbstractExtension::setErrorMessage(const QString& errorMsg)
{
  Q_D(ctkPluginGeneratorAbstractExtension);
  if (d->errorMessage != errorMsg)
  {
    d->errorMessage = errorMsg;
    emit errorMessageChanged(errorMsg);
  }
}
