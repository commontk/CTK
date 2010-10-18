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

#include "ctkQtResourcesTreeModel.h"


class ctkQtResourceTreeNode;

class ctkQtResourceTreeItem
{
public:

  ctkQtResourceTreeItem(const QFileInfo& fileInfo, ctkQtResourceTreeNode* parent = 0);

  virtual ~ctkQtResourceTreeItem();

  virtual ctkQtResourceTreeItem* child(int row);

  virtual int childCount() const;

  int row();

  QVariant data(int role) const;

  ctkQtResourceTreeNode* parent() const;

protected:
  QFileInfo entry;
  ctkQtResourceTreeNode* parentNode;
};

class ctkQtResourceTreeNode : public ctkQtResourceTreeItem
{
public:

  ctkQtResourceTreeNode(const QFileInfo& dirInfo, ctkQtResourceTreeNode* parent = 0);

  ~ctkQtResourceTreeNode();

  ctkQtResourceTreeItem* child(int row);

  int childCount() const;

  int indexOf(ctkQtResourceTreeItem* child) const;

private:

  QList<ctkQtResourceTreeItem*> children;

};


ctkQtResourceTreeItem::ctkQtResourceTreeItem(const QFileInfo& fileInfo, ctkQtResourceTreeNode* parent)
  : entry(fileInfo), parentNode(parent)
{

}

ctkQtResourceTreeItem::~ctkQtResourceTreeItem()
{

}

ctkQtResourceTreeItem* ctkQtResourceTreeItem::child(int row)
{
  Q_UNUSED(row)

  return 0;
}

int ctkQtResourceTreeItem::childCount() const
{
  return 0;
}

int ctkQtResourceTreeItem::row()
{
  if (parentNode)
  {
    return parentNode->indexOf(this);
  }

  return 0;
}

QVariant ctkQtResourceTreeItem::data(int role) const
{
  if (role == Qt::DisplayRole)
  {
    if (entry.isFile())
      return entry.fileName();

    QString lastDir = entry.absoluteFilePath();
    int i = lastDir.lastIndexOf('/');

    if (i == lastDir.size()-1) return lastDir;
    return lastDir.mid(i+1);
  }

  return QVariant();
}

ctkQtResourceTreeNode* ctkQtResourceTreeItem::parent() const
{
  return parentNode;
}


ctkQtResourceTreeNode::ctkQtResourceTreeNode(const QFileInfo& dirInfo, ctkQtResourceTreeNode* parent)
  : ctkQtResourceTreeItem(dirInfo, parent)
{
  QFileInfoList infoList = QDir(dirInfo.absoluteFilePath()).entryInfoList();
  QListIterator<QFileInfo> it(infoList);
  while (it.hasNext())
  {
    const QFileInfo& info = it.next();
    if (info.isFile())
    {
      children.push_back(new ctkQtResourceTreeItem(info, this));
    }
    else
    {
      children.push_back(new ctkQtResourceTreeNode(info, this));
    }
  }
}

ctkQtResourceTreeNode::~ctkQtResourceTreeNode()
{
  qDeleteAll(children);
}

ctkQtResourceTreeItem* ctkQtResourceTreeNode::child(int row)
{
  return children.value(row);
}

int ctkQtResourceTreeNode::childCount() const
{
  return children.size();
}

int ctkQtResourceTreeNode::indexOf(ctkQtResourceTreeItem* child) const
{
  return children.indexOf(child);
}


ctkQtResourcesTreeModel::ctkQtResourcesTreeModel(QObject* parent)
  : QAbstractItemModel(parent)
{
  rootItem = new ctkQtResourceTreeNode(QFileInfo(":/"));
}

ctkQtResourcesTreeModel::~ctkQtResourcesTreeModel()
{
  delete rootItem;
}

QVariant ctkQtResourcesTreeModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole)
  {
    ctkQtResourceTreeItem* item = static_cast<ctkQtResourceTreeItem*>(index.internalPointer());
    return item->data(role);
  }

  return QVariant();
}

Qt::ItemFlags ctkQtResourcesTreeModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
    return 0;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant ctkQtResourcesTreeModel::headerData(int section, Qt::Orientation orientation,
                   int role) const
{
  Q_UNUSED(section)
  Q_UNUSED(orientation)
  Q_UNUSED(role)

  return QVariant();
}

QModelIndex ctkQtResourcesTreeModel::index(int row, int column,
                 const QModelIndex &parent) const
{
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  if (!parent.isValid())
    return createIndex(row, column, rootItem);

  ctkQtResourceTreeItem* parentItem = static_cast<ctkQtResourceTreeItem*>(parent.internalPointer());
  ctkQtResourceTreeItem* childItem = parentItem->child(row);

  if (childItem)
    return createIndex(row, column, childItem);
  else
    return QModelIndex();
}

QModelIndex ctkQtResourcesTreeModel::parent(const QModelIndex &index) const
{
  if (!index.isValid())
    return QModelIndex();

  ctkQtResourceTreeItem* childItem = static_cast<ctkQtResourceTreeItem*>(index.internalPointer());
  ctkQtResourceTreeItem* parentItem = childItem->parent();

  if (parentItem)
    return createIndex(parentItem->row(), 0, parentItem);

  return QModelIndex();
}

int ctkQtResourcesTreeModel::rowCount(const QModelIndex &parent) const
{
  if (parent.column() > 0) return 0;

  if (!parent.isValid())
  {
    return 1;
  }
  else
  {
    ctkQtResourceTreeItem* parentItem = static_cast<ctkQtResourceTreeItem*>(parent.internalPointer());
    return parentItem->childCount();
  }
}

int ctkQtResourcesTreeModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent)

  return 1;
}
