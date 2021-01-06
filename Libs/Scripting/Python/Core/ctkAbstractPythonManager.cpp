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

  PythonQt_init_QtBindings();

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
  PythonQt::self()->setRedirectStdOutCallbackEnabled(value);
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
    // Re-throwing is only needed in Python 2.7
    QStringList code = QStringList()
        << "import sys"
        << QString("sys.path.insert(0, %1)").arg(ctkAbstractPythonManager::toPythonStringLiteral(path))
        << "_updated_globals = globals()"
        << QString("_updated_globals['__file__'] = %1").arg(ctkAbstractPythonManager::toPythonStringLiteral(filename))
#if PY_MAJOR_VERSION >= 3
        << QString("exec(open(%1).read(), _updated_globals)").arg(ctkAbstractPythonManager::toPythonStringLiteral(filename));
#else
        << "_ctk_executeFile_exc_info = None"
        << "try:"
        << QString("    execfile(%1, _updated_globals)").arg(ctkAbstractPythonManager::toPythonStringLiteral(filename))
        << "except Exception as e:"
        << "    _ctk_executeFile_exc_info = sys.exc_info()"
        << "finally:"
        << "    del _updated_globals"
        << QString("    if sys.path[0] == %1: sys.path.pop(0)").arg(ctkAbstractPythonManager::toPythonStringLiteral(path))
        << "    if _ctk_executeFile_exc_info:"
        << "        raise _ctk_executeFile_exc_info[1], None, _ctk_executeFile_exc_info[2]";
#endif
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

//-----------------------------------------------------------------------------
QStringList ctkAbstractPythonManager::dir_object(PyObject* object,
                                                 bool appendParenthesis)
{
  QStringList results;
  if (!object)
    {
    return results;
    }
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
  Q_ASSERT(PyThreadState_GET()->interp);
  PyObject* dict = PyImport_GetModuleDict();

  // Split module by '.' and retrieve the object associated if the last module
  QString precedingModule = module;
  PyObject* object = ctkAbstractPythonManager::pythonModule(precedingModule);
  PyObject* prevObject = 0;
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
  QStringList moduleList = module.split(".", Qt::SkipEmptyParts);
  #else
  QStringList moduleList = module.split(".", QString::SkipEmptyParts);
  #endif

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

  PyObject* main_object = object; // save the module object (usually __main__ or __main__.__builtins__)
  QString instantiated_class_name = "_ctkAbstractPythonManager_autocomplete_tmp";
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
        if (PyDict_Check(dict))
          classToInstantiate = PyDict_GetItemString(dict, tmpName.data());
        else
          classToInstantiate = PyObject_GetAttrString(object, tmpName.data());

        if (classToInstantiate)
          {
          QString code = " = ";
          code.prepend(instantiated_class_name);
          line_code.remove(line_code.size()-1,1); // remove the last char which is a dot
          code.append(line_code);
          // create a temporary attribute which will instantiate the class
          // For example: code = '_ctkAbstractPythonManager_autocomplete_tmp = d.foo_class()'
          PyRun_SimpleString(code.toLatin1().data());
          line_code.append('.'); // add the point again in case we need to continue to fill line_code
          object = PyObject_GetAttrString(main_object,instantiated_class_name.toLatin1().data());

          dict = object;
          results = ctkAbstractPythonManager::dir_object(object,appendParenthesis);
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
          dict = object;
          }
        Py_DECREF(prevObj);

        if (object)
          {
          results = ctkAbstractPythonManager::dir_object(object,appendParenthesis);
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

  if (object)
    {
    Py_DECREF(object);
    }

  // remove the temporary attribute (created to instantiate a class) from the module object
  if (PyObject_HasAttrString(main_object,instantiated_class_name.toLatin1().data()))
    {
    PyObject_DelAttrString(main_object,instantiated_class_name.toLatin1().data());
    }

  return results;
}

//-----------------------------------------------------------------------------
PyObject* ctkAbstractPythonManager::pythonObject(const QString& variableNameAndFunction)
{
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
        QString keyStr = PyString_AsString(key);
        if (keyStr.operator ==(compareFunction.toLatin1()))
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
  PyObject* dict = PyImport_GetModuleDict();
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
    object = PyDict_GetItemString(dict, module.toLatin1().data());
    if (prevObject)
      {
      Py_DECREF(prevObject);
      }
    if (!object)
      {
      break;
      }
    Py_INCREF(object); // This is required, otherwise python destroys object.
    if (PyObject_HasAttrString(object, "__dict__"))
      {
      dict = PyObject_GetAttrString(object, "__dict__");
      }\
    prevObject = object;
    }
  return object;
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
  if (!PythonQt::self())
    {
    qWarning() << Q_FUNC_INFO << " failed: PythonQt is not initialized";
    return;
    }
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

//-----------------------------------------------------------------------------
QString ctkAbstractPythonManager::toPythonStringLiteral(QString path)
{
  path = path.replace("\\", "\\\\");
  path = path.replace("'", "\\'");
  // since we enclose string in single quotes, double-quotes do not require escaping
  return "'" + path + "'";
}
