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

#ifndef ctkXnatReconstruction_h
#define ctkXnatReconstruction_h

#include "ctkXNATExport.h"

#include "ctkXnatObject.h"

class ctkXnatConnection;
class ctkXnatReconstructionPrivate;

class CTK_XNAT_EXPORT ctkXnatReconstruction : public ctkXnatObject
{
  Q_OBJECT

  Q_PROPERTY(QString xnat_reconstructedimagedata_id READ reconstructedImageId WRITE setReconstructedImageId)
  Q_PROPERTY(QString ID READ id WRITE setId)
  Q_PROPERTY(QString type READ type WRITE setType)
  Q_PROPERTY(QString baseScanType READ baseScanType WRITE setBaseScanType)
  Q_PROPERTY(QString URI READ uri WRITE setUri)

public:
  explicit ctkXnatReconstruction(ctkXnatObject* parent = 0);
  virtual ~ctkXnatReconstruction();

  const QString& reconstructedImageId() const;
  void setReconstructedImageId(const QString& reconstructedImageId);

  const QString& id() const;
  void setId(const QString& id);

  const QString& type() const;
  void setType(const QString& type);

  const QString& baseScanType() const;
  void setBaseScanType(const QString& baseScanType);

  const QString& uri() const;
  void setUri(const QString& uri);

  virtual void fetch(ctkXnatConnection* connection);

  virtual void download(ctkXnatConnection* connection, const QString& zipFilename);
  virtual void add(ctkXnatConnection* connection, const QString& name);
  virtual void remove(ctkXnatConnection* connection);

  virtual QString getKind() const;
  virtual bool holdsFiles() const;
  virtual bool receivesFiles() const;
  virtual bool isDeletable() const;

private:
  QScopedPointer<ctkXnatReconstructionPrivate> d_ptr;

  Q_DECLARE_PRIVATE(ctkXnatReconstruction);
  Q_DISABLE_COPY(ctkXnatReconstruction);
};

#endif
