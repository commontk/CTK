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

#include "ctkXnatDataModel.h"
#include "ctkXnatDefaultSchemaTypes.h"
#include "ctkXnatException.h"
#include "ctkXnatResource.h"
#include "ctkXnatResourceFolder.h"
#include "ctkXnatSession.h"

#include <QDateTime>
#include <QDebug>
#include <QStringList>
#include <QVariant>


const QString ctkXnatObject::ID = "ID";
const QString ctkXnatObject::NAME = "name";
const QString ctkXnatObject::LABEL = "label";

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
  return this->property(ID);
}

//----------------------------------------------------------------------------
void ctkXnatObject::setId(const QString& id)
{
  this->setProperty(ID, id);
}

//----------------------------------------------------------------------------
QString ctkXnatObject::name() const
{
  return this->property(NAME);
}

//----------------------------------------------------------------------------
void ctkXnatObject::setName(const QString& name)
{
  this->setProperty(NAME, name);
}

//----------------------------------------------------------------------------
QString ctkXnatObject::description() const
{
  Q_D(const ctkXnatObject);
  return d->description;
}

//----------------------------------------------------------------------------
void ctkXnatObject::setDescription(const QString& description)
{
  Q_D(ctkXnatObject);
  d->description = description;
}

//----------------------------------------------------------------------------
QString ctkXnatObject::childDataType() const
{
  return "Resources";
}

QDateTime ctkXnatObject::lastModifiedTimeOnServer()
{
  Q_D(ctkXnatObject);
  QUuid queryId = this->session()->httpHead(this->resourceUri());
  QMap<QByteArray, QByteArray> header = this->session()->httpHeadSync(queryId);
  QVariant lastModifiedHeader = header.value("Last-Modified");
  QDateTime lastModifiedTime;

  if (lastModifiedHeader.isValid())
  {
    QStringList dateformates;
    // In case http date formate RFC 822 ( "Sun, 06 Nov 1994 08:49:37 GMT" )
    dateformates<<"ddd, dd MMM yyyy HH:mm:ss";
    // In case http date formate ANSI ( "Sun Nov  6 08:49:37 1994" )
    dateformates<<"ddd MMM  d HH:mm:ss yyyy";
    // In case http date formate RFC 850 ( "Sunday, 06-Nov-94 08:49:37 GMT" )
    dateformates<<"dddd, dd-MMM-yy HH:mm:ss";

    QString dateText = lastModifiedHeader.toString();
    // Remove "GMT" addition at the end of the http timestamp
    if (dateText.indexOf("GMT") != -1)
    {
      dateText = dateText.left(dateText.length()-4);
    }

    foreach (QString format, dateformates)
    {
      lastModifiedTime = QDateTime::fromString(dateText, format);
      if (lastModifiedTime.isValid())
        break;
    }
  }
  return lastModifiedTime;
}

void ctkXnatObject::setLastModifiedTime(const QDateTime &lastModifiedTime)
{
  Q_D(ctkXnatObject);
  if (d->lastModifiedTime < lastModifiedTime)
  {
    d->lastModifiedTime = lastModifiedTime;
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
  if (d->properties[name] != value)
  {
    d->properties.insert(name, value.toString());
  }
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

  bool childExists (false);
  foreach (ctkXnatObject* existingChild, d->children)
  {
    if ((existingChild->id().length() != 0 && existingChild->id() == child->id()) ||
        (existingChild->id().length() == 0 && existingChild->name() == child->name()))
    {
      d->children.replace(d->children.indexOf(existingChild), child);
      childExists = true;
    }
  }
  if (!childExists)
  {
    d->children.push_back(child);
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
void ctkXnatObject::setSchemaType(const QString& schemaType)
{
  this->setProperty("xsiType", schemaType);
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
void ctkXnatObject::download(const QString& filename)
{
  this->downloadImpl(filename);
}

//----------------------------------------------------------------------------
void ctkXnatObject::save(bool overwrite)
{
  Q_D(ctkXnatObject);
  this->saveImpl(overwrite);
}

//----------------------------------------------------------------------------
ctkXnatResource* ctkXnatObject::addResourceFolder(QString foldername, QString format,
                                   QString content, QString tags)
{
  if (foldername.size() == 0)
  {
    throw ctkXnatException("Error creating resource! Foldername must not be empty!");
  }

  ctkXnatResourceFolder* resFolder = 0;
  QList<ctkXnatObject*> children = this->children();
  for (unsigned int i = 0; i < children.size(); ++i)
  {
    resFolder = dynamic_cast<ctkXnatResourceFolder*>(children.at(i));
    if (resFolder)
    {
      break;
    }
  }

  if (!resFolder)
  {
    resFolder = new ctkXnatResourceFolder();
    this->add(resFolder);
  }

  ctkXnatResource* resource = new ctkXnatResource();
  resource->setName(foldername);
  if (format.size() != 0)
    resource->setFormat(format);
  if (content.size() != 0)
    resource->setContent(content);
  if (tags.size() != 0)
    resource->setTags(tags);

  resFolder->add(resource);
  resource->save();

  return resource;
}

//----------------------------------------------------------------------------
bool ctkXnatObject::exists() const
{
  return this->session()->exists(this);
}

//----------------------------------------------------------------------------
void ctkXnatObject::saveImpl(bool /*overwrite*/)
{
  Q_D(ctkXnatObject);
  ctkXnatSession::UrlParameters urlParams;
  urlParams["xsi:type"] = this->schemaType();

  // Just do this if there is already a valid last-modification-time,
  // otherwise the object is not yet on the server!
  QDateTime remoteModTime;
  if (d->lastModifiedTime.isValid())
  {
    // TODO Overwrite this for e.g. project and subject which already support modification time!
    remoteModTime = this->lastModifiedTimeOnServer();
    // If the object has been modified on the server, perform an update
    if (d->lastModifiedTime < remoteModTime)
    {
      qWarning()<<"Uploaded object maybe overwritten on server!";
      // TODO update from server, since modification time is not really supported
      // by xnat right now this is not of high priority
      // something like this->updateImpl + setLastModifiedTime()
    }
  }

  const QMap<QString, QString>& properties = this->properties();
  QMapIterator<QString, QString> itProperties(properties);
  while (itProperties.hasNext())
  {
    itProperties.next();
    if (itProperties.key() == "ID")
      continue;

    urlParams[itProperties.key()] = itProperties.value();
  }

  // Execute the update
  QUuid queryID = this->session()->httpPut(this->resourceUri(), urlParams);
  const QList<QVariantMap> results = this->session()->httpSync(queryID);

  // If this xnat object did not exist before on the server set the ID returned by Xnat
  if (results.size() == 1 && results[0].size() == 1)
  {
    QVariant id = results[0][ID];
    if (!id.isNull())
    {
      this->setId(id.toString());
    }
  }

  // Finally update the modification time on the server
  remoteModTime = this->lastModifiedTimeOnServer();
  d->lastModifiedTime = remoteModTime;
}

//----------------------------------------------------------------------------
void ctkXnatObject::fetchResources(const QString& path)
{
  ctkXnatResourceFolder* resFolder = new ctkXnatResourceFolder();
  this->add(resFolder);
}

//----------------------------------------------------------------------------
void ctkXnatObject::erase()
{
  this->session()->remove(this);
  this->parent()->remove(this);
}
