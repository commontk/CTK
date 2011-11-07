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

#ifndef __ctkUtils_h
#define __ctkUtils_h

// Qt includes
#include <QStringList>

// STD includes
#include <vector>

#include "ctkCoreExport.h"

namespace ctk {
///
/// \ingroup Core
/// Convert a QStringList to Vector of char*
/// Caller will be responsible to delete the content of the vector
void CTK_CORE_EXPORT qListToSTLVector(const QStringList& list, std::vector<char*>& vector);

///
/// \ingroup Core
/// Convert a QStringList to a Vector of string
void CTK_CORE_EXPORT qListToSTLVector(const QStringList& list, std::vector<std::string>& vector);

///
/// \ingroup Core
/// Convert a Vector of string to QStringList
void CTK_CORE_EXPORT stlVectorToQList(const std::vector<std::string>& vector, QStringList& list);

///
/// \ingroup Core
/// Convert a nameFilter to a list of file extensions:
/// "Images (*.png *.jpg *.tiff)" -> "*.png", "*.jpg", "*.tiff"
/// Note: the nameFilter can be a simple wildcard "*.jpg" in that case, it
/// will simply return it.
/// \sa nameFiltersToExtensions
QStringList CTK_CORE_EXPORT nameFilterToExtensions(const QString& nameFilter);

///
/// \ingroup Core
/// Convert a nameFilter to a list of file extensions:
/// "Images (*.png *.jpg *.tiff)", "Text (*.txt)" -> "*.png", "*.jpg", "*.tiff", "*.txt"
QStringList CTK_CORE_EXPORT nameFiltersToExtensions(const QStringList& nameFilters);

///
/// \ingroup Core
/// Convert a wildcar extension filter ("*.jpg") into a regular expression string
/// "*.jpg" -> ".*\\.jpg?$"
QString CTK_CORE_EXPORT extensionToRegExp(const QString& extension);

///
/// \ingroup Core
/// Convert a list of wildcar extension filters ("*.jpg")
/// into a regular expression string
/// "*.jpg", "*.txt" -> "(.*\\.jpg?$|.*\\.txt?$)"
QRegExp CTK_CORE_EXPORT nameFiltersToRegExp(const QStringList& nameFilters);

///
/// \ingroup Core
/// Return a "smart" number of decimals needed to display (in a gui) a floating
/// number.
/// e.g. significantDecimals(120.01) returns 2
///      significantDecimals(123456.1333333) returns 3
///      significantDecimals(123456.26999999999999996) returns 2
/// See more cases in the test ctkUtilsSignificantDecimalsTest1
int CTK_CORE_EXPORT significantDecimals(double value);

///
/// \ingroup Core
/// Return the order of magnitude of a number.
/// e.g.: orderOfMagnitude(1) returns 0
///       orderOfMagnitude(10) returns 1
///       orderOfMagnitude(99) returns 1
///       orderOfMagnitude(101) returns 2
///       orderOfMagnitude(0.1) returns -1
///       orderOfMagnitude(0.15) returns -1
///       orderOfMagnitude(0.) returns NaN
/// See more cases in the test ctkUtilsOrderOfMagnitudeTest1
int CTK_CORE_EXPORT orderOfMagnitude(double value);

///
/// \ingroup Core
/// Return the order of magnitude of a number.
/// e.g.: closestPowerOfTen(11) returns 10
///       closestPowerOfTen(-40) returns -10
///       closestPowerOfTen(99) returns 100
///       closestPowerOfTen(0.012) returns 0.010
///       closestPowerOfTen(0.)  returns 0
/// See more cases in the test ctkUtilsClosestPowerOfTenTest1
double CTK_CORE_EXPORT closestPowerOfTen(double value);

///
/// \ingroup Core
/// Remove a directory recursively.
/// \param dirName The directory to remove
/// \return <code>true</code> on success, <code>false</code> otherwise.
/// \sa QDir::rmdir
bool CTK_CORE_EXPORT removeDirRecursively(const QString & dirName);

///
/// \ingroup Core
/// Convert Qt::HANDLE to string
/// \sa Qt::HANDLE
QString CTK_CORE_EXPORT qtHandleToString(Qt::HANDLE handle);
}

#endif
