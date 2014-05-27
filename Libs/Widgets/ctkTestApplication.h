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
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: QTestApp.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/*-------------------------------------------------------------------------
  Copyright 2008 Sandia Corporation.
  Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
  the U.S. Government retains certain rights in this software.
-------------------------------------------------------------------------*/

#ifndef __ctkTestApplication_h
#define __ctkTestApplication_h

// Qt includes
#include <QApplication>
#include <QVector>
#include <QByteArray>
#include <QTimer>

/// Helper macro allowing to declare a test
#define QCTK_DECLARE_TEST(TEST_NAME)                \
namespace                                           \
{                                                   \
class _TEST_NAME : public ctkTestApplication       \
{                                                   \
public:                                             \
  _TEST_NAME(int _argc, char * _argv []):           \
    ctkTestApplication(_argc, _argv){}             \
  virtual void runTest();                           \
};                                                  \
                                                    \
void _TEST_NAME::runTest()                          \

/// Helper macro allowing to define a test
#define QCTK_RUN_TEST(TEST_NAME)                      \
}                                                     \
                                                      \
int TEST_NAME(int _argc, char * _argv [] )            \
{                                                     \
  _TEST_NAME app(_argc, _argv);                       \
  QTimer::singleShot(0, &app, SLOT(runTestSlot()));   \
  return _TEST_NAME::exec();                          \
}

/// Helper macro allowing to exit the event loop specifying a return code
#define QCTK_EXIT_TEST(_status)    \
  QCoreApplication::exit(_status); \
  return;
  
#include "ctkWidgetsExport.h"
  

/// \ingroup Widgets
class CTK_WIDGETS_EXPORT ctkTestApplication : public QObject
{
  Q_OBJECT
  
public:
  ctkTestApplication(int _argc, char** _argv);
  ~ctkTestApplication();

  /// This function could be overloaded to implement test that required
  /// an active event loop
  virtual void runTest();
  
  /// 
  /// If reportErrorsOnExit is true, then the return value will
  /// be the number of warning messages plus the number of error messages
  /// produced by QDebug during execution.
  static int exec(bool reportErrorsOnExit=false);

#if QT_VERSION >= 0x50000
  static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
#else
  static void messageHandler(QtMsgType type, const char *msg);
#endif

  static void delay(int ms);

  static bool simulateEvent(QWidget* w, QEvent* e);

  static void keyUp(QWidget* w, Qt::Key key, Qt::KeyboardModifiers mod, int ms);

  static void keyDown(QWidget* w, Qt::Key key, Qt::KeyboardModifiers mod, int ms);

  static void keyClick(QWidget* w, Qt::Key key, Qt::KeyboardModifiers mod, int ms);

  static void mouseDown(QWidget* w, QPoint pos, Qt::MouseButton btn, 
                        Qt::KeyboardModifiers mod, int ms);
  
  static void mouseUp(QWidget* w, QPoint pos, Qt::MouseButton btn, 
                      Qt::KeyboardModifiers mod, int ms);
  
  static void mouseMove(QWidget* w, QPoint pos, Qt::MouseButton btn, 
                        Qt::KeyboardModifiers mod, int ms);

  static void mouseClick(QWidget* w, QPoint pos, Qt::MouseButton btn, 
                         Qt::KeyboardModifiers mod, int ms);

  static void mouseDClick(QWidget* w, QPoint pos, Qt::MouseButton btn, 
                          Qt::KeyboardModifiers mod, int ms);

public Q_SLOTS:

  /// Slot responsible to invoke the virtual function 'runTest'.
  /// The typical use case consists in calling that slot using a singleShot QTimer
  void runTestSlot(); 

private:
  QApplication*     App;
  static int        Error;
  QList<QByteArray> Argv;
  QVector<char*>    Argvp;
  int               Argc;
};

#endif
