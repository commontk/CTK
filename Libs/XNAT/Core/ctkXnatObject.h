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

class ctkXnatConnection;
class ctkXnatObjectPrivate;

class CTK_XNAT_CORE_EXPORT ctkXnatObject : public QObject
{
  Q_OBJECT

public:
  explicit ctkXnatObject(ctkXnatObject* parent = 0);
  virtual ~ctkXnatObject();

  QString getName() const;
  ctkXnatObject* getParent() const;
  int parentIndex();
  const QList<ctkXnatObject*>& getChildren() const;
  QString childName(int childIndex) const;

  void addChild(const QString& name, ctkXnatObject* child);

  virtual void fetch(ctkXnatConnection* connection);
  void removeChild(int parentIndex);

  virtual void download(ctkXnatConnection* connection, const QString& zipFilename);
  virtual void upload(ctkXnatConnection* connection, const QString& zipFilename);
  virtual void add(ctkXnatConnection* connection, const QString& name);
  virtual void remove(ctkXnatConnection* connection);

  virtual QString getKind() const;
  virtual QString getModifiableChildKind() const;
  virtual QString getModifiableParentName() const;

  virtual bool isFile() const;
  virtual bool holdsFiles() const;
  virtual bool receivesFiles() const;
  virtual bool isModifiable(int childIndex) const;
  virtual bool isDeletable() const;

  virtual bool isModifiable() const;

private:
  void setParent(ctkXnatObject* parent);
  void setParentIndex(int index);

  QScopedPointer<ctkXnatObjectPrivate> d_ptr;

  Q_DECLARE_PRIVATE(ctkXnatObject);
  Q_DISABLE_COPY(ctkXnatObject);
};

#endif
