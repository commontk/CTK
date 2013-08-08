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
#include "ctkDoubleSpinBox.h"
#include "ctkDoubleSpinBox_p.h"
#include "ctkUtils.h"
#include "ctkValueProxy.h"

// STD includes
#include <cmath>
#include <limits>

//------------------------------------------------------------------------------
ctkCoordinatesWidget::ctkCoordinatesWidget(QWidget* _parent) :QWidget(_parent)
{
  this->Decimals = 3;
  ctkDoubleSpinBox temp;
  this->DecimalsOption = temp.decimalsOption();
  this->SingleStep = 1.;
  this->Minimum = -std::numeric_limits<double>::max();
  this->Maximum = std::numeric_limits<double>::max();
  this->Normalized = false;
  this->Dimension = 0;
  this->SizeHintPolicy = ctkDoubleSpinBox::SizeHintByValue;
  this->Coordinates = 0;
  this->ChangingDecimals = false;

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
  ctkDoubleSpinBox* spinBox = new ctkDoubleSpinBox(this);
  spinBox->setDecimals(this->Decimals);
  spinBox->setDecimalsOption(this->DecimalsOption);
  spinBox->setSingleStep(this->SingleStep);
  spinBox->setMinimum(this->Minimum);
  spinBox->setMaximum(this->Maximum);
  spinBox->setSizeHintPolicy(this->SizeHintPolicy);
  spinBox->setValueProxy(this->Proxy.data());
  connect( spinBox, SIGNAL(valueChanged(double)),
           this, SLOT(updateCoordinate(double)));
  // Same number of decimals within the spinboxes.
  connect( spinBox, SIGNAL(decimalsChanged(int)),
           this, SLOT(updateOtherDecimals(int)));
  qobject_cast<QHBoxLayout*>(this->layout())->addWidget(spinBox, 1.);
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
  for (int i = 0; i < this->Dimension; ++i)
    {
    this->spinBox(i)->setMinimum(min);
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
  for (int i = 0; i < this->Dimension; ++i)
    {
    this->spinBox(i)->setMaximum(max);
    }
  this->Maximum = max;
}

//------------------------------------------------------------------------------
double ctkCoordinatesWidget::maximum() const
{
  return this->Maximum;
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::setRange(double min, double max)
{
  for (int i = 0; i < this->Dimension; ++i)
    {
    this->spinBox(i)->setRange(min, max);
    }
  this->Minimum = min;
  this->Maximum = max;
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
  for (int i = 0; i < this->Dimension; ++i)
    {
    this->spinBox(i)->setDecimals(newDecimals);
    }
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::updateDecimals()
{
  if (this->ChangingDecimals)
    {
    return;
    }
  int maxDecimals = 0;
  for (int i = 0; i < this->Dimension; ++i)
    {
    int spinBoxDecimals = this->Decimals;
    if (this->decimalsOption() & ctkDoubleSpinBox::DecimalsByKey ||
        this->decimalsOption() & ctkDoubleSpinBox::DecimalsByShortcuts)
      {
      spinBoxDecimals = this->spinBox(i)->decimals();
      }
    if (this->decimalsOption() & ctkDoubleSpinBox::DecimalsByValue)
      {
      spinBoxDecimals = ctkCoordinatesWidget::spinBoxSignificantDecimals(
        this->spinBox(i));
      if (spinBoxDecimals == 16)
        {
        spinBoxDecimals = this->spinBox(i)->decimals();
        }
      }
    maxDecimals = qMax(maxDecimals, spinBoxDecimals);
    }
  this->ChangingDecimals = true;
  this->setTemporaryDecimals(maxDecimals);
  this->ChangingDecimals = false;
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::updateOtherDecimals(int senderDecimals)
{
  if (this->ChangingDecimals)
    {
    return;
    }
  int senderSpinBoxDecimals = ctkCoordinatesWidget::spinBoxSignificantDecimals(
    qobject_cast<ctkDoubleSpinBox*>(this->sender()));

  int maxDecimals = senderDecimals;
  for (int i = 0; i < this->Dimension; ++i)
    {
    if (this->sender() == this->spinBox(i))
      {
      continue;
      }
    int spinBoxDecimals = maxDecimals;
    if (this->decimalsOption() & ctkDoubleSpinBox::DecimalsByKey)
      {
      spinBoxDecimals = this->spinBox(i)->decimals();
      }
    if (this->decimalsOption() & ctkDoubleSpinBox::DecimalsByValue)
      {
      spinBoxDecimals = ctkCoordinatesWidget::spinBoxSignificantDecimals(
        this->spinBox(i));
      // if the edited spinbox has an undefined number of decimals and the
      // the current spinbox too, then use the new number of decimals otherwise
      // there would be no way to increase/decrease decimals for all the
      // spinboxes.
      if (spinBoxDecimals == 16)
        {
        spinBoxDecimals = (senderSpinBoxDecimals == 16)?
          senderDecimals : this->spinBox(i)->decimals();
        }
      }
    maxDecimals = qMax(maxDecimals, spinBoxDecimals);
    }
  this->ChangingDecimals = true;
  this->setTemporaryDecimals(maxDecimals);
  this->ChangingDecimals = false;
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::setTemporaryDecimals(int newDecimals)
{
  for (int i = 0; i < this->Dimension; ++i)
    {
    if (this->sender() == this->spinBox(i))
      {
      continue;
      }
    // Increasing the number of decimals might have lost precision.
    double currentValue = this->spinBox(i)->value();
    if (this->spinBox(i)->valueProxy())
      {
      currentValue = this->spinBox(i)->valueProxy()->proxyValueFromValue(currentValue);
      }
    this->spinBox(i)->d_ptr->setValue(currentValue, newDecimals);
    }
}

//------------------------------------------------------------------------------
int ctkCoordinatesWidget::spinBoxSignificantDecimals(ctkDoubleSpinBox* spinBox)
{
  if (!spinBox)
    {
    return 0;
    }
  double currentValue = spinBox->value();
  if (spinBox->valueProxy())
    {
    currentValue = spinBox->valueProxy()->proxyValueFromValue(currentValue);
    }
  return ctk::significantDecimals(currentValue);
}

//------------------------------------------------------------------------------
int ctkCoordinatesWidget::decimals() const
{
  return this->Decimals;
}

// --------------------------------------------------------------------------
ctkDoubleSpinBox::DecimalsOptions ctkCoordinatesWidget::decimalsOption()const
{
  return this->DecimalsOption;
}

// --------------------------------------------------------------------------
void ctkCoordinatesWidget
::setDecimalsOption(ctkDoubleSpinBox::DecimalsOptions newDecimalsOption)
{
  for (int i = 0; i < this->Dimension; ++i)
    {
    this->spinBox(i)->setDecimalsOption(newDecimalsOption);
    }
  this->DecimalsOption = newDecimalsOption;
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::setSingleStep(double step)
{
  for (int i = 0; i < this->Dimension; ++i)
    {
    this->spinBox(i)->setSingleStep(step);
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
  bool valuesModified = false;
  bool blocked = this->blockSignals(true);
  for (int i = 0; i < this->Dimension; ++i)
    {
    ctkDoubleSpinBox* spinbox = this->spinBox(i);
    if (spinbox)
      {
      // we don't want updateCoordinate() to be called.
      // it could mess with the LastUserEditedCoordinates list.
      bool spinBoxSignalWasBlocked = spinbox->blockSignals(true);
      if (spinbox->value() != this->Coordinates[i])
        {
        valuesModified = true;
        }
      // Still setValue needs to be called to recompute the number of decimals
      // if DecimalsByValue is set.
      spinbox->setValue(this->Coordinates[i]);
      spinbox->blockSignals(spinBoxSignalWasBlocked);
      }
    }
  this->blockSignals(blocked);
  this->updateDecimals();
  if (valuesModified)
    {
    this->updateCoordinates();
    }
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
    if ( this->spinBox(i) && this->spinBox(i) == this->sender())
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
    this->Coordinates[i] = this->spinBox(i)->value();
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


//----------------------------------------------------------------------------
void ctkCoordinatesWidget::setSizeHintPolicy(ctkDoubleSpinBox::SizeHintPolicy newSizeHintPolicy)
{
  for (int i = 0; i < this->Dimension; ++i)
    {
    this->spinBox(i)->setSizeHintPolicy(newSizeHintPolicy);
    }
  this->SizeHintPolicy = newSizeHintPolicy;
}

//----------------------------------------------------------------------------
ctkDoubleSpinBox::SizeHintPolicy ctkCoordinatesWidget::sizeHintPolicy()const
{
  return this->SizeHintPolicy;
}

//----------------------------------------------------------------------------
ctkDoubleSpinBox* ctkCoordinatesWidget::spinBox(int i)
{
  QLayoutItem* item = this->layout()->itemAt(i);
  ctkDoubleSpinBox* spinBox =
    item ? qobject_cast<ctkDoubleSpinBox*>(item->widget()) : 0;
  return spinBox;
}

//----------------------------------------------------------------------------
void ctkCoordinatesWidget::setValueProxy(ctkValueProxy* proxy)
{
  if (this->Proxy.data() == proxy)
    {
    return;
    }

  this->onValueProxyAboutToBeModified();

  if (this->Proxy)
    {
    disconnect(this->Proxy.data(), SIGNAL(proxyAboutToBeModified()),
               this, SLOT(onValueProxyAboutToBeModified()));
    disconnect(this->Proxy.data(), SIGNAL(proxyModified()),
               this, SLOT(onValueProxyModified()));
    }

  this->Proxy = proxy;

  if (this->Proxy)
    {
    connect(this->Proxy.data(), SIGNAL(proxyAboutToBeModified()),
            this, SLOT(onValueProxyAboutToBeModified()));
    }

  for (int i = 0; i < this->Dimension; ++i)
    {
    this->spinBox(i)->setValueProxy(this->Proxy.data());
    }

  if (this->Proxy)
    {
    connect(this->Proxy.data(), SIGNAL(proxyModified()),
            this, SLOT(onValueProxyModified()));
    }
  this->onValueProxyModified();
}

//----------------------------------------------------------------------------
ctkValueProxy* ctkCoordinatesWidget::valueProxy() const
{
  return this->Proxy.data();
}

//----------------------------------------------------------------------------
void ctkCoordinatesWidget::onValueProxyAboutToBeModified()
{
  for (int i = 0; i < this->Dimension; ++i)
    {
    this->spinBox(i)->blockSignals(true);
    }
}

//----------------------------------------------------------------------------
void ctkCoordinatesWidget::onValueProxyModified()
{
  for (int i = 0; i < this->Dimension; ++i)
    {
    this->spinBox(i)->blockSignals(false);
    }
  // Only decimals (not range/nor value) may have change during a proxy
  // modification.
  this->updateDecimals();
}
