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

#ifndef __ctkIconEnginePlugin_h
#define __ctkIconEnginePlugin_h

#include <QtGlobal>

// The MOC is not able to parse "#if QT_VERSION >= 0x..." in Qt 5 and
// "#if QT_VERSION >= QT_VERSION_CHECK(...)" in Qt4. As a workaround,
// we check for the Qt version here (not parsed by MOC) and provide two
// versions of the actual header file according to the used Qt version.
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
# include <ctkIconEnginePlugin_qt5.h>
#else
# include <ctkIconEnginePlugin_qt4.h>
#endif

#endif
