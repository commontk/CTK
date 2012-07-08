/*=========================================================================

  Library:   CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

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

#include <ctkException.h>

#include <QDebug>

#include <cstring>
#include <iostream>

//-----------------------------------------------------------------------------
void exc_func1()
{
  throw ctkException("My exception");
}

//-----------------------------------------------------------------------------
void exc_func2()
{
  try {
    exc_func1();
  }
  catch (const ctkException& exc)
  {
    throw ctkRuntimeException("runtime test error", exc);
  }
}

//-----------------------------------------------------------------------------
int ctkExceptionTest(int /*argc*/, char* /*argv*/[])
{
  try
  {
    exc_func2();
  }
  catch (const ctkException& exc)
  {
	  qDebug() << exc.printStackTrace();
    if (std::strcmp(exc.name(), "ctkRuntimeException"))
    {
      std::cerr << "Exception name mismatch." << std::endl;
      return EXIT_FAILURE;
    }

    if (std::strcmp(exc.what(), "ctkRuntimeException: runtime test error"))
    {
      std::cerr << "Exception what() mismatch." << std::endl;
      return EXIT_FAILURE;
    }

    if(exc.message() != "runtime test error")
    {
      std::cerr << "Exception message mismatch." << std::endl;
      return EXIT_FAILURE;
    }

    if(std::strcmp(exc.cause()->name(), "ctkException"))
    {
      std::cerr << "Exception cause mismatch." << std::endl;
      return EXIT_FAILURE;
    }

    QList<QString> trace = exc.stackTrace();
    ctkException* clonedExc = exc.clone();
    if (trace != clonedExc->stackTrace())
    {
      std::cerr << "Cloned exception stack trace mismatch." << std::endl;
      return EXIT_FAILURE;
    }

    try
    {
      clonedExc->rethrow();
    }
    catch (const ctkRuntimeException&)
    {
      delete clonedExc;
    }
    catch (const ctkException&)
    {
      std::cerr << "Wrong exception type rethrown" << std::endl;
      delete clonedExc;
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}
