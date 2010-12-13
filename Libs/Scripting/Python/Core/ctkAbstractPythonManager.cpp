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
#include <QDir>
#include <QDebug>

// CTK includes
#include "ctkAbstractPythonManager.h"
#include "ctkScriptingPythonCoreConfigure.h"

// PythonQT includes
#include <PythonQt.h>
#include <PythonQt_QtBindings.h>

// STD includes
#include <csignal>

#ifdef __GNUC__
// Disable warnings related to signal() function
// See http://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Pragmas.html
// Note: Ideally the incriminated functions and macros should be fixed upstream ...
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

//-----------------------------------------------------------------------------
ctkAbstractPythonManager::ctkAbstractPythonManager(QObject* _parent) : Superclass(_parent)
{
  this->InitFunction = 0;
}

//-----------------------------------------------------------------------------
ctkAbstractPythonManager::~ctkAbstractPythonManager()
{
  PyThreadState* state = PyThreadState_Get();
  Py_EndInterpreter(state);
  PythonQt::cleanup();
}

//-----------------------------------------------------------------------------
PythonQtObjectPtr ctkAbstractPythonManager::mainContext()
{
  if (!PythonQt::self())
    {
    this->initPythonQt();
    }
  if (PythonQt::self())
    {
    return PythonQt::self()->getMainModule();
    }
  return PythonQtObjectPtr();
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManager::initPythonQt()
{
  PythonQt::init(PythonQt::IgnoreSiteModule | PythonQt::RedirectStdOut);

  // Python maps SIGINT (control-c) to its own handler.  We will remap it
  // to the default so that control-c works.
  #ifdef SIGINT
  signal(SIGINT, SIG_DFL);
  #endif

  PythonQtObjectPtr _mainContext = PythonQt::self()->getMainModule();

  this->connect(PythonQt::self(), SIGNAL(pythonStdOut(const QString&)),
                SLOT(printStdout(const QString&)));
  this->connect(PythonQt::self(), SIGNAL(pythonStdErr(const QString&)),
                SLOT(printStderr(const QString&)));
  
  PythonQt_init_QtBindings();
  
  QStringList initCode;
  initCode << "import sys";
  foreach (QString path, this->pythonPaths())
    {
    initCode << QString("sys.path.append('%1')").arg(QDir::fromNativeSeparators(path));
    }

  _mainContext.evalScript(initCode.join("\n"));

  this->preInitialization();
  if (this->InitFunction)
    {
    (*this->InitFunction)();
    }
  emit this->pythonPreInitialized();

  this->executeInitializationScripts();
  emit this->pythonInitialized();
}

//-----------------------------------------------------------------------------
QStringList ctkAbstractPythonManager::pythonPaths()
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManager::preInitialization()
{
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManager::executeInitializationScripts()
{
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManager::registerPythonQtDecorator(QObject* decorator)
{
  PythonQt::self()->addDecorators(decorator);
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManager::registerClassForPythonQt(const QMetaObject* metaobject)
{
  PythonQt::self()->registerClass(metaobject);
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManager::registerCPPClassForPythonQt(const char* name)
{
  PythonQt::self()->registerCPPClass(name);
}

//-----------------------------------------------------------------------------
QVariant ctkAbstractPythonManager::executeString(const QString& code)
{
  QVariant ret;
  PythonQtObjectPtr main = ctkAbstractPythonManager::mainContext();
  if (main)
    {
    ret = main.evalScript(code, Py_single_input);
    }
  return ret;
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManager::executeFile(const QString& filename)
{
  PythonQtObjectPtr main = ctkAbstractPythonManager::mainContext();
  if (main)
    {
    main.evalFile(filename);
    }
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManager::setInitializationFunction(void (*initFunction)())
{
  this->InitFunction = initFunction;
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManager::addObjectToPythonMain(const QString& name, QObject* obj)
{
  PythonQtObjectPtr main = ctkAbstractPythonManager::mainContext();
  if (main && obj)
    {
    main.addObject(name, obj);
    }
}

//-----------------------------------------------------------------------------
QVariant ctkAbstractPythonManager::getVariable(const QString& name)
{
  PythonQtObjectPtr main = ctkAbstractPythonManager::mainContext();
  if (main)
    {
    return PythonQt::self()->getVariable(main, name);
    }
  return QVariant();
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManager::printStdout(const QString& text)
{
  std::cout << qPrintable(text);
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManager::printStderr(const QString& text)
{
  std::cout << qPrintable(text);
}
