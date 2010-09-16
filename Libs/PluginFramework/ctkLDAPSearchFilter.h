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

#ifndef CTKLDAPSEARCHFILTER_H
#define CTKLDAPSEARCHFILTER_H

#include "CTKPluginFrameworkExport.h"

#include "ctkPluginFramework_global.h"

#include <QMap>
#include <QString>
#include <QVariant>

class ctkLDAPSearchFilterPrivate;

class CTK_PLUGINFW_EXPORT ctkLDAPSearchFilter {

public:

  ctkLDAPSearchFilter(const QString& filter = "");
  ctkLDAPSearchFilter(const ctkLDAPSearchFilter& filter);

  ~ctkLDAPSearchFilter();

  bool match(const ctkDictionary& dictionary) const;
  bool matchCase(const ctkDictionary& dictionary) const;

  bool operator==(const ctkLDAPSearchFilter& other) const;
  ctkLDAPSearchFilter& operator=(const ctkLDAPSearchFilter& filter);

protected:

  ctkLDAPSearchFilterPrivate * d;

};

#endif // CTKLDAPSEARCHFILTER_H
