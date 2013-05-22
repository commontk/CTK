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

#ifndef ctkXnatServer_h
#define ctkXnatServer_h

#include "ctkXNATExport.h"

#include "ctkXnatObject.h"

class ctkXnatConnection;

class CTK_XNAT_EXPORT ctkXnatServer : public ctkXnatObject
{
public:
  explicit ctkXnatServer(ctkXnatObject* parent = 0);
  virtual ~ctkXnatServer();

  virtual void fetch(ctkXnatConnection* connection);

  virtual QString getKind() const;

};

#endif
