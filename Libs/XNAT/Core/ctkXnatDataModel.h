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

#ifndef CTKXNATDATAMODEL_H
#define CTKXNATDATAMODEL_H

#include "ctkXNATCoreExport.h"

#include "ctkXnatObject.h"

class ctkXnatDataModelPrivate;
class ctkXnatProject;
class ctkXnatSession;

/**
 * @ingroup XNAT_Core
 *
 * @brief The ctkXnatDataModel class reprents the root object in a
 * XNAT data hierarchy.
 */
class ctkXnatDataModel : public ctkXnatObject
{

public:

  explicit ctkXnatDataModel(ctkXnatSession* connection);

  QList<ctkXnatProject*> projects() const;

  ctkXnatSession* session() const;

  virtual QString childDataType() const;

private:

  virtual QString resourceUri() const;

  virtual void fetchImpl();

  virtual void downloadImpl(const QString&);

  Q_DECLARE_PRIVATE(ctkXnatDataModel)
};

#endif
