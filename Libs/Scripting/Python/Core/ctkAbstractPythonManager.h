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

#ifndef __ctkAbstractPythonManager_h
#define __ctkAbstractPythonManager_h

// Qt includes
#include <QObject>
#include <QList>
#include <QStringList>
#include <QVariant>

// PythonQt includes
#include <PythonQtPythonInclude.h> // For PyObject

// CTK includes
#include "ctkScriptingPythonCoreExport.h"

class ctkAbstractPythonManagerPrivate;
class PythonQtForeignWrapperFactory;
class PythonQtObjectPtr;

/// \ingroup Scripting_Python_Core
class CTK_SCRIPTING_PYTHON_CORE_EXPORT ctkAbstractPythonManager : public QObject
{
  Q_OBJECT
  Q_PROPERTY(bool systemExitExceptionHandlerEnabled READ systemExitExceptionHandlerEnabled WRITE setSystemExitExceptionHandlerEnabled)
  Q_PROPERTY(bool redirectStdOutCallbackEnabled READ redirectStdOutCallbackEnabled WRITE setRedirectStdOutCallbackEnabled)

public:
  typedef QObject Superclass;
  ctkAbstractPythonManager(QObject* _parent=NULL);
  virtual ~ctkAbstractPythonManager();

  /// Calling this function after mainContext() has been called at least once is a no-op.
  /// If not overridden calling this function, the default initialization flags are
  /// PythonQt::IgnoreSiteModule and PythonQt::RedirectStdOut.
  /// \sa PythonQt::InitFlags
  void setInitializationFlags(int flags);

  /// \sa setInitializationFlags
  int initializationFlags()const;

  /// Initialize python context considering the initializationFlags.
  /// Return \a True if python has been successfully initialized.
  /// \sa setInitializationFlags, mainContext, isPythonInitialized
  /// \sa preInitialization, executeInitializationScripts, pythonPreInitialized, pythonInitialized
  bool initialize();

  /// Return a reference to the python main context.
  /// Calling this function implicitly call initialize() if it hasn't been done.
  PythonQtObjectPtr mainContext();

  void addObjectToPythonMain(const QString& name, QObject* obj);
  void addWrapperFactory(PythonQtForeignWrapperFactory* factory);
  void registerPythonQtDecorator(QObject* decorator);
  void registerClassForPythonQt(const QMetaObject* metaobject);
  void registerCPPClassForPythonQt(const char* name);

  /// \sa PythonQt::systemExitExceptionHandlerEnabled
  bool systemExitExceptionHandlerEnabled()const;

  /// \sa PythonQt::setSystemExitExceptionHandlerEnabled
  void setSystemExitExceptionHandlerEnabled(bool value);

  /// \sa PythonQt::redirectStdOutCallbackEnabled
  bool redirectStdOutCallbackEnabled()const;

  /// \sa PythonQt::setRedirectStdOutCallbackEnabled
  void setRedirectStdOutCallbackEnabled(bool value);

  /// This enum maps to Py_eval_input, Py_file_input and Py_single_input
  /// \see http://docs.python.org/c-api/veryhigh.html#Py_eval_input
  /// \see http://docs.python.org/c-api/veryhigh.html#Py_file_input
  /// \see http://docs.python.org/c-api/veryhigh.html#Py_single_input
  enum ExecuteStringMode
    {
    EvalInput = 0,
    FileInput,
    SingleInput
    };

  /// Execute a python of python code (can be multiple lines separated with newline)
  /// and return the result as a QVariant.
  Q_INVOKABLE QVariant executeString(const QString& code, ExecuteStringMode mode = FileInput);

  /// Gets the value of the variable looking in the __main__ module.
  /// If the variable is not found returns a default initialized QVariant.
  QVariant getVariable(const QString& varName);

  /// Execute a python script with the given filename.
  Q_INVOKABLE void executeFile(const QString& filename);

  /// Set function that is initialized after preInitialization and before executeInitializationScripts
  /// \sa preInitialization executeInitializationScripts
  void setInitializationFunction(void (*initFunction)());

  /// Given a python object, lookup its attributes and return them in a string list.
  /// If the argument \c appendParenthesis is set to True, "()" will be appended to attributes
  /// being Python callable.
  static QStringList dir_object(PyObject* object,
                                bool appendParenthesis = false);

  /// Given a python variable name, it returns the string list splited
  /// at every dots which will be outside parenthesis
  /// (It also takes care about the possibility that quotes can include parenthesis)
  static QStringList splitByDotOutsideParenthesis(const QString& pythonVariableName);

  /// Given a python variable name, if it can be called, try to call the method or instantiate the class,
  /// lookup its attributes and return them in a string list.
  /// By default the attributes are looked up from \c __main__.
  /// If the argument \c appendParenthesis is set to True, "()" will be appended to attributes
  /// being Python callable.
  QStringList pythonAttributes(const QString& pythonVariableName,
                               const QString& module = QLatin1String("__main__"),
                               bool appendParenthesis = false) const;

  /// Given a string of the form "<modulename1>[.<modulenameN>...]" containing modules, return the final module as a PyObject*
  static PyObject* pythonModule(const QString &module);

  /// Given a string of the form "<modulename1>[.<modulenameN>...].correspondingObject, return the final object as a PyObject*
  /// \sa pythonModule
  static PyObject* pythonObject(const QString& variableNameAndFunction);

  /// Returns True if python is initialized
  /// \sa pythonInitialized
  bool isPythonInitialized()const;

  /// Returns True if a python error occured.
  /// \sa PythonQt::hadError()
  bool pythonErrorOccured()const;

  /// Reset error flag
  /// \sa PythonQt::clearError()
  void resetErrorFlag();

  /// Convert a string to a safe python string literal.
  /// Backslash, single-quote characters are escaped
  /// and the string is enclosed between single quotes.
  ///
  /// Examples:
  ///   some simple string   => 'some simple string'
  ///   some " string        => 'some " string'
  ///   some other ' string  => 'some other \' string'
  ///   some backslash \ str => 'some backslash \\ str'
  Q_INVOKABLE static QString toPythonStringLiteral(QString path);

Q_SIGNALS:

  /// This signal is emitted after python is pre-initialized. Observers can listen
  /// for this signal to handle additional initialization steps.
  /// \sa preInitialization
  void pythonPreInitialized();

  /// This signal is emitted after python is initialized and scripts are executed
  /// \sa preInitialization
  /// \sa executeScripts
  void pythonInitialized();

  //! emitted when both custom SystemExit exception handler is enabled and a SystemExit
  //! exception is raised.
  //! \sa setSystemExitExceptionHandlerEnabled(bool), PythonQt::systemExitExceptionRaised(int)
  void systemExitExceptionRaised(int exitCode);

protected Q_SLOTS:
  void printStderr(const QString&);
  void printStdout(const QString&);

protected:

  void initPythonQt(int flags);

  virtual QStringList     pythonPaths();

  /// Overload this function to load Decorator and pythonQt wrapper at initialization time
  virtual void            preInitialization();

  /// Overload this function to execute script at initialization time
  virtual void            executeInitializationScripts();

protected:
  QScopedPointer<ctkAbstractPythonManagerPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkAbstractPythonManager);
  Q_DISABLE_COPY(ctkAbstractPythonManager);

};
#endif
