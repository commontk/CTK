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


#include "ctkMTLocalizationElement_p.h"

ctkMTLocalizationElement::ctkMTLocalizationElement(const QString& localization,
                                                   const QString& context)
  : _localization(localization), _context(context)
{

}

void ctkMTLocalizationElement::setPluginLocalization(const ctkPluginLocalization& pl)
{
  _pl = pl;
}

QString ctkMTLocalizationElement::getLocalized(const QString& key) const
{
  if (key.isEmpty())
  {
    return key;
  }

  QString localized = _pl.getLocalized(_context, key);
  if (localized.isEmpty())
  {
    // If no localization file available or no localized value found
    // for the key, then return the raw data
    return key;
  }

  return localized;
}

QString ctkMTLocalizationElement::getLocalizationBase() const
{
  return _localization;
}
