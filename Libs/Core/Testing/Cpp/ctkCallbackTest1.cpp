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
#include <QCoreApplication>
#include <QTimer>
#include <QVariant>

// CTK includes
#include "ctkCallback.h"

// STD includes
#include <cstdlib>
#include <iostream>

namespace
{
bool Done1;
ctkCallback * Callback1;
void doSomething1(void* data)
{
  Done1 = (Callback1 == data);
}

bool Done2;
ctkCallback * Callback2;
void doSomething2(void* data)
{
  ctkCallback * callback = reinterpret_cast<ctkCallback*>(data);
  Done2 = (Callback2 == data && callback->property("foo").toInt() == 7);
}

bool Done3;
void* CallbackData3;
void doSomething3(void* data)
{
  Done3 = (CallbackData3 == data);
}

} // end of anomymous namespace

//-----------------------------------------------------------------------------
int ctkCallbackTest1(int argc, char * argv [] )
{
  QCoreApplication app(argc, argv);
  

  Done1 = false;
  Done2 = false;
  Done3 = false;

  //-----------------------------------------------------------------------------
  ctkCallback callback;
  if (callback.callbackData() != &callback)
    {
    std::cerr << "Line " << __LINE__ << " - Problem vith ctkCallback constructor" << std::endl;
    return EXIT_FAILURE;
    }
  QTimer::singleShot(0, &callback, SLOT(invoke()));
  
  //-----------------------------------------------------------------------------
  ctkCallback callback1;
  Callback1 = &callback1;
  if (callback1.callback() != 0)
    {
    std::cerr << "Line " << __LINE__ << " - Problem vith ctkCallback constructor"
              << " - ctkCallback::callback() should return 0" << std::endl;
    return EXIT_FAILURE;
    }
    
  callback1.setCallback(doSomething1);
  if (callback1.callback() != doSomething1)
    {
    std::cerr << "Line " << __LINE__ << " - Problem vith ctkCallback::setCallback()" << std::endl;
    return EXIT_FAILURE;
    }
  
  QTimer::singleShot(0, &callback1, SLOT(invoke()));
  
  //-----------------------------------------------------------------------------
  ctkCallback callback2(doSomething2);
  callback2.setProperty("foo", QVariant(7));
  Callback2 = &callback2;
  if (callback2.callback() != doSomething2)
    {
    std::cerr << "Line " << __LINE__ << " - Problem vith ctkCallback constructor" << std::endl;
    return EXIT_FAILURE;
    }
  
  QTimer::singleShot(0, &callback2, SLOT(invoke()));

  //-----------------------------------------------------------------------------
  QObject dummyData;
  CallbackData3 = &dummyData;
  ctkCallback callback3(doSomething3);
  callback3.setCallbackData(&dummyData);
  QTimer::singleShot(0, &callback3, SLOT(invoke()));
  
  
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

  if (!Done3)
    {
    std::cerr << "Line " << __LINE__ << " - Probem with ctkCallback::setCallback" << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
