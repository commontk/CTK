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

#include "ctkXnatTreeModel.h"

#include "ctkXnatConstants.h"
#include "ctkXnatDataModel.h"
#include "ctkXnatObject.h"
#include "ctkXnatTreeItem_p.h"

#include <QList>

class ctkXnatTreeModelPrivate
{
public:

  ctkXnatTreeModelPrivate()
    : m_RootItem(new ctkXnatTreeItem())
  {
  }

  ctkXnatTreeItem* itemAt(const QModelIndex& index) const
  {
    return static_cast<ctkXnatTreeItem*>(index.internalPointer());
  }

  QScopedPointer<ctkXnatTreeItem> m_RootItem;

};

//----------------------------------------------------------------------------
ctkXnatTreeModel::ctkXnatTreeModel()
: d_ptr(new ctkXnatTreeModelPrivate())
{
}

//----------------------------------------------------------------------------
ctkXnatTreeModel::~ctkXnatTreeModel()
{
}

//----------------------------------------------------------------------------
// returns name (project, subject, etc.) for row and column of
//   parent in index if role is Qt::DisplayRole
QVariant ctkXnatTreeModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
  {
    return QVariant();
  }

  if (role == Qt::TextAlignmentRole)
  {
    return QVariant(int(Qt::AlignTop | Qt::AlignLeft));
  }
  else if (role == Qt::DisplayRole)
  {
    ctkXnatObject* xnatObject = this->xnatObject(index);

    QString displayData = xnatObject->name();
    if (displayData.isEmpty())
    {
      displayData = xnatObject->property(ctkXnatObjectFields::LABEL);
    }
    return displayData;
  }
  else if (role == Qt::ToolTipRole)
  {
    return this->xnatObject(index)->description();
  }
  else if (role == Qt::UserRole)
  {
    return QVariant::fromValue<ctkXnatObject*>(this->xnatObject(index));
  }

  return QVariant();
}

//----------------------------------------------------------------------------
QModelIndex ctkXnatTreeModel::index(int row, int column, const QModelIndex& index) const
{
  if (!this->hasIndex(row, column, index))
  {
    return QModelIndex();
  }

  Q_D(const ctkXnatTreeModel);
  ctkXnatTreeItem* item;
  if (!index.isValid())
  {
    item = d->m_RootItem.data();
  }
  else
  {
    item = d->itemAt(index);
  }

  ctkXnatTreeItem* childItem = item->child(row);

  if (childItem)
  {
    return this->createIndex(row, column, childItem);
  }

  return QModelIndex();
}

//----------------------------------------------------------------------------
QModelIndex ctkXnatTreeModel::parent(const QModelIndex& index) const
{
  if (!index.isValid())
  {
    return QModelIndex();
  }

  Q_D(const ctkXnatTreeModel);
  ctkXnatTreeItem* item = d->itemAt(index);
  ctkXnatTreeItem* parentItem = item->parent();

  if (parentItem == d->m_RootItem.data())
  {
    return QModelIndex();
  }

  return this->createIndex(parentItem->row(), 0, parentItem);
}

//----------------------------------------------------------------------------
int ctkXnatTreeModel::rowCount(const QModelIndex& index) const
{
  if (index.column() > 0)
  {
    return 0;
  }

  Q_D(const ctkXnatTreeModel);
  ctkXnatTreeItem* item;
  if (!index.isValid())
  {
    item = d->m_RootItem.data();
  }
  else
  {
    item = d->itemAt(index);
  }

  return item->childCount();
}

//----------------------------------------------------------------------------
int ctkXnatTreeModel::columnCount(const QModelIndex& index) const
{
  Q_UNUSED(index);
  return 1;
}

//----------------------------------------------------------------------------
// defer request for children until actually needed by QTreeView object
bool ctkXnatTreeModel::hasChildren(const QModelIndex& index) const
{
  Q_D(const ctkXnatTreeModel);
  if (!index.isValid())
  {
    return d->m_RootItem->childCount() > 0;
  }

  ctkXnatTreeItem* item = d->itemAt(index);
  return !item->xnatObject()->isFetched() || !item->xnatObject()->children().isEmpty();
}

//----------------------------------------------------------------------------
bool ctkXnatTreeModel::canFetchMore(const QModelIndex& index) const
{
  if (!index.isValid())
  {
    return false;
  }

  Q_D(const ctkXnatTreeModel);
  ctkXnatTreeItem* item = d->itemAt(index);
  return !(item->childCount() > 0);
}

//----------------------------------------------------------------------------
void ctkXnatTreeModel::fetchMore(const QModelIndex& index)
{
  if (!index.isValid())
  {
    return;
  }

  Q_D(const ctkXnatTreeModel);
  ctkXnatTreeItem* item = d->itemAt(index);

  ctkXnatObject* xnatObject = item->xnatObject();

  xnatObject->fetch();

  QList<ctkXnatObject*> children = xnatObject->children();
  if (!children.isEmpty())
  {
    beginInsertRows(index, 0, children.size() - 1);
    foreach (ctkXnatObject* child, children)
    {
      item->appendChild(new ctkXnatTreeItem(child, item));
    }
    endInsertRows();
  }
}

//----------------------------------------------------------------------------
ctkXnatObject* ctkXnatTreeModel::xnatObject(const QModelIndex& index) const
{
  Q_D(const ctkXnatTreeModel);
  return d->itemAt(index)->xnatObject();
}

//----------------------------------------------------------------------------
void ctkXnatTreeModel::addDataModel(ctkXnatDataModel* dataModel)
{
  Q_D(ctkXnatTreeModel);
  d->m_RootItem->appendChild(new ctkXnatTreeItem(dataModel, d->m_RootItem.data()));
}

//----------------------------------------------------------------------------
void ctkXnatTreeModel::removeDataModel(ctkXnatDataModel* dataModel)
{
  Q_D(ctkXnatTreeModel);
  d->m_RootItem->remove(dataModel);
}

//----------------------------------------------------------------------------
bool ctkXnatTreeModel::removeAllRows(const QModelIndex& parent)
{
  // do nothing for the root
  if ( !parent.isValid() )
  {
    return false;
  }

  ctkXnatObject* xnatObject = this->xnatObject(parent);

  // nt: not sure why the parent.row() is used here instead of the first item in list
  // that is xnatObject->children()[0];
  ctkXnatObject* child = xnatObject->children()[parent.row()];

  if ( child == NULL )
  {
    return false;
  }

  int numberofchildren = child->children().size();
  if (numberofchildren > 0)
  {
    beginRemoveRows(parent, 0, numberofchildren - 1);
    // xnatObject->removeChild(parent.row());
    // nt: not sure if this is the right implementation here, should iterate ?
    xnatObject->remove(child);
    endRemoveRows();
  }
  else
  {
    // xnatObject->removeChild(parent.row());
    // nt: not sure if this is the right implementation here, should iterate ?
    xnatObject->remove(child);
  }
  return true;
}

//----------------------------------------------------------------------------
void ctkXnatTreeModel::downloadFile(const QModelIndex& index, const QString& zipFileName)
{
  if (!index.isValid())
  {
    return;
  }

  this->xnatObject(index)->download(zipFileName);

  return;
}

//----------------------------------------------------------------------------
void ctkXnatTreeModel::uploadFile(const QModelIndex& index, const QString& zipFileName)
{
  if (!index.isValid())
  {
    return;
  }

  ctkXnatObject* xnatObject = this->xnatObject(index);
  ctkXnatObject* child = xnatObject->children()[index.row()];

  child->upload(zipFileName);
}
