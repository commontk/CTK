/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QStatusBar>
#include <QTextStream>
#include <QTimer>

// CTK includes
#include <ctkCallback.h>
#include <ctkPythonConsole.h>
#include <ctkCommandLineParser.h>

#include "ctkSimplePythonManager.h"
#include "ctkTestWrappedQProperty.h"
#include "ctkTestWrappedQInvokable.h"
#include "ctkTestWrappedSlot.h"
#include "ctkSimplePythonShellConfigure.h" // For CTK_WRAP_PYTHONQT_USE_VTK

#ifdef CTK_WRAP_PYTHONQT_USE_VTK
# include "ctkTestWrappedQListOfVTKObject.h"
# include "ctkTestWrappedVTKObserver.h"
# include "ctkTestWrappedVTKQInvokable.h"
# include "ctkTestWrappedVTKSlot.h"
# include <vtkDebugLeaks.h>
#endif

namespace
{
//-----------------------------------------------------------------------------
void executeScripts(void * data)
{
  ctkSimplePythonManager * pythonManager = reinterpret_cast<ctkSimplePythonManager*>(data);
  QStringList scripts = pythonManager->property("scripts").toStringList();
  foreach(const QString& script, scripts)
    {
    pythonManager->executeFile(script);
    if (pythonManager->pythonErrorOccured())
      {
      QApplication::exit(EXIT_FAILURE);
      }
    }
}

//-----------------------------------------------------------------------------
void onCursorPositionChanged(void *data)
{
  ctkPythonConsole* pythonConsole = reinterpret_cast<ctkPythonConsole*>(data);
  QMainWindow* mainWindow = qobject_cast<QMainWindow*>(
        pythonConsole->parentWidget());
  QLabel * label = mainWindow->statusBar()->findChild<QLabel*>();
  label->setText(QString("Position %1, Column %2, Line %3")
                 .arg(pythonConsole->cursorPosition())
                 .arg(pythonConsole->cursorColumn())
                 .arg(pythonConsole->cursorLine()));
}

} // end of anonymous namespace

//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
#ifdef CTK_WRAP_PYTHONQT_USE_VTK
  vtkDebugLeaks::SetExitError(true);
  ctkTestWrappedVTKObserver testWrappedVTKObserver;
#endif

  int exitCode = EXIT_FAILURE;
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

    QMainWindow * mainWindow = new QMainWindow();
    mainWindow->setCentralWidget(&console);
    mainWindow->resize(600, 280);
    mainWindow->show();

    QLabel cursorPositionLabel;
    mainWindow->statusBar()->addWidget(&cursorPositionLabel);

    ctkCallback cursorPositionChangedCallback;
    cursorPositionChangedCallback.setCallbackData(&console);
    cursorPositionChangedCallback.setCallback(onCursorPositionChanged);
    QObject::connect(&console, SIGNAL(cursorPositionChanged()),
                     &cursorPositionChangedCallback, SLOT(invoke()));

    console.setProperty("isInteractive", parsedArgs.contains("interactive"));

    QStringList list;
    list << "qt.QPushButton";
    console.completer()->setAutocompletePreferenceList(list);

    pythonManager.addObjectToPythonMain("_ctkPythonConsoleInstance", &console);
    pythonManager.addObjectToPythonMain("_ctkPythonManagerInstance", &pythonManager);

    ctkTestWrappedQProperty testWrappedQProperty;
    pythonManager.addObjectToPythonMain("_testWrappedQPropertyInstance", &testWrappedQProperty);

    ctkTestWrappedQInvokable testWrappedQInvokable;
    pythonManager.addObjectToPythonMain("_testWrappedQInvokableInstance", &testWrappedQInvokable);

    ctkTestWrappedSlot testWrappedSlot;
    pythonManager.addObjectToPythonMain("_testWrappedSlotInstance", &testWrappedSlot);

  #ifdef CTK_WRAP_PYTHONQT_USE_VTK
    pythonManager.addObjectToPythonMain("_testWrappedVTKObserverInstance", &testWrappedVTKObserver);

    ctkTestWrappedVTKQInvokable testWrappedVTKQInvokable;
    pythonManager.addObjectToPythonMain("_testWrappedVTKQInvokableInstance", &testWrappedVTKQInvokable);

    ctkTestWrappedVTKSlot testWrappedVTKSlot;
    pythonManager.addObjectToPythonMain("_testWrappedVTKSlotInstance", &testWrappedVTKSlot);

  //  ctkTestWrappedQListOfVTKObject testWrappedQListOfVTKObject;
  //  pythonManager.addObjectToPythonMain("_testWrappedQListOfVTKObjectInstance", &testWrappedQListOfVTKObject);
  #endif

    ctkCallback callback;
    callback.setCallbackData(&pythonManager);
    pythonManager.setProperty("scripts", parser.unparsedArguments());
    callback.setCallback(executeScripts);
    QTimer::singleShot(0, &callback, SLOT(invoke()));

    exitCode = app.exec();
  }
#ifdef CTK_WRAP_PYTHONQT_USE_VTK
  testWrappedVTKObserver.getTable()->Modified();
#endif
  return exitCode;
}
