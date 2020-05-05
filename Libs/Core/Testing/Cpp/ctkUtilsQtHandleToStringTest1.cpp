/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QDebug>
#include <QTextStream>

// CTK includes
#include "ctkUtils.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkUtilsQtHandleToStringTest1(int argc, char * argv [] )
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);

  //#if defined(Q_WS_MAC)
  //  typedef void * HANDLE;
  //#elif defined(Q_WS_WIN)
  //  typedef void *HANDLE;
  //#elif defined(Q_WS_X11)
  //  typedef unsigned long HANDLE;
  //#elif defined(Q_WS_QWS)
  //  typedef void * HANDLE;
  //#elif defined(Q_OS_SYMBIAN)
  // typedef unsigned long int HANDLE; // equivalent to TUint32
  //#endif

  {
  unsigned long handle = 1234;
  QString str;
  QTextStream s(&str);
  s << handle;
  //std::cout << "handle:" << qPrintable(str) << std::endl;
  }

  {
  int foo = 0;
  void * handle = &foo;
  QString str;
  QTextStream s(&str);
  s << handle;
  //std::cout << "handle:" << qPrintable(str) << std::endl;
  }

  return EXIT_SUCCESS;
}
