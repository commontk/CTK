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

#ifndef ctkXnatScanResource_h
#define ctkXnatScanResource_h

#include "ctkXNATExport.h"

#include "ctkXnatObject.h"

class ctkXnatConnection;
class ctkXnatScanResourcePrivate;

class CTK_XNAT_EXPORT ctkXnatScanResource : public ctkXnatObject
{
  Q_OBJECT

  Q_PROPERTY(QString xnat_abstractresource_id READ resourceId WRITE setResourceId)
  Q_PROPERTY(QString label READ label WRITE setLabel)
  Q_PROPERTY(QString elementName READ elementName WRITE setElementName)
  Q_PROPERTY(QString category READ category WRITE setCategory)
  Q_PROPERTY(QString cat_id READ categoryId WRITE setCategoryId)
  Q_PROPERTY(QString cat_desc READ categoryDescription WRITE setCategoryDescription)

public:
  explicit ctkXnatScanResource(ctkXnatObject* parent = 0);
  virtual ~ctkXnatScanResource();

  const QString& resourceId() const;
  void setResourceId(const QString& resourceId);

  const QString& label() const;
  void setLabel(const QString& label);

  const QString& elementName() const;
  void setElementName(const QString& elementName);

  const QString& category() const;
  void setCategory(const QString& category);

  const QString& categoryId() const;
  void setCategoryId(const QString& categoryId);

  const QString& categoryDescription() const;
  void setCategoryDescription(const QString& categoryDescription);

  virtual void fetch(ctkXnatConnection* connection);

  virtual void download(ctkXnatConnection* connection, const QString& zipFilename);

  virtual bool isFile() const;

private:
  QScopedPointer<ctkXnatScanResourcePrivate> d_ptr;

  Q_DECLARE_PRIVATE(ctkXnatScanResource);
  Q_DISABLE_COPY(ctkXnatScanResource);
};

#endif
