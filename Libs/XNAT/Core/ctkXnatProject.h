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

#ifndef ctkctkXnatProject_h
#define ctkctkXnatProject_h

#include "ctkXNATCoreExport.h"

#include "ctkXnatObject.h"

class ctkXnatConnection;
class ctkXnatProjectPrivate;

class CTK_XNAT_CORE_EXPORT ctkXnatProject : public ctkXnatObject
{

public:

  typedef QSharedPointer<ctkXnatProject> Pointer;
  typedef QWeakPointer<ctkXnatProject> WeakPointer;

  static Pointer Create();

  virtual ~ctkXnatProject();

  const QString& secondaryId() const;
  void setSecondaryId(const QString& secondaryId);

  const QString& piFirstName() const;
  void setPiFirstName(const QString& piFirstName);

  const QString& piLastName() const;
  void setPiLastName(const QString& piLastName);

  const QString& uri() const;
  void setUri(const QString& uri);

  virtual void reset();

  virtual void remove();

private:

  friend class qRestResult;

  explicit ctkXnatProject();

  virtual void fetchImpl();

  Q_DECLARE_PRIVATE(ctkXnatProject)
  Q_DISABLE_COPY(ctkXnatProject)
};

#endif
