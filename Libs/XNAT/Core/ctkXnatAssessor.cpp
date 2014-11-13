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

#include "ctkXnatAssessor.h"

#include "ctkXnatSession.h"
#include "ctkXnatObject.h"
#include "ctkXnatObjectPrivate.h"
#include "ctkXnatDefaultSchemaTypes.h"

#include <QDebug>

//----------------------------------------------------------------------------
class ctkXnatAssessorPrivate : public ctkXnatObjectPrivate
{
public:

  ctkXnatAssessorPrivate()
  : ctkXnatObjectPrivate()
  {
  }

  void reset()
  {
    uri.clear();
  }

  QString uri;
};


//----------------------------------------------------------------------------
ctkXnatAssessor::ctkXnatAssessor(ctkXnatObject* parent, const QString& schemaType)
: ctkXnatObject(*new ctkXnatAssessorPrivate(), parent, schemaType)
{
}

//----------------------------------------------------------------------------
ctkXnatAssessor::~ctkXnatAssessor()
{
}

//----------------------------------------------------------------------------
QString ctkXnatAssessor::resourceUri() const
{
  return QString("%1/%2").arg(parent()->resourceUri(), this->id());
}

//----------------------------------------------------------------------------
void ctkXnatAssessor::reset()
{
  ctkXnatObject::reset();
}

//----------------------------------------------------------------------------
void ctkXnatAssessor::fetchImpl()
{
  this->fetchResources();
  this->fetchResources("/out/resources");
}

//----------------------------------------------------------------------------
void ctkXnatAssessor::downloadImpl(const QString& filename)
{
  qDebug() << "ctkXnatAssessor::downloadImpl(const QString& filename) not yet tested";
  QString query = this->resourceUri() + "/files";
  ctkXnatSession::UrlParameters parameters;
  parameters["format"] = "zip";
  this->session()->download(filename, query, parameters);
}
