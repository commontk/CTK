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

#ifndef ctkXnatSubject_h
#define ctkXnatSubject_h

#include "ctkXNATCoreExport.h"

#include "ctkXnatObject.h"
#include "ctkXnatDefaultSchemaTypes.h"

class ctkXnatProject;
class ctkXnatSubjectPrivate;

/**
 * @ingroup XNAT_Core
 */
class CTK_XNAT_CORE_EXPORT ctkXnatSubject : public ctkXnatObject
{

public:

  ctkXnatSubject(ctkXnatObject* parent = 0, const QString& schemaType = ctkXnatDefaultSchemaTypes::XSI_SUBJECT);

  virtual ~ctkXnatSubject();

  ctkXnatProject* getPrimaryProject() const;
  QList<ctkXnatProject*> getProjects() const;

  /// Sets the name, i.e. the label of the subject
  virtual void setName(const QString &name);
  /// Returns the name, i.e. the label of the subject
  virtual QString name() const;

  void setLabel(const QString &label);
  QString label() const;

  const QString& insertDate() const;

  /// @warning does not overwrite the insert date on the server, since this is read only
  void setInsertDate(const QString& insertDate);

  const QString& insertUser() const;

  /// @warning does not overwrite the insert user on the server, since this is read only
  void setInsertUser(const QString& insertUser);

  virtual QString resourceUri() const;

  void reset();

  static const QString INSERT_DATE;
  static const QString INSERT_USER;

private:

  friend class qRestResult;

  virtual void fetchImpl();

  virtual void downloadImpl(const QString&);

  Q_DECLARE_PRIVATE(ctkXnatSubject)
};

#endif
