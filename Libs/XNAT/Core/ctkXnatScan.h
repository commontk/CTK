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

#ifndef ctkXnatScan_h
#define ctkXnatScan_h

#include "ctkXNATCoreExport.h"

#include "ctkXnatObject.h"

class ctkXnatConnection;
class ctkXnatScanPrivate;

class CTK_XNAT_CORE_EXPORT ctkXnatScan : public ctkXnatObject
{

public:
  explicit ctkXnatScan(ctkXnatObject* parent = 0);
  virtual ~ctkXnatScan();

  const QString& imageScanId() const;
  void setImageScanId(const QString& imageScanId);

  const QString& id() const;
  void setId(const QString& id);

  const QString& type() const;
  void setType(const QString& type);

  const QString& quality() const;
  void setQuality(const QString& quality);

  const QString& xsiType() const;
  void setXsiType(const QString& xsiType);

  const QString& note() const;
  void setNote(const QString& note);

  const QString& seriesDescription() const;
  void setSeriesDescription(const QString& seriesDescription);

  const QString& uri() const;
  void setUri(const QString& uri);

  virtual void fetch(ctkXnatConnection* connection);

  virtual void download(ctkXnatConnection* connection, const QString& zipFilename);

  virtual QString getKind() const;
  virtual bool holdsFiles() const;

private:
  QScopedPointer<ctkXnatScanPrivate> d_ptr;

  Q_DECLARE_PRIVATE(ctkXnatScan);
  Q_DISABLE_COPY(ctkXnatScan);
};

#endif
