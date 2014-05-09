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

#include "ctkXnatObject.h"
#include "ctkXnatObjectPrivate.h"

#include "ctkXnatSession.h"
#include "ctkXnatDataModel.h"

#include <QDateTime>
#include <QDebug>
#include <QVariant>


//----------------------------------------------------------------------------
ctkXnatObject::ctkXnatObject(const ctkXnatObject&)
{
  throw ctkRuntimeException("Copy constructor not implemented");
}

//----------------------------------------------------------------------------
ctkXnatObject::ctkXnatObject(ctkXnatObject* parent, const QString& schemaType)
: d_ptr(new ctkXnatObjectPrivate())
{
  this->setParent(parent);
  this->setSchemaType(schemaType);
}

//----------------------------------------------------------------------------
ctkXnatObject::ctkXnatObject(ctkXnatObjectPrivate& dd, ctkXnatObject* parent, const QString& schemaType)
: d_ptr(&dd)
{
  this->setParent(parent);
  this->setSchemaType(schemaType);
}

//----------------------------------------------------------------------------
ctkXnatObject::~ctkXnatObject()
{
  Q_D(ctkXnatObject);
  foreach (ctkXnatObject* child, d->children)
  {
    delete child;
  }
}

//----------------------------------------------------------------------------
QString ctkXnatObject::id() const
{
  return property("ID");
}

//----------------------------------------------------------------------------
void ctkXnatObject::setId(const QString& id)
{
  setProperty("ID", id);
}

//----------------------------------------------------------------------------
QString ctkXnatObject::name() const
{
  return property("name");
}

//----------------------------------------------------------------------------
void ctkXnatObject::setName(const QString& name)
{
  setProperty("name", name);
}

//----------------------------------------------------------------------------
QString ctkXnatObject::description() const
{
  return property("description");
}

//----------------------------------------------------------------------------
void ctkXnatObject::setDescription(const QString& description)
{
  setProperty("description", description);
}

//----------------------------------------------------------------------------
QString ctkXnatObject::childDataType() const
{
  return "Resources";
}

QDateTime ctkXnatObject::lastModifiedTime() const
{
  return this->session()->lastModified(this->resourceUri());
}

void ctkXnatObject::setLastModifiedTime(const QDateTime &lastModifiedTime)
{
  Q_D(ctkXnatObject);
  if (d->lastModifedTime < lastModifiedTime)
  {
    d->lastModifedTime = lastModifiedTime;
  }
}

//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
void ctkXnatObject::setProperty(const QString& name, const QVariant& value)
{
  Q_D(ctkXnatObject);
  d->properties.insert(name, value.toString());
}

//----------------------------------------------------------------------------
const QMap<QString, QString>& ctkXnatObject::properties() const
{
  Q_D(const ctkXnatObject);
  return d->properties;
}

//----------------------------------------------------------------------------
ctkXnatObject* ctkXnatObject::parent() const
{
  Q_D(const ctkXnatObject);
  return d->parent;
}

//----------------------------------------------------------------------------
void ctkXnatObject::setParent(ctkXnatObject* parent)
{
  Q_D(ctkXnatObject);
  if (d->parent != parent)
  {
    if (d->parent)
    {
      d->parent->remove(this);
    }
    if (parent)
    {
      parent->add(this);
    }
  }
}

//----------------------------------------------------------------------------
QList<ctkXnatObject*> ctkXnatObject::children() const
{
  Q_D(const ctkXnatObject);
  return d->children;
}

//----------------------------------------------------------------------------
void ctkXnatObject::add(ctkXnatObject* child)
{
  Q_D(ctkXnatObject);
  if (child->parent() != this)
  {
    child->d_func()->parent = this;
  }
  if (!d->children.contains(child))
  {
    d->children.push_back(child);
  }
  else
  {
    qWarning() << "ctkXnatObject::add(): Child already exists";
  }
}

//----------------------------------------------------------------------------
void ctkXnatObject::remove(ctkXnatObject* child)
{
  Q_D(ctkXnatObject);
  if (!d->children.removeOne(child))
  {
    qWarning() << "ctkXnatObject::remove(): Child does not exist";
  }
}

//----------------------------------------------------------------------------
void ctkXnatObject::reset()
{
  Q_D(ctkXnatObject);
  // d->properties.clear();
  d->children.clear();
  d->fetched = false;
}

//----------------------------------------------------------------------------
bool ctkXnatObject::isFetched() const
{
  Q_D(const ctkXnatObject);
  return d->fetched;
}

//----------------------------------------------------------------------------
QString ctkXnatObject::schemaType() const
{
  return this->property("xsiType");
}

//----------------------------------------------------------------------------
void ctkXnatObject::fetch()
{
  Q_D(ctkXnatObject);
  if (!d->fetched)
  {
    this->fetchImpl();
    d->fetched = true;
  }
}

//----------------------------------------------------------------------------
ctkXnatSession* ctkXnatObject::session() const
{
  const ctkXnatObject* xnatObject = this;
  while (ctkXnatObject* parent = xnatObject->parent())
  {
    xnatObject = parent;
  }
  const ctkXnatDataModel* dataModel = dynamic_cast<const ctkXnatDataModel*>(xnatObject);
  return dataModel ? dataModel->session() : NULL;
}

//----------------------------------------------------------------------------
void ctkXnatObject::setSchemaType(const QString& schemaType)
{
  this->setProperty("xsiType", schemaType);
}

//----------------------------------------------------------------------------
void ctkXnatObject::download(const QString& /*zipFilename*/)
{
}

//----------------------------------------------------------------------------
void ctkXnatObject::upload(const QString& /*zipFilename*/)
{
}

//----------------------------------------------------------------------------
bool ctkXnatObject::exists() const
{
  return this->session()->exists(this);
}

//----------------------------------------------------------------------------
void ctkXnatObject::save()
{
  this->session()->save(this);
}

//----------------------------------------------------------------------------
void ctkXnatObject::erase()
{
  this->session()->remove(this);
  this->parent()->remove(this);
}
