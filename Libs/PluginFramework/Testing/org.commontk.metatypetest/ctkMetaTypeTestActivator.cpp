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

#include "ctkMetaTypeTestActivator_p.h"

#include <ctkPluginContext.h>
#include <ctkPluginConstants.h>

#include "ctkMTAttrPasswordTestSuite_p.h"
#include "ctkMTLocaleTestSuite_p.h"

#include <QtPlugin>
#include <QStringList>

//----------------------------------------------------------------------------
void ctkMetaTypeTestActivator::start(ctkPluginContext* context)
{
  QString symbolicName = context->getProperty("metatype.impl").toString();
  if (symbolicName.isEmpty())
  {
    throw ctkRuntimeException("Framework property 'metatype.impl' containing the symbolic "
                              "name of the MetaType implementation not found!");
  }

  long mtPluginId = -1;
  foreach(QSharedPointer<ctkPlugin> p, context->getPlugins())
  {
    if (p->getSymbolicName() == symbolicName)
    {
      mtPluginId = p->getPluginId();
      break;
    }
  }

  if (mtPluginId < 0)
  {
    QString msg = QString("The MetaType implementation '%1' is not installed.")
        .arg(symbolicName);
    throw ctkRuntimeException(msg);
  }

  ctkDictionary props;

  attrPwdTestSuite = new ctkMTAttrPasswordTestSuite(context, mtPluginId);
  props.clear();
  props.insert(ctkPluginConstants::SERVICE_PID, attrPwdTestSuite->metaObject()->className());
  context->registerService<ctkTestSuiteInterface>(attrPwdTestSuite, props);

  localeTestSuite = new ctkMTLocaleTestSuite(context, mtPluginId);
  props.clear();
  props.insert(ctkPluginConstants::SERVICE_PID, localeTestSuite->metaObject()->className());
  context->registerService<ctkTestSuiteInterface>(localeTestSuite, props);
}

//----------------------------------------------------------------------------
void ctkMetaTypeTestActivator::stop(ctkPluginContext* context)
{
  Q_UNUSED(context);

  delete attrPwdTestSuite;
  delete localeTestSuite;

  attrPwdTestSuite = 0;
  localeTestSuite = 0;
}

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
Q_EXPORT_PLUGIN2(org_commontk_metatypetest, ctkMetaTypeTestActivator)
#endif
