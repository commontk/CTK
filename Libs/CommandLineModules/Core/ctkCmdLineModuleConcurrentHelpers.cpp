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

#include "ctkCmdLineModuleConcurrentHelpers.h"

#include "ctkCmdLineModuleManager.h"
#include "ctkCmdLineModuleRunException.h"

#include <qtconcurrentexception.h>
#include <QUrl>
#include <QDebug>

//----------------------------------------------------------------------------
ctkCmdLineModuleConcurrentRegister::ctkCmdLineModuleConcurrentRegister(ctkCmdLineModuleManager* manager,
                                                                       bool debug)
  : ModuleManager(manager), Debug(debug)
{}

//----------------------------------------------------------------------------
ctkCmdLineModuleReferenceResult ctkCmdLineModuleConcurrentRegister::operator()(const QString& moduleLocation)
{
  return this->operator ()(QUrl::fromLocalFile(moduleLocation));
}

//----------------------------------------------------------------------------
ctkCmdLineModuleReferenceResult ctkCmdLineModuleConcurrentRegister::operator()(const QUrl& moduleUrl)
{
  try
  {
    ctkCmdLineModuleReference reference = this->ModuleManager->registerModule(moduleUrl);
    return ctkCmdLineModuleReferenceResult(reference);
  }
  catch (const ctkException& e)
  {
    if (this->Debug)
    {
      qDebug() << e.message();
    }
    return ctkCmdLineModuleReferenceResult(moduleUrl, e.message());
  }
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
  catch (const QException& e)
#else
  catch (const QtConcurrent::Exception& e)
#endif
  {
    if (this->Debug)
    {
      qDebug() << e.what();
    }
    return ctkCmdLineModuleReferenceResult(moduleUrl, e.what());
  }
  catch (...)
  {
    QString errorMessage = QObject::tr("Module %1 failed with an unknown exception.").arg(moduleUrl.toString());
    if (this->Debug)
    {
      qDebug() << errorMessage;
    }
    return ctkCmdLineModuleReferenceResult(moduleUrl, errorMessage);
  }
  return ctkCmdLineModuleReferenceResult(moduleUrl);
}

//----------------------------------------------------------------------------
ctkCmdLineModuleConcurrentUnRegister::ctkCmdLineModuleConcurrentUnRegister(ctkCmdLineModuleManager* manager)
  : ModuleManager(manager)
{}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleConcurrentUnRegister::operator()(const QString& moduleLocation)
{
  return this->operator ()(QUrl::fromLocalFile(moduleLocation));
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleConcurrentUnRegister::operator()(const QUrl& moduleUrl)
{
  return this->operator ()(this->ModuleManager->moduleReference(moduleUrl));
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleConcurrentUnRegister::operator()(const ctkCmdLineModuleReference& moduleRef)
{
  if (moduleRef)
  {
    this->ModuleManager->unregisterModule(moduleRef);
    return true;
  }
  return false;
}
