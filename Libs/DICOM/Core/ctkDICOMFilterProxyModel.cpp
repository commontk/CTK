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

#include "ctkDICOMFilterProxyModel.h"

#include "ctkDICOMModel.h"

//logger
#include <ctkLogger.h>
static ctkLogger logger("org.commontk.DICOM.Core.ctkDICOMFilterProxyModel");


//----------------------------------------------------------------------------
class ctkDICOMFilterProxyModelPrivate
{
protected:
  ctkDICOMFilterProxyModel* const q_ptr;
  Q_DECLARE_PUBLIC(ctkDICOMFilterProxyModel);

public:
  ctkDICOMFilterProxyModelPrivate(ctkDICOMFilterProxyModel* parent = 0);

  QString searchTextName;
  QString searchTextStudy;
  QString searchTextSeries;
  QString searchTextID;
};

//----------------------------------------------------------------------------
ctkDICOMFilterProxyModelPrivate::ctkDICOMFilterProxyModelPrivate(ctkDICOMFilterProxyModel* parent): q_ptr(parent){

}

//----------------------------------------------------------------------------
ctkDICOMFilterProxyModel::ctkDICOMFilterProxyModel(QObject *parent):Superclass(parent),
    d_ptr(new ctkDICOMFilterProxyModelPrivate(this))
{

}

//----------------------------------------------------------------------------
ctkDICOMFilterProxyModel::~ctkDICOMFilterProxyModel(){

}

//----------------------------------------------------------------------------
void ctkDICOMFilterProxyModel::setNameSearchText(const QString &text){
    Q_D(ctkDICOMFilterProxyModel);
    d->searchTextName = text;
    this->invalidateFilter();
}

//----------------------------------------------------------------------------
void ctkDICOMFilterProxyModel::setStudySearchText(const QString &text){
    Q_D(ctkDICOMFilterProxyModel);
    d->searchTextStudy = text;
    this->invalidateFilter();
}

//----------------------------------------------------------------------------
void ctkDICOMFilterProxyModel::setSeriesSearchText(const QString &text){
    Q_D(ctkDICOMFilterProxyModel);
    d->searchTextSeries = text;
    this->invalidateFilter();
}

//----------------------------------------------------------------------------
void ctkDICOMFilterProxyModel::setIdSearchText(const QString &text){
    Q_D(ctkDICOMFilterProxyModel);
    d->searchTextID = text;
    this->invalidateFilter();
}

bool ctkDICOMFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const{
    Q_D(const ctkDICOMFilterProxyModel);

    ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(this->sourceModel()));

    if(model){
        QModelIndex index = model->index(source_row, 0, source_parent);
        if(model->data(index, ctkDICOMModel::TypeRole) == static_cast<int>(ctkDICOMModel::PatientType)){
            QRegExp regExp = QRegExp(d->searchTextName);
            if(model->data(index, Qt::DisplayRole).toString().contains(regExp)){
                return true;
            }else{
                return false;
            }
        }else if(model->data(index, ctkDICOMModel::TypeRole) == static_cast<int>(ctkDICOMModel::StudyType)){
            QRegExp regExp = QRegExp(d->searchTextStudy);
            if(model->data(index, Qt::DisplayRole).toString().contains(regExp)){
                return true;
            }else{
                return false;
            }
        }else if(model->data(index, ctkDICOMModel::TypeRole) == static_cast<int>(ctkDICOMModel::SeriesType)){
            QRegExp regExp = QRegExp(d->searchTextSeries);
            if(model->data(index, Qt::DisplayRole).toString().contains(regExp)){
                return true;
            }else{
                return false;
            }
        }
    }

    return true;
}
