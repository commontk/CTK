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
#include <QDir>
#include <QDebug>

// CTK includes
#include "ctkAbstractPythonManager.h"
#include "ctkScriptingPythonCoreConfigure.h"

// PythonQT includes
#include <PythonQt.h>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  #include <PythonQt_QtBindings.h>
#endif

// STD includes
#include <csignal>

#ifdef __GNUC__
// Disable warnings related to signal() function
// See http://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Pragmas.html
// Note: Ideally the incriminated functions and macros should be fixed upstream ...
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

//-----------------------------------------------------------------------------
class ctkAbstractPythonManagerPrivate
{
  Q_DECLARE_PUBLIC(ctkAbstractPythonManager);
protected:
  ctkAbstractPythonManager* q_ptr;
public:
  ctkAbstractPythonManagerPrivate(ctkAbstractPythonManager& object);
  virtual ~ctkAbstractPythonManagerPrivate();

  void (*InitFunction)();

  int PythonQtInitializationFlags;
};

//-----------------------------------------------------------------------------
// ctkAbstractPythonManagerPrivate methods

//-----------------------------------------------------------------------------
ctkAbstractPythonManagerPrivate::ctkAbstractPythonManagerPrivate(ctkAbstractPythonManager &object) :
  q_ptr(&object)
{
  this->InitFunction = 0;
  this->PythonQtInitializationFlags = PythonQt::IgnoreSiteModule | PythonQt::RedirectStdOut;
}

//-----------------------------------------------------------------------------
ctkAbstractPythonManagerPrivate::~ctkAbstractPythonManagerPrivate()
{
}

//-----------------------------------------------------------------------------
// ctkAbstractPythonManager methods

//-----------------------------------------------------------------------------
ctkAbstractPythonManager::ctkAbstractPythonManager(QObject* _parent) : Superclass(_parent),
  d_ptr(new ctkAbstractPythonManagerPrivate(*this))
{
}

//-----------------------------------------------------------------------------
ctkAbstractPythonManager::~ctkAbstractPythonManager()
{
  if (Py_IsInitialized())
    {
    Py_Finalize();
    }
  PythonQt::cleanup();
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManager::setInitializationFlags(int flags)
{
  Q_D(ctkAbstractPythonManager);
  if (PythonQt::self())
    {
    return;
    }
  d->PythonQtInitializationFlags = flags;
}

//-----------------------------------------------------------------------------
int ctkAbstractPythonManager::initializationFlags()const
{
  Q_D(const ctkAbstractPythonManager);
  return d->PythonQtInitializationFlags;
}

//-----------------------------------------------------------------------------
bool ctkAbstractPythonManager::initialize()
{
  Q_D(ctkAbstractPythonManager);
  if (!PythonQt::self())
    {
    this->initPythonQt(d->PythonQtInitializationFlags);
    }
  return this->isPythonInitialized();
}

//-----------------------------------------------------------------------------
PythonQtObjectPtr ctkAbstractPythonManager::mainContext()
{
  bool initalized = this->initialize();
  if (initalized)
    {
    return PythonQt::self()->getMainModule();
    }
  return PythonQtObjectPtr();
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManager::initPythonQt(int flags)
{
  Q_D(ctkAbstractPythonManager);

  PythonQt::init(flags);

  // Python maps SIGINT (control-c) to its own handler.  We will remap it
  // to the default so that control-c works.
  #ifdef SIGINT
  signal(SIGINT, SIG_DFL);
  #endif

  // Forward signal from PythonQt::self() to this instance of ctkAbstractPythonManager
  this->connect(PythonQt::self(), SIGNAL(systemExitExceptionRaised(int)),
                SIGNAL(systemExitExceptionRaised(int)));

  this->connect(PythonQt::self(), SIGNAL(pythonStdOut(QString)),
                SLOT(printStdout(QString)));
  this->connect(PythonQt::self(), SIGNAL(pythonStdErr(QString)),
                SLOT(printStderr(QString)));

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  PythonQt_init_QtBindings();
#endif

  QStringList initCode;

  // Update 'sys.path'
  initCode << "import sys";
  foreach (const QString& path, this->pythonPaths())
    {
    initCode << QString("sys.path.append('%1')").arg(QDir::fromNativeSeparators(path));
    }

  PythonQtObjectPtr _mainContext = PythonQt::self()->getMainModule();
  _mainContext.evalScript(initCode.join("\n"));

  this->preInitialization();
  if (d->InitFunction)
    {
    (*d->InitFunction)();
    }
  emit this->pythonPreInitialized();

  this->executeInitializationScripts();
  emit this->pythonInitialized();
}

//-----------------------------------------------------------------------------
bool ctkAbstractPythonManager::isPythonInitialized()const
{
  return PythonQt::self() != 0;
}

//-----------------------------------------------------------------------------
bool ctkAbstractPythonManager::pythonErrorOccured()const
{
  return PythonQt::self()->hadError();
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManager::resetErrorFlag()
{
  PythonQt::self()->clearError();
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
bool ctkAbstractPythonManager::systemExitExceptionHandlerEnabled()const
{
  return PythonQt::self()->systemExitExceptionHandlerEnabled();
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManager::setSystemExitExceptionHandlerEnabled(bool value)
{
  PythonQt::self()->setSystemExitExceptionHandlerEnabled(value);
}

//-----------------------------------------------------------------------------
QVariant ctkAbstractPythonManager::executeString(const QString& code,
                                                 ctkAbstractPythonManager::ExecuteStringMode mode)
{
  int start = -1;
  switch(mode)
    {
    case ctkAbstractPythonManager::FileInput: start = Py_file_input; break;
    case ctkAbstractPythonManager::SingleInput: start = Py_single_input; break;
    case ctkAbstractPythonManager::EvalInput:
    default: start = Py_eval_input; break;
    }

  QVariant ret;
  PythonQtObjectPtr main = ctkAbstractPythonManager::mainContext();
  if (main)
    {
    ret = main.evalScript(code, start);
    }
  return ret;
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManager::executeFile(const QString& filename)
{
  PythonQtObjectPtr main = ctkAbstractPythonManager::mainContext();
  if (main)
    {
    QString path = QFileInfo(filename).absolutePath();
    // See http://nedbatchelder.com/blog/200711/rethrowing_exceptions_in_python.html
    QStringList code = QStringList()
        << "import sys"
        << QString("sys.path.insert(0, '%1')").arg(path)
        << "_updated_globals = globals()"
        << QString("_updated_globals['__file__'] = '%1'").arg(filename)
        << "_ctk_executeFile_exc_info = None"
        << "try:"
        << QString("    execfile('%1', _updated_globals)").arg(filename)
        << "except Exception, e:"
        << "    _ctk_executeFile_exc_info = sys.exc_info()"
        << "finally:"
        << "    del _updated_globals"
        << QString("    if sys.path[0] == '%1': sys.path.pop(0)").arg(path)
        << "    if _ctk_executeFile_exc_info:"
        << "        raise _ctk_executeFile_exc_info[1], None, _ctk_executeFile_exc_info[2]";
    this->executeString(code.join("\n"));
    //PythonQt::self()->handleError(); // Clear errorOccured flag
    }
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManager::setInitializationFunction(void (*initFunction)())
{
  Q_D(ctkAbstractPythonManager);
  d->InitFunction = initFunction;
}

//----------------------------------------------------------------------------
QStringList ctkAbstractPythonManager::pythonAttributes(const QString& pythonVariableName,
                                                       const QString& module,
                                                       bool appendParenthesis) const
{
  Q_ASSERT(PyThreadState_GET()->interp);
  PyObject* dict = PyImport_GetModuleDict();

  // Split module by '.' and retrieve the object associated if the last module
  PyObject* object = 0;
  PyObject* prevObject = 0;
  QStringList moduleList = module.split(".", QString::SkipEmptyParts);
  foreach(const QString& module, moduleList)
    {
    object = PyDict_GetItemString(dict, module.toLatin1().data());
    if (prevObject) { Py_DECREF(prevObject); }
    if (!object)
      {
      break;
      }
    Py_INCREF(object);
    dict = PyModule_GetDict(object);
    prevObject = object;
    }
  if (!object)
    {
    return QStringList();
    }

//  PyObject* object = PyDict_GetItemString(dict, module.toLatin1().data());
//  if (!object)
//    {
//    return QStringList();
//    }
//  Py_INCREF(object);

  if (!pythonVariableName.isEmpty())
    {
    QStringList tmpNames = pythonVariableName.split('.');
    for (int i = 0; i < tmpNames.size() && object; ++i)
      {
      QByteArray tmpName = tmpNames.at(i).toLatin1();
      PyObject* prevObj = object;
      if (PyDict_Check(object))
        {
        object = PyDict_GetItemString(object, tmpName.data());
        Py_XINCREF(object);
        }
      else
        {
        object = PyObject_GetAttrString(object, tmpName.data());
        }
      Py_DECREF(prevObj);
      }
    PyErr_Clear();
    }

  QStringList results;
  if (object)
    {
    PyObject* keys = PyObject_Dir(object);
    if (keys)
      {
      PyObject* key;
      PyObject* value;
      int nKeys = PyList_Size(keys);
      for (int i = 0; i < nKeys; ++i)
        {
        key = PyList_GetItem(keys, i);
        value = PyObject_GetAttr(object, key);
        if (!value)
          {
          continue;
          }
        QString key_str(PyString_AsString(key));
        // Append "()" if the associated object is a function
        if (appendParenthesis && PyCallable_Check(value))
          {
          key_str.append("()");
          }
        results << key_str;
        Py_DECREF(value);
        }
      Py_DECREF(keys);
      }
    Py_DECREF(object);
    }
  return results;
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
void ctkAbstractPythonManager::addWrapperFactory(PythonQtForeignWrapperFactory* factory)
{
  PythonQt::self()->addWrapperFactory(factory);
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
  std::cerr << qPrintable(text);
}
