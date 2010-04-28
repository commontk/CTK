/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

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
#include "CTKScriptingPythonCoreExport.h"

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

  ///
  /// Execute a python of python code (can be multiple lines separated with newline)
  /// and return the result as a QVariant.
  QVariant executeString(const QString& code);

  ///
  /// Gets the value of the variable looking in the __main__ module.
  /// If the variable is not found returns a default initialized QVariant.
  QVariant getVariable(const QString& varName);

  ///
  /// Execute a python script with the given filename.
  void executeFile(const QString& filename);

signals:

  ///
  /// This signal is emitted after python is initialized.  Observers can listen
  /// for this signal to handle additional initialization steps.
  void pythonInitialized();

protected slots:
  void printStderr(const QString&);
  void printStdout(const QString&);

protected:

  void initPythonQt();

  virtual QStringList     pythonPaths();
  virtual void            preInitialization();

};
#endif
