/*=============================================================================

 Library: XNAT/Core

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

#ifndef CTKXNATCONSTANTS_H
#define CTKXNATCONSTANTS_H

#include "ctkXNATCoreExport.h"

#include <QString>

/**
 * @ingroup XNAT_Core
 */

struct CTK_XNAT_CORE_EXPORT ctkXnatObjectFields
{
  static QString ID;
  static QString NAME;
  static QString LABEL;

  // XnatProject specific
  static QString SECONDARY_ID;
  static QString DESCRIPTION;
  static QString PI_FIRSTNAME;
  static QString PI_LASTNAME;

  // XnatSubject specific
  static QString INSERT_DATE;
  static QString INSERT_USER;

  // XnatScan specific
  static QString TYPE;
  static QString QUALITY;
  static QString SERIES_DESCRIPTION;

  // XnatResource specific
  static QString ABSTRACT_RESOURCE_ID;

  // XnatFile specific
  static QString FILE_TAGS;
  static QString FILE_FORMAT;
  static QString FILE_CONTENT;
};
#endif // CTKXNATCONSTANTS_H
