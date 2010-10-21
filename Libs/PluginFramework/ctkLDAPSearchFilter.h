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

#ifndef CTKLDAPSEARCHFILTER_H
#define CTKLDAPSEARCHFILTER_H

#include "ctkPluginFrameworkExport.h"

#include "ctkServiceReference.h"

#include <QSharedDataPointer>
#include <QDebug>

class ctkLDAPSearchFilterData;

class CTK_PLUGINFW_EXPORT ctkLDAPSearchFilter {

public:

  ctkLDAPSearchFilter();
  ctkLDAPSearchFilter(const QString& filter);
  ctkLDAPSearchFilter(const ctkLDAPSearchFilter& other);

  ~ctkLDAPSearchFilter();

  bool match(const ctkServiceReference& reference) const;
  bool match(const ctkDictionary& dictionary) const;
  bool matchCase(const ctkDictionary& dictionary) const;

  QString toString() const;

  bool operator==(const ctkLDAPSearchFilter& other) const;
  ctkLDAPSearchFilter& operator=(const ctkLDAPSearchFilter& filter);

protected:

  QSharedDataPointer<ctkLDAPSearchFilterData> d;

};

CTK_PLUGINFW_EXPORT QDebug operator<<(QDebug dbg, const ctkLDAPSearchFilter& filter);

#endif // CTKLDAPSEARCHFILTER_H
