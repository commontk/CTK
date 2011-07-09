/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

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

// Qt includes
#include <QApplication>
#include <QTextStream>

// CTK includes
#include <ctkPythonConsole.h>
#include <ctkCommandLineParser.h>

#include "ctkSimplePythonManager.h"
#include "ctkTestWrappedQProperty.h"
#include "ctkTestWrappedQInvokable.h"
#include "ctkTestWrappedSlot.h"
#include "ctkSimplePythonShellConfigure.h" // For CTK_WRAP_PYTHONQT_USE_VTK

#ifdef CTK_WRAP_PYTHONQT_USE_VTK
# include "ctkTestWrappedQListOfVTKObject.h"
# include "ctkTestWrappedVTKSlot.h"
# include "ctkTestWrappedVTKQInvokable.h"
#endif

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  ctkCommandLineParser parser;
  // Use Unix-style argument names
  parser.setArgumentPrefix("--", "-");

  // Add command line argument names
  parser.addArgument("help", "h", QVariant::Bool, "Print usage information and exit.");
  parser.addArgument("interactive", "I", QVariant::Bool, "Enable interactive mode");

  // Parse the command line arguments
  bool ok = false;
  QHash<QString, QVariant> parsedArgs = parser.parseArguments(QCoreApplication::arguments(), &ok);
  if (!ok)
  {
    QTextStream(stderr, QIODevice::WriteOnly) << "Error parsing arguments: "
                                              << parser.errorString() << "\n";
    return EXIT_FAILURE;
  }

  // Show a help message
  if (parsedArgs.contains("help") || parsedArgs.contains("h"))
  {
    QTextStream(stdout, QIODevice::WriteOnly) << "ctkSimplePythonShell\n"
          << "Usage\n\n"
          << "  ctkSimplePythonShell [options] [<path-to-python-script> ...]\n\n"
          << "Options\n"
          << parser.helpText();
    return EXIT_SUCCESS;
  }
  
  ctkSimplePythonManager pythonManager;
  
  ctkPythonConsole console;
  console.initialize(&pythonManager);
  console.setAttribute(Qt::WA_QuitOnClose, true);
  console.resize(600, 280);
  console.show();

  console.setProperty("isInteractive", parsedArgs.contains("interactive"));

  QStringList list;
  list << "qt.QPushButton";
  console.completer()->setAutocompletePreferenceList(list);

  pythonManager.addObjectToPythonMain("_ctkPythonConsoleInstance", &console);

  ctkTestWrappedQProperty testWrappedQProperty;
  pythonManager.addObjectToPythonMain("_testWrappedQPropertyInstance", &testWrappedQProperty);

  ctkTestWrappedQInvokable testWrappedQInvokable;
  pythonManager.addObjectToPythonMain("_testWrappedQInvokableInstance", &testWrappedQInvokable);

  ctkTestWrappedSlot testWrappedSlot;
  pythonManager.addObjectToPythonMain("_testWrappedSlotInstance", &testWrappedSlot);

#ifdef CTK_WRAP_PYTHONQT_USE_VTK
  ctkTestWrappedVTKQInvokable testWrappedVTKQInvokable;
  pythonManager.addObjectToPythonMain("_testWrappedVTKQInvokableInstance", &testWrappedVTKQInvokable);

  ctkTestWrappedVTKSlot testWrappedVTKSlot;
  pythonManager.addObjectToPythonMain("_testWrappedVTKSlotInstance", &testWrappedVTKSlot);

  ctkTestWrappedQListOfVTKObject testWrappedQListOfVTKObject;
  pythonManager.addObjectToPythonMain("_testWrappedQListOfVTKObjectInstance", &testWrappedQListOfVTKObject);
#endif

  foreach(const QString& script, parser.unparsedArguments())
    {
    pythonManager.executeFile(script);
    }
  
  return app.exec();
}
