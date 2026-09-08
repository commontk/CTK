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

#ifndef __ctkStringLiterals_h
#define __ctkStringLiterals_h

// Qt includes
#include <QLatin1String>

#include <cstddef>

namespace ctk {
inline namespace string_literals {

/// Backport of Qt 6.4's Qt::StringLiterals::operator""_L1.
///
/// Allows defining compile-time string constants as
/// \code
/// constexpr QLatin1String Example = "example"_L1;
/// \endcode
/// avoiding non-POD static QString globals (clazy:non-pod-global-static,
/// static initialization order fiasco). The compiler supplies the string
/// length, so no strlen() call is involved and the constructor is fully
/// constexpr even on Qt 5 / Apple clang, where QLatin1String(const char*)
/// is not.
///
/// Once CTK requires Qt >= 6.4, users of this header can switch to
/// \c Qt::StringLiterals with no call-site changes.
constexpr QLatin1String operator""_L1(const char* str, std::size_t size)
{
  return QLatin1String{str, static_cast<int>(size)};
}

} // namespace string_literals
} // namespace ctk

#endif
