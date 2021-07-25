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

// Qt includes
#include <QDebug>
#include <QDir>
#include <QRegExp>
#include <QString>
#include <QStringList>

#include "ctkUtils.h"

// STD includes
#include <algorithm>
#include <limits>

#ifdef _MSC_VER
  #pragma warning(disable: 4996)
#endif

//------------------------------------------------------------------------------
void ctk::qListToSTLVector(const QStringList& list,
                                 std::vector<char*>& vector)
{
  // Resize if required
  if (list.count() != static_cast<int>(vector.size()))
    {
    vector.resize(list.count());
    }
  for (int i = 0; i < list.count(); ++i)
    {
    // Allocate memory
    char* str = new char[list[i].size()+1];
    strcpy(str, list[i].toUtf8());
    vector[i] = str;
    }
}

//------------------------------------------------------------------------------
namespace
{
/// Convert QString to std::string
static std::string qStringToSTLString(const QString& qstring)
{
  return qstring.toStdString();
}
}

//------------------------------------------------------------------------------
void ctk::qListToSTLVector(const QStringList& list,
                                 std::vector<std::string>& vector)
{
  // To avoid unnessesary relocations, let's reserve the required amount of space
  vector.reserve(list.size());
  std::transform(list.begin(),list.end(),std::back_inserter(vector),&qStringToSTLString);
}

//------------------------------------------------------------------------------
void ctk::stlVectorToQList(const std::vector<std::string>& vector,
                                 QStringList& list)
{
  std::transform(vector.begin(),vector.end(),std::back_inserter(list),&QString::fromStdString);
}

//-----------------------------------------------------------------------------
const char *ctkNameFilterRegExp =
  "^(.*)\\(([a-zA-Z0-9_.*? +;#\\-\\[\\]@\\{\\}/!<>\\$%&=^~:\\|]*)\\)$";
const char *ctkValidWildCard =
  "^[\\w\\s\\.\\*\\_\\~\\$\\[\\]]+$";

//-----------------------------------------------------------------------------
QStringList ctk::nameFilterToExtensions(const QString& nameFilter)
{
  QRegExp regexp(QString::fromLatin1(ctkNameFilterRegExp));
  int i = regexp.indexIn(nameFilter);
  if (i < 0)
    {
    QRegExp isWildCard(QString::fromLatin1(ctkValidWildCard));
    if (isWildCard.indexIn(nameFilter) >= 0)
      {
      return QStringList(nameFilter);
      }
    return QStringList();
    }
  QString f = regexp.cap(2);
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
  return f.split(QLatin1Char(' '), Qt::SkipEmptyParts);
  #else
  return f.split(QLatin1Char(' '), QString::SkipEmptyParts);
  #endif
}

//-----------------------------------------------------------------------------
QStringList ctk::nameFiltersToExtensions(const QStringList& nameFilters)
{
  QStringList extensions;
  foreach(const QString& nameFilter, nameFilters)
    {
    extensions << nameFilterToExtensions(nameFilter);
    }
  return extensions;
}

//-----------------------------------------------------------------------------
QString ctk::extensionToRegExp(const QString& extension)
{
  // typically *.jpg
  QRegExp extensionExtractor("\\*\\.(\\w+)");
  int pos = extensionExtractor.indexIn(extension);
  if (pos < 0)
    {
    return QString();
    }
  return ".*\\." + extensionExtractor.cap(1) + "?$";
}

//-----------------------------------------------------------------------------
QRegExp ctk::nameFiltersToRegExp(const QStringList& nameFilters)
{
  QString pattern;
  foreach(const QString& nameFilter, nameFilters)
    {
    foreach(const QString& extension, nameFilterToExtensions(nameFilter))
      {
      QString regExpExtension = extensionToRegExp(extension);
      if (!regExpExtension.isEmpty())
        {
        if (pattern.isEmpty())
          {
          pattern = "(";
          }
        else
          {
          pattern += "|";
          }
        pattern +=regExpExtension;
        }
      }
    }
  if (pattern.isEmpty())
    {
    pattern = ".+";
    }
  else
    {
    pattern += ")";
    }
  return QRegExp(pattern);
}

//-----------------------------------------------------------------------------
int ctk::significantDecimals(double value, int defaultDecimals)
{
  if (value == 0.
      || qAbs(value) == std::numeric_limits<double>::infinity())
    {
    return 0;
    }
  if (value != value) // is NaN
    {
    return -1;
    }
  QString number = QString::number(value, 'f', 16);
  QString fractional = number.section('.', 1, 1);
  Q_ASSERT(fractional.length() == 16);
  QChar previous;
  int previousRepeat=0;
  bool only0s = true;
  bool isUnit = value > -1. && value < 1.;
  for (int i = 0; i < fractional.length(); ++i)
    {
    QChar digit = fractional.at(i);
    if (digit != '0')
      {
      only0s = false;
      }
    // Has the digit been repeated too many times ?
    if (digit == previous && previousRepeat == 2 &&
        !only0s)
      {
      if (digit == '0' || digit == '9')
        {
        return i - previousRepeat;
        }
      return i;
      }
    // Last digit
    if (i == fractional.length() - 1)
      {
      // If we are here, that means that the right number of significant
      // decimals for the number has not been figured out yet.
      if (previousRepeat > 2 && !(only0s && isUnit) )
        {
        return i - previousRepeat;
        }
      // If defaultDecimals has been provided, just use it.
      if (defaultDecimals >= 0)
        {
        return defaultDecimals;
        }
      return fractional.length();
      }
    // get ready for next
    if (previous != digit)
      {
      previous = digit;
      previousRepeat = 1;
      }
    else
      {
      ++previousRepeat;
      }
    }
  Q_ASSERT(false);
  return fractional.length();
}

//-----------------------------------------------------------------------------
int ctk::orderOfMagnitude(double value)
{
  value = qAbs(value);
  if (value == 0.
      || value == std::numeric_limits<double>::infinity()
      || value != value // is NaN
      || value < std::numeric_limits<double>::epsilon() // is tool small to compute
  )
    {
    return std::numeric_limits<int>::min();
    }
  double magnitude = 1.00000000000000001;
  int magnitudeOrder = 0;

  int magnitudeStep = 1;
  double magnitudeFactor = 10;

  if (value < 1.)
    {
    magnitudeOrder = -1;
    magnitudeStep = -1;
    magnitudeFactor = 0.1;
    }

  double epsilon = std::numeric_limits<double>::epsilon();
  while ( (magnitudeStep > 0 && value >= magnitude) ||
          (magnitudeStep < 0 && value < magnitude - epsilon))
    {
    magnitude *= magnitudeFactor;
    magnitudeOrder += magnitudeStep;
    }
  // we went 1 order too far, so decrement it
  return magnitudeOrder - magnitudeStep;
}

//-----------------------------------------------------------------------------
double ctk::closestPowerOfTen(double _value)
{
  const double sign = _value >= 0. ? 1 : -1;
  const double value = qAbs(_value);
  if (value == 0.
      || value == std::numeric_limits<double>::infinity()
      || value != value // is NaN
      || value < std::numeric_limits<double>::epsilon() // is denormalized
  )
    {
    return _value;
    }

  double magnitude = 1.;
  double nextMagnitude = magnitude;

  if (value >= 1.)
    {
    do
      {
      magnitude = nextMagnitude;
      nextMagnitude *= 10.;
      }
    while ( (value - magnitude)  > (nextMagnitude - value) );
    }
  else
    {
    do
      {
      magnitude = nextMagnitude;
      nextMagnitude /= 10.;
      }
    while ( (value - magnitude)  < (nextMagnitude - value) );
    }
  return magnitude * sign;
}

//-----------------------------------------------------------------------------
bool ctk::removeDirRecursively(const QString & dirName)
{
  bool result = false;
  QDir dir(dirName);

  if (dir.exists())
    {
    foreach (QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst))
      {
      if (info.isDir())
        {
        result = ctk::removeDirRecursively(info.absoluteFilePath());
        }
      else
        {
        result = QFile::remove(info.absoluteFilePath());
        }

      if (!result)
        {
        return result;
        }
      }
    QDir parentDir(QFileInfo(dirName).absolutePath());
    result = parentDir.rmdir(dirName);
    }

  return result;
}

//-----------------------------------------------------------------------------
bool ctk::copyDirRecursively(const QString &srcPath, const QString &dstPath, bool includeHiddenFiles)
{
  // See http://stackoverflow.com/questions/2536524/copy-directory-using-qt
  if (!QFile::exists(srcPath))
    {
    qCritical() << "ctk::copyDirRecursively: Failed to copy nonexistent directory" << srcPath;
    return false;
    }

  QDir srcDir(srcPath);
  if (!srcDir.relativeFilePath(dstPath).startsWith(".."))
    {
    qCritical() << "ctk::copyDirRecursively: Cannot copy directory" << srcPath << "into itself" << dstPath;
    return false;
    }


  QDir parentDstDir(QFileInfo(dstPath).path());
  if (!QFile::exists(dstPath) && !parentDstDir.mkdir(QFileInfo(dstPath).fileName()))
    {
    qCritical() << "ctk::copyDirRecursively: Failed to create destination directory" << QFileInfo(dstPath).fileName();
    return false;
    }

  QDir::Filter hiddenFilter;
  if(includeHiddenFiles)
    {
    hiddenFilter = QDir::Hidden;
    }

  foreach(const QFileInfo &info, srcDir.entryInfoList(QDir::Dirs | QDir::Files | hiddenFilter | QDir::NoDotAndDotDot))
    {
    QString srcItemPath = srcPath + "/" + info.fileName();
    QString dstItemPath = dstPath + "/" + info.fileName();
    if (info.isDir())
      {
      if (!ctk::copyDirRecursively(srcItemPath, dstItemPath, includeHiddenFiles))
        {
        qCritical() << "ctk::copyDirRecursively: Failed to copy files from " << srcItemPath << " into " << dstItemPath;
        return false;
        }
      }
    else if (info.isFile())
      {
      if (!QFile::copy(srcItemPath, dstItemPath))
        {
        return false;
        }
      }
    else
      {
      qWarning() << "ctk::copyDirRecursively: Unhandled item" << info.filePath();
      }
    }
  return true;
}

//-----------------------------------------------------------------------------
QString ctk::qtHandleToString(Qt::HANDLE handle)
{
  QString str;
  QTextStream s(&str);
  s << handle;
  return str;
}

//-----------------------------------------------------------------------------
qint64 ctk::msecsTo(const QDateTime& t1, const QDateTime& t2)
{
  QDateTime utcT1 = t1.toUTC();
  QDateTime utcT2 = t2.toUTC();

  return static_cast<qint64>(utcT1.daysTo(utcT2)) * static_cast<qint64>(1000*3600*24)
      + static_cast<qint64>(utcT1.time().msecsTo(utcT2.time()));
}

//------------------------------------------------------------------------------
QString ctk::absolutePathFromInternal(const QString& internalPath, const QString& basePath)
{
  if (internalPath.isEmpty() || basePath.isEmpty())
  {
    return internalPath;
  }
  if (QFileInfo(internalPath).isRelative())
  {
    QDir baseDirectory(basePath);
    return QDir::cleanPath(baseDirectory.filePath(internalPath));
  }
  else
  {
    return internalPath;
  }
}

//------------------------------------------------------------------------------
QString ctk::internalPathFromAbsolute(const QString& absolutePath, const QString& basePath)
{
  if (absolutePath.isEmpty() || basePath.isEmpty())
  {
    return absolutePath;
  }
  // Make it a relative path if it is within the base folder
  if (QFileInfo(absolutePath).isRelative())
  {
    // already relative path, return it as is
    return absolutePath;
  }
  QString baseFolderClean = QDir::cleanPath(QDir::fromNativeSeparators(basePath));
  QString absolutePathClean = QDir::cleanPath(QDir::fromNativeSeparators(absolutePath));
#ifdef Q_OS_WIN32
  Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive;
#else
  Qt::CaseSensitivity sensitivity = Qt::CaseSensitive;
#endif
  if (absolutePathClean.startsWith(baseFolderClean, sensitivity))
  {
    // file is in the base folder, make it a relative path
    // (remove size+1 to remove the leading forward slash)
    return absolutePathClean.remove(0, baseFolderClean.size() + 1);
  }
  else
  {
    return absolutePath;
  }
}
