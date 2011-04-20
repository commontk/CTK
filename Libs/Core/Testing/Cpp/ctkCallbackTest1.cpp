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

// Qt includes
#include <QCoreApplication>
#include <QTimer>

// CTK includes
#include "ctkCallback.h"

// STD includes
#include <cstdlib>
#include <iostream>

namespace
{
bool Done1;
void doSomething1()
{
  Done1 = true;
}

bool Done2;
void doSomething2()
{
  Done2 = true;
}

} // end of anomymous namespace

//-----------------------------------------------------------------------------
int ctkCallbackTest1(int argc, char * argv [] )
{
  QCoreApplication app(argc, argv);

  Done1 = false;
  Done2 = false;

  ctkCallback callback(doSomething1);
  QTimer::singleShot(0, &callback, SLOT(invoke()));
  
  ctkCallback callback2;
  callback2.setCallback(doSomething2);
  QTimer::singleShot(0, &callback2, SLOT(invoke()));

  QTimer::singleShot(0, &app, SLOT(quit()));

  int status = app.exec();
  if (status == EXIT_FAILURE)
    {
    return EXIT_FAILURE;
    }

  if (!Done1)
    {
    std::cerr << "Line " << __LINE__ << " - Probem with ctkCallback" << std::endl;
    return EXIT_FAILURE;
    }
    
  if (!Done2)
    {
    std::cerr << "Line " << __LINE__ << " - Probem with ctkCallback::setCallback" << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
