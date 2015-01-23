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

#include "ctkXnatAssessorFolder.h"

#include "ctkXnatAssessor.h"
#include "ctkXnatDefaultSchemaTypes.h"
#include "ctkXnatExperiment.h"
#include "ctkXnatObjectPrivate.h"
#include "ctkXnatSession.h"

#include <QDebug>

//----------------------------------------------------------------------------
class ctkXnatAssessorFolderPrivate : public ctkXnatObjectPrivate
{
public:

  ctkXnatAssessorFolderPrivate()
  : ctkXnatObjectPrivate()
  {
  }

  void reset()
  {
  }

};

//----------------------------------------------------------------------------
ctkXnatAssessorFolder::ctkXnatAssessorFolder(ctkXnatObject* parent)
  : ctkXnatObject(*new ctkXnatAssessorFolderPrivate(), parent, QString::null)
{
  
  this->setProperty(ID, "assessors");
  this->setProperty(LABEL, "Assessments");
}

//----------------------------------------------------------------------------
ctkXnatAssessorFolder::~ctkXnatAssessorFolder()
{
}

//----------------------------------------------------------------------------
QString ctkXnatAssessorFolder::resourceUri() const
{
  return QString("%1/%2").arg(parent()->resourceUri(), this->id());
}

//----------------------------------------------------------------------------
void ctkXnatAssessorFolder::reset()
{
  ctkXnatObject::reset();
}

//----------------------------------------------------------------------------
void ctkXnatAssessorFolder::fetchImpl()
{
  QString assessorsUri = this->resourceUri();
  ctkXnatSession* const session = this->session();
  QUuid queryId = session->httpGet(assessorsUri);

  QList<ctkXnatObject*> assessors = session->httpResults(queryId,
                                                     ctkXnatDefaultSchemaTypes::XSI_ASSESSOR);

  foreach (ctkXnatObject* assessor, assessors)
  {
    
    this->add(assessor);
  }
}

//----------------------------------------------------------------------------
void ctkXnatAssessorFolder::downloadImpl(const QString& filename)
{
  qDebug() << "ctkXnatExperiment::downloadImpl(const QString& filename) not yet tested";
  QString query = this->resourceUri() + "/ALL/resources/files";
  ctkXnatSession::UrlParameters parameters;
  parameters["format"] = "zip";
  this->session()->download(filename, query, parameters);
}
