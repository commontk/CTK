/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QHBoxLayout>
#include <QDoubleSpinBox>

// CTK includes
#include "ctkCoordinatesWidget.h"

//------------------------------------------------------------------------------
ctkCoordinatesWidget::ctkCoordinatesWidget(QWidget* _parent) :QWidget(_parent)
{
  this->Minimum = -100000.;
  this->Maximum = 100000.;
  this->SingleStep = 1.;
  this->Dimension = 3;
  this->Coordinates = new double [this->Dimension];
  
  QHBoxLayout* hboxLayout = new QHBoxLayout(this);
  this->setLayout(hboxLayout);
  for (int i = 0; i < this->Dimension; ++i)
    {
    this->Coordinates[i] = 0.;
    this->addSpinBox();
    }
  hboxLayout->setContentsMargins(0, 0, 0, 0);
}

//------------------------------------------------------------------------------
ctkCoordinatesWidget::~ctkCoordinatesWidget()
{
  delete [] this->Coordinates;
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::addSpinBox()
{
  QDoubleSpinBox* spinBox = new QDoubleSpinBox(this);
  spinBox->setMinimum(this->Minimum);
  spinBox->setMaximum(this->Maximum);
  spinBox->setSingleStep(this->SingleStep);
  connect( spinBox, SIGNAL(valueChanged(double)), 
           this, SLOT(coordinateChanged(double)));
  this->layout()->addWidget(spinBox);
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::setDimension(int dim)
{
  if (dim < 1)
    {
    return;
    }
  double* newPos = new double[dim];
  if (dim > this->Dimension)
    {
    memcpy(newPos, this->Coordinates, this->Dimension * sizeof(double));
    for (int i = this->Dimension; i < dim; ++i)
      {
      newPos[i] = 0.;
      this->addSpinBox();
      }
    }
  else
    {
    memcpy(newPos, this->Coordinates, dim * sizeof(double));
    for (int i = this->Dimension - 1 ; i >= dim; --i)
      {
      QLayoutItem* item = this->layout()->takeAt(i);
      QWidget* widget = item ? item->widget() : 0;
      delete item;
      delete widget;
      }
    }
  delete [] this->Coordinates;
  this->Coordinates = newPos;
  this->Dimension = dim;

  this->updateGeometry();
  
  this->coordinatesChanged();
}

//------------------------------------------------------------------------------
int ctkCoordinatesWidget::dimension() const
{
  return this->Dimension;
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::setMinimum(double min)
{
  for (int i = 0; this->layout()->itemAt(i); ++i)
    {
    QLayoutItem* item = this->layout()->itemAt(i);
    QDoubleSpinBox* spinBox = item ? dynamic_cast<QDoubleSpinBox*>(
      item->widget()) : 0;
    if (spinBox)
      {
      spinBox->setMinimum(min);
      }
    }
  this->Minimum = min;
}

//------------------------------------------------------------------------------
double ctkCoordinatesWidget::minimum() const
{
  return this->Minimum;
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::setMaximum(double max)
{
  for (int i = 0; this->layout()->itemAt(i); ++i)
    {
    QLayoutItem* item = this->layout()->itemAt(i);
    QDoubleSpinBox* spinBox = item ? dynamic_cast<QDoubleSpinBox*>(
      item->widget()) : 0;
    if (spinBox)
      {
      spinBox->setMaximum(max);
      }
    }
  this->Maximum = max;
}

//------------------------------------------------------------------------------
double ctkCoordinatesWidget::maximum() const
{
  return this->Maximum;
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::setSingleStep(double step)
{
  for (int i = 0; this->layout()->itemAt(i); ++i)
    {
    QLayoutItem* item = this->layout()->itemAt(i);
    QDoubleSpinBox* spinBox = item ? dynamic_cast<QDoubleSpinBox*>(
      item->widget()) : 0;
    if (spinBox)
      {
      spinBox->setSingleStep(step);
      }
    }
  this->SingleStep = step;
}

//------------------------------------------------------------------------------
double ctkCoordinatesWidget::singleStep() const
{
  return this->SingleStep;
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::setCoordinatesAsString(QString _pos)
{
  QStringList posList = _pos.split(',');
  if (posList.count() != this->Dimension)
    {
    return;
    }
  double* newPos = new double[this->Dimension];
  for (int i = 0; i < this->Dimension; ++i)
    {
    newPos[i] = posList[i].toDouble();
    }
  this->setCoordinates(newPos);
  delete [] newPos;
}

//------------------------------------------------------------------------------
QString ctkCoordinatesWidget::coordinatesAsString()const
{
  QString res;
  for (int i = 0; i < this->Dimension; ++i)
    {
    if (i != 0)
      {
      res += ",";
      }
    res += QString::number(this->Coordinates[i]);
    }
  return res;
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::setCoordinates(double* _pos)
{
  for (int i = 0; i < this->Dimension; ++i)
    {
    this->Coordinates[i] = _pos[i];
    }
  bool blocked = this->blockSignals(true);
  for (int i = 0; i < this->Dimension; ++i)
    {
    QLayoutItem* item = this->layout()->itemAt(i);
    QDoubleSpinBox* spinBox = 
      item ? dynamic_cast<QDoubleSpinBox*>(item->widget()) : 0;
    if (spinBox)
      {
      spinBox->setValue(this->Coordinates[i]);
      }
    }
  this->blockSignals(blocked);
  emit valueChanged(this->Coordinates);
}

//------------------------------------------------------------------------------
double* ctkCoordinatesWidget::coordinates()const
{
  return this->Coordinates;
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::coordinateChanged(double coordinate)
{
  for (int i = 0; i < this->Dimension; ++i)
    {
    QLayoutItem* item = this->layout()->itemAt(i);
    QObject* spinBox = 
      item ? dynamic_cast<QObject*>(item->widget()) : 0;
    if ( spinBox && spinBox == this->sender())
      {
      this->Coordinates[i] = coordinate;
      }
    }
  emit valueChanged(this->Coordinates);
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::coordinatesChanged()
{
  for (int i = 0; i < this->Dimension; ++i)
    {
    QLayoutItem* item = this->layout()->itemAt(i);
    QSpinBox* spinBox = 
      item ? dynamic_cast<QSpinBox*>(item->widget()) : 0;
    if ( spinBox)
      {
      this->Coordinates[i] = spinBox->value();
      }
    }
  emit valueChanged(this->Coordinates);
}
