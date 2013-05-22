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

#ifndef ctkXnatReconstructionFolder_h
#define ctkXnatReconstructionFolder_h

#include "ctkXNATCoreExport.h"

#include "ctkXnatObject.h"

class ctkXnatConnection;

class CTK_XNAT_CORE_EXPORT ctkXnatReconstructionFolder : public ctkXnatObject
{
public:
  explicit ctkXnatReconstructionFolder(ctkXnatObject* parent = 0);
  virtual ~ctkXnatReconstructionFolder();

  virtual void fetch(ctkXnatConnection* connection);

  void download(ctkXnatConnection* connection, const QString& zipFilename);
  void add(ctkXnatConnection* connection, const QString& categoryEntry);
  QString getModifiableChildKind() const;
  QString getModifiableParentName() const;
  bool isModifiable() const;

  virtual QString getKind() const;
  virtual QString getModifiableChildKind(int parentIndex) const;
  virtual QString getModifiableParentName(int parentIndex) const;

  virtual bool holdsFiles() const;
  virtual bool isModifiable(int parentIndex) const;
  virtual bool isDeletable() const;
};

#endif
