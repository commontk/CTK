/*=============================================================================

  Library: XNAT/Core

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

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

#ifndef CTKXNATLISTMODEL_H
#define CTKXNATLISTMODEL_H

#include "QAbstractListModel"

#include "ctkXNATCoreExport.h"

class ctkXnatObject;

/**
 * @ingroup XNAT_Core
 */
class CTK_XNAT_CORE_EXPORT ctkXnatListModel : public QAbstractListModel
{
  Q_OBJECT

public:
  ctkXnatListModel();
  void setRootObject(ctkXnatObject* root);
  ctkXnatObject* rootObject();

  int rowCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

protected:
  ctkXnatObject* RootObject;
};

#endif // CTKXNATLISTMODEL_H
