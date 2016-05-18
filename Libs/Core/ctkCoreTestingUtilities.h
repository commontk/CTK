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

#ifndef __ctkCoreTestingUtilities_h
#define __ctkCoreTestingUtilities_h

// CTK includes
#include <ctkCoreExport.h>

// Qt includes
#include <QString>
#include <QStringList>
#include <QVariant>

/// This module provides functions to facilitate writing tests.
///
/// Before using this module, first consider the QTestLib
/// unit testing framework available in Qt.
///
/// Example:
///
/// \code{.cpp}
/// int current = 40 + 2;
/// int expected = 43;
/// if (!CheckInt(__LINE__, "40 + 2", current, expected))
///   {
///   return false;
///   }
/// \endcode
///
/// Usually these test methods are used by single-line convenience macros
/// defined in ctkCoreTestingMacros.h.

namespace ctkCoreTestingUtilities
{

CTK_CORE_EXPORT
bool CheckInt(int line, const QString& description,
              int current, int expected);

CTK_CORE_EXPORT
bool CheckNotNull(int line, const QString& description,
                  const void* pointer);

CTK_CORE_EXPORT
bool CheckNull(int line, const QString& description,
               const void* pointer);

CTK_CORE_EXPORT
bool CheckPointer(int line, const QString& description,
                  void* current, void* expected, bool errorIfDifferent = true);

CTK_CORE_EXPORT
bool CheckString(int line, const QString& description,
                 const char* current, const char* expected, bool errorIfDifferent = true );

CTK_CORE_EXPORT
bool CheckStringList(int line, const QString& description,
                     const QStringList& current, const QStringList& expected);

CTK_CORE_EXPORT
bool CheckVariant(int line, const QString& description,
                  const QVariant& current, const QVariant& expected);

} // namespace ctkCoreTestingUtilities

#include "ctkCoreTestingUtilities.tpp"

#endif
