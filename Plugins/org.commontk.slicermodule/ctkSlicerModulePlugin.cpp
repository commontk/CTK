/*=============================================================================

  Library: CTK

  Copyright (c) 2010 CISTIB - Universitat Pompeu Fabra

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


#include "ctkSlicerModulePlugin_p.h"
#include "ctkSlicerModuleReader.h"
#include <QtPlugin>
#include <QDebug>

ctkSlicerModulePlugin* ctkSlicerModulePlugin::instance = 0;

ctkSlicerModulePlugin::ctkSlicerModulePlugin()
  : context(0)
{
}

ctkSlicerModulePlugin::~ctkSlicerModulePlugin()
{
  
}

void ctkSlicerModulePlugin::start(ctkPluginContext* context)
{
  instance = this;
  this->context = context;

  this->reader = new ctkSlicerModuleReader( );
  context->registerService(QStringList("ctkModuleDescriptionReaderInterface"),
    this->reader);

  qDebug() << "Registered Slicer Module Description";
}

void ctkSlicerModulePlugin::stop(ctkPluginContext* context)
{
  delete this->reader;
  this->reader = NULL;
  Q_UNUSED(context)
}

ctkSlicerModulePlugin* ctkSlicerModulePlugin::getInstance()
{
  return instance;
}

ctkPluginContext* ctkSlicerModulePlugin::getPluginContext() const
{
  return context;
}

Q_EXPORT_PLUGIN2(org_commontk_slicermodule, ctkSlicerModulePlugin)


