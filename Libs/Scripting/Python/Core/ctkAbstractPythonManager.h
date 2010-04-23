/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

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
