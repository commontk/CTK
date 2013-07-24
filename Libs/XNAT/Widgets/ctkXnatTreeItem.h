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

#ifndef ctkXnatTreeItem_h
#define ctkXnatTreeItem_h

#include "ctkXnatObject.h"

#include <QList>
#include <QVariant>

class ctkXnatTreeItem;

class ctkXnatTreeItem
{
public:

  explicit ctkXnatTreeItem();
  explicit ctkXnatTreeItem(ctkXnatObject::Pointer xnatObject, ctkXnatTreeItem* parentItem = 0);
  virtual ~ctkXnatTreeItem();

  ctkXnatObject::Pointer xnatObject() const;

  void appendChild(ctkXnatTreeItem* child);
  void removeChildren();

  ctkXnatTreeItem* child(int row);
  int childCount() const;
  int columnCount() const;
  QVariant data(int column) const;
  int row() const;
  ctkXnatTreeItem* parent();

private:

  ctkXnatObject::Pointer m_XnatObject;

  ctkXnatTreeItem* m_ParentItem;
  QList<ctkXnatTreeItem*> m_ChildItems;
};

#endif
