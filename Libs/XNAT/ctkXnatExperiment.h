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

#ifndef ctkXnatExperiment_h
#define ctkXnatExperiment_h

#include "ctkXNATExport.h"

#include "ctkXnatObject.h"

class ctkXnatConnection;

class ctkXnatExperimentPrivate;

class CTK_XNAT_EXPORT ctkXnatExperiment : public ctkXnatObject
{
  Q_OBJECT

  Q_PROPERTY(QString ID READ id WRITE setId)
  // FIXME encode colons and slashes to valid characters for C++ identifiers
//  Q_PROPERTY(QString xnat:subjectassessordata/id READ id WRITE setId)
  Q_PROPERTY(QString project READ project WRITE setProject)
  Q_PROPERTY(QString date READ date WRITE setDate)
  Q_PROPERTY(QString xsiType READ xsiType WRITE setXsiType)
  Q_PROPERTY(QString label READ label WRITE setLabel)
  Q_PROPERTY(QString insert_date READ insertDate WRITE setInsertDate)
  Q_PROPERTY(QString URI READ uri WRITE setUri)

public:
  explicit ctkXnatExperiment(ctkXnatObject* parent = 0);
  virtual ~ctkXnatExperiment();

  const QString& id() const;
  void setId(const QString& id);

  const QString& project() const;
  void setProject(const QString& project);

  const QString& date() const;
  void setDate(const QString& date);

  const QString& xsiType() const;
  void setXsiType(const QString& xsiType);

  const QString& label() const;
  void setLabel(const QString& label);

  const QString& insertDate() const;
  void setInsertDate(const QString& insertDate);

  const QString& uri() const;
  void setUri(const QString& uri);

  virtual void fetch(ctkXnatConnection* connection);

  void add(ctkXnatConnection* connection, const QString& reconstruction);
  QString getModifiableChildKind() const;
  QString getModifiableParentName() const;
  bool isModifiable() const;

  virtual bool holdsFiles() const;
  virtual bool isModifiable(int parentIndex) const;

private:
  QScopedPointer<ctkXnatExperimentPrivate> d_ptr;

  Q_DECLARE_PRIVATE(ctkXnatExperiment);
  Q_DISABLE_COPY(ctkXnatExperiment);
};

#endif
