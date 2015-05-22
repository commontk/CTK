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

#ifndef ctkXnatObject_h
#define ctkXnatObject_h

#include "ctkXNATCoreExport.h"

#include "ctkException.h"

#include <QList>
#include <QObject>
#include <QString>
#include <QMetaType>

class ctkXnatResource;
class ctkXnatSession;
class ctkXnatObjectPrivate;

/**
 * @ingroup XNAT_Core
 *
 * ctkXnatObject is the base class of the objects that represent the nodes in
 * the XNAT data hierarchy.
 */
class CTK_XNAT_CORE_EXPORT ctkXnatObject
{

public:

  /// Destructs the ctkXnatObject.
  virtual ~ctkXnatObject();

  /// Gets the global ID of the object.
  virtual QString id() const;

  /// Sets the ID of the object.
  /// @warning You must not change the ID of an existing object
  virtual void setId(const QString& id);

  /// Gets the resource URI of the object that can be used to access it through
  /// the REST API.
  virtual QString resourceUri() const = 0;

  /// Gets the name of the object.
  virtual QString name() const;

  /// Sets the name of the object.
  virtual void setName(const QString& name);

  /// Gets the description of the object.
  QString description() const;

  /// Sets the description of the object.
  void setDescription(const QString& description);

  /// Gets the value of the property with the given name.
  QString property(const QString& name) const;

  /// Sets the value of the property with the given name.
  void setProperty(const QString& name, const QVariant& value);

  /// Gets the last modification time from the server
  virtual QDateTime lastModifiedTimeOnServer();

  /// Sets the last modfication time on the server
  void setLastModifiedTime(const QDateTime& lastModifiedTime);

  /// Gets the properties of the object.
  const QMap<QString, QString>& properties() const;

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

  QString schemaType() const;

  /// Gets a human readable name of the child object type.
  virtual QString childDataType() const;

  /// Resets the object so that its properties and children needs to be fetched
  /// again at the next request.
  virtual void reset();

  /// Fetches the children and the properties of the object.
  void fetch();

  /// Checks if the object exists on the XNAT server.
  bool exists() const;

  /// Creates the object on the XNAT server and sets the new ID.
  /// @param overwrite, if true and the object already exists on the server
  ///                   it will be overwritten by the changes
  void save(bool overwrite = true);

  /// Deletes the object on the XNAT server and removes it from its parent.
  void erase();

  void download(const QString&);

  /// Creates a new resource folder with the given foldername, format, content and tags
  /// for this ctkXnatObject on the server.
  /// @param foldername the name of the resource folder on the server (mandatory)
  /// @param format the text of the format field of a resource (optional)
  /// @param content the text of the content field of a resource (optional)
  /// @param tags the content of the tags field of a resource (optional)
  /// @returns ctkXnatResource the created resource
  virtual ctkXnatResource* addResourceFolder(QString foldername,
                           QString format = "", QString content = "", QString tags = "");

  //QObject* asyncObject() const;

  // *********************
  // Add signals for async API
  //Q_SIGNAL downloadFinished(const QString&);

  // *********************
  // SLOTS for async error handling
  //Q_SLOT serverError(XnatError errorType, const QString& message);

  // *********************
  // Add blocking methods
  // throws ctkXnatTimeoutException
  //bool waitForDownloadFinished(const QString&);

  static const QString ID;
  static const QString NAME;
  static const QString LABEL;

protected:

  ctkXnatObject(const ctkXnatObject&);

  /// Constructs the ctkXnatObject.
  ctkXnatObject(ctkXnatObject* parent = 0, const QString& schemaType = QString::null);

  /// Constructs the ctkXnatObject with the given private part.
  ctkXnatObject(ctkXnatObjectPrivate& dd, ctkXnatObject* parent = 0, const QString& schemaType = QString::null);

  /// Gets the object that represents the connection to the XNAT server
  /// that stores the current object.
  ctkXnatSession* session() const;

  /// Fetches the resources of the object
  virtual void fetchResources(const QString &path = "/resources");

  /// The private implementation part of the object.
  const QScopedPointer<ctkXnatObjectPrivate> d_ptr;

private:

  friend class ctkXnatSessionPrivate;

  void setSchemaType(const QString& schemaType);

  /// The implementation of the fetch mechanism, called by the fetch() function.
  virtual void fetchImpl() = 0;

  /// The implementation of the download mechanism, called by the download(const QString&) function.
  virtual void downloadImpl(const QString&) = 0;

  /// The implementation of the upload mechanism, called by the save() function.
  /// Subclasses of ctkXnatObject can overwrite this function if needed
  /// @param overwrite, if true and the object already exists on the server
  ///                   it will be overwritten by the changes
  virtual void saveImpl(bool overwrite = true);

  Q_DECLARE_PRIVATE(ctkXnatObject)
};

Q_DECLARE_METATYPE(ctkXnatObject*)

#endif
