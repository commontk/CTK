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

#ifndef ctkXnatResourceFolder_h
#define ctkXnatResourceFolder_h

#include "ctkXNATCoreExport.h"

#include "ctkXnatObject.h"

class ctkXnatResourceFolderPrivate;

/**
 * @ingroup XNAT_Core
 */
class CTK_XNAT_CORE_EXPORT ctkXnatResourceFolder : public ctkXnatObject
{

public:

  ctkXnatResourceFolder(ctkXnatObject* parent = NULL);

  virtual ~ctkXnatResourceFolder();

  virtual QString resourceUri() const;

  virtual QString name() const;
  virtual QString label()const;

  void reset();

private:

  friend class qRestResult;
  virtual void fetchImpl();
  virtual void downloadImpl(const QString&);
  virtual void saveImpl(bool overwrite);

  Q_DECLARE_PRIVATE(ctkXnatResourceFolder)
};

#endif
