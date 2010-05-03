/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
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

#include "ctkPluginResourcesTreeModel.h"

#include <PluginFramework/ctkPlugin.h>

namespace ctk {

  class PluginResourceTreeNode;

  class PluginResourceTreeItem
  {
  public:

    PluginResourceTreeItem(const QString& path, PluginResourceTreeNode* parent = 0);

    virtual ~PluginResourceTreeItem();

    virtual PluginResourceTreeItem* child(int row);

    virtual int childCount() const;

    int row();

    QVariant data(int role) const;

    PluginResourceTreeNode* parent() const;

  protected:
    const QString path;
    PluginResourceTreeNode* parentNode;
  };

  class PluginResourceTreeNode : public PluginResourceTreeItem
  {
  public:

    PluginResourceTreeNode(const Plugin* plugin, const QString& path, PluginResourceTreeNode* parent = 0);

    ~PluginResourceTreeNode();

    PluginResourceTreeItem* child(int row);

    int childCount() const;

    int indexOf(PluginResourceTreeItem* child) const;

  private:

    const Plugin* plugin;
    QList<PluginResourceTreeItem*> children;

  };


  PluginResourceTreeItem::PluginResourceTreeItem(const QString& path,
                                                 PluginResourceTreeNode* parent)
    : path(path), parentNode(parent)
  {

  }

  PluginResourceTreeItem::~PluginResourceTreeItem()
  {

  }

  PluginResourceTreeItem* PluginResourceTreeItem::child(int row)
  {
    return 0;
  }

  int PluginResourceTreeItem::childCount() const
  {
    return 0;
  }

  int PluginResourceTreeItem::row()
  {
    if (parentNode)
    {
      return parentNode->indexOf(this);
    }

    return 0;
  }

  QVariant PluginResourceTreeItem::data(int role) const
  {
    if (role == Qt::DisplayRole)
    {
      QString p = path;
      if (p.endsWith('/')) p = p.left(p.size()-1);

      int i = p.lastIndexOf('/');

      return p.mid(i+1);
    }

    return QVariant();
  }

  PluginResourceTreeNode* PluginResourceTreeItem::parent() const
  {
    return parentNode;
  }


  PluginResourceTreeNode::PluginResourceTreeNode(const Plugin* plugin, const QString& path, PluginResourceTreeNode* parent)
    : PluginResourceTreeItem(path, parent), plugin(plugin)
  {
    QStringList pathEntries = plugin->getResourceList(path);
    QStringListIterator it(pathEntries);
    while (it.hasNext())
    {
      const QString& entry = it.next();
      if (entry.endsWith('/'))
      {
        children.push_back(new PluginResourceTreeNode(plugin, entry, this));
      }
      else
      {
        children.push_back(new PluginResourceTreeItem(entry, this));
      }
    }
  }

  PluginResourceTreeNode::~PluginResourceTreeNode()
  {
    qDeleteAll(children);
  }

  PluginResourceTreeItem* PluginResourceTreeNode::child(int row)
  {
    return children.value(row);
  }

  int PluginResourceTreeNode::childCount() const
  {
    return children.size();
  }

  int PluginResourceTreeNode::indexOf(PluginResourceTreeItem* child) const
  {
    return children.indexOf(child);
  }


  PluginResourcesTreeModel::PluginResourcesTreeModel(const Plugin* plugin, QObject* parent)
    : QAbstractItemModel(parent), plugin(plugin)
  {
    rootItem = new PluginResourceTreeNode(plugin, "/");
  }

  PluginResourcesTreeModel::~PluginResourcesTreeModel()
  {
    delete rootItem;
  }

  QVariant PluginResourcesTreeModel::data(const QModelIndex &index, int role) const
  {
    if (!index.isValid())
      return QVariant();

    if (role == Qt::DisplayRole)
    {
      PluginResourceTreeItem* item = static_cast<PluginResourceTreeItem*>(index.internalPointer());
      return item->data(role);
    }

    return QVariant();
  }

  Qt::ItemFlags PluginResourcesTreeModel::flags(const QModelIndex &index) const
  {
    if (!index.isValid())
      return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  }

  QVariant PluginResourcesTreeModel::headerData(int section, Qt::Orientation orientation,
                     int role) const
  {
    return QVariant();
  }

  QModelIndex PluginResourcesTreeModel::index(int row, int column,
                   const QModelIndex &parent) const
  {
    if (!hasIndex(row, column, parent))
      return QModelIndex();

    PluginResourceTreeItem* parentItem;

    if (!parent.isValid())
      parentItem = rootItem;
    else
      parentItem = static_cast<PluginResourceTreeItem*>(parent.internalPointer());

    PluginResourceTreeItem* childItem = parentItem->child(row);

    if (childItem)
      return createIndex(row, column, childItem);
    else
      return QModelIndex();
  }

  QModelIndex PluginResourcesTreeModel::parent(const QModelIndex &index) const
  {
    if (!index.isValid())
      return QModelIndex();

    PluginResourceTreeItem* childItem = static_cast<PluginResourceTreeItem*>(index.internalPointer());
    PluginResourceTreeItem* parentItem = childItem->parent();

    if (parentItem == rootItem)
      return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
  }

  int PluginResourcesTreeModel::rowCount(const QModelIndex &parent) const
  {
    if (parent.column() > 0) return 0;

    PluginResourceTreeItem* parentItem;
    if (!parent.isValid())
    {
      parentItem = rootItem;
    }
    else
    {
      parentItem = static_cast<PluginResourceTreeItem*>(parent.internalPointer());
    }

    return parentItem->childCount();
  }

  int PluginResourcesTreeModel::columnCount(const QModelIndex &parent) const
  {
    return 1;
  }

}

