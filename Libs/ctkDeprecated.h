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

#ifndef __ctkDeprecated_h
#define __ctkDeprecated_h

// CTK API deprecation framework, modeled after Qt's QT_DEPRECATED_SINCE.
//
// Usage in a CTK header:
//
//   #include <ctkDeprecated.h>
//
//   #if CTK_DEPRECATED_SINCE(0, 1)
//     CTK_FOO_EXPORT void oldApi();   // deprecated as of CTK 0.1
//   #endif
//
// Downstream projects can hide deprecated APIs by defining
// CTK_DISABLE_DEPRECATED_BEFORE at compile time, e.g.
//
//   -DCTK_DISABLE_DEPRECATED_BEFORE=0x000100   # hide APIs deprecated since 0.1.x
//
// The CTK build propagates the configured value as a PUBLIC compile
// definition so consumers automatically inherit the project-wide setting
// (see CMake/ctkMacroBuildLib.cmake).

// Encode a (major, minor, patch) triple into a comparable hex value.
#define CTK_VERSION_CHECK(major, minor, patch) ((major << 16) | (minor << 8) | (patch))

// Default: keep all deprecated APIs available.
#ifndef CTK_DISABLE_DEPRECATED_BEFORE
#  define CTK_DISABLE_DEPRECATED_BEFORE 0x000000
#endif

// Evaluates to true when an API deprecated in (major, minor) should still
// be compiled in (i.e. has not been hidden by CTK_DISABLE_DEPRECATED_BEFORE).
#define CTK_DEPRECATED_SINCE(major, minor) \
    (CTK_VERSION_CHECK(major, minor, 0) > CTK_DISABLE_DEPRECATED_BEFORE)

#endif
