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

//----------------------------------------------------------------------------
class ctkXnatResourceCatalogXmlParserPrivate
{
public:

  ctkXnatResourceCatalogXmlParserPrivate()
  {
  }

  QList<QVariantMap> result;
};

ctkXnatResourceCatalogXmlParser::ctkXnatResourceCatalogXmlParser()
  : d_ptr (new ctkXnatResourceCatalogXmlParserPrivate())
{
}
ctkXnatResourceCatalogXmlParser::~ctkXnatResourceCatalogXmlParser()
{
  delete d_ptr;
}

bool ctkXnatResourceCatalogXmlParser::startElement(const QString &/*namespaceURI*/, const QString &localName,
                                                   const QString &qName, const QXmlAttributes &atts)
{
  Q_D(ctkXnatResourceCatalogXmlParser);
  if (qName == "cat:entry")
  {
    QString name("");
    QString md5("");
    QVariantMap map;
    for( int i=0; i<atts.count(); i++ )
    {
      // TODO ID would be better
      if( atts.localName(i) == "name")
        name = atts.value(i);

      if( atts.localName( i ) == "digest" )
      {
        md5 = atts.value(i);
      }
    }

    if (name.length() == 0 || md5.length() == 0)
    {
      qWarning()<<"Error parsing XNAT resource catalog xml!";
      return false;
    }
    else
    {
      map[name] = QVariant(md5);
      d->result.append(map);
    }
  }
  return true;
}

const QList<QVariantMap>& ctkXnatResourceCatalogXmlParser::md5Hashes()
{
  Q_D(ctkXnatResourceCatalogXmlParser);
  return d->result;
}
