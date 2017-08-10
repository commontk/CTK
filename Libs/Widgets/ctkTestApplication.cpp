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
  Module:    $RCSfile: ctkTestApplication.cxx,v $

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

// Qt includes
#include <QTimer>
#include <QWidget>
#include <QKeyEvent>
#include <QMouseEvent>

// CTK includes
#include "ctkTestApplication.h"

// STD includes
#include <iostream>

int ctkTestApplication::Error = 0;

//-----------------------------------------------------------------------------
ctkTestApplication::ctkTestApplication(int _argc, char** _argv)
{
#if QT_VERSION > QT_VERSION_CHECK(5,0,0)
  qInstallMessageHandler(ctkTestApplication::messageHandler);
#else
  qInstallMsgHandler(ctkTestApplication::messageHandler);
#endif
  // CMake generated driver removes argv[0],
  // so let's put a dummy back in
  this->Argv.append("ctkTestApplication");
  for(int i=0; i<_argc; i++)
    {
    this->Argv.append(_argv[i]);
    }
  for(int j=0; j<this->Argv.size(); j++)
    {
    this->Argvp.append(this->Argv[j].data());
    }
  this->Argc = this->Argvp.size();
  this->App = new QApplication(this->Argc, this->Argvp.data());
}

//-----------------------------------------------------------------------------
ctkTestApplication::~ctkTestApplication()
{
  delete this->App;
#if QT_VERSION > QT_VERSION_CHECK(5,0,0)
  qInstallMessageHandler(0);
#else
  qInstallMsgHandler(0);
#endif
}

//-----------------------------------------------------------------------------
void ctkTestApplication::runTestSlot()
{
  this->runTest();
}

//-----------------------------------------------------------------------------
void ctkTestApplication::runTest()
{
}

//-----------------------------------------------------------------------------
int ctkTestApplication::exec(bool reportErrorsOnExit)
{
  if(QCoreApplication::arguments().contains("--exit"))
    {
    QTimer::singleShot(100, QApplication::instance(),
                       SLOT(quit()));
    }

  int ret = QApplication::exec();
  if (reportErrorsOnExit)
    {
    return Error + ret;
    }
  else
    {
    return ret;
    }
}

//-----------------------------------------------------------------------------
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
void ctkTestApplication::messageHandler(QtMsgType type, const QMessageLogContext& /*context*/, const QString& msg)
{
#else
void ctkTestApplication::messageHandler(QtMsgType type, const char *msgAsStr)
{
  QString msg(msgAsStr);
#endif
  switch(type)
  {
  case QtDebugMsg:
    std::cerr << "Debug: " << qPrintable(msg) << std::endl;
    break;
#if QT_VERSION >= QT_VERSION_CHECK(5,5,0)
  case QtInfoMsg:
    std::cerr << "Info: " << qPrintable(msg) << std::endl;
    break;
#endif
  case QtWarningMsg:
    std::cerr << "Warning: " << qPrintable(msg) << std::endl;
    Error++;
    break;
  case QtCriticalMsg:
    std::cerr << "Critical: " << qPrintable(msg) << std::endl;
    Error++;
    break;
  case QtFatalMsg:
    std::cerr << "Fatal: " << qPrintable(msg) << std::endl;
    abort();
  }
}

//-----------------------------------------------------------------------------
void ctkTestApplication::delay(int ms)
{
  if(ms > 0)
  {
    QTimer::singleShot(ms, QApplication::instance(), SLOT(quit()));
    QApplication::exec();
  }
}

//-----------------------------------------------------------------------------
bool ctkTestApplication::simulateEvent(QWidget* w, QEvent* e)
{
  bool status = QApplication::sendEvent(w, e);
  QApplication::processEvents();
  return status;
}

//-----------------------------------------------------------------------------
void ctkTestApplication::keyUp(QWidget* w, Qt::Key key, Qt::KeyboardModifiers mod, int ms)
{
  if(!w)
    return;
  delay(ms);
  QString text;
  char off = 'a';
  if(mod & Qt::ShiftModifier)
    off = 'A';
  if(key >= Qt::Key_A && key <= Qt::Key_Z)
    {
    text.append(QChar::fromLatin1(key - Qt::Key_A + off));
    }
  QKeyEvent e(QEvent::KeyRelease, key, mod, text);
  if(!simulateEvent(w, &e))
    {
    qWarning("keyUp not handled\n");
    }
}

//-----------------------------------------------------------------------------
void ctkTestApplication::keyDown(QWidget* w, Qt::Key key, Qt::KeyboardModifiers mod, int ms)
{
  if(!w)
    return;
  delay(ms);
  QString text;
  char off = 'a';
  if(mod & Qt::ShiftModifier)
    off = 'A';
  if(key >= Qt::Key_A && key <= Qt::Key_Z)
    {
    text.append(QChar::fromLatin1(key - Qt::Key_A + off));
    }
  QKeyEvent e(QEvent::KeyPress, key, mod, text);
  if(!simulateEvent(w, &e))
    {
    qWarning("keyDown not handled\n");
    }
}

//-----------------------------------------------------------------------------
void ctkTestApplication::keyClick(QWidget* w, Qt::Key key, Qt::KeyboardModifiers mod, int ms)
{
  delay(ms);
  keyDown(w, key, mod, 0);
  keyUp(w, key, mod, 0);
}

//-----------------------------------------------------------------------------
void ctkTestApplication::mouseDown(QWidget* w, QPoint pos, Qt::MouseButton btn,
                        Qt::KeyboardModifiers mod, int ms)
{
  delay(ms);
  QMouseEvent e(QEvent::MouseButtonPress, pos, btn, btn, mod);
  if(!simulateEvent(w, &e))
    {
    qWarning("mouseDown not handled\n");
    }
}

//-----------------------------------------------------------------------------
void ctkTestApplication::mouseUp(QWidget* w, QPoint pos, Qt::MouseButton btn,
                      Qt::KeyboardModifiers mod, int ms)
{
  delay(ms);
  QMouseEvent e(QEvent::MouseButtonRelease, pos, btn, btn, mod);
  if(!simulateEvent(w, &e))
    {
    qWarning("mouseUp not handled\n");
    }
}

//-----------------------------------------------------------------------------
void ctkTestApplication::mouseMove(QWidget* w, QPoint pos, Qt::MouseButton btn,
                        Qt::KeyboardModifiers mod, int ms)
{
  delay(ms);
  QMouseEvent e(QEvent::MouseMove, pos, btn, btn, mod);
  if(!simulateEvent(w, &e))
    {
    qWarning("mouseMove not handled\n");
    }
}

//-----------------------------------------------------------------------------
void ctkTestApplication::mouseClick(QWidget* w, QPoint pos, Qt::MouseButton btn,
                         Qt::KeyboardModifiers mod, int ms)
{
  delay(ms);
  mouseDown(w, pos, btn, mod, 0);
  mouseUp(w, pos, btn, mod, 0);
}

//-----------------------------------------------------------------------------
void ctkTestApplication::mouseDClick(QWidget* w, QPoint pos, Qt::MouseButton btn,
                         Qt::KeyboardModifiers mod, int ms)
{
  delay(ms);
  QMouseEvent e(QEvent::MouseButtonDblClick, pos, btn, btn, mod);
  if(!simulateEvent(w, &e))
    {
    qWarning("mouseMove not handled\n");
    }
}
