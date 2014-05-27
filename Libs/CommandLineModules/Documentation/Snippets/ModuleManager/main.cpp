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
#include "ctkCmdLineModuleBackendLocalProcess.h"
#include "ctkCmdLineModuleFrontendFactoryQtGui.h"
#include "ctkCmdLineModuleFrontendQtGui.h"
#include "ctkCmdLineModuleFuture.h"
#include "ctkException.h"
#include "ctkCmdLineModuleRunException.h"

#include <QApplication>
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
#include <QDesktopServices>
#else
#include <QStandardPaths>
#endif
#include <QWidget>
#include <QUrl>
#include <QDebug>

#include <cstdlib>

int main(int argc, char** argv)
{
  QApplication myApp(argc, argv);
  myApp.setOrganizationName("CommonTK");
  myApp.setApplicationName("ModuleManagerSnippet");

  // [instantiate-mm]
  // Instantiate a ctkCmdLineModuleManager class.
  ctkCmdLineModuleManager moduleManager(
        // Use "strict" validation mode, rejecting modules with non-valid XML descriptions.
        ctkCmdLineModuleManager::STRICT_VALIDATION,
        // Use the default cache location for this application
      #if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
        QDesktopServices::storageLocation(QDesktopServices::CacheLocation)
      #else
        QStandardPaths::writableLocation(QStandardPaths::CacheLocation)
      #endif
        );
  // [instantiate-mm]

  // [register-backend]
  // Instantiate a back-end for running executable modules in a local process.
  // This back-end handles the "file" Url scheme.
  QScopedPointer<ctkCmdLineModuleBackend> processBackend(new ctkCmdLineModuleBackendLocalProcess);

  // Register the back-end with the module manager.
  moduleManager.registerBackend(processBackend.data());
  // [register-backend]

  // [register-module]
  ctkCmdLineModuleReference moduleRef;
  try
  {
    // Register a local executable as a module, the ctkCmdLineModuleBackendLocalProcess
    // can handle it.
    moduleRef = moduleManager.registerModule(QUrl::fromLocalFile("C:/modules/MyModule.exe"));
  }
  catch (const ctkInvalidArgumentException& e)
  {
    // Module validation failed.
    qDebug() << e;
    return EXIT_FAILURE;
  }
  // [register-module]

  // [create-frontend]
  // We use the "Qt Gui" frontend factory.
  QScopedPointer<ctkCmdLineModuleFrontendFactory> frontendFactory(new ctkCmdLineModuleFrontendFactoryQtGui);
  myApp.addLibraryPath(QCoreApplication::applicationDirPath() + "/../");

  QScopedPointer<ctkCmdLineModuleFrontend> frontend(frontendFactory->create(moduleRef));

  // Create the actual GUI representation.
  QWidget* gui = qobject_cast<QWidget*>(frontend->guiHandle());
  // [create-frontend]
  Q_UNUSED(gui);

  // Now try and run the module (using the default values for the parameters)
  // and print out any reported output and results.
  // [run-module]
  try
  {
    ctkCmdLineModuleFuture future = moduleManager.run(frontend.data());
    future.waitForFinished();
    qDebug() << "Console output:";
    qDebug() << future.readAllOutputData();
    qDebug() << "Error output:";
    qDebug() << future.readAllErrorData();
    qDebug() << "Results:";
    qDebug() << future.results();
  }
  catch (const ctkCmdLineModuleRunException& e)
  {
    qWarning() << e;
  }
  // [run-module]

  return EXIT_SUCCESS;
}
