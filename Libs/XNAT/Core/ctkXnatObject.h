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

#ifndef ctkXnatObject_h
#define ctkXnatObject_h

#include "ctkXNATCoreExport.h"

#include <QList>
#include <QObject>
#include <QString>
#include <QMetaType>

class ctkXnatConnection;
class ctkXnatObjectPrivate;

//----------------------------------------------------------------------------
/// \ingroup XNATCore
/// ctkXnatObject is the base class of the objects that represent the nodes in
/// the XNAT data hierarchy.
class CTK_XNAT_CORE_EXPORT ctkXnatObject
{

public:

  /// Destructs the ctkXnatObject.
  virtual ~ctkXnatObject();

  /// Gets the XML Schema type of the object.
  QString schemaType() const;

  /// Gets the ID of the object.
  QString id() const;

  /// Sets the ID of the object.
  void setId(const QString& id);

  /// Gets the resource URI of the object that can be used to access it through
  /// the REST API.
  virtual QString resourceUri() const = 0;

  /// Gets the name of the object.
  QString name() const;

  /// Sets the name of the object.
  void setName(const QString& name);

  /// Gets the description of the object.
  QString description() const;

  /// Sets the description of the object.
  void setDescription(const QString& description);

  /// Gets the value of the property with the given name.
  QString property(const QString& name) const;

  /// Sets the value of the property with the given name.
  void setProperty(const QString& name, const QVariant& value);

  /// Gets the list of the properties of the object.
  QList<QString> properties();
  
  /// Gets the parent of the object in the data hierarchy. The returned pointer
  /// is 0 for the ctkXnatServer objects and different for any other type of
  /// XNAT objects.
  ctkXnatObject* parent() const;

  /// Sets the parent of the object in the data hierarchy.
  void setParent(ctkXnatObject* parent);

  /// Gets the children of the object.
  QList<ctkXnatObject*> children() const;

  /// Adds an object to the children of the current one.
  void add(ctkXnatObject* child);

  /// Removes the object from the children of the current object.
  void remove(ctkXnatObject* child);

  /// Tells if the children and the properties of the objects have been fetched.
  bool isFetched() const;

  /// Resets the object so that its properties and children needs to be fetched
  /// again at the next request.
  virtual void reset();

  /// Fetches the children and the properties of the object.
  void fetch();

  virtual void download(const QString&);
  virtual void upload(const QString&);

protected:

  /// Constructs the ctkXnatObject.
  ctkXnatObject(ctkXnatObject* parent = 0, const QString& schemaType = QString());

  /// Constructs the ctkXnatObject with the given private part.
  ctkXnatObject(ctkXnatObjectPrivate& dd, ctkXnatObject* parent = 0, const QString& schemaType = QString());

  /// Gets the object that represents the connection to the XNAT server
  /// that stores the current object.
  virtual ctkXnatConnection* connection() const;

  /// The private implementation part of the object.
  const QScopedPointer<ctkXnatObjectPrivate> d_ptr;

private:

  friend class ctkXnatConnection;

  /// The implementation of the fetch mechanism, called by the fetch() function.
  virtual void fetchImpl() = 0;

  Q_DECLARE_PRIVATE(ctkXnatObject)
  Q_DISABLE_COPY(ctkXnatObject)
};

Q_DECLARE_METATYPE(ctkXnatObject*)

#endif
