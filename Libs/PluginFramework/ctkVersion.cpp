/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

#include "ctkVersion.h"

#include <stdexcept>

#include <QStringListIterator>
#include <QDebug>


  const QString ctkVersion::SEPARATOR = ".";
  const QRegExp ctkVersion::RegExp = QRegExp("[a-zA-Z0-9_\\-]*");

  const ctkVersion& ctkVersion::emptyVersion()
  {
    static ctkVersion emptyV;
    return emptyV;
  }

  ctkVersion& ctkVersion::operator=(const ctkVersion& v)
  {
    majorVersion = v.majorVersion;
    minorVersion = v.minorVersion;
    microVersion = v.microVersion;
    qualifier = v.qualifier;
    return *this;
  }

  ctkVersion::ctkVersion()
    : majorVersion(0), minorVersion(0), microVersion(0), qualifier("")
  {

  }


  void ctkVersion::validate()
  {
    if (!RegExp.exactMatch(qualifier))
      throw std::invalid_argument(std::string("invalid qualifier: ") + qualifier.toStdString());
  }

  ctkVersion::ctkVersion(unsigned int majorVersion, unsigned int minorVersion, unsigned int microVersion)
    : majorVersion(majorVersion), minorVersion(minorVersion), microVersion(microVersion), qualifier("")
  {

  }

  ctkVersion::ctkVersion(unsigned int majorVersion, unsigned int minorVersion, unsigned int microVersion, const QString& qualifier)
     : majorVersion(majorVersion), minorVersion(minorVersion), microVersion(microVersion), qualifier(qualifier)
  {
    this->validate();
  }

  ctkVersion::ctkVersion(const QString& version)
    : majorVersion(0), minorVersion(0), microVersion(0)
  {
    unsigned int maj = 0;
    unsigned int min = 0;
    unsigned int mic = 0;
    QString qual("");

    QStringList st = version.split(SEPARATOR);

    if (st.empty()) return;

    QStringListIterator i(st);

    bool ok = true;
    maj = i.next().toUInt(&ok);

    if (i.hasNext())
    {
      min = i.next().toUInt(&ok);
      if (i.hasNext())
      {
        mic = i.next().toUInt(&ok);
        if (i.hasNext())
        {
          qual = i.next();
          if (i.hasNext())
          {
             ok = false;
          }
        }
      }
    }

    if (!ok) throw std::invalid_argument("invalid format");

    majorVersion = maj;
    minorVersion = min;
    microVersion = mic;
    qualifier = qual;
    this->validate();
  }

  ctkVersion::ctkVersion(const ctkVersion& version)
  : majorVersion(version.majorVersion), minorVersion(version.minorVersion),
    microVersion(version.microVersion), qualifier(version.qualifier)
  {

  }

  ctkVersion ctkVersion::parseVersion(const QString& version)
  {
    if (version.isEmpty())
    {
      return emptyVersion();
    }

    QString version2 = version.trimmed();
    if (version2.isEmpty())
    {
      return emptyVersion();
    }

    return ctkVersion(version2);
  }

  unsigned int ctkVersion::getMajor() const
  {
    return majorVersion;
  }

  unsigned int ctkVersion::getMinor() const
  {
    return minorVersion;
  }

  unsigned int ctkVersion::getMicro() const
  {
    return microVersion;
  }

  QString ctkVersion::getQualifier() const
  {
    return qualifier;
  }

  QString ctkVersion::toString() const
  {
    QString result;
    result += QString::number(majorVersion) + SEPARATOR + QString::number(minorVersion) + SEPARATOR + QString::number(microVersion);
    if (!qualifier.isEmpty())
    {
      result += SEPARATOR + qualifier;
    }
    return result;
  }

  bool ctkVersion::operator==(const ctkVersion& other) const
  {
    if (&other == this)
    { // quicktest
      return true;
    }

    return (majorVersion == other.majorVersion) && (minorVersion == other.minorVersion) && (microVersion
        == other.microVersion) && qualifier == other.qualifier;
  }

  int ctkVersion::compare(const ctkVersion& other) const
  {
    if (&other == this)
    { // quicktest
      return 0;
    }

    if (majorVersion < other.majorVersion)
    {
      return -1;
    }

    if (majorVersion == other.majorVersion)
    {

      if (minorVersion < other.minorVersion)
      {
        return -1;
      }

      if (minorVersion == other.minorVersion)
      {

        if (microVersion < other.microVersion)
        {
          return -1;
        }

        if (microVersion == other.microVersion)
        {
          return qualifier.compare(other.qualifier);
        }
      }
    }
    return 1;
  }


QDebug operator<<(QDebug dbg, const ctk::ctkVersion& v)
{
  dbg << v.toString();

  return dbg.maybeSpace();
}
