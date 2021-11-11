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
#include <QAbstractItemModel>
#include <QDateTime>
#include <QDebug>
#include <QModelIndex>
#include <QStringList>

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
CTK_CORE_EXPORT void qListToSTLVector(const QStringList& list, std::vector<char*>& vector);

///
/// \ingroup Core
/// Convert a QStringList to a Vector of string
CTK_CORE_EXPORT void qListToSTLVector(const QStringList& list, std::vector<std::string>& vector);

///
/// \ingroup Core
/// Convert a Vector of string to QStringList
CTK_CORE_EXPORT void stlVectorToQList(const std::vector<std::string>& vector, QStringList& list);

///
/// \ingroup Core
/// \brief Convert a QStringList to a set.
///
/// This method was added so that the same code compiles without deprecation warnings
/// pre and post Qt 5.14.
CTK_CORE_EXPORT QSet<QString> qStringListToQSet(const QStringList& list);

///
/// \ingroup Core
/// \brief Convert a set of strings to a QStringList
///
/// This method was added so that the same code compiles without deprecation warnings
/// pre and post Qt 5.14.
CTK_CORE_EXPORT QStringList qSetToQStringList(const QSet<QString>& set);


///
/// \ingroup Core
/// Convert a nameFilter to a list of file extensions:
/// "Images (*.png *.jpg *.tiff)" -> "*.png", "*.jpg", "*.tiff"
/// Note: the nameFilter can be a simple wildcard "*.jpg" in that case, it
/// will simply return it.
/// \sa nameFiltersToExtensions
CTK_CORE_EXPORT QStringList nameFilterToExtensions(const QString& nameFilter);

///
/// \ingroup Core
/// Convert a nameFilter to a list of file extensions:
/// "Images (*.png *.jpg *.tiff)", "Text (*.txt)" -> "*.png", "*.jpg", "*.tiff", "*.txt"
CTK_CORE_EXPORT QStringList nameFiltersToExtensions(const QStringList& nameFilters);

///
/// \ingroup Core
/// Convert a wildcar extension filter ("*.jpg") into a regular expression string
/// "*.jpg" -> ".*\\.jpg?$"
CTK_CORE_EXPORT QString extensionToRegExp(const QString& extension);

///
/// \ingroup Core
/// Convert a list of wildcar extension filters ("*.jpg")
/// into a regular expression string
/// "*.jpg", "*.txt" -> "(.*\\.jpg?$|.*\\.txt?$)"
CTK_CORE_EXPORT QRegExp nameFiltersToRegExp(const QStringList& nameFilters);

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
CTK_CORE_EXPORT int significantDecimals(double value, int defaultDecimals = -1);

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
CTK_CORE_EXPORT int orderOfMagnitude(double value);

///
/// \ingroup Core
/// Return the order of magnitude of a number.
/// e.g.: closestPowerOfTen(11) returns 10
///       closestPowerOfTen(-40) returns -10
///       closestPowerOfTen(99) returns 100
///       closestPowerOfTen(0.012) returns 0.010
///       closestPowerOfTen(0.)  returns 0
/// See more cases in the test ctkUtilsClosestPowerOfTenTest1
CTK_CORE_EXPORT double closestPowerOfTen(double value);

///
/// \ingroup Core
/// Remove a directory recursively.
/// \param dirName The directory to remove
/// \return <code>true</code> on success, <code>false</code> otherwise.
/// \sa QDir::rmdir
CTK_CORE_EXPORT bool removeDirRecursively(const QString & dirName);


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
CTK_CORE_EXPORT bool copyDirRecursively(const QString &srcPath, const QString &dstPath, bool includeHiddenFiles=true);

///
/// \ingroup Core
/// Convert Qt::HANDLE to string
/// \sa Qt::HANDLE
CTK_CORE_EXPORT QString qtHandleToString(Qt::HANDLE handle);


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
CTK_CORE_EXPORT qint64 msecsTo(const QDateTime& t1, const QDateTime& t2);

/// Get absolute path from an "internal" path. If internal path is already an absolute path
/// then that is returned unchanged. If internal path is relative path then basePath is used
/// as a basis (prepended to internalPath).
CTK_CORE_EXPORT QString absolutePathFromInternal(const QString& internalPath, const QString& basePath);

/// Get "internal" path from an absolute path. internalPath will be a relative path if
/// absolutePath is within the basePath, otherwise interalPath will be the same as absolutePath.
/// This is useful for paths/directories relative to a base folder, to make the data or application relocatable.
/// Absolute path can be retrieved from an internal path using absolutePathFromInternal function.
CTK_CORE_EXPORT QString internalPathFromAbsolute(const QString& absolutePath, const QString& basePath);


/// \ingroup Core
/// \brief Calls QTextStream::flush() on \a stream and returns \a stream.
///
/// This method was added so that the same code compiles without deprecation warnings
/// pre and post Qt 5.14.
///
/// For Qt >= 5.14, it is equivalent to using \a Qt::flush;
CTK_CORE_EXPORT QTextStream &flush(QTextStream &stream);

/// \ingroup Core
/// \brief Writes '\n' to the stream and flushes the stream.
///
/// This method was added so that the same code compiles without deprecation warnings
/// pre and post Qt 5.14.
///
/// For Qt >= 5.14, it is equivalent to \a Qt::endl;
CTK_CORE_EXPORT QTextStream & endl(QTextStream &stream);


///@{
/// \ingroup Core
/// \brief Returns the child of the model index that is stored in the given row and column.
///
/// This method was added so that the same code compiles without deprecation warnings
/// pre and post Qt 5.8.
CTK_CORE_EXPORT QModelIndex modelChildIndex(QAbstractItemModel* item, const QModelIndex &parent, int row, int column);
CTK_CORE_EXPORT QModelIndex modelChildIndex(const QAbstractItemModel* item, const QModelIndex &parent, int row, int column);
///}@
}

#endif
