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
#include "ctkCoreTestingUtilities.h"

using namespace ctkCoreTestingUtilities;

//----------------------------------------------------------------------------
bool TestCheckInt();
bool TestCheckNotNull();
bool TestCheckNull();
bool TestCheckPointer();
bool TestCheckString();
bool TestCheckStringList();
bool TestCheckVariant();

//----------------------------------------------------------------------------
int ctkCoreTestingUtilitiesTest(int , char * [])
{
  bool res = true;
  res = res && TestCheckInt();
  res = res && TestCheckNotNull();
  res = res && TestCheckNull();
  res = res && TestCheckPointer();
  res = res && TestCheckString();
  res = res && TestCheckStringList();
  res = res && TestCheckVariant();
  return res ? EXIT_SUCCESS : EXIT_FAILURE;
}

//----------------------------------------------------------------------------
bool TestCheckInt()
{
  if (!CheckInt(__LINE__, "TestCheckInt", 1, 1)
      || CheckInt(__LINE__, "TestCheckInt Expected Failure", 1, -1))
    {
    qWarning() << "Line " << __LINE__ << " - TestCheckInt failed";
    return false;
    }
  return true;
}

//----------------------------------------------------------------------------
bool TestCheckNotNull()
{
  int foo = 1;
  if (!CheckNotNull(__LINE__, "TestCheckNotNull", &foo)
      || CheckNotNull(__LINE__, "TestCheckNotNull Expected Failure", 0))
    {
    qWarning() << "Line " << __LINE__ << " - TestCheckNotNull failed";
    return false;
    }
  return true;
}

//----------------------------------------------------------------------------
bool TestCheckNull()
{
  int foo = 1;
  if (!CheckNull(__LINE__, "TestCheckNull", 0)
      || CheckNull(__LINE__, "TestCheckNull Expected Failure", &foo))
    {
    qWarning() << "Line " << __LINE__ << " - TestCheckNull failed";
    return false;
    }
  return true;
}

//----------------------------------------------------------------------------
bool TestCheckPointer()
{
  int foo = 1;
  int bar = 1;
  if (!CheckPointer(__LINE__, "TestCheckPointer", &foo, &foo)
      || CheckPointer(__LINE__, "TestCheckPointer Expected Failure", &foo, &bar))
    {
    qWarning() << "Line " << __LINE__ << " - TestCheckPointer failed";
    return false;
    }
  return true;
}

//----------------------------------------------------------------------------
bool TestCheckString()
{
  const char* foo = "foo";
  const char* bar = "bar";
  if (!CheckString(__LINE__, "TestCheckString", 0, 0)
      ||!CheckString(__LINE__, "TestCheckString", foo, foo)
      || CheckString(__LINE__, "TestCheckString Expected Failure", foo, bar)
      || CheckString(__LINE__, "TestCheckString Expected Failure", foo, 0))
    {
    qWarning() << "Line " << __LINE__ << " - TestCheckString failed";
    return false;
    }
  return true;
}

//----------------------------------------------------------------------------
bool TestCheckStringList()
{
  QStringList abc = QStringList() << "a" << "b" << "c";
  QStringList axc = QStringList() << "a" << "x" << "c";
  QStringList abcd = QStringList() << "a" << "b" << "c" << "d";
  if (!CheckStringList(__LINE__, "TestCheckStringList", QStringList(), QStringList())
      ||!CheckStringList(__LINE__, "TestCheckStringList", abc, abc)
      || CheckStringList(__LINE__, "TestCheckStringList Expected Failure", abc, axc)
      || CheckStringList(__LINE__, "TestCheckStringList Expected Failure", abc, abcd))
    {
    qWarning() << "Line " << __LINE__ << " - TestCheckString failed";
    return false;
    }
  return true;
}

//----------------------------------------------------------------------------
bool TestCheckVariant()
{
  QVariant foo = QVariant(4);
  QVariant bar = QVariant(2);
  if (!CheckVariant(__LINE__, "TestCheckVariant", QVariant(), QVariant())
      ||!CheckVariant(__LINE__, "TestCheckVariant", foo, foo)
      || CheckVariant(__LINE__, "TestCheckVariant Expected Failure", foo, bar))
    {
    qWarning() << "Line " << __LINE__ << " - TestCheckVariant failed";
    return false;
    }
  return true;
}

