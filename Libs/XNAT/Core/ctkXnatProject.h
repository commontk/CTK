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

class ctkXnatDataModel;
class ctkXnatProjectPrivate;

class CTK_XNAT_CORE_EXPORT ctkXnatProject : public ctkXnatObject
{

public:

  CTK_XNAT_OBJECT(ctkXnatProject, ctkXnatObject, "xnat::projectData")

  explicit ctkXnatProject(ctkXnatDataModel* parent = 0);
  virtual ~ctkXnatProject();

  virtual QString resourceUri() const;

  const QString& secondaryId() const;
  void setSecondaryId(const QString& secondaryId);

  const QString& piFirstName() const;
  void setPiFirstName(const QString& piFirstName);

  const QString& piLastName() const;
  void setPiLastName(const QString& piLastName);

  void reset();

private:

  virtual void fetchImpl();

  Q_DECLARE_PRIVATE(ctkXnatProject)
};

#endif
