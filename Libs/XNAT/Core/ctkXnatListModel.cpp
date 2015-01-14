/*=============================================================================

  Library: XNAT/Core

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

#include "ctkXnatConstants.h"
#include "ctkXnatExperiment.h"
#include "ctkXnatListModel.h"
#include "ctkXnatProject.h"
#include "ctkXnatScan.h"
#include "ctkXnatScanFolder.h"
#include "ctkXnatSubject.h"

#include <iostream>
#include <typeinfo>

#include <QDebug>


//----------------------------------------------------------------------------
ctkXnatListModel::ctkXnatListModel()
  : RootObject(0)
{
}

//----------------------------------------------------------------------------
void ctkXnatListModel::setRootObject(ctkXnatObject* root)
{
  RootObject = root;
}

//----------------------------------------------------------------------------
ctkXnatObject* ctkXnatListModel::rootObject()
{
  return RootObject;
}

//----------------------------------------------------------------------------
int ctkXnatListModel::rowCount(const QModelIndex& /*parent*/) const
{
  if (!RootObject) return 0;
  return RootObject->children().size();
}

//----------------------------------------------------------------------------
QVariant ctkXnatListModel::data(const QModelIndex& index, int role) const
{
  if (!RootObject) return QVariant();

  if (role == Qt::DisplayRole)
  {
    ctkXnatObject* child = RootObject->children().at(index.row());
    QString displayData = child->name();
    if (displayData.isEmpty())
    {
      displayData = child->property(ctkXnatObjectFields::LABEL);
    }
    return displayData;
  }
  else if (role == Qt::UserRole)
  {
    return QVariant::fromValue(RootObject->children().at(index.row()));
  }
  return QVariant();
}

//----------------------------------------------------------------------------
QVariant ctkXnatListModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int role) const
{
  if (role == Qt::DisplayRole)
  {
    if (!RootObject) return QString("Unavailable");

    return RootObject->childDataType();
  }
  return QVariant();
}
