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
#include <QDoubleSpinBox>
#include <QHBoxLayout>

// CTK includes
#include "ctkCoordinatesWidget.h"

// STD includes
#include <cmath>

//------------------------------------------------------------------------------
ctkCoordinatesWidget::ctkCoordinatesWidget(QWidget* _parent) :QWidget(_parent)
{
  this->Decimals = 3;
  this->SingleStep = 1.;
  this->Minimum = -100000.;
  this->Maximum = 100000.;
  this->Normalized = false;
  this->Dimension = 0;
  this->Coordinates = 0;

  QHBoxLayout* hboxLayout = new QHBoxLayout(this);
  hboxLayout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(hboxLayout);

  this->setDimension(3);
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
  spinBox->setDecimals(this->Decimals);
  spinBox->setSingleStep(this->SingleStep);
  spinBox->setMinimum(this->Minimum);
  spinBox->setMaximum(this->Maximum);
  connect( spinBox, SIGNAL(valueChanged(double)),
           this, SLOT(updateCoordinate(double)));
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
      this->LastUserEditedCoordinates.push_back(i);
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
      this->LastUserEditedCoordinates.pop_back();
      }
    }
  delete [] this->Coordinates;
  this->Coordinates = newPos;
  this->Dimension = dim;

  this->updateGeometry();
  
  this->updateCoordinates();
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
void ctkCoordinatesWidget::setNormalized(bool normalized)
{
  this->Normalized = normalized;
  if (this->Normalized)
    {
    double* normalizedCoordinates = new double[this->Dimension];
    memcpy(normalizedCoordinates, this->Coordinates, sizeof(double)*this->Dimension);
    ctkCoordinatesWidget::normalize(normalizedCoordinates, this->Dimension);

    this->setMinimum(-1.);
    this->setMaximum(1.);

    this->setCoordinates(normalizedCoordinates);
    delete [] normalizedCoordinates;
    }
}

//------------------------------------------------------------------------------
bool ctkCoordinatesWidget::isNormalized() const
{
  return this->Normalized;
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::setDecimals(int newDecimals)
{
  this->Decimals = newDecimals;
  for (int i = 0; this->layout()->itemAt(i); ++i)
    {
    QLayoutItem* item = this->layout()->itemAt(i);
    QDoubleSpinBox* spinBox = item ? dynamic_cast<QDoubleSpinBox*>(
      item->widget()) : 0;
    if (spinBox)
      {
      spinBox->setDecimals(newDecimals);
      }
    }
}

//------------------------------------------------------------------------------
int ctkCoordinatesWidget::decimals() const
{
  return this->Decimals;
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
void ctkCoordinatesWidget::setCoordinates(double* coordinates)
{
  for (int i = 0; i < this->Dimension; ++i)
    {
    this->Coordinates[i] = coordinates[i];
    }
  if (this->Normalized)
    {
    this->normalize(this->Coordinates, this->Dimension);
    }
  bool blocked = this->blockSignals(true);
  for (int i = 0; i < this->Dimension; ++i)
    {
    QLayoutItem* item = this->layout()->itemAt(i);
    QDoubleSpinBox* spinBox =
      item ? dynamic_cast<QDoubleSpinBox*>(item->widget()) : 0;
    if (spinBox)
      {
      // we don't want updateCoordinate() to be called.
      // it could mess with the LastUserEditedCoordinates list.
      bool spinBoxSignalWasBlocked = spinBox->blockSignals(true);
      spinBox->setValue(this->Coordinates[i]);
      spinBox->blockSignals(spinBoxSignalWasBlocked);
      }
    }
  this->blockSignals(blocked);
  this->updateCoordinates();
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::setCoordinates(double x, double y, double z, double w)
{
  double* coordinates = new double[this->Dimension];
  if (this->Dimension >= 1)
    {
    coordinates[0] = x;
    }
  if (this->Dimension >= 2)
    {
    coordinates[1] = y;
    }
  if (this->Dimension >= 3)
    {
    coordinates[2] = z;
    }
  if (this->Dimension >= 4)
    {
    coordinates[3] = w;
    }
  for (int i = 4; i < this->Dimension; ++i)
    {
    coordinates[i] = this->Coordinates[i];
    }
  this->setCoordinates(coordinates);
  delete [] coordinates;
}

//------------------------------------------------------------------------------
double const * ctkCoordinatesWidget::coordinates()const
{
  return this->Coordinates;
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::updateCoordinate(double coordinate)
{
  int element = -1;
  for (int i = 0; i < this->Dimension; ++i)
    {
    QLayoutItem* item = this->layout()->itemAt(i);
    QDoubleSpinBox* spinBox =
      item ? qobject_cast<QDoubleSpinBox*>(item->widget()) : 0;
    if ( spinBox && spinBox == this->sender())
      {
      this->Coordinates[i] = coordinate;
      element = i;
      }
    }
  Q_ASSERT(element != -1);
  // Update the last edited history by push first the element.
  for (int i = this->Dimension -1; i > 0; --i)
    {
    if (this->LastUserEditedCoordinates[i] == element)
      {
      this->LastUserEditedCoordinates.swap(i,i-1);
      }
    }
  // What is the oldest coordinate to be edited
  int oldestElement = this->LastUserEditedCoordinates.last();

  if (this->isNormalized())
    {
    // We have to ensure the coordinates are normalized.
    double den = 0.;
    double squaredNorm = this->squaredNorm();
    // Old Values xx + yy + zz = 1
    // New values: x'x' + y'y' + z'z' = 1
    // Say we are changing y into y':
    // x'x' + z'z' = 1 - y'y'
    if (oldestElement != -1 &&
        this->Coordinates[oldestElement] != 0.0 &&
        squaredNorm != 0.0)
      {
      // 1) Normalize only with the oldest user edited value
      // The oldest element is z, that means we try to have
      // x = x' (so that the user doesn't loose the edit he just made on the
      // element (x) he edited before this one (y).
      // Let's pose a the coef to multiply z into z' that keeps the norm to 1
      // xx + z'z' = 1 - y'y' (because x = x')
      // xx + azaz = 1 - y'y' (because z' = az)
      // aa*zz = 1 - y'y' - xx
      // a = sqrt( (1 - y'y' - xx) / zz )
      den = (1. - (squaredNorm -
             this->Coordinates[oldestElement] *
             this->Coordinates[oldestElement])) /
              (this->Coordinates[oldestElement] *
               this->Coordinates[oldestElement]);
      if (den > 0.)
        {
        den = sqrt(den);
        }
      }
    // Maybe 1) failed, then give 2) a chance.
    if (den <= 0)
      {
      oldestElement = -1;
      }
    bool mult = true;
    if (oldestElement == -1)
      {
      // 2) Normalize with all the coordinates
      // Let's pose a the coef to multiply x into x' and z into z' that keeps
      // the norm to 1:
      // axax + azaz = 1 - y'y'
      // aa(xx + zz) = 1 - y'y'
      // a = sqrt( (1 - y'y') / (xx + zz) )
      squaredNorm -= coordinate * coordinate;
      if (squaredNorm != 0.0)
        {
        den = sqrt( (1. - coordinate * coordinate) / squaredNorm);
        }
      else if (this->Dimension > 1)
        {
        mult = false;
        den = sqrt((1. - coordinate*coordinate) / (this->Dimension - 1));
        }
      }
    // Normalize coordinates
    double* normalizedCoordinates = new double[this->Dimension];
    for (int i = 0; i < this->Dimension; ++i)
      {
      if ((i != element && oldestElement == -1) ||
          (i == oldestElement && oldestElement != -1))
        {
        normalizedCoordinates[i] = mult ? this->Coordinates[i] * den : den;
        }
      else
        {
        normalizedCoordinates[i] = this->Coordinates[i];
        }
      }
    this->setCoordinates(normalizedCoordinates);
    delete [] normalizedCoordinates;
    }
  else
    {
    emit coordinatesChanged(this->Coordinates);
    }
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::updateCoordinates()
{
  for (int i = 0; i < this->Dimension; ++i)
    {
    QLayoutItem* item = this->layout()->itemAt(i);
    QDoubleSpinBox* spinBox =
      item ? qobject_cast<QDoubleSpinBox*>(item->widget()) : 0;
    if ( spinBox)
      {
      this->Coordinates[i] = spinBox->value();
      }
    }
  emit coordinatesChanged(this->Coordinates);
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::normalize()
{
  double* normalizedCoordinates = new double[this->Dimension];
  memcpy(normalizedCoordinates, this->Coordinates,
         sizeof(double) * this->Dimension);
  ctkCoordinatesWidget::normalize(normalizedCoordinates, this->Dimension);
  this->setCoordinates(normalizedCoordinates);
  delete [] normalizedCoordinates;
}

//------------------------------------------------------------------------------
double ctkCoordinatesWidget::normalize(double* coordinates, int dimension)
{
  double den = ctkCoordinatesWidget::norm( coordinates, dimension );
  if ( den != 0.0 )
    {
    for (int i = 0; i < dimension; ++i)
      {
      coordinates[i] /= den;
      }
    }
  return den;
}

//------------------------------------------------------------------------------
double ctkCoordinatesWidget::norm()const
{
  return ctkCoordinatesWidget::norm(this->Coordinates, this->Dimension);
}

//------------------------------------------------------------------------------
double ctkCoordinatesWidget::norm(double* coordinates, int dimension)
{
  return sqrt(ctkCoordinatesWidget::squaredNorm(coordinates, dimension));
}

//------------------------------------------------------------------------------
double ctkCoordinatesWidget::squaredNorm()const
{
  return ctkCoordinatesWidget::squaredNorm(this->Coordinates, this->Dimension);
}

//------------------------------------------------------------------------------
double ctkCoordinatesWidget::squaredNorm(double* coordinates, int dimension)
{
  double sum = 0.;
  for (int i = 0; i < dimension; ++i)
    {
    sum += coordinates[i] * coordinates[i];
    }
  return sum;
}
