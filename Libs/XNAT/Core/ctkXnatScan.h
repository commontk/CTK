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

#ifndef ctkXnatScan_h
#define ctkXnatScan_h

#include "ctkXNATCoreExport.h"

#include "ctkXnatObject.h"
#include "ctkXnatDefaultSchemaTypes.h"

class ctkXnatConnection;
class ctkXnatScanFolder;
class ctkXnatScanPrivate;

/**
 * @ingroup XNAT_Core
 */
class CTK_XNAT_CORE_EXPORT ctkXnatScan : public ctkXnatObject
{

public:

  ctkXnatScan(ctkXnatObject* parent = 0, const QString& schemaType = ctkXnatDefaultSchemaTypes::XSI_SCAN);

  virtual ~ctkXnatScan();

  void setQuality(const QString& quality);
  QString quality() const;

  void setSeriesDescription(const QString& seriesDescription);
  QString seriesDescription() const;

  void setType (const QString& type);
  QString type () const;

  virtual QString resourceUri() const;

  void reset();

  static const QString QUALITY;
  static const QString SERIES_DESCRIPTION;
  static const QString TYPE;

private:

  friend class qRestResult;

  virtual void fetchImpl();

  virtual void downloadImpl(const QString&);

  Q_DECLARE_PRIVATE(ctkXnatScan)
};

#endif
