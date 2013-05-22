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

#include "ctkXnatObject.h"

#include <QDebug>

class ctkXnatObjectPrivate
{
public:
  ctkXnatObject* parent;
  int parentIndex;
  QList<QString> childrenNames;
  QList<ctkXnatObject*> children;
};

ctkXnatObject::ctkXnatObject(ctkXnatObject* parent)
: QObject(parent)
, d_ptr(new ctkXnatObjectPrivate())
{
  Q_D(ctkXnatObject);
  d->parent = parent;
  d->parentIndex = -1;
}

ctkXnatObject::~ctkXnatObject()
{
}

void ctkXnatObject::setParent(ctkXnatObject* parent)
{
  Q_D(ctkXnatObject);
  QObject::setParent(parent);
  d->parent = parent;
}

int ctkXnatObject::parentIndex()
{
  Q_D(ctkXnatObject);
  return d->parentIndex;
}

void ctkXnatObject::setParentIndex(int parentIndex)
{
  Q_D(ctkXnatObject);
  d->parentIndex = parentIndex;
}

void ctkXnatObject::fetch(ctkXnatConnection* connection)
{
//  connection->fetch(this);
}

QString ctkXnatObject::getName() const
{
  Q_D(const ctkXnatObject);
  return d->parent ? d->parent->childName(d->parentIndex) : 0;
}

ctkXnatObject* ctkXnatObject::getParent() const
{
  Q_D(const ctkXnatObject);
  return d->parent;
}

const QList<ctkXnatObject*>& ctkXnatObject::getChildren() const
{
  Q_D(const ctkXnatObject);
  return d->children;
}

QString ctkXnatObject::childName(int childIndex) const
{
  Q_D(const ctkXnatObject);
  return d->childrenNames[childIndex];
}

void ctkXnatObject::addChild(const QString& name, ctkXnatObject* child)
{
  Q_D(ctkXnatObject);
  int index = d->childrenNames.indexOf(name);
  if (index == -1)
  {
    index = d->childrenNames.size();
    d->childrenNames.push_back(name);
    d->children.push_back(child);
  }
  else
  {
    d->children[index] = child;
  }
  child->setParent(this);
  child->setParentIndex(index);
}

void ctkXnatObject::removeChild(int childIndex)
{
  Q_D(ctkXnatObject);
  if (d->children[childIndex])
  {
    delete d->children[childIndex];
    d->children[childIndex] = NULL;
  }
}

void ctkXnatObject::download(ctkXnatConnection* connection, const QString& zipFilename)
{
  // do nothing
}

void ctkXnatObject::upload(ctkXnatConnection* connection, const QString& zipFilename)
{
  // do nothing
}

void ctkXnatObject::add(ctkXnatConnection* connection, const QString& name)
{
  // do nothing
}

void ctkXnatObject::remove(ctkXnatConnection* connection)
{
  // do nothing
}

QString ctkXnatObject::getKind() const
{
  return NULL;
}

QString ctkXnatObject::getModifiableChildKind() const
{
  return NULL;
}

QString ctkXnatObject::getModifiableParentName() const
{
  return NULL;
}

bool ctkXnatObject::isFile() const
{
  return false;
}

bool ctkXnatObject::holdsFiles() const
{
  return false;
}

bool ctkXnatObject::receivesFiles() const
{
  return false;
}

bool ctkXnatObject::isModifiable(int childIndex) const
{
  return false;
}

bool ctkXnatObject::isModifiable() const
{
  return false;
}

bool ctkXnatObject::isDeletable() const
{
  return false;
}
