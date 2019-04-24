/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QStandardItem>

// CTK includes
#include "ctkErrorLogModel.h"


// --------------------------------------------------------------------------
// ctkErrorLogModelPrivate

// --------------------------------------------------------------------------
class ctkErrorLogModelPrivate
{
  Q_DECLARE_PUBLIC(ctkErrorLogModel);
protected:
  ctkErrorLogModel* const q_ptr;
public:
  ctkErrorLogModelPrivate(ctkErrorLogModel& object);
  ~ctkErrorLogModelPrivate();
};

// --------------------------------------------------------------------------
// ctkErrorLogModelPrivate methods

// --------------------------------------------------------------------------
ctkErrorLogModelPrivate::ctkErrorLogModelPrivate(ctkErrorLogModel& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
ctkErrorLogModelPrivate::~ctkErrorLogModelPrivate()
{
}

// --------------------------------------------------------------------------
// ctkErrorLogModel methods

//------------------------------------------------------------------------------
ctkErrorLogModel::ctkErrorLogModel(QObject * parentObject)
  : Superclass(new QStandardItemModel(), parentObject)
  , d_ptr(new ctkErrorLogModelPrivate(*this))
{
  QStandardItemModel* itemModel = qobject_cast<QStandardItemModel*>(this->sourceModel());
  Q_ASSERT(itemModel);
  itemModel->setColumnCount(ctkErrorLogAbstractModel::MaxColumn);
}

//------------------------------------------------------------------------------
ctkErrorLogModel::~ctkErrorLogModel()
{
}

//------------------------------------------------------------------------------
void ctkErrorLogModel::addModelEntry(const QString& currentDateTime, const QString& threadId,
                                     const QString& logLevel, const QString& origin, const QString& text)
{
  QList<QStandardItem*> itemList;

  // Time item
  QStandardItem * timeItem = new QStandardItem(currentDateTime);
  timeItem->setEditable(false);
  itemList << timeItem;

  // ThreadId item
  QStandardItem * threadIdItem = new QStandardItem(threadId);
  threadIdItem->setEditable(false);
  itemList << threadIdItem;

  // LogLevel item
  QStandardItem * logLevelItem = new QStandardItem(logLevel);
  logLevelItem->setEditable(false);
  itemList << logLevelItem;

  // Origin item
  QStandardItem * originItem = new QStandardItem(origin);
  originItem->setEditable(false);
  itemList << originItem;

  // Description item
  QStandardItem * descriptionItem = new QStandardItem();
  QString descriptionText(text);
  descriptionItem->setData(descriptionText.left(160).append((descriptionText.size() > 160) ? "..." : ""), Qt::DisplayRole);
  descriptionItem->setData(descriptionText, ctkErrorLogModel::DescriptionTextRole);
  descriptionItem->setEditable(false);
  itemList << descriptionItem;

  QStandardItemModel* itemModel = qobject_cast<QStandardItemModel*>(this->sourceModel());
  Q_ASSERT(itemModel);
  itemModel->invisibleRootItem()->appendRow(itemList);
}

