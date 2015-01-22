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

#include "ctkXnatExperiment.h"

#include "ctkXnatSession.h"
#include "ctkXnatObjectPrivate.h"
#include "ctkXnatSubject.h"
#include "ctkXnatScan.h"
#include "ctkXnatReconstruction.h"
#include "ctkXnatAssessor.h"
#include "ctkXnatScanFolder.h"
#include "ctkXnatReconstructionFolder.h"
#include "ctkXnatAssessorFolder.h"
#include "ctkXnatDefaultSchemaTypes.h"

#include <QDebug>

//----------------------------------------------------------------------------
class ctkXnatExperimentPrivate : public ctkXnatObjectPrivate
{
public:

  ctkXnatExperimentPrivate()
  : ctkXnatObjectPrivate()
  {
  }

  void reset()
  {
//    uri.clear();
  }

//  QString uri;
};


//----------------------------------------------------------------------------
ctkXnatExperiment::ctkXnatExperiment(ctkXnatObject* parent, const QString& schemaType)
: ctkXnatObject(*new ctkXnatExperimentPrivate(), parent, schemaType)
{
}

//----------------------------------------------------------------------------
ctkXnatExperiment::~ctkXnatExperiment()
{
}

//----------------------------------------------------------------------------
QString ctkXnatExperiment::resourceUri() const
{
  return QString("%1/experiments/%2").arg(parent()->resourceUri(), this->id());
}

//----------------------------------------------------------------------------
QString ctkXnatExperiment::name() const
{
  return this->label();
}

//----------------------------------------------------------------------------
void ctkXnatExperiment::setName(const QString &name)
{
  this->setLabel(name);
}

//----------------------------------------------------------------------------
QString ctkXnatExperiment::label() const
{
  return this->property(LABEL);
}

//----------------------------------------------------------------------------
void ctkXnatExperiment::setLabel(const QString &label)
{
  this->setProperty(LABEL, label);
}

//----------------------------------------------------------------------------
void ctkXnatExperiment::reset()
{
  ctkXnatObject::reset();
}

//----------------------------------------------------------------------------
void ctkXnatExperiment::fetchImpl()
{
  QString scansUri = this->resourceUri() + "/scans";
  ctkXnatSession* const session = this->session();
  QUuid scansQueryId = session->httpGet(scansUri);

  QList<ctkXnatObject*> scans;
  
  try
  {
    scans = session->httpResults(scansQueryId,
				 ctkXnatDefaultSchemaTypes::XSI_SCAN);
  }
  catch (const ctkException& e)
  {
    qWarning() << QString(e.what());
  }

  if (!scans.isEmpty())
  {
    ctkXnatScanFolder* scanFolder = new ctkXnatScanFolder();
    this->add(scanFolder);
  }

  QString reconstructionsUri = this->resourceUri() + "/reconstructions";
  QUuid reconstructionsQueryId = session->httpGet(reconstructionsUri);

  QList<ctkXnatObject*> reconstructions;
  try
  {
    reconstructions = session->httpResults(reconstructionsQueryId,
					   ctkXnatDefaultSchemaTypes::XSI_RECONSTRUCTION);
  }
  catch (const ctkException& e)
  {
    qWarning() << QString(e.what());
  }
  
  if (!reconstructions.isEmpty())
  {
    ctkXnatReconstructionFolder* reconstructionFolder = new ctkXnatReconstructionFolder();
    this->add(reconstructionFolder);
  }

  QString assessorsUri = this->resourceUri() + "/assessors";
  QUuid assessorsQueryId = session->httpGet(assessorsUri);
  
  QList<ctkXnatObject*> assessors;
  
  try
  {
    assessors = session->httpResults(assessorsQueryId,
				     ctkXnatDefaultSchemaTypes::XSI_ASSESSOR);
  }
  catch (const ctkException& e)
  {
    qWarning() << QString(e.what());
  }

  if (!assessors.isEmpty())
  {
    ctkXnatAssessorFolder* assessorFolder = new ctkXnatAssessorFolder(this);
    this->add(assessorFolder);
  }

  this->fetchResources();
}

//----------------------------------------------------------------------------
void ctkXnatExperiment::downloadImpl(const QString& filename)
{
  qDebug() << "ctkXnatExperiment::downloadImpl(const QString& filename) not yet implemented or not available by REST API";
}
