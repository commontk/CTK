/*=============================================================================

  Library: CTK

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

#ifndef CTKPLUGINRESOURCESTREEMODEL_H
#define CTKPLUGINRESOURCESTREEMODEL_H

#include <QAbstractItemModel>
#include <QDir>
#include <QSharedPointer>

class ctkPlugin;
class ctkPluginResourceTreeItem;

class ctkPluginResourcesTreeModel : public QAbstractItemModel
{
public:

  ctkPluginResourcesTreeModel(QSharedPointer<const ctkPlugin> plugin, QObject* parent = 0);
  ~ctkPluginResourcesTreeModel();

  QVariant data(const QModelIndex &index, int role) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;
  QVariant headerData(int section, Qt::Orientation orientation,
                     int role = Qt::DisplayRole) const;
  QModelIndex index(int row, int column,
                   const QModelIndex &parent = QModelIndex()) const;
  QModelIndex parent(const QModelIndex &index) const;
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;

private:

  QSharedPointer<const ctkPlugin> plugin;
  ctkPluginResourceTreeItem* rootItem;

};

#endif // CTKPLUGINRESOURCESTREEMODEL_H
