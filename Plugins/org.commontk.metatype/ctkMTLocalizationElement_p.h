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


#ifndef CTKMTLOCALIZATIONELEMENT_P_H
#define CTKMTLOCALIZATIONELEMENT_P_H

#include <QLocale>

#include <ctkPluginLocalization.h>

class ctkMTLocalizationElement
{

  const QString _localization;
  const QString _context;
  ctkPluginLocalization _pl;

public:

  ctkMTLocalizationElement(const QString& localization, const QString& context);

  /**
   * Internal method
   */
  void setPluginLocalization(const ctkPluginLocalization& pl);

  /**
   * Method to get the localized text of inputed String.
   */
  QString getLocalized(const QString& key) const;

  QString getLocalizationBase() const;
};

#endif // CTKMTLOCALIZATIONELEMENT_P_H
