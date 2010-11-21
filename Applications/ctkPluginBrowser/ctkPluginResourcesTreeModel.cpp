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

#include "ctkPluginResourcesTreeModel.h"

#include <ctkPlugin.h>

class ctkPluginResourceTreeNode;

class ctkPluginResourceTreeItem
{
public:

  ctkPluginResourceTreeItem(const QString& path, ctkPluginResourceTreeNode* parent = 0);

  virtual ~ctkPluginResourceTreeItem();

  virtual ctkPluginResourceTreeItem* child(int row);

  virtual int childCount() const;

  int row();

  QVariant data(int role) const;

  ctkPluginResourceTreeNode* parent() const;

protected:
  const QString path;
  ctkPluginResourceTreeNode* parentNode;
};

class ctkPluginResourceTreeNode : public ctkPluginResourceTreeItem
{
public:

  ctkPluginResourceTreeNode(QSharedPointer<const ctkPlugin> plugin, const QString& path, ctkPluginResourceTreeNode* parent = 0);

  ~ctkPluginResourceTreeNode();

  ctkPluginResourceTreeItem* child(int row);

  int childCount() const;

  int indexOf(ctkPluginResourceTreeItem* child) const;

private:

  QSharedPointer<const ctkPlugin> plugin;
  QList<ctkPluginResourceTreeItem*> children;

};


ctkPluginResourceTreeItem::ctkPluginResourceTreeItem(const QString& path,
                                               ctkPluginResourceTreeNode* parent)
  : path(path), parentNode(parent)
{

}

ctkPluginResourceTreeItem::~ctkPluginResourceTreeItem()
{

}

ctkPluginResourceTreeItem* ctkPluginResourceTreeItem::child(int row)
{
  Q_UNUSED(row)

  return 0;
}

int ctkPluginResourceTreeItem::childCount() const
{
  return 0;
}

int ctkPluginResourceTreeItem::row()
{
  if (parentNode)
  {
    return parentNode->indexOf(this);
  }

  return 0;
}

QVariant ctkPluginResourceTreeItem::data(int role) const
{
  if (role == Qt::DisplayRole)
  {
    QString p = path;
    if (p.endsWith('/')) p = p.left(p.size()-1);

    int i = p.lastIndexOf('/');

    return p.mid(i+1);
  }
  else if (role == Qt::UserRole)
  {
    if (this->parent())
    {
      return this->parent()->data(role).toString() + path;
    }
    else
    {
      return path;
    }
  }

  return QVariant();
}

ctkPluginResourceTreeNode* ctkPluginResourceTreeItem::parent() const
{
  return parentNode;
}


ctkPluginResourceTreeNode::ctkPluginResourceTreeNode(QSharedPointer<const ctkPlugin> plugin, const QString& path, ctkPluginResourceTreeNode* parent)
  : ctkPluginResourceTreeItem(path, parent), plugin(plugin)
{
  QStringList pathEntries = plugin->getResourceList(path);
  QStringListIterator it(pathEntries);
  while (it.hasNext())
  {
    const QString& entry = it.next();
    if (entry.endsWith('/'))
    {
      children.push_back(new ctkPluginResourceTreeNode(plugin, entry, this));
    }
    else
    {
      children.push_back(new ctkPluginResourceTreeItem(entry, this));
    }
  }
}

ctkPluginResourceTreeNode::~ctkPluginResourceTreeNode()
{
  qDeleteAll(children);
}

ctkPluginResourceTreeItem* ctkPluginResourceTreeNode::child(int row)
{
  return children.value(row);
}

int ctkPluginResourceTreeNode::childCount() const
{
  return children.size();
}

int ctkPluginResourceTreeNode::indexOf(ctkPluginResourceTreeItem* child) const
{
  return children.indexOf(child);
}


ctkPluginResourcesTreeModel::ctkPluginResourcesTreeModel(QSharedPointer<const ctkPlugin> plugin, QObject* parent)
  : QAbstractItemModel(parent), plugin(plugin)
{
  rootItem = new ctkPluginResourceTreeNode(plugin, "/");
}

ctkPluginResourcesTreeModel::~ctkPluginResourcesTreeModel()
{
  delete rootItem;
}

QVariant ctkPluginResourcesTreeModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if ((role == Qt::DisplayRole) | (role == Qt::UserRole))
  {
    ctkPluginResourceTreeItem* item = static_cast<ctkPluginResourceTreeItem*>(index.internalPointer());
    return item->data(role);
  }

  return QVariant();
}

Qt::ItemFlags ctkPluginResourcesTreeModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
    return 0;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant ctkPluginResourcesTreeModel::headerData(int section, Qt::Orientation orientation,
                   int role) const
{
  Q_UNUSED(section)
  Q_UNUSED(orientation)
  Q_UNUSED(role)

  return QVariant();
}

QModelIndex ctkPluginResourcesTreeModel::index(int row, int column,
                 const QModelIndex &parent) const
{
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  ctkPluginResourceTreeItem* parentItem;

  if (!parent.isValid())
    parentItem = rootItem;
  else
    parentItem = static_cast<ctkPluginResourceTreeItem*>(parent.internalPointer());

  ctkPluginResourceTreeItem* childItem = parentItem->child(row);

  if (childItem)
    return createIndex(row, column, childItem);
  else
    return QModelIndex();
}

QModelIndex ctkPluginResourcesTreeModel::parent(const QModelIndex &index) const
{
  if (!index.isValid())
    return QModelIndex();

  ctkPluginResourceTreeItem* childItem = static_cast<ctkPluginResourceTreeItem*>(index.internalPointer());
  ctkPluginResourceTreeItem* parentItem = childItem->parent();

  if (parentItem == rootItem)
    return QModelIndex();

  return createIndex(parentItem->row(), 0, parentItem);
}

int ctkPluginResourcesTreeModel::rowCount(const QModelIndex &parent) const
{
  if (parent.column() > 0) return 0;

  ctkPluginResourceTreeItem* parentItem;
  if (!parent.isValid())
  {
    parentItem = rootItem;
  }
  else
  {
    parentItem = static_cast<ctkPluginResourceTreeItem*>(parent.internalPointer());
  }

  return parentItem->childCount();
}

int ctkPluginResourcesTreeModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent)

  return 1;
}
