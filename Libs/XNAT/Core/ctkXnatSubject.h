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

class ctkXnatProject;
class ctkXnatSubjectPrivate;

class CTK_XNAT_CORE_EXPORT ctkXnatSubject : public ctkXnatObject
{

public:

  typedef QSharedPointer<ctkXnatSubject> Pointer;
  typedef QWeakPointer<ctkXnatSubject> WeakPointer;

  static Pointer Create(ctkXnatConnection* connection);

  virtual ~ctkXnatSubject();

  QSharedPointer<ctkXnatProject> getPrimaryProject() const;
  QList<QSharedPointer<ctkXnatProject> > getProjects() const;

  const QString& insertDate() const;
  void setInsertDate(const QString& insertDate);

  const QString& insertUser() const;
  void setInsertUser(const QString& insertUser);

  const QString& uri() const;
  void setUri(const QString& uri);

  virtual void reset();

private:

  friend class qRestResult;

  explicit ctkXnatSubject(ctkXnatConnection* connection = NULL);

  virtual void fetchImpl();

  Q_DECLARE_PRIVATE(ctkXnatSubject)
  Q_DISABLE_COPY(ctkXnatSubject)
};

#endif
