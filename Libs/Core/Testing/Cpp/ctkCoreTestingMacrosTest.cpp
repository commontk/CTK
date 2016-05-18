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

// CTK includes
#include "ctkCoreTestingMacros.h"

using namespace ctkCoreTestingUtilities;

//----------------------------------------------------------------------------
int TestCheckNullSuccess();
int TestCheckNullFailure();

int TestCheckNotNullSuccess();
int TestCheckNotNullFailure();

int TestCheckExitSuccessSuccess();
int TestCheckExitSuccessFailure();

int TestCheckIntSuccess();
int TestCheckIntFailure();

int TestCheckPointerSuccess();
int TestCheckPointerFailure();

int TestCheckPointerDifferentSuccess();
int TestCheckPointerDifferentFailure();

int TestCheckBoolSuccess();
int TestCheckBoolFailure();

int TestCheckStringSuccess();
int TestCheckStringFailure();

int TestCheckStdStringSuccess();
int TestCheckStdStringFailure();

int TestCheckQStringSuccess();
int TestCheckQStringFailure();

int TestCheckStdStringDifferentSuccess();
int TestCheckStdStringDifferentFailure();

int TestCheckQStringDifferentSuccess();
int TestCheckQStringDifferentFailure();

int TestCheckQStringListSuccess();
int TestCheckQStringListFailure();

int TestCheckQVariantSuccess();
int TestCheckQVariantFailure();

//----------------------------------------------------------------------------
#define TestMacro(MACRO_NAME) \
  if (Test##MACRO_NAME##Success() != EXIT_SUCCESS) \
    { \
    return EXIT_FAILURE; \
    } \
  if (Test##MACRO_NAME##Failure() != EXIT_FAILURE) \
    { \
    return EXIT_FAILURE; \
    }

//----------------------------------------------------------------------------
int ctkCoreTestingMacrosTest(int , char * [])
{
  TestMacro(CheckNull)
  TestMacro(CheckNotNull)
  TestMacro(CheckExitSuccess)
  TestMacro(CheckInt)
  TestMacro(CheckPointer)
  TestMacro(CheckPointerDifferent)
  TestMacro(CheckBool)
  TestMacro(CheckString)
  TestMacro(CheckStdString)
  TestMacro(CheckQString)
  TestMacro(CheckStdStringDifferent)
  TestMacro(CheckQStringDifferent)
  TestMacro(CheckQStringList)
  TestMacro(CheckQVariant)
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
// Test CHECK_NULL

//----------------------------------------------------------------------------
int TestCheckNullSuccess()
{
  void * nullPtr = 0;
  CHECK_NULL(nullPtr);
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
int TestCheckNullFailure()
{
  int integer = 42;
  void* notNullPtr = &integer;
  CHECK_NULL(notNullPtr);
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
// Test CHECK_NOT_NULL

//----------------------------------------------------------------------------
int TestCheckNotNullSuccess()
{
  int integer = 42;
  void* notNullPtr = &integer;
  CHECK_NOT_NULL(notNullPtr);
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
int TestCheckNotNullFailure()
{
  void * nullPtr = 0;
  CHECK_NOT_NULL(nullPtr);
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
// Test CHECK_EXIT_SUCCESS

//----------------------------------------------------------------------------
int TestCheckExitSuccessSuccess()
{
  int status = EXIT_SUCCESS;
  CHECK_EXIT_SUCCESS(status);
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
int TestCheckExitSuccessFailure()
{
  int status = EXIT_FAILURE;
  CHECK_EXIT_SUCCESS(status);
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
// Test CHECK_INT

//----------------------------------------------------------------------------
int TestCheckIntSuccess()
{
  CHECK_INT(4, 4);
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
int TestCheckIntFailure()
{
  CHECK_INT(4, 2);
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
// Test CHECK_POINTER

//----------------------------------------------------------------------------
int TestCheckPointerSuccess()
{
  int integer = 42;
  void* actual = &integer;
  void* expected = &integer;
  CHECK_POINTER(actual, expected);
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
int TestCheckPointerFailure()
{
  int integer = 42;
  void* actual = &integer;
  int integer2 = 42;
  void* expected = &integer2;
  CHECK_POINTER(actual, expected);
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
// Test CHECK_POINTER_DIFFERENT

//----------------------------------------------------------------------------
int TestCheckPointerDifferentSuccess()
{
  int integer = 42;
  void* actual = &integer;
  int integer2 = 42;
  void* expected = &integer2;
  CHECK_POINTER_DIFFERENT(actual, expected);
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
int TestCheckPointerDifferentFailure()
{
  int integer = 42;
  void* actual = &integer;
  void* expected = &integer;
  CHECK_POINTER_DIFFERENT(actual, expected);
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
// Test CHECK_BOOL

//----------------------------------------------------------------------------
int TestCheckBoolSuccess()
{
  CHECK_BOOL(true, true);
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
int TestCheckBoolFailure()
{
  CHECK_BOOL(true, false);
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
// Test CHECK_STRING

//----------------------------------------------------------------------------
int TestCheckStringSuccess()
{
  const char* actual = "string";
  const char* expected = "string";
  CHECK_STRING(actual, expected);
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
int TestCheckStringFailure()
{
  const char* actual = "string";
  const char* expected = "string2";
  CHECK_STRING(actual, expected);
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
// Test CHECK_STD_STRING

//----------------------------------------------------------------------------
int TestCheckStdStringSuccess()
{
  std::string actual = "string";
  std::string expected = "string";
  CHECK_STD_STRING(actual, expected);
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
int TestCheckStdStringFailure()
{
  std::string actual = "string";
  std::string expected = "string2";
  CHECK_STD_STRING(actual, expected);
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
// Test CHECK_QSTRING

//----------------------------------------------------------------------------
int TestCheckQStringSuccess()
{
  QString actual = "string";
  QString expected = "string";
  CHECK_QSTRING(actual, expected);
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
int TestCheckQStringFailure()
{
  QString actual = "string";
  QString expected = "string2";
  CHECK_QSTRING(actual, expected);
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
// Test CHECK_STD_STRING_DIFFERENT

//----------------------------------------------------------------------------
int TestCheckStdStringDifferentSuccess()
{
  std::string actual = "string";
  std::string expected = "string2";
  CHECK_STD_STRING_DIFFERENT(actual, expected);
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
int TestCheckStdStringDifferentFailure()
{
  std::string actual = "string";
  std::string expected = "string";
  CHECK_STD_STRING_DIFFERENT(actual, expected);
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
// Test CHECK_QSTRING_DIFFERENT

//----------------------------------------------------------------------------
int TestCheckQStringDifferentSuccess()
{
  QString actual = "string";
  QString expected = "string2";
  CHECK_QSTRING_DIFFERENT(actual, expected);
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
int TestCheckQStringDifferentFailure()
{
  QString actual = "string";
  QString expected = "string";
  CHECK_QSTRING_DIFFERENT(actual, expected);
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
// Test CHECK_QSTRINGLIST

//----------------------------------------------------------------------------
int TestCheckQStringListSuccess()
{
  QStringList actual = QStringList() << "a" << "b" << "c";
  QStringList expected = actual;
  CHECK_QSTRINGLIST(actual, expected);
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
int TestCheckQStringListFailure()
{
  QStringList actual = QStringList() << "a" << "b" << "c";
  QStringList expected = QStringList() << "a" << "x" << "c";
  CHECK_QSTRINGLIST(actual, expected);
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
// Test CHECK_QVARIANT

//----------------------------------------------------------------------------
int TestCheckQVariantSuccess()
{
  QVariant actual = QVariant(4);
  QVariant expected = actual;
  CHECK_QVARIANT(actual, expected);
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
int TestCheckQVariantFailure()
{
  QVariant actual = QVariant(4);
  QVariant expected = QVariant(2);
  CHECK_QVARIANT(actual, expected);
  return EXIT_SUCCESS;
}

