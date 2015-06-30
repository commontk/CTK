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

#include "ctkXnatResourceCatalogXmlParser.h"

#include <QDebug>
#include <QXmlStreamReader>

//----------------------------------------------------------------------------
class ctkXnatResourceCatalogXmlParserPrivate
{
public:

  ctkXnatResourceCatalogXmlParserPrivate()
  {
  }

  QXmlStreamReader xmlReader;
};

ctkXnatResourceCatalogXmlParser::ctkXnatResourceCatalogXmlParser()
  : d_ptr (new ctkXnatResourceCatalogXmlParserPrivate())
{
}
ctkXnatResourceCatalogXmlParser::~ctkXnatResourceCatalogXmlParser()
{
  delete d_ptr;
}

void ctkXnatResourceCatalogXmlParser::setData(const QByteArray &xmlInput)
{
  Q_D(ctkXnatResourceCatalogXmlParser);
  d->xmlReader.addData(xmlInput);
}

void ctkXnatResourceCatalogXmlParser::parseXml(QList<QVariantMap>& result)
{
  Q_D(ctkXnatResourceCatalogXmlParser);

  while (!d->xmlReader.atEnd())
  {
    if (d->xmlReader.name().compare("entry") == 0)
    {
      QVariantMap map;
      QXmlStreamAttributes attributes = d->xmlReader.attributes();

      if( attributes.hasAttribute("name") && attributes.hasAttribute("digest"))
      {
        QString name("");
        name += attributes.value("name");
        QString md5("");
        md5 += attributes.value("digest");
        map[name] = md5;
        result.append(map);
      }
    }
    d->xmlReader.readNext();
  }
  if (d->xmlReader.hasError())
  {
    qWarning()<<"Error parsing XNAT resource catalog xml!";
  }
}
