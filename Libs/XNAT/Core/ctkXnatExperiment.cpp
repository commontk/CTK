/*=============================================================================

  Plugin: org.commontk.xnat

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

#include "ctkXnatConnection.h"

class ctkXnatExperimentPrivate
{
public:
  QString id;
  QString project;
  QString date;
  QString xsiType;
  QString label;
  QString insertDate;
  QString uri;
};

ctkXnatExperiment::ctkXnatExperiment(ctkXnatObject* parent)
: ctkXnatObject(parent)
, d_ptr(new ctkXnatExperimentPrivate())
{
}

ctkXnatExperiment::~ctkXnatExperiment()
{
}

const QString& ctkXnatExperiment::id() const
{
  Q_D(const ctkXnatExperiment);
  return d->id;
}

void ctkXnatExperiment::setId(const QString& id)
{
  Q_D(ctkXnatExperiment);
  d->id = id;
}

const QString& ctkXnatExperiment::project() const
{
  Q_D(const ctkXnatExperiment);
  return d->project;
}

void ctkXnatExperiment::setProject(const QString& project)
{
  Q_D(ctkXnatExperiment);
  d->project = project;
}

const QString& ctkXnatExperiment::date() const
{
  Q_D(const ctkXnatExperiment);
  return d->date;
}

void ctkXnatExperiment::setDate(const QString& date)
{
  Q_D(ctkXnatExperiment);
  d->date = date;
}

const QString& ctkXnatExperiment::xsiType() const
{
  Q_D(const ctkXnatExperiment);
  return d->xsiType;
}

void ctkXnatExperiment::setXsiType(const QString& xsiType)
{
  Q_D(ctkXnatExperiment);
  d->xsiType = xsiType;
}

const QString& ctkXnatExperiment::label() const
{
  Q_D(const ctkXnatExperiment);
  return d->label;
}

void ctkXnatExperiment::setLabel(const QString& label)
{
  Q_D(ctkXnatExperiment);
  d->label = label;
}

const QString& ctkXnatExperiment::insertDate() const
{
  Q_D(const ctkXnatExperiment);
  return d->insertDate;
}

void ctkXnatExperiment::setInsertDate(const QString& insertDate)
{
  Q_D(ctkXnatExperiment);
  d->insertDate = insertDate;
}

const QString& ctkXnatExperiment::uri() const
{
  Q_D(const ctkXnatExperiment);
  return d->uri;
}

void ctkXnatExperiment::setUri(const QString& uri)
{
  Q_D(ctkXnatExperiment);
  d->uri = uri;
}

bool ctkXnatExperiment::holdsFiles() const
{
  return true;
}

void ctkXnatExperiment::fetch(ctkXnatConnection* connection)
{
  connection->fetch(this);
}

bool ctkXnatExperiment::isModifiable(int parentIndex) const
{
  ctkXnatObject* child = getChildren()[parentIndex];
  if (child == 0)
  {
    return false;
  }
  return child->isModifiable();
}

void ctkXnatExperiment::add(ctkXnatConnection* connection, const QString& reconstruction)
{
  connection->addReconstruction(this, reconstruction);
}

QString ctkXnatExperiment::getModifiableChildKind() const
{
  return "reconstruction";
}

QString ctkXnatExperiment::getModifiableParentName() const
{
  return getName();
}

bool ctkXnatExperiment::isModifiable() const
{
  int childNumber = getChildren().size();
  for (int i = 0; i < childNumber; i++)
  {
    if (childName(i) == QString("Reconstruction"))
    {
      return false;
    }
  }

  return true;
}
