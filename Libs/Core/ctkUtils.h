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

#ifndef __ctkUtils_h
#define __ctkUtils_h

// Qt includes
#include <QStringList>

// STD includes
#include <vector>

#include "ctkCoreExport.h"

namespace ctk {
///
/// Convert a QStringList to Vector of char*
/// Caller will be responsible to delete the content of the vector
void CTK_CORE_EXPORT qListToSTLVector(const QStringList& list, std::vector<char*>& vector);

///
/// Convert a QStringList to a Vector of string
void CTK_CORE_EXPORT qListToSTLVector(const QStringList& list, std::vector<std::string>& vector);

///
/// Convert a Vector of string to QStringList
void CTK_CORE_EXPORT stlVectorToQList(const std::vector<std::string>& vector, QStringList& list);

}

#endif
