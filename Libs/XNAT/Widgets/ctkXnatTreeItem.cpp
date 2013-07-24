/*=============================================================================

  Plugin: org.commontk.xnat

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

#include "ctkXnatTreeItem.h"

#include "ctkXnatObject.h"
#include "ctkXnatTreeItem.h"

ctkXnatTreeItem::ctkXnatTreeItem()
: m_XnatObject(0)
, m_ParentItem(0)
{
}

ctkXnatTreeItem::ctkXnatTreeItem(ctkXnatObject::Pointer xnatObject, ctkXnatTreeItem* parentItem)
: m_XnatObject(xnatObject)
, m_ParentItem(parentItem)
{
}

ctkXnatTreeItem::~ctkXnatTreeItem()
{
  qDeleteAll(m_ChildItems);
}

ctkXnatObject::Pointer ctkXnatTreeItem::xnatObject() const
{
  return m_XnatObject;
}

void ctkXnatTreeItem::appendChild(ctkXnatTreeItem* item)
{
  m_ChildItems.append(item);
  item->m_ParentItem = this;
}

void ctkXnatTreeItem::removeChildren()
{
  qDeleteAll(m_ChildItems);
}

ctkXnatTreeItem* ctkXnatTreeItem::child(int row)
{
  return m_ChildItems.value(row);
}

int ctkXnatTreeItem::childCount() const
{
  return m_ChildItems.count();
}

int ctkXnatTreeItem::row() const
{
  return m_ParentItem->m_ChildItems.indexOf(const_cast<ctkXnatTreeItem*>(this));
}

int ctkXnatTreeItem::columnCount() const
{
  return 1;
}

QVariant ctkXnatTreeItem::data(int column) const
{
  Q_UNUSED(column);
  return m_XnatObject->getName();
}

ctkXnatTreeItem* ctkXnatTreeItem::parent()
{
  return m_ParentItem;
}
