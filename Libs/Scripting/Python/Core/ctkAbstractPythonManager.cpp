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
#include <atomic>

// CTK includes
#include "ctkAbstractPythonManager.h"
#include "ctkScriptingPythonCoreConfigure.h"

// PythonQt includes
#include <PythonQt.h>
#include <PythonQt_QtAll.h>

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
  bool ThreadSupportEnabled;
  PyThreadState* MainThreadState;
};

//-----------------------------------------------------------------------------
// ctkAbstractPythonManagerPrivate methods

//-----------------------------------------------------------------------------
ctkAbstractPythonManagerPrivate::ctkAbstractPythonManagerPrivate(ctkAbstractPythonManager &object) :
  q_ptr(&object)
{
  this->InitFunction = 0;
  this->PythonQtInitializationFlags = PythonQt::IgnoreSiteModule | PythonQt::RedirectStdOut;
  this->ThreadSupportEnabled = false;
  this->MainThreadState = nullptr;
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
  Q_D(ctkAbstractPythonManager);
  if (Py_IsInitialized())
  {
    if (d->MainThreadState)
    {
      PyEval_RestoreThread(d->MainThreadState);
      d->MainThreadState = nullptr;
    }
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
bool ctkAbstractPythonManager::threadSupportEnabled()const
{
  Q_D(const ctkAbstractPythonManager);
  return d->ThreadSupportEnabled;
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManager::setThreadSupportEnabled(bool enabled)
{
  Q_D(ctkAbstractPythonManager);
  if (PythonQt::self())
  {
    return;
  }
  d->ThreadSupportEnabled = enabled;
}

//-----------------------------------------------------------------------------
bool ctkAbstractPythonManager::initialize()
{
  Q_D(ctkAbstractPythonManager);
  if (!PythonQt::self())
  {
    this->initPythonQt(d->PythonQtInitializationFlags);

    if (d->ThreadSupportEnabled)
    {
      // Release GIL from the main thread to allow other threads to acquire it
      d->MainThreadState = PyEval_SaveThread();
    }
  }
  return this->isPythonInitialized();
}

//-----------------------------------------------------------------------------
bool ctkAbstractPythonManager::initialize(bool enableThreadSupport)
{
  this->setThreadSupportEnabled(enableThreadSupport);
  return this->initialize();
}

//-----------------------------------------------------------------------------
PythonQtObjectPtr ctkAbstractPythonManager::mainContext()
{
  bool initialized = this->initialize();
  if (initialized)
  {
    PYTHONQT_GIL_SCOPE;
    return PythonQt::self()->getMainModule();
  }
  return PythonQtObjectPtr();
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManager::initPythonQt(int flags)
{
  Q_D(ctkAbstractPythonManager);

  PythonQt::init(flags);
  PythonQt::setEnableThreadSupport(d->ThreadSupportEnabled);
  PYTHONQT_GIL_SCOPE;

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

  PythonQt_QtAll::init();

  QStringList initCode;

  // Update 'sys.path'
  initCode << "import sys";
  foreach (const QString& path, this->pythonPaths())
  {
    initCode << QString("sys.path.append(%1)").arg(QDir::fromNativeSeparators(path));
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
  if (!PythonQt::self())
  {
    qWarning() << Q_FUNC_INFO << " failed: PythonQt is not initialized";
    return false;
  }
  PYTHONQT_GIL_SCOPE;
  return PythonQt::self()->hadError();
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManager::resetErrorFlag()
{
  if (!PythonQt::self())
  {
    qWarning() << Q_FUNC_INFO << " failed: PythonQt is not initialized";
    return;
  }
  PYTHONQT_GIL_SCOPE;
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
  if (!PythonQt::self())
  {
    qWarning() << Q_FUNC_INFO << " failed: PythonQt is not initialized";
    return;
  }
  PYTHONQT_GIL_SCOPE;
  PythonQt::self()->addDecorators(decorator);
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManager::registerClassForPythonQt(const QMetaObject* metaobject)
{
  if (!PythonQt::self())
  {
    qWarning() << Q_FUNC_INFO << " failed: PythonQt is not initialized";
    return;
  }
  PYTHONQT_GIL_SCOPE;
  PythonQt::self()->registerClass(metaobject);
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManager::registerCPPClassForPythonQt(const char* name)
{
  if (!PythonQt::self())
  {
    qWarning() << Q_FUNC_INFO << " failed: PythonQt is not initialized";
    return;
  }
  PYTHONQT_GIL_SCOPE;
  PythonQt::self()->registerCPPClass(name);
}

//-----------------------------------------------------------------------------
bool ctkAbstractPythonManager::systemExitExceptionHandlerEnabled()const
{
  if (!PythonQt::self())
  {
    qWarning() << Q_FUNC_INFO << " failed: PythonQt is not initialized";
    return false;
  }
  PYTHONQT_GIL_SCOPE;
  return PythonQt::self()->systemExitExceptionHandlerEnabled();
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManager::setSystemExitExceptionHandlerEnabled(bool value)
{
  if (!PythonQt::self())
  {
    qWarning() << Q_FUNC_INFO << " failed: PythonQt is not initialized";
    return;
  }
  PYTHONQT_GIL_SCOPE;
  PythonQt::self()->setSystemExitExceptionHandlerEnabled(value);
}

//-----------------------------------------------------------------------------
bool ctkAbstractPythonManager::redirectStdOutCallbackEnabled()const
{
  if (!PythonQt::self())
  {
    qWarning() << Q_FUNC_INFO << " failed: PythonQt is not initialized";
    return false;
  }
  PYTHONQT_GIL_SCOPE;
  return PythonQt::self()->redirectStdOutCallbackEnabled();
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManager::setRedirectStdOutCallbackEnabled(bool value)
{
  if (!PythonQt::self())
  {
    qWarning() << Q_FUNC_INFO << " failed: PythonQt is not initialized";
    return;
  }
  PYTHONQT_GIL_SCOPE;
  PythonQt::self()->setRedirectStdOutCallbackEnabled(value);
}

//-----------------------------------------------------------------------------
QVariant ctkAbstractPythonManager::executeString(const QString& code,
                                                 ctkAbstractPythonManager::ExecuteStringMode mode)
{
  if (!this->initialize())
  {
    return QVariant();
  }
  PYTHONQT_GIL_SCOPE;
  int start = -1;
  switch(mode)
  {
    case ctkAbstractPythonManager::FileInput: start = Py_file_input; break;
    case ctkAbstractPythonManager::SingleInput: start = Py_single_input; break;
    case ctkAbstractPythonManager::EvalInput:
    default: start = Py_eval_input; break;
  }

  QVariant ret;
  PythonQtObjectPtr main = PythonQt::self()->getMainModule();
  if (main)
  {
    ret = main.evalScript(code, start);
  }
  return ret;
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManager::executeFile(const QString& filename)
{
  if (!this->initialize())
  {
    return;
  }
  PYTHONQT_GIL_SCOPE;
  PythonQtObjectPtr main = PythonQt::self()->getMainModule();
  if (main)
  {
    QString path = QFileInfo(filename).absolutePath();

    PyObject* helper = PyDict_New();
    if (!helper)
    {
      return;
    }
    PyDict_SetItemString(helper, "__builtins__", PyEval_GetBuiltins());
    PyDict_SetItemString(helper, "_ctk_main_globals", PyModule_GetDict(main.object()));

    // try/finally: without it the sys.path prepend would leak for the rest of the process lifetime.
    QStringList code = QStringList()
        << "import sys"
        << QString("_ctk_executefile_dir = %1").arg(ctkAbstractPythonManager::toPythonStringLiteral(path))
        << "sys.path.insert(0, _ctk_executefile_dir)"
        << "try:"
        << QString("    _ctk_main_globals['__file__'] = %1").arg(ctkAbstractPythonManager::toPythonStringLiteral(filename))
        << QString("    exec(open(%1).read(), _ctk_main_globals)").arg(ctkAbstractPythonManager::toPythonStringLiteral(filename))
        << "finally:"
        << "    try:"
        << "        sys.path.remove(_ctk_executefile_dir)"
        << "    except ValueError:"
        << "        pass";
    PythonQt::self()->evalScript(code.join("\n"), helper, helper, Py_file_input);
    Py_DECREF(helper);
    //PythonQt::self()->handleError(); // Clear errorOccured flag
  }
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManager::setInitializationFunction(void (*initFunction)())
{
  Q_D(ctkAbstractPythonManager);
  d->InitFunction = initFunction;
}

//-----------------------------------------------------------------------------
QStringList ctkAbstractPythonManager::dir_object(PyObject* object,
                                                 bool appendParenthesis)
{
  QStringList results;
  if (!object)
  {
    return results;
  }
  PYTHONQT_GIL_SCOPE;
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
        PyErr_Clear();
        continue;
      }
      QString key_str(PyUnicode_AsUTF8(key));
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
  return results;
}

//----------------------------------------------------------------------------
QStringList ctkAbstractPythonManager::splitByDotOutsideParenthesis(const QString& pythonVariableName)
{
  QStringList tmpNames;
  int last_pos_dot = pythonVariableName.length();
  int numberOfParenthesisClosed = 0;
  bool betweenSingleQuotes = false;
  bool betweenDoubleQuotes = false;
  for (int i = pythonVariableName.length()-1; i >= 0; --i)
  {
    QChar c = pythonVariableName.at(i);
    if (c == '\'' && !betweenDoubleQuotes)
    {
      betweenSingleQuotes = !betweenSingleQuotes;
    }
    if (c == '"' && !betweenSingleQuotes)
    {
      betweenDoubleQuotes = !betweenDoubleQuotes;
    }
    // note that we must not count parenthesis if they are between quote...
    if (!betweenSingleQuotes && !betweenDoubleQuotes)
    {
      if (c == '(')
      {
        if (numberOfParenthesisClosed>0)
        {
          numberOfParenthesisClosed--;
        }
      }
      if (c == ')')
      {
        numberOfParenthesisClosed++;
      }
    }
    // if we are outside parenthesis and we find a dot, then split
    if ((c == '.' && numberOfParenthesisClosed<=0)
        || i == 0)
    {
      if (i == 0) {i--;} // last case where we have to split the begging this time
      QString textToSplit = pythonVariableName.mid(i+1,last_pos_dot-(i+1));
      if (!textToSplit.isEmpty())
      {
        tmpNames.push_front(textToSplit);
      }
      last_pos_dot =i;
    }
  }
  return tmpNames;
}

//----------------------------------------------------------------------------
QStringList ctkAbstractPythonManager::pythonAttributes(const QString& pythonVariableName,
                                                       const QString& module,
                                                       bool appendParenthesis) const
{
  PYTHONQT_GIL_SCOPE;
  Q_ASSERT(PyThreadState_GET()->interp);

  PyObject* object = ctkAbstractPythonManager::pythonModule(module);
  if (!object)
  {
    PyErr_Clear();
    return QStringList();
  }


  PyObject* main_object = object; // save the module object (usually __main__ or __main__.__builtins__)
  static std::atomic<unsigned long long> autocompleteCounter(0);
  QString instantiated_class_name = QString("_ctkAbstractPythonManager_autocomplete_tmp_%1")
      .arg(autocompleteCounter.fetch_add(1, std::memory_order_relaxed));
  QStringList results; // the list of attributes to return
  QString line_code="";

  if (!pythonVariableName.isEmpty())
  {
    // Split the pythonVariableName at every dot
    // /!\ // CAREFUL to don't take dot which are between parenthesis
    // To avoid the problem: split by dots in a smarter way!
    QStringList tmpNames = splitByDotOutsideParenthesis(pythonVariableName);

    for (int i = 0; i < tmpNames.size() && object; ++i)
    {
      // fill the line step by step
      // For example: pythonVariableName = d.foo_class().instantiate_bar().
      // line_code will be filled first by 'd.' and then, line_code = 'd.foo_class().', etc
      line_code.append(tmpNames[i]);
      line_code.append(".");

      QByteArray tmpName = tmpNames.at(i).toLatin1();
      if (tmpName.contains('(') && tmpName.contains(')'))
      {
        tmpNames[i] = tmpNames[i].left(tmpName.indexOf('('));
        tmpName = tmpNames.at(i).toLatin1();

        // Attempt to instantiate the associated python class
        PyObject* classToInstantiate;
        if (PyDict_Check(object))
        {
          classToInstantiate = PyDict_GetItemString(object, tmpName.data());
          Py_XINCREF(classToInstantiate);
        }
        else
        {
          classToInstantiate = PyObject_GetAttrString(object, tmpName.data());
        }

        if (classToInstantiate)
        {
          Py_DECREF(classToInstantiate);

          QString code = " = ";
          code.prepend(instantiated_class_name);
          line_code.remove(line_code.size()-1,1); // remove the last char which is a dot
          code.append(line_code);
          // create a temporary attribute which will instantiate the class
          // For example: code = '_ctkAbstractPythonManager_autocomplete_tmp = d.foo_class()'
          PyRun_SimpleString(code.toLatin1().data());
          line_code.append('.'); // add the point again in case we need to continue to fill line_code
          if (object != main_object)
          {
            Py_DECREF(object);
          }
          object = PyObject_GetAttrString(main_object,instantiated_class_name.toLatin1().data());

          if (object)
          {
            results = ctkAbstractPythonManager::dir_object(object,appendParenthesis);
          }
          else
          {
            // failed to invoke callable, no completions are available
            PyErr_Clear();
            results.clear();
          }
        }
      }
      else
      {
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
        if (prevObj != main_object)
        {
          Py_DECREF(prevObj);
        }

        if (object)
        {
          results = ctkAbstractPythonManager::dir_object(object,appendParenthesis);
        }
        else
        {
          PyErr_Clear();
          // not a valid attribute, no completions are available
          results.clear();
        }
      }
    }
    PyErr_Clear();
  }
  // By default if pythonVariable is empty, return the attributes of the module
  else
  {
    results = ctkAbstractPythonManager::dir_object(object,appendParenthesis);
  }

  if (object && object != main_object)
  {
    Py_DECREF(object);
  }

  // remove the temporary attribute (created to instantiate a class) from the module object
  if (PyObject_HasAttrString(main_object,instantiated_class_name.toLatin1().data()))
  {
    PyObject_DelAttrString(main_object,instantiated_class_name.toLatin1().data());
  }

  Py_DECREF(main_object);

  return results;
}

//-----------------------------------------------------------------------------
PyObject* ctkAbstractPythonManager::pythonObject(const QString& variableNameAndFunction)
{
  PYTHONQT_GIL_SCOPE;
  QStringList variableNameAndFunctionList = variableNameAndFunction.split(".");
  QString compareFunction = variableNameAndFunctionList.last();
  variableNameAndFunctionList.removeLast();
  QString pythonVariableName = variableNameAndFunctionList.last();
  variableNameAndFunctionList.removeLast();
  QString precedingModules = variableNameAndFunctionList.join(".");

  Q_ASSERT(PyThreadState_GET()->interp);
  PyObject* object = ctkAbstractPythonManager::pythonModule(precedingModules);
  if (!object)
  {
    return NULL;
  }
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
  }
  PyObject* finalPythonObject = NULL;
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
        QString keyStr = QString::fromUtf8(PyUnicode_AsUTF8(key));
        if (keyStr == compareFunction)
        {
          finalPythonObject = value;
          break;
        }
        Py_DECREF(value);
      }
      Py_DECREF(keys);
    }
    Py_DECREF(object);
  }
  return finalPythonObject;
}

//-----------------------------------------------------------------------------
PyObject* ctkAbstractPythonManager::pythonModule(const QString& module)
{
  PYTHONQT_GIL_SCOPE;
  PyObject* dict = PyImport_GetModuleDict();
  PyObject* ownedDict = 0;
  PyObject* object = 0;
  PyObject* prevObject = 0;
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
  QStringList moduleList = module.split(".", Qt::KeepEmptyParts);
  #else
  QStringList moduleList = module.split(".", QString::KeepEmptyParts);
  #endif
  if (!dict)
  {
    return object;
  }
  foreach(const QString& module, moduleList)
  {
    if (PyDict_Check(dict))
    {
      object = PyDict_GetItemString(dict, module.toLatin1().data());
      Py_XINCREF(object);
    }
    else if (PyMapping_Check(dict))
    {
      object = PyMapping_GetItemString(dict, module.toLatin1().data());
      if (!object)
      {
        PyErr_Clear();
      }
    }
    else if (prevObject)
    {
      object = PyObject_GetAttrString(prevObject, module.toLatin1().data());
      if (!object)
      {
        PyErr_Clear();
      }
    }
    else
    {
      object = 0;
    }

    if (prevObject)
    {
      Py_DECREF(prevObject);
    }
    if (!object)
    {
      break;
    }
    if (PyObject_HasAttrString(object, "__dict__"))
    {
      PyObject* nextDict = PyObject_GetAttrString(object, "__dict__");
      if (nextDict)
      {
        Py_XDECREF(ownedDict);
        ownedDict = nextDict;
        dict = ownedDict;
      }
    }
    prevObject = object;
  }
  Py_XDECREF(ownedDict);
  return object;
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManager::addObjectToPythonMain(const QString& name, QObject* obj)
{
  if (!this->initialize())
  {
    return;
  }
  PYTHONQT_GIL_SCOPE;
  PythonQtObjectPtr main = PythonQt::self()->getMainModule();
  if (main && obj)
  {
    main.addObject(name, obj);
  }
}

//-----------------------------------------------------------------------------
void ctkAbstractPythonManager::addWrapperFactory(PythonQtForeignWrapperFactory* factory)
{
  if (!PythonQt::self())
  {
    qWarning() << Q_FUNC_INFO << " failed: PythonQt is not initialized";
    return;
  }
  PYTHONQT_GIL_SCOPE;
  PythonQt::self()->addWrapperFactory(factory);
}

//-----------------------------------------------------------------------------
QVariant ctkAbstractPythonManager::getVariable(const QString& name)
{
  if (!this->initialize())
  {
    return QVariant();
  }
  PYTHONQT_GIL_SCOPE;
  PythonQtObjectPtr main = PythonQt::self()->getMainModule();
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

//-----------------------------------------------------------------------------
QString ctkAbstractPythonManager::toPythonStringLiteral(QString path)
{
  path = path.replace("\\", "\\\\");
  path = path.replace("'", "\\'");
  // since we enclose string in single quotes, double-quotes do not require escaping
  return "'" + path + "'";
}
