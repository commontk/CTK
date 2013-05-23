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

#ifndef ctkXnatSubject_h
#define ctkXnatSubject_h

#include "ctkXNATCoreExport.h"

#include "ctkXnatObject.h"

class ctkXnatConnection;

class ctkXnatSubjectPrivate;

class CTK_XNAT_CORE_EXPORT ctkXnatSubject : public ctkXnatObject
{
  Q_OBJECT

  Q_PROPERTY(QString ID READ id WRITE setId)
  Q_PROPERTY(QString project READ project WRITE setProject)
  Q_PROPERTY(QString label READ label WRITE setLabel)
  Q_PROPERTY(QString insert_date READ insertDate WRITE setInsertDate)
  Q_PROPERTY(QString insert_user READ insertUser WRITE setInsertUser)
  Q_PROPERTY(QString URI READ uri WRITE setUri)

  typedef ctkXnatObject Superclass;

public:
  explicit ctkXnatSubject(ctkXnatObject* parent = 0);
  virtual ~ctkXnatSubject();

  const QString& id() const;
  void setId(const QString& id);

  const QString& project() const;
  void setProject(const QString& project);

  const QString& label() const;
  void setLabel(const QString& label);

  const QString& insertDate() const;
  void setInsertDate(const QString& insertDate);

  const QString& insertUser() const;
  void setInsertUser(const QString& insertUser);

  const QString& uri() const;
  void setUri(const QString& uri);

  virtual void fetch(ctkXnatConnection* connection);

  virtual QString getKind() const;

  using Superclass::isModifiable;
  virtual bool isModifiable(int parentIndex) const;

private:
  QScopedPointer<ctkXnatSubjectPrivate> d_ptr;

  Q_DECLARE_PRIVATE(ctkXnatSubject);
  Q_DISABLE_COPY(ctkXnatSubject);
};

#endif
