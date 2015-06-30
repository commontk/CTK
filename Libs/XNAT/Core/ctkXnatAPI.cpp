/*=============================================================================

  Library: XNAT/Core

  Copyright (c) University College London,
    Centre for Medical Image Computing

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

// ctkXnatAPI includes
#include "ctkXnatAPI_p.h"

#include "ctkXnatResourceCatalogXmlParser.h"

#include "qRestResult.h"

#include <QNetworkReply>
#include <QRegExp>
#include <QUrl>
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#include <QUrlQuery>
#endif

// --------------------------------------------------------------------------
// ctkXnatAPI methods

// --------------------------------------------------------------------------
ctkXnatAPI::ctkXnatAPI(QObject* _parent)
  : Superclass(_parent)
{
}

// --------------------------------------------------------------------------
ctkXnatAPI::~ctkXnatAPI()
{
}

// --------------------------------------------------------------------------
QUuid ctkXnatAPI::get(const QString& resource, const Parameters& parameters, const qRestAPI::RawHeaders& rawHeaders)
{
  QUrl url = this->createUrl(resource, parameters);
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
  url.addQueryItem("format", "json");
#else
  QUrlQuery urlQuery(url);
  urlQuery.addQueryItem("format", "json");
  url.setQuery(urlQuery);
#endif
  QNetworkReply* queryReply = this->sendRequest(QNetworkAccessManager::GetOperation, url, rawHeaders);
  QUuid queryId = queryReply->property("uuid").toString();
  return queryId;
}

// --------------------------------------------------------------------------
void ctkXnatAPI::parseResponse(qRestResult* restResult, const QByteArray& response)
{
  static QRegExp identifierPattern("[a-zA-Z][a-zA-Z0-9_]*");

  QList<QVariantMap> result;

  if (response.isEmpty())
    {
    // Some operations do not return result. E.g. creating a project.
    }
  else if (response.startsWith("<html>"))
    {
    // Some operations return an XML description of an object.
    // E.g. GET query for a specific subject.
    restResult->setError(QString("Bad data: ") + response, qRestAPI::ResponseParseError);
    }
  else if (response.startsWith("<?xml "))
    {
    // Some operations return an XML description of an object.
    // E.g. GET query for a specific subject.
    result = this->parseXmlResponse(restResult, response);
    }
  else if (response[0] == '{')
    {
    // Other operations return a json description of an object.
    // E.g. GET query of the list of subjects
    result = this->parseJsonResponse(restResult, response);
    }
  else if (identifierPattern.exactMatch(response))
    {
    // Some operations return the identifier of the newly created object.
    // E.g. creating a subject.
    QVariantMap map;
    map["ID"] = response;
    map["content"] = response;
    result.push_back(map);
    }
  else
    {
    QVariantMap map;
    map["content"] = response;
    result.push_back(map);
    }

  restResult->setResult(result);
}

// --------------------------------------------------------------------------
QList<QVariantMap> ctkXnatAPI::parseXmlResponse(qRestResult* /*restResult*/, const QByteArray& response)
{
  QList<QVariantMap> result;
  // In this case a resource catalog xml was requested
  if (response.contains("<cat:Catalog"))
  {
    ctkXnatResourceCatalogXmlParser parser;
    parser.setData(response);
    parser.parseXml(result);
  }
  return result;
}

// --------------------------------------------------------------------------
QList<QVariantMap> ctkXnatAPI::parseJsonResponse(qRestResult* restResult, const QByteArray& response)
{
  QScriptValue scriptValue = this->ScriptEngine.evaluate("(" + QString(response) + ")");

  QList<QVariantMap> result;

  // e.g. {"ResultSet":{"Result": [{"p1":"v1","p2":"v2",...}], "totalRecords":"13"}}
  QScriptValue resultSet = scriptValue.property("ResultSet");
  QScriptValue data = resultSet.property("Result");
  if (!data.isObject())
    {
    if (!data.toString().isEmpty())
      {
      restResult->setError(QString("Bad data: ") + data.toString(), qRestAPI::ResponseParseError);
      }
    }
  if (data.isArray())
    {
    quint32 length = data.property("length").toUInt32();
    for(quint32 i = 0; i < length; ++i)
      {
      qRestAPI::appendScriptValueToVariantMapList(result, data.property(i));
      }
    }
  else
    {
    qRestAPI::appendScriptValueToVariantMapList(result, data);
    }

  return result;
}
