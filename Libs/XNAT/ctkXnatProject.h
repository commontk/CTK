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

#ifndef ctkctkXnatProject_h
#define ctkctkXnatProject_h

#include "ctkXNATExport.h"

#include "ctkXnatObject.h"

class ctkXnatConnection;
class ctkXnatProjectPrivate;

class CTK_XNAT_EXPORT ctkXnatProject : public ctkXnatObject
{
  Q_OBJECT

  Q_PROPERTY(QString ID READ id WRITE setId)
  Q_PROPERTY(QString secondary_ID READ secondaryId WRITE setSecondaryId)
  Q_PROPERTY(QString name READ name WRITE setName)
  Q_PROPERTY(QString description READ description WRITE setDescription)
  Q_PROPERTY(QString pi_firstname READ piFirstName WRITE setPiFirstName)
  Q_PROPERTY(QString pi_lastname READ piLastName WRITE setPiLastName)
  Q_PROPERTY(QString URI READ uri WRITE setUri)

public:
  explicit ctkXnatProject(ctkXnatObject* parent = 0);
  virtual ~ctkXnatProject();

  const QString& id() const;
  void setId(const QString& id);

  const QString& secondaryId() const;
  void setSecondaryId(const QString& secondaryId);

  const QString& name() const;
  void setName(const QString& name);

  const QString& description() const;
  void setDescription(const QString& description);

  const QString& piFirstName() const;
  void setPiFirstName(const QString& piFirstName);

  const QString& piLastName() const;
  void setPiLastName(const QString& piLastName);

  const QString& uri() const;
  void setUri(const QString& uri);

  virtual void fetch(ctkXnatConnection* connection);
  virtual void remove(ctkXnatConnection* connection);

  virtual QString getKind() const;
  virtual bool isModifiable(int parentIndex) const;
  virtual QString getModifiableChildKind(int parentIndex) const;

private:
  QScopedPointer<ctkXnatProjectPrivate> d_ptr;

  Q_DECLARE_PRIVATE(ctkXnatProject);
  Q_DISABLE_COPY(ctkXnatProject);
};

#endif
