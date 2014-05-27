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

// Qt includes
#include <QtPlugin>
#include <QStringList>
#include <QString>

// CTK includes
#include "ctkCommandLineModuleAppPlugin_p.h"
#include "ctkCommandLineModuleAppLogic_p.h"
#include <ctkCommandLineParser.h>

ctkPluginContext* ctkCommandLineModuleAppPlugin::Context = 0;

//----------------------------------------------------------------------------
ctkCommandLineModuleAppPlugin::ctkCommandLineModuleAppPlugin()
  : AppLogic(0)
{
}

//----------------------------------------------------------------------------
ctkCommandLineModuleAppPlugin::~ctkCommandLineModuleAppPlugin()
{
  qDebug()<< "delete applogic";
  delete this->AppLogic;
  this->AppLogic = 0;
}

//----------------------------------------------------------------------------
void ctkCommandLineModuleAppPlugin::start(ctkPluginContext* context)
{
  ctkCommandLineModuleAppPlugin::Context = context;

  delete this->AppLogic;

  ctkCommandLineParser cmdLineParser;
  cmdLineParser.setArgumentPrefix("--", "-");
  cmdLineParser.setStrictModeEnabled(true);

  cmdLineParser.addArgument("module", "", QVariant::String, "Path to a CLI module (executable)", "CLIModuleBlur2dImage");

  QString argsstring("pluginname ");
  argsstring.append(context->getProperty("dah.args").toString());

  QStringList argslist = argsstring.split(" ");

  bool parseOkay = false;
  QHash<QString, QVariant> args = cmdLineParser.parseArguments(argslist, &parseOkay);

  bool canStart = true;
  if(!args.contains("module"))
  {
    qDebug() << "ctkCommandLineModuleAppPlugin: The plugin framework does not contain a valid \"dah.args\" property that specifies a CLModule as \"--module <modulename>\".";
    canStart = false;
  }

  if(canStart)
  {
    this->AppLogic = new ctkCommandLineModuleAppLogic(args["module"].toString());
    context->registerService<ctkDicomAppInterface>(this->AppLogic);
  }
}

//----------------------------------------------------------------------------
void ctkCommandLineModuleAppPlugin::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)
  ctkCommandLineModuleAppPlugin::Context = 0;
}

//----------------------------------------------------------------------------
ctkPluginContext* ctkCommandLineModuleAppPlugin::getPluginContext()
{
  return ctkCommandLineModuleAppPlugin::Context;
}

#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
Q_EXPORT_PLUGIN2(org_commontk_dah_cmdlinemoduleapp, ctkCommandLineModuleAppPlugin)
#endif
