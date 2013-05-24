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

#include "ctkXnatProjectListModel.h"

#include "ctkXnatProject.h"

#include <QDebug>

ctkXnatProjectListModel::ctkXnatProjectListModel()
{
}

void ctkXnatProjectListModel::setRootObject(const ctkXnatObject::Pointer& root)
{
  rootObject = root;
}

int ctkXnatProjectListModel::rowCount(const QModelIndex& /*parent*/) const
{
  if (!rootObject) return 0;
  return rootObject->getChildren().size();
}

QVariant ctkXnatProjectListModel::data(const QModelIndex& index, int role) const
{
  if (!rootObject) return QVariant();

  if (role == Qt::DisplayRole)
  {
    ctkXnatObject::Pointer child = rootObject->getChildren().at(index.row());
    if (child.isNull())
    {
      qWarning() << "child at index" << index << "is NULL!";
    }
    else
    {
      QString displayData = child->getName();
      if (displayData.isEmpty())
      {
        displayData = child->getId();
      }
      return displayData;
    }
  }
  else if (role == Qt::UserRole)
  {
    return QVariant::fromValue(rootObject->getChildren().at(index.row()));
  }
  return QVariant();
}

QVariant ctkXnatProjectListModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int role) const
{
  if (role == Qt::DisplayRole)
  {
    if (!rootObject) return QString("Unavailable");
    return QString("Bla");
  }
  return QVariant();
}

