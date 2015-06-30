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

#ifndef CTKXNATRESOURCECATALOGXMLPARSER_H
#define CTKXNATRESOURCECATALOGXMLPARSER_H

#include "ctkXNATCoreExport.h"

#include <QList>
#include <QVariantMap>

class ctkXnatResourceCatalogXmlParserPrivate;

/**
 * @brief Parses the xml response when requesting the catalog xml of a resource
 *
 * When sending the following request to XNAT:
 * <xnat-url>/projects/myProject/resources/<resource-id>
 * you get the catalog xml of the resource folder as response
 *
 * This class parses this response and returns the MD5 hashes of all the resource files,
 * which are contained by the resource folder.
 *
 * @ingroup XNAT_Core
 */
class CTK_XNAT_CORE_EXPORT ctkXnatResourceCatalogXmlParser
{

public:
  ctkXnatResourceCatalogXmlParser();
  ~ctkXnatResourceCatalogXmlParser();

  /**
   * @brief Set the xml input for the parser
   * @param xmlInput as QByteArray
   */
  void setData(const QByteArray& xmlInput);

  /**
   * @brief Parses the xml input and extracts the md5 hashes of the resource catalog
   * @param result the QList in which the md5 hashes will be stored
   */
  void parseXml(QList<QVariantMap> &result);
private:

  ctkXnatResourceCatalogXmlParser(const ctkXnatResourceCatalogXmlParser& );
  ctkXnatResourceCatalogXmlParser& operator=(const ctkXnatResourceCatalogXmlParser& );

  /// The private implementation part of the object.
  ctkXnatResourceCatalogXmlParserPrivate* d_ptr;

  Q_DECLARE_PRIVATE(ctkXnatResourceCatalogXmlParser)
};

#endif //CTKXNATRESOURCECATALOGXMLPARSER_H
