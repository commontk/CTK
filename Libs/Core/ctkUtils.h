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
#include <QDateTime>
#include <QDebug>

// STD includes
#include <vector>

#include "ctkCoreExport.h"

/// This macro can be used instead of Q_ASSERT to warn developers
/// when some assumption fails. CTK_SOFT_ASSERT behavior differs
/// in two key aspects: (1) it only logs a warning (instead of terminating
/// the application) and (2) the message is always logged (instead of
/// ignoring the check in release builds).
#define CTK_SOFT_ASSERT(condition) do \
  { \
    if (! (condition) ) \
    { \
      qWarning() << "Assertion `" #condition "` failed in " << __FILE__ \
                 << " line " << __LINE__; \
    } \
  } while (false)

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
/// number. 16 is the max that can be returned, -1 for NaN numbers. When the
/// number of decimals is not obvious, it defaults to defaultDecimals if it is
/// different from -1, 16 otherwise.
/// e.g. significantDecimals(120.01) returns 2
///      significantDecimals(123456.1333333) returns 3
///      significantDecimals(123456.26999999999999996) returns 2
///      significantDecimals(123456.12345678901234567, 3) return 3
/// See more cases in the test ctkUtilsSignificantDecimalsTest1
int CTK_CORE_EXPORT significantDecimals(double value, int defaultDecimals = -1);

///
/// \ingroup Core
/// Return the order of magnitude of a number or numeric_limits<int>::min() if
/// the order of magnitude can't be computed (e.g. 0, inf, Nan, denorm)...
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
/// \brief Copy a directory recursively.
///
/// Setting <code>includeHiddenFiles</code> to <code>false</code> allows to skip the copy of hidden files.
///
/// \param srcPath The directory to be copied
/// \param dstPath The directory where the file should be copied
/// \return <code>true</code> on success, <code>false</code> otherwise.
/// \sa QFile::copy
bool CTK_CORE_EXPORT copyDirRecursively(const QString &srcPath, const QString &dstPath, bool includeHiddenFiles=true);

///
/// \ingroup Core
/// Convert Qt::HANDLE to string
/// \sa Qt::HANDLE
QString CTK_CORE_EXPORT qtHandleToString(Qt::HANDLE handle);


///
/// \ingroup Core
/// \brief Compute the milli seconds from one QDateTime to an other.
///
/// This function can be used to correctly compute the amount of milli
/// seconds from <code>t1</code> to <code>t2</code>. The QDateTime objects
/// are converted to Qt::UTC to take daylight saving time into account. This is for
/// back-wards compatibility with Qt 4.6. Since Qt 4.7 there exists
/// a QDateTime::msecsTo() method which should be used instead, after
/// bumping the minimum required Qt version for CTK.
qint64 CTK_CORE_EXPORT msecsTo(const QDateTime& t1, const QDateTime& t2);

/// Get absolute path from an "internal" path. If internal path is already an absolute path
/// then that is returned unchanged. If internal path is relative path then basePath is used
/// as a basis (prepended to internalPath).
QString CTK_CORE_EXPORT absolutePathFromInternal(const QString& internalPath, const QString& basePath);

/// Get "internal" path from an absolute path. internalPath will be a relative path if
/// absolutePath is within the basePath, otherwise interalPath will be the same as absolutePath.
/// This is useful for paths/directories relative to a base folder, to make the data or application relocatable.
/// Absolute path can be retrieved from an internal path using absolutePathFromInternal function.
QString CTK_CORE_EXPORT internalPathFromAbsolute(const QString& absolutePath, const QString& basePath);

}

#endif
