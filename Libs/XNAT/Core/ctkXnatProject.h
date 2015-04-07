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

#ifndef ctkctkXnatProject_h
#define ctkctkXnatProject_h

#include "ctkXNATCoreExport.h"

#include "ctkXnatObject.h"
#include "ctkXnatDefaultSchemaTypes.h"

class ctkXnatDataModel;
class ctkXnatProjectPrivate;

/**
 * @ingroup XNAT_Core
 */
class CTK_XNAT_CORE_EXPORT ctkXnatProject : public ctkXnatObject
{

public:

  ctkXnatProject(ctkXnatObject* parent = 0, const QString& schemaType = ctkXnatDefaultSchemaTypes::XSI_PROJECT);

  virtual ~ctkXnatProject();

  virtual QString resourceUri() const;

  virtual QString childDataType() const;

  const QString secondaryId() const;
  void setSecondaryId(const QString& secondaryId);

  const QString piFirstName() const;
  void setPiFirstName(const QString& piFirstName);

  const QString piLastName() const;
  void setPiLastName(const QString& piLastName);

  QString projectDescription() const;
  void setProjectDescription(const QString &description);

  void reset();

  static const QString SECONDARY_ID;
  static const QString DESCRIPTION;
  static const QString PI_FIRSTNAME;
  static const QString PI_LASTNAME;

private:

  virtual void fetchImpl();

  virtual void downloadImpl(const QString&);

  Q_DECLARE_PRIVATE(ctkXnatProject)
};

#endif
