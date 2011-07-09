/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
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

// Qt include
#include <QSharedData>

// CTK DICOM Core
#include "ctkDICOMPersonName.h"

//------------------------------------------------------------------------------
class ctkDICOMPersonNameData : public QSharedData
{
public:
  QString m_LastName;
  QString m_FirstName;
  QString m_MiddleName;
  QString m_NamePrefix;
  QString m_NameSuffix;
};

//------------------------------------------------------------------------------
ctkDICOMPersonName::ctkDICOMPersonName(const QString& lastName,
                             const QString& firstName,
                             const QString& middleName,
                             const QString& namePrefix,
                             const QString& nameSuffix)
  : d(new ctkDICOMPersonNameData)
{
  d->m_LastName = lastName;
  d->m_FirstName = firstName;
  d->m_MiddleName = middleName;
  d->m_NamePrefix = namePrefix;
  d->m_NameSuffix = nameSuffix;
}

//------------------------------------------------------------------------------
ctkDICOMPersonName::ctkDICOMPersonName(const ctkDICOMPersonName& other)
  : d(other.d)
{
}

//------------------------------------------------------------------------------
ctkDICOMPersonName& ctkDICOMPersonName::operator=(const ctkDICOMPersonName& other)
{
  d = other.d;
  return *this;
}

//------------------------------------------------------------------------------
ctkDICOMPersonName::~ctkDICOMPersonName()
{
}

//------------------------------------------------------------------------------
QString ctkDICOMPersonName::formattedName() const
{
  QString result("");

  /* not sortable
  if (!m_NamePrefix.isEmpty())  result += QString("%1 ").arg(m_NamePrefix);
  if (!m_FirstName.isEmpty())  result += QString("%1 " ).arg(m_FirstName);
  if (!m_MiddleName.isEmpty()) result += QString("%1 ").arg(m_MiddleName);
  if (!m_LastName.isEmpty())   result += QString("%1").arg(m_LastName);
  if (!m_NameSuffix.isEmpty()) result += QString(", %1").arg(m_NameSuffix); // this might be unclean if last name is empty
  */

  if (!d->m_LastName.isEmpty())   result += QString("%1").arg(d->m_LastName);
  if (!d->m_FirstName.isEmpty())  result += QString(", %1" ).arg(d->m_FirstName);
  if (!d->m_MiddleName.isEmpty()) result += QString(" %1").arg(d->m_MiddleName);
  if (!d->m_NameSuffix.isEmpty()) result += QString(", %1").arg(d->m_NameSuffix); // this might be unclean if last name is empty

  return result;
}

//------------------------------------------------------------------------------
QString ctkDICOMPersonName::lastName() const
{
  return d->m_LastName;
}

//------------------------------------------------------------------------------
QString ctkDICOMPersonName::firstName() const
{
  return d->m_FirstName;
}

//------------------------------------------------------------------------------
QString ctkDICOMPersonName::middleName() const
{
  return d->m_MiddleName;
}

//------------------------------------------------------------------------------
QString ctkDICOMPersonName::namePrefix() const
{
  return d->m_NamePrefix;
}

//------------------------------------------------------------------------------
QString ctkDICOMPersonName::nameSuffix() const
{
  return d->m_NameSuffix;
}

//------------------------------------------------------------------------------
ctkDICOMPersonName::operator QString() const
{
  return this->formattedName();
}

//------------------------------------------------------------------------------
std::string ctkDICOMPersonName::toStdString() const
{
  // the complicated looking .toLocal8Bit().constData() is on purpose.
  // if we'd use .toStdString(), the string would be converted to ASCII
  // instead of the local 8bit character encoding.
  // changes for correctly looking output of the strings are higher with .toLocal8Bit()
  return this->formattedName().toLocal8Bit().constData();
}
