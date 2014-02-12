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

#ifndef ctkXnatScanResource_h
#define ctkXnatScanResource_h

#include "ctkXNATCoreExport.h"

#include "ctkXnatObject.h"
#include "ctkXnatDefaultSchemaTypes.h"

class ctkXnatConnection;
class ctkXnatScan;
class ctkXnatScanResourcePrivate;

/**
 * @ingroup XNAT_Core
 */
class CTK_XNAT_CORE_EXPORT ctkXnatScanResource : public ctkXnatObject
{

public:

  ctkXnatScanResource(ctkXnatObject* parent = 0,
                      const QString& schemaType = ctkXnatDefaultSchemaTypes::XSI_SCAN_RESOURCE);

  virtual ~ctkXnatScanResource();

  QString resourceUri() const;

  void reset();

  void download(const QString& filename);

private:

  friend class qRestResult;
  virtual void fetchImpl();

  Q_DECLARE_PRIVATE(ctkXnatScanResource)

};

#endif
