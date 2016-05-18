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

#ifndef __ctkCoreTestingMacros_h
#define __ctkCoreTestingMacros_h

#include "ctkCoreTestingUtilities.h"

/// Convenience macros for unit tests.
///
/// The macro returns from the current method with EXIT_FAILURE if the check fails.
/// Expressions can be passed as arguments, they are guaranteed to be executed only once.
///
/// Example:
///
/// \code{.cpp}
/// int testedFunction(int a, int b) { return a+b; }
///
/// int MyTest1(int , char * [])
/// {
///
///   int current = 40 + 2;
///   int expected = 42;
///   CHECK_INT(current, expected);
///
///   CHECK_INT(testedFunction(40,2), 42);
///   CHECK_INT(testedFunction(35,5), 40);
///
///   return EXIT_SUCCESS;
/// }
///
/// \endcode

/// Verifies that pointer is NULL
#define CHECK_NULL(pointer) \
  { \
  const void* pointerValue = (pointer); \
  if (!ctkCoreTestingUtilities::CheckNull(__LINE__,#pointer " is not NULL", pointerValue)) \
    { \
    return EXIT_FAILURE; \
    } \
  }

/// Verifies that pointer is not NULL
#define CHECK_NOT_NULL(pointer) \
  { \
  if (!ctkCoreTestingUtilities::CheckNotNull(__LINE__,#pointer " is NULL", (pointer))) \
    { \
    return EXIT_FAILURE; \
    } \
  }

#define CHECK_EXIT_SUCCESS(actual) \
  { \
  if (!ctkCoreTestingUtilities::CheckInt(__LINE__,#actual " != EXIT_SUCCESS", (actual), EXIT_SUCCESS)) \
    { \
    return EXIT_FAILURE; \
    } \
  }

/// Verifies if actual int value is the same as expected
#define CHECK_INT(actual, expected) \
  { \
  if (!ctkCoreTestingUtilities::CheckInt(__LINE__,#actual " != " #expected, (actual), (expected))) \
    { \
    return EXIT_FAILURE; \
    } \
  }

/// Verifies if actual pointer value is the same as expected
#define CHECK_POINTER(actual, expected) \
  { \
  if (!ctkCoreTestingUtilities::CheckPointer(__LINE__,#actual " != " #expected, (actual), (expected))) \
    { \
    return EXIT_FAILURE; \
    } \
  }

/// Verifies if actual pointer value is the same as expected
#define CHECK_POINTER_DIFFERENT(actual, expected) \
  { \
  if (!ctkCoreTestingUtilities::CheckPointer(__LINE__,#actual " == " #expected, (actual), (expected), false)) \
    { \
    return EXIT_FAILURE; \
    } \
  }

/// Verifies if actual bool value is the same as expected
#define CHECK_BOOL(actual, expected) \
  { \
  if (!ctkCoreTestingUtilities::CheckInt(__LINE__,#actual " != " #expected, (actual)?1:0, (expected)?1:0)) \
    { \
    return EXIT_FAILURE; \
    } \
  }

/// Verifies if actual const char* value is the same as expected.
/// It can handle NULL pointer inputs.
#define CHECK_STRING(actual, expected) \
  { \
  if (!ctkCoreTestingUtilities::CheckString(__LINE__,#actual " != " #expected, (actual), (expected))) \
    { \
    return EXIT_FAILURE; \
    } \
  }

/// Verifies if actual std::string value is the same as expected.
/// It is safe to use for comparing std::string values.
/// It cannot handle NULL pointer inputs.
#define CHECK_STD_STRING(actual, expected) \
  { \
  std::string a = (actual); \
  std::string e = (expected); \
  if (!ctkCoreTestingUtilities::CheckString(__LINE__,#actual " != " #expected, a.c_str(), e.c_str())) \
    { \
    return EXIT_FAILURE; \
    } \
  }

/// Verifies if actual QString value is the same as expected.
/// It is safe to use for comparing QString values.
/// It cannot handle NULL pointer inputs.
#define CHECK_QSTRING(actual, expected) \
  { \
  QString a = (actual); \
  QString e = (expected); \
  if (!ctkCoreTestingUtilities::CheckString(__LINE__,#actual " != " #expected, qPrintable(a), qPrintable(e))) \
    { \
    return EXIT_FAILURE; \
    } \
  }

/// Verifies if actual const char* value is not the same as expected.
/// It can handle NULL pointer inputs.
#define CHECK_STRING_DIFFERENT(actual, expected) \
  { \
  if (!ctkCoreTestingUtilities::CheckString(__LINE__,#actual " != " #expected, (actual), (expected), false)) \
    { \
    return EXIT_FAILURE; \
    } \
  }

/// Verifies if actual std::string value is not the same as expected.
/// It is safe to use for comparing std::string values.
/// It cannot handle NULL pointer inputs.
#define CHECK_STD_STRING_DIFFERENT(actual, expected) \
  { \
  std::string a = (actual); \
  std::string e = (expected); \
  if (!ctkCoreTestingUtilities::CheckString(__LINE__,#actual " != " #expected, a.c_str(), e.c_str(), false)) \
    { \
    return EXIT_FAILURE; \
    } \
  }

/// Verifies if actual QString value is not the same as expected.
/// It is safe to use for comparing QString values.
/// It cannot handle NULL pointer inputs.
#define CHECK_QSTRING_DIFFERENT(actual, expected) \
  { \
  QString a = (actual); \
  QString e = (expected); \
  if (!ctkCoreTestingUtilities::CheckString(__LINE__,#actual " != " #expected, qPrintable(a), qPrintable(e), false)) \
    { \
    return EXIT_FAILURE; \
    } \
  }

/// Verifies if actual QStringList is the same as expected.
#define CHECK_QSTRINGLIST(actual, expected) \
  { \
  QStringList a = (actual); \
  QStringList e = (expected); \
  if (!ctkCoreTestingUtilities::CheckStringList(__LINE__,#actual " != " #expected, a, e)) \
    { \
    return EXIT_FAILURE; \
    } \
  }

/// Verifies if actual QVariant is the same as expected.
#define CHECK_QVARIANT(actual, expected) \
  { \
  QVariant a = (actual); \
  QVariant e = (expected); \
  if (!ctkCoreTestingUtilities::CheckVariant(__LINE__,#actual " != " #expected, a, e)) \
    { \
    return EXIT_FAILURE; \
    } \
  }

#endif

