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
    strcpy(str, list[i].toLatin1());
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
  return f.split(QLatin1Char(' '), QString::SkipEmptyParts);
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
int ctk::significantDecimals(double value)
{
  QString number = QString::number(value, 'f', 16);
  QString fractional = number.section('.', 1, 1);
  Q_ASSERT(fractional.length() == 16);
  QChar previous;
  int previousRepeat=0;
  bool only0s = true;
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
      if (previousRepeat > 2)
        {
        return i - previousRepeat;
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
  if (value == 0.)
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

  while ( (magnitudeStep > 0 && value >= magnitude) ||
          (magnitudeStep < 0 && value < magnitude - std::numeric_limits<double>::epsilon()))
    {
    magnitude *= magnitudeFactor;
    magnitudeOrder += magnitudeStep;
    }
  // we went 1 order too far, so decrement it
  return magnitudeOrder - magnitudeStep;
}

//-----------------------------------------------------------------------------
double ctk::closestPowerOfTen(double value)
{
  double sign = value >= 0. ? 1 : -1;
  value = qAbs(value);
  if (value == 0.)
    {
    return 0.;
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

  if (dir.exists(dirName))
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
    result = dir.rmdir(dirName);
    }

  return result;
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
