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

#ifndef __ctkAbstractPythonManager_h
#define __ctkAbstractPythonManager_h

// Qt includes
#include <QObject>
#include <QList>
#include <QStringList>

// CTK includes
#include "ctkScriptingPythonCoreExport.h"

class PythonQtObjectPtr;

class CTK_SCRIPTING_PYTHON_CORE_EXPORT ctkAbstractPythonManager : public QObject
{
  Q_OBJECT

public:
  typedef QObject Superclass;
  ctkAbstractPythonManager(QObject* _parent=NULL);
  ~ctkAbstractPythonManager();

  PythonQtObjectPtr mainContext();
  void addObjectToPythonMain(const QString& name, QObject* obj);
  void registerPythonQtDecorator(QObject* decorator);
  void registerClassForPythonQt(const QMetaObject* metaobject);
  void registerCPPClassForPythonQt(const char* name);

  /// Execute a python of python code (can be multiple lines separated with newline)
  /// and return the result as a QVariant.
  QVariant executeString(const QString& code);

  /// Gets the value of the variable looking in the __main__ module.
  /// If the variable is not found returns a default initialized QVariant.
  QVariant getVariable(const QString& varName);

  /// Execute a python script with the given filename.
  void executeFile(const QString& filename);

  /// Set function that is initialized after preInitialization and before executeInitializationScripts
  /// \sa preInitialization executeInitializationScripts
  void setInitializationFunction(void (*initFunction)());

signals:

  /// This signal is emitted after python is pre-initialized. Observers can listen
  /// for this signal to handle additional initialization steps.
  /// \sa preInitialization
  void pythonPreInitialized();

  /// This signal is emitted after python is initialized and scripts are executed
  /// \sa preInitialization
  /// \sa executeScripts
  void pythonInitialized();

protected slots:
  void printStderr(const QString&);
  void printStdout(const QString&);

protected:

  void initPythonQt();

  virtual QStringList     pythonPaths();

  /// Overload this function to load Decorator and pythonQt wrapper at initialization time
  virtual void            preInitialization();

  /// Overload this function to execute script at initialization time
  virtual void            executeInitializationScripts();

private:
  void (*InitFunction)();

};
#endif
