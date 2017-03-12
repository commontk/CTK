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
#include <QDebug>

// CTK includes
#include "ctkVTKDataSetArrayComboBox.h"

// VTK includes
#include <vtkAbstractArray.h>

//-----------------------------------------------------------------------------
class ctkVTKDataSetArrayComboBoxPrivate
{
  Q_DECLARE_PUBLIC(ctkVTKDataSetArrayComboBox);
protected:
  ctkVTKDataSetArrayComboBox* const q_ptr;
public:
  ctkVTKDataSetArrayComboBoxPrivate(ctkVTKDataSetArrayComboBox& object);
  void init();
  ctkVTKDataSetModel* dataSetModel()const;

  int indexFromArrayName(const QString& dataArrayName)const;
  int indexFromArray(vtkAbstractArray* dataArray)const;
  vtkAbstractArray* arrayFromIndex(int index)const;
  QString arrayNameFromIndex(int index)const;
  int locationFromIndex(int index)const;
};

// --------------------------------------------------------------------------
// ctkVTKDataSetArrayComboBoxPrivate methods

// --------------------------------------------------------------------------
ctkVTKDataSetArrayComboBoxPrivate::ctkVTKDataSetArrayComboBoxPrivate(ctkVTKDataSetArrayComboBox& object)
  :q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void ctkVTKDataSetArrayComboBoxPrivate::init()
{
  Q_Q(ctkVTKDataSetArrayComboBox);
  q->setModel(new ctkVTKDataSetModel(q));
  QObject::connect(q, SIGNAL(currentIndexChanged(int)),
                   q, SLOT(onCurrentIndexChanged(int)));
}

// --------------------------------------------------------------------------
ctkVTKDataSetModel* ctkVTKDataSetArrayComboBoxPrivate::dataSetModel()const
{
  Q_Q(const ctkVTKDataSetArrayComboBox);
  return qobject_cast<ctkVTKDataSetModel*>(q->model());
}

// --------------------------------------------------------------------------
int ctkVTKDataSetArrayComboBoxPrivate::indexFromArrayName(const QString& dataArrayName)const
{
  Q_Q(const ctkVTKDataSetArrayComboBox);
  return q->findText(dataArrayName);
}

// --------------------------------------------------------------------------
int ctkVTKDataSetArrayComboBoxPrivate::indexFromArray(vtkAbstractArray* dataArray)const
{
  return this->dataSetModel()->indexFromArray(dataArray,0).row();
}

// --------------------------------------------------------------------------
vtkAbstractArray* ctkVTKDataSetArrayComboBoxPrivate::arrayFromIndex(int index)const
{
  return this->dataSetModel()->arrayFromIndex(
    this->dataSetModel()->index(index, 0));
}

// --------------------------------------------------------------------------
QString ctkVTKDataSetArrayComboBoxPrivate::arrayNameFromIndex(int index)const
{
  vtkAbstractArray* dataArray = this->arrayFromIndex(index);
  return dataArray ? dataArray->GetName() : "";
}

// --------------------------------------------------------------------------
int ctkVTKDataSetArrayComboBoxPrivate::locationFromIndex(int index)const
{
  return this->dataSetModel()->locationFromIndex(
    this->dataSetModel()->index(index, 0));
}

// --------------------------------------------------------------------------
// ctkVTKDataSetArrayComboBox methods

// --------------------------------------------------------------------------
ctkVTKDataSetArrayComboBox::ctkVTKDataSetArrayComboBox(QWidget* _parent)
  : Superclass(_parent)
  , d_ptr(new ctkVTKDataSetArrayComboBoxPrivate(*this))
{
  Q_D(ctkVTKDataSetArrayComboBox);
  d->init();
}

// --------------------------------------------------------------------------
ctkVTKDataSetArrayComboBox::~ctkVTKDataSetArrayComboBox()
{
}

// --------------------------------------------------------------------------
vtkAbstractArray* ctkVTKDataSetArrayComboBox::currentArray()const
{
  Q_D(const ctkVTKDataSetArrayComboBox);
  return d->arrayFromIndex(this->currentIndex());
}

// --------------------------------------------------------------------------
QString ctkVTKDataSetArrayComboBox::currentArrayName()const
{
  vtkAbstractArray* dataArray = this->currentArray();
  return dataArray ? dataArray->GetName() : "";
}

// --------------------------------------------------------------------------
int ctkVTKDataSetArrayComboBox::currentArrayLocation()const
{
  Q_D(const ctkVTKDataSetArrayComboBox);
  return d->locationFromIndex(this->currentIndex());
}

// --------------------------------------------------------------------------
void ctkVTKDataSetArrayComboBox::setDataSet(vtkDataSet* dataSet)
{
  Q_D(ctkVTKDataSetArrayComboBox);
  d->dataSetModel()->setDataSet(dataSet);
}

// --------------------------------------------------------------------------
vtkDataSet* ctkVTKDataSetArrayComboBox::dataSet()const
{
  Q_D(const ctkVTKDataSetArrayComboBox);
  return d->dataSetModel()->dataSet();
}

// --------------------------------------------------------------------------
ctkVTKDataSetModel::AttributeTypes ctkVTKDataSetArrayComboBox::attributeTypes()const
{
  return this->dataSetModel()->attributeTypes();
}

// --------------------------------------------------------------------------
void ctkVTKDataSetArrayComboBox::setAttributeTypes(const ctkVTKDataSetModel::AttributeTypes& attributeTypes)
{
  this->dataSetModel()->setAttributeTypes(attributeTypes);
}

// ----------------------------------------------------------------------------
bool ctkVTKDataSetArrayComboBox::noneEnabled()const
{
  return this->dataSetModel()->includeNullItem();
}

// ----------------------------------------------------------------------------
void ctkVTKDataSetArrayComboBox::setNoneEnabled(bool noneEnabled)
{
  return this->dataSetModel()->setIncludeNullItem(noneEnabled);
}

// --------------------------------------------------------------------------
ctkVTKDataSetModel* ctkVTKDataSetArrayComboBox::dataSetModel()const
{
  return qobject_cast<ctkVTKDataSetModel*>(this->model());
}

// --------------------------------------------------------------------------
void ctkVTKDataSetArrayComboBox::setCurrentArray(vtkAbstractArray* dataArray)
{
  Q_D(ctkVTKDataSetArrayComboBox);
  this->setCurrentIndex(d->indexFromArray(dataArray));
}

// --------------------------------------------------------------------------
void ctkVTKDataSetArrayComboBox::setCurrentArray(const QString& dataArrayName)
{
  Q_D(ctkVTKDataSetArrayComboBox);
  this->setCurrentIndex(d->indexFromArrayName(dataArrayName));
}

// --------------------------------------------------------------------------
void ctkVTKDataSetArrayComboBox::onCurrentIndexChanged(int index)
{
  Q_D(ctkVTKDataSetArrayComboBox);
  emit currentArrayChanged(d->arrayNameFromIndex(index));
  emit currentArrayChanged(d->arrayFromIndex(index));
}
