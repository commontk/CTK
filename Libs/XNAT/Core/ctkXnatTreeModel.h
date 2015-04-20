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

#ifndef ctkXnatTreeModel_h
#define ctkXnatTreeModel_h

#include "ctkXNATCoreExport.h"

#include <QAbstractItemModel>

class ctkXnatObject;
class ctkXnatDataModel;
class ctkXnatTreeModelPrivate;

/**
 * @ingroup XNAT_Core
 */
class CTK_XNAT_CORE_EXPORT ctkXnatTreeModel : public QAbstractItemModel
{
  Q_OBJECT

public:
  explicit ctkXnatTreeModel();
  virtual ~ctkXnatTreeModel();

  virtual QVariant data(const QModelIndex& index, int role) const;
  virtual QModelIndex parent(const QModelIndex& child) const;
  virtual QModelIndex index(int row, int column, const QModelIndex& parent) const;
  virtual int rowCount(const QModelIndex& parent) const;
  virtual int columnCount(const QModelIndex& parent) const;
  virtual bool hasChildren(const QModelIndex& parent) const;
  virtual bool canFetchMore(const QModelIndex& parent) const;
  virtual void fetchMore(const QModelIndex& parent);

  ctkXnatObject* xnatObject(const QModelIndex& index) const;

  void addDataModel(ctkXnatDataModel* dataModel);
  void removeDataModel(ctkXnatDataModel* dataModel);

  bool removeAllRows(const QModelIndex& parent);

  void downloadFile (const QModelIndex& index, const QString& zipFilename);

  void addChildNode(const QModelIndex& index, ctkXnatObject *child);

private:

  const QScopedPointer<ctkXnatTreeModelPrivate> d_ptr;

  Q_DECLARE_PRIVATE(ctkXnatTreeModel)

};

#endif
