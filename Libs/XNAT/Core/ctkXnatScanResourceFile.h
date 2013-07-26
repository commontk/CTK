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

#ifndef ctkXnatScanResourceFile_h
#define ctkXnatScanResourceFile_h

#include "ctkXNATCoreExport.h"

#include "ctkXnatObject.h"

class ctkXnatConnection;
class ctkXnatScanResourceFilePrivate;

class CTK_XNAT_CORE_EXPORT ctkXnatScanResourceFile : public ctkXnatObject
{

public:

  typedef QSharedPointer<ctkXnatScanResourceFile> Pointer;
  typedef QWeakPointer<ctkXnatScanResourceFile> WeakPointer;
  
  static Pointer Create();
  virtual ~ctkXnatScanResourceFile();
  
  const QString& uri() const;
  void setUri(const QString& uri);
  
  virtual void reset();
  virtual void remove();
  
private:
  
  friend class qRestResult;
  explicit ctkXnatScanResourceFile();
  virtual void fetchImpl();
  
  Q_DECLARE_PRIVATE(ctkXnatScanResourceFile);
  Q_DISABLE_COPY(ctkXnatScanResourceFile);
};

#endif
