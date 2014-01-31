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

#include "ctkXnatDataModel.h"
#include "ctkXnatListModel.h"
#include "ctkXnatProject.h"
#include "ctkXnatSubject.h"
#include "ctkXnatExperiment.h"
#include "ctkXnatScanFolder.h"
#include "ctkXnatScan.h"
#include "ctkXnatScanResource.h"

#include <iostream>
#include <typeinfo>

#include <QDebug>

ctkXnatListModel::ctkXnatListModel()
  : rootObject(0)
{
}

void ctkXnatListModel::setRootObject(ctkXnatObject* root)
{
  rootObject = root;
}

ctkXnatObject* ctkXnatListModel::getRootObject()
{
  return rootObject;
}

int ctkXnatListModel::rowCount(const QModelIndex& /*parent*/) const
{
  if (!rootObject) return 0;
  return rootObject->children().size();
}

QVariant ctkXnatListModel::data(const QModelIndex& index, int role) const
{
  if (!rootObject) return QVariant();

  if (role == Qt::DisplayRole)
  {
    ctkXnatObject* child = rootObject->children().at(index.row());
    if (!child)
    {
      qWarning() << "child at index" << index << "is NULL!";
    }
    else
    {
      QString displayData = child->name();
      if (displayData.isEmpty())
      {
        displayData = child->id();
      }
      return displayData;
    }
  }
  else if (role == Qt::UserRole)
  {
    return QVariant::fromValue(rootObject->children().at(index.row()));
  }
  return QVariant();
}

QVariant ctkXnatListModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int role) const
{
  if (role == Qt::DisplayRole)
  {
    if (!rootObject) return QString("Unavailable");

    if( dynamic_cast<ctkXnatDataModel*>(rootObject) != NULL )
    {
      return QString("Projects");
    }
    else if( dynamic_cast<ctkXnatProject*>(rootObject) != NULL )
    {
      return QString("Subjects");
    }
    else if( dynamic_cast<ctkXnatSubject*>(rootObject) != NULL )
    {
      return QString("Experiments");
    }
    else if( dynamic_cast<ctkXnatExperiment*>(rootObject) != NULL )
    {
      return QString("Kinds of data");
    }
    else if( dynamic_cast<ctkXnatScanFolder*>(rootObject) != NULL )
    {
      return QString("Image Sessions");
    }
    else if( dynamic_cast<ctkXnatScan*>(rootObject) != NULL )
    {
      return QString("Resource Folders");
    }
    else if( dynamic_cast<ctkXnatScanResource*>(rootObject) != NULL )
    {
      return QString("Files");
    }
    else
    {
      return QString("ERROR");
    }
  }
  return QVariant();
}
