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


#include "ctkCaseInsensitiveString.h"

#include <QHash>  // for qHash(const QString&)

ctkCaseInsensitiveString::ctkCaseInsensitiveString()
{
}

ctkCaseInsensitiveString::ctkCaseInsensitiveString(const char* str)
  : str(str)
{
}

ctkCaseInsensitiveString::ctkCaseInsensitiveString(const QString& str)
  : str(str)
{
}

ctkCaseInsensitiveString::ctkCaseInsensitiveString(const ctkCaseInsensitiveString& str)
  : str(str.str)
{
}

ctkCaseInsensitiveString& ctkCaseInsensitiveString::operator=(const ctkCaseInsensitiveString& str)
{
  this->str = str.str;
  return *this;
}

bool ctkCaseInsensitiveString::operator==(const ctkCaseInsensitiveString& str) const
{
  return this->str.toLower() == str.str.toLower();
}

bool ctkCaseInsensitiveString::operator<(const ctkCaseInsensitiveString& str) const
{
  return this->str.toLower() < str.str.toLower();
}

ctkCaseInsensitiveString::operator QString() const
{
  return this->str;
}

uint qHash(const ctkCaseInsensitiveString& str)
{
  return qHash(QString(str).toLower());
}

QDataStream& operator<<(QDataStream &out, const ctkCaseInsensitiveString& str)
{
  out << QString(str);
  return out;
}

QDataStream& operator>>(QDataStream &in, ctkCaseInsensitiveString& str)
{
  QString inStr;
  in >> inStr;
  str = inStr;
  return in;
}
