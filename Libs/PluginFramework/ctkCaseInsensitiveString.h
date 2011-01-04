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


#ifndef CTKCASEINSENSITIVESTRING_P_H
#define CTKCASEINSENSITIVESTRING_P_H

#include <QString>

#include <ctkPluginFrameworkExport.h>

class CTK_PLUGINFW_EXPORT ctkCaseInsensitiveString
{

public:

  ctkCaseInsensitiveString();
  ctkCaseInsensitiveString(const char* str);
  ctkCaseInsensitiveString(const QString& str);
  ctkCaseInsensitiveString(const ctkCaseInsensitiveString& str);

  ctkCaseInsensitiveString& operator=(const ctkCaseInsensitiveString& str);
  bool operator==(const ctkCaseInsensitiveString& str) const;
  bool operator<(const ctkCaseInsensitiveString& str) const;

  operator QString() const;

private:

  QString str;
};

uint CTK_PLUGINFW_EXPORT qHash(const ctkCaseInsensitiveString& str);

CTK_PLUGINFW_EXPORT QDataStream& operator<<(QDataStream &out, const ctkCaseInsensitiveString& str);
CTK_PLUGINFW_EXPORT QDataStream& operator>>(QDataStream &in, ctkCaseInsensitiveString& str);

#endif // CTKCASEINSENSITIVESTRING_P_H
