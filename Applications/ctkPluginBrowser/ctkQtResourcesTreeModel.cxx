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

#include "ctkQtResourcesTreeModel.h"


namespace ctk {

  class QtResourceTreeNode;

  class QtResourceTreeItem
  {
  public:

    QtResourceTreeItem(const QFileInfo& fileInfo, QtResourceTreeNode* parent = 0);

    virtual ~QtResourceTreeItem();

    virtual QtResourceTreeItem* child(int row);

    virtual int childCount() const;

    int row();

    QVariant data(int role) const;

    QtResourceTreeNode* parent() const;

  protected:
    QFileInfo entry;
    QtResourceTreeNode* parentNode;
  };

  class QtResourceTreeNode : public QtResourceTreeItem
  {
  public:

    QtResourceTreeNode(const QFileInfo& dirInfo, QtResourceTreeNode* parent = 0);

    ~QtResourceTreeNode();

    QtResourceTreeItem* child(int row);

    int childCount() const;

    int indexOf(QtResourceTreeItem* child) const;

  private:

    QList<QtResourceTreeItem*> children;

  };


  QtResourceTreeItem::QtResourceTreeItem(const QFileInfo& fileInfo, QtResourceTreeNode* parent)
    : entry(fileInfo), parentNode(parent)
  {

  }

  QtResourceTreeItem::~QtResourceTreeItem()
  {

  }

  QtResourceTreeItem* QtResourceTreeItem::child(int row)
  {
    return 0;
  }

  int QtResourceTreeItem::childCount() const
  {
    return 0;
  }

  int QtResourceTreeItem::row()
  {
    if (parentNode)
    {
      return parentNode->indexOf(this);
    }

    return 0;
  }

  QVariant QtResourceTreeItem::data(int role) const
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

  QtResourceTreeNode* QtResourceTreeItem::parent() const
  {
    return parentNode;
  }


  QtResourceTreeNode::QtResourceTreeNode(const QFileInfo& dirInfo, QtResourceTreeNode* parent)
    : QtResourceTreeItem(dirInfo, parent)
  {
    QFileInfoList infoList = QDir(dirInfo.absoluteFilePath()).entryInfoList();
    QListIterator<QFileInfo> it(infoList);
    while (it.hasNext())
    {
      const QFileInfo& info = it.next();
      if (info.isFile())
      {
        children.push_back(new QtResourceTreeItem(info, this));
      }
      else
      {
        children.push_back(new QtResourceTreeNode(info, this));
      }
    }
  }

  QtResourceTreeNode::~QtResourceTreeNode()
  {
    qDeleteAll(children);
  }

  QtResourceTreeItem* QtResourceTreeNode::child(int row)
  {
    return children.value(row);
  }

  int QtResourceTreeNode::childCount() const
  {
    return children.size();
  }

  int QtResourceTreeNode::indexOf(QtResourceTreeItem* child) const
  {
    return children.indexOf(child);
  }


  QtResourcesTreeModel::QtResourcesTreeModel(QObject* parent)
    : QAbstractItemModel(parent)
  {
    rootItem = new QtResourceTreeNode(QFileInfo(":/"));
  }

  QtResourcesTreeModel::~QtResourcesTreeModel()
  {
    delete rootItem;
  }

  QVariant QtResourcesTreeModel::data(const QModelIndex &index, int role) const
  {
    if (!index.isValid())
      return QVariant();

    if (role == Qt::DisplayRole)
    {
      QtResourceTreeItem* item = static_cast<QtResourceTreeItem*>(index.internalPointer());
      return item->data(role);
    }

    return QVariant();
  }

  Qt::ItemFlags QtResourcesTreeModel::flags(const QModelIndex &index) const
  {
    if (!index.isValid())
      return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  }

  QVariant QtResourcesTreeModel::headerData(int section, Qt::Orientation orientation,
                     int role) const
  {
    return QVariant();
  }

  QModelIndex QtResourcesTreeModel::index(int row, int column,
                   const QModelIndex &parent) const
  {
    if (!hasIndex(row, column, parent))
      return QModelIndex();

    if (!parent.isValid())
      return createIndex(row, column, rootItem);

    QtResourceTreeItem* parentItem = static_cast<QtResourceTreeItem*>(parent.internalPointer());
    QtResourceTreeItem* childItem = parentItem->child(row);

    if (childItem)
      return createIndex(row, column, childItem);
    else
      return QModelIndex();
  }

  QModelIndex QtResourcesTreeModel::parent(const QModelIndex &index) const
  {
    if (!index.isValid())
      return QModelIndex();

    QtResourceTreeItem* childItem = static_cast<QtResourceTreeItem*>(index.internalPointer());
    QtResourceTreeItem* parentItem = childItem->parent();

    if (parentItem)
      return createIndex(parentItem->row(), 0, parentItem);

    return QModelIndex();
  }

  int QtResourcesTreeModel::rowCount(const QModelIndex &parent) const
  {
    if (parent.column() > 0) return 0;

    if (!parent.isValid())
    {
      return 1;
    }
    else
    {
      QtResourceTreeItem* parentItem = static_cast<QtResourceTreeItem*>(parent.internalPointer());
      return parentItem->childCount();
    }
  }

  int QtResourcesTreeModel::columnCount(const QModelIndex &parent) const
  {
    return 1;
  }

}
