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

#ifndef __ctkXnatAPI_h
#define __ctkXnatAPI_h

#include "qRestAPI.h"

#include <QList>
#include <QJSEngine>
#include <QJSValue>

/**
 * ctkXnatAPI is a simple interface class to communicate with an XNAT
 * server through its REST API.
 */
class ctkXnatAPI : public qRestAPI
{
  Q_OBJECT

  typedef qRestAPI Superclass;

public:
  explicit ctkXnatAPI(QObject* parent = 0);
  virtual ~ctkXnatAPI();

  using Superclass::get;

  virtual QUuid get(const QString& resource,
    const Parameters& parameters = Parameters(),
    const RawHeaders& rawHeaders = RawHeaders());

protected:
  void parseResponse(qRestResult* restResult, const QByteArray& result);

private:
  QList<QVariantMap> parseXmlResponse(qRestResult* restResult, const QByteArray& response);

  QList<QVariantMap> parseJsonResponse(qRestResult* restResult, const QByteArray& response);

  QJSEngine ScriptEngine;

  Q_DISABLE_COPY(ctkXnatAPI)
};

#endif
