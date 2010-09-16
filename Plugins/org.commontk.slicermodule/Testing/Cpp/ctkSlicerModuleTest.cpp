/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  CISTIB - Universitat Pompeu Fabra

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 
=========================================================================*/

// CTK includes
#include "ctkModuleDescription.h"
#include "ctkPluginFrameworkFactory.h"
#include "ctkPluginFramework.h"
#include "ctkPluginException.h"
#include "ctkModuleDescriptionReaderInterface.h"
#include "ctkCommandLineParser.h"

// STD includes
#include <iostream>

#include "QFile"


//-----------------------------------------------------------------------------
int ctkSlicerModuleTest(int argc, char * argv [] )
{

  ctkCommandLineParser parser;
  parser.addArgument("", "-F", QVariant::String);
  bool ok = false;
  QHash<QString, QVariant> parsedArgs = parser.parseArguments(argc, argv, &ok);
  if (!ok)
  {
    std::cerr << qPrintable(parser.errorString()) << std::endl;
    return EXIT_FAILURE;
  }
  QString xmlFileName = parsedArgs["-F"].toString();

  ctkPluginFrameworkFactory fwFactory;
  ctkPluginFramework* framework = fwFactory.getFramework();

  try {
    framework->init();
  }
  catch (const ctkPluginException& exc)
  {
    qCritical() << "Failed to initialize the plug-in framework:" << exc;
    exit(1);
  }


#ifdef CMAKE_INTDIR
  QString pluginPath = "/../plugins/" CMAKE_INTDIR "/";
#else
  QString pluginPath = "/plugins/";
#endif

  try
  {
    QString pluginLocation = "." + pluginPath + "liborg_commontk_slicermodule.dll";
    ctkPlugin* plugin = framework->getPluginContext()->installPlugin(QUrl::fromLocalFile(pluginLocation));
    plugin->start(ctkPlugin::START_TRANSIENT);

    framework->start();

    ctkServiceReference* serviceRef;
    serviceRef = framework->getPluginContext()->getServiceReference( 
      "ctkModuleDescriptionReaderInterface" );

    ctkModuleDescriptionReaderInterface* reader;
    reader = qobject_cast<ctkModuleDescriptionReaderInterface*>
      (framework->getPluginContext()->getService(serviceRef));


    // Read file
    QFile file( xmlFileName );
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      std::cout << "XML file " << xmlFileName.toStdString( ) << " could not be opened." << endl;
      return false;
    }
    QTextStream stream( &file );

    // Parse XML file
    reader->setXml( stream.readAll() );
    reader->Update();

    // Print module description
    QTextStream(stdout) << reader->moduleDescription( );

  }
  catch (const ctkPluginException& e)
  {
    qCritical() << e.what();
  }

  return EXIT_SUCCESS;
}
