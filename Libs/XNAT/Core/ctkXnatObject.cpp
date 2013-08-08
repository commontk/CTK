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
#include "ctkXnatObjectPrivate.h"

#include "ctkXnatServer.h"
#include <QDebug>
#include <QVariant>


ctkXnatObject::ctkXnatObject(ctkXnatObjectPrivate& d)
: d_ptr(&d)
{
}


ctkXnatObject::ctkXnatObject()
: d_ptr(new ctkXnatObjectPrivate())
{
}

ctkXnatObject::~ctkXnatObject()
{
}

QString ctkXnatObject::id() const
{
  return property("ID");
}

void ctkXnatObject::setId(const QString& id)
{
  setProperty("ID", id);
}

QString ctkXnatObject::uri() const
{
  return property("URI");
}

void ctkXnatObject::setUri(const QString& uri)
{
  setProperty("URI", uri);
}

QString ctkXnatObject::name() const
{
  return property("name");
}

QString ctkXnatObject::description() const
{
  return property("description");
}

QString ctkXnatObject::property(const QString& name) const
{
  Q_D(const ctkXnatObject);
  ctkXnatObjectPrivate::PropertyMapConstInterator iter = d->properties.find(name);
  if (iter != d->properties.end())
  {
    return iter.value();
  }
  return QString::null;
}

void ctkXnatObject::setProperty(const QString& name, const QVariant& value)
{
  Q_D(ctkXnatObject);
  d->properties.insert(name, value.toString());
}


QList<QString> ctkXnatObject::properties()
{
  Q_D(ctkXnatObject);
  
  QList<QString> value;

  QMapIterator<QString, QString> it(d->properties);
  while (it.hasNext())
  {
    it.next();
    value.push_back (it.key());
  }

  return value;
}

ctkXnatObject::Pointer ctkXnatObject::parent() const
{
  Q_D(const ctkXnatObject);
  return d->parent;
}

QList<ctkXnatObject::Pointer> ctkXnatObject::children() const
{
  Q_D(const ctkXnatObject);
  return d->children;
}

void ctkXnatObject::addChild(ctkXnatObject::Pointer& child)
{
  Q_D(ctkXnatObject);
  d->children.push_back(child);
  child->d_func()->parent = d->selfPtr;
}

void ctkXnatObject::removeChild(ctkXnatObject::Pointer& child)
{
  Q_D(ctkXnatObject);
  if (!d->children.removeOne(child))
  {
    qWarning() << "ctkXnatObject::removeChild(): Child does not exist";
  }
}

void ctkXnatObject::reset()
{
  Q_D(ctkXnatObject);
  // d->properties.clear();
  d->children.clear();
  d->fetched = false;
}

bool ctkXnatObject::isFetched() const
{
  Q_D(const ctkXnatObject);
  return d->fetched;
}

void ctkXnatObject::fetch()
{
  Q_D(ctkXnatObject);
  if (!d->fetched)
  {
    this->fetchImpl();
    d->fetched = true;
  }
}

ctkXnatConnection* ctkXnatObject::connection() const
{
  const ctkXnatObject* xnatObject = this;
  const ctkXnatServer* server;
  do {
    xnatObject = xnatObject->parent().data();
    server = dynamic_cast<const ctkXnatServer*>(xnatObject);
  }
  while (xnatObject && !server);

  return server ? xnatObject->connection() : 0;
}


bool ctkXnatObject::isFile() const
{
  return false;
}

bool ctkXnatObject::receivesFiles() const
{
  return false;
}

bool ctkXnatObject::holdsFiles() const
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


void ctkXnatObject::download(const QString& /*zipFilename*/)
{
  // do nothing
  // if (!this->isFile())
  //   return;  
}

void ctkXnatObject::upload(const QString& /*zipFilename*/)
{
  // do nothing
  // if (!this->isFile())
  //   return;  
}

void ctkXnatObject::add(const QString& /*name*/)
{
  // do nothing
}

void ctkXnatObject::remove()
{
  // do nothing
}
