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


#ifndef CTKDICTIONARY_H
#define CTKDICTIONARY_H

#include <ctkPluginFrameworkExport.h>

#include "ctkCaseInsensitiveString.h"
#include "ctkPluginFramework_global.h"

/**
 * \ingroup PluginFramework
 *
 * A QHash based dictionary class with case-insensitive keys. This class
 * uses ctkCaseInsensitiveString as key type and QVariant as values. Due
 * to the conversion capabilities of ctkCaseInsensitiveString, QString objects
 * can be used transparantly to insert or retrieve key-value pairs.
 *
 * <p>
 * Note that the case of the keys will be preserved.
 */
class CTK_PLUGINFW_EXPORT ctkDictionary : public QHash<ctkCaseInsensitiveString, QVariant>
{

public:

  ctkDictionary();
  ctkDictionary(const ctkDictionary& other);

  /**
   * Constructs a ctkDictionary object by using the entries of a
   * ctkProperties object. The keys in <code>properties</code> must
   * not contain case-variants of the same string.
   *
   * @param properties The ctkProperties object from which to copy the key-value pairs.
   * @throws ctkInvalidArgumentException if <code>properties</code> contains case-variants of a key
   */
  ctkDictionary(const ctkProperties& properties);

protected:

  typedef QHash<ctkCaseInsensitiveString, QVariant> Super;
};

#endif // CTKDICTIONARY_H
