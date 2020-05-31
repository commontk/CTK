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
#include "ctkCoordinatesWidget_p.h"
#include "ctkDoubleSpinBox.h"
#include "ctkDoubleSpinBox_p.h"
#include "ctkUtils.h"
#include "ctkValueProxy.h"

// STD includes
#include <cmath>
#include <limits>

// --------------------------------------------------------------------------
ctkCoordinatesWidgetPrivate
::ctkCoordinatesWidgetPrivate(ctkCoordinatesWidget& object)
  :q_ptr(&object)
{
  this->Decimals = 3;
  this->Frame = true;
  this->ReadOnly = false;
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
}

// --------------------------------------------------------------------------
ctkCoordinatesWidgetPrivate
::~ctkCoordinatesWidgetPrivate()
{
  delete [] this->Coordinates;
}

// --------------------------------------------------------------------------
void ctkCoordinatesWidgetPrivate::init()
{
  Q_Q(ctkCoordinatesWidget);
  QHBoxLayout* hboxLayout = new QHBoxLayout(q);
  hboxLayout->setContentsMargins(0, 0, 0, 0);
  q->setLayout(hboxLayout);

  q->setDimension(3);
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidgetPrivate::addSpinBox()
{
  Q_Q(ctkCoordinatesWidget);
  ctkDoubleSpinBox* spinBox = new ctkDoubleSpinBox(q);
  spinBox->setDecimals(this->Decimals);
  spinBox->setDecimalsOption(this->DecimalsOption);
  spinBox->setSingleStep(this->SingleStep);
  spinBox->setMinimum(this->Minimum);
  spinBox->setMaximum(this->Maximum);
  spinBox->setSizeHintPolicy(this->SizeHintPolicy);
  spinBox->setValueProxy(this->Proxy.data());
  spinBox->setReadOnly(this->ReadOnly);
  connect( spinBox, SIGNAL(valueChanged(double)),
           q, SLOT(updateCoordinate(double)));
  // Same number of decimals within the spinboxes.
  connect( spinBox, SIGNAL(decimalsChanged(int)),
           this, SLOT(updateOtherDecimals(int)));
  qobject_cast<QHBoxLayout*>(q->layout())->addWidget(spinBox, 1.);
}



//------------------------------------------------------------------------------
void ctkCoordinatesWidgetPrivate::updateDecimals()
{
  Q_Q(ctkCoordinatesWidget);

  if (this->ChangingDecimals)
    {
    return;
    }
  int maxDecimals = 0;
  for (int i = 0; i < this->Dimension; ++i)
    {
    int spinBoxDecimals = this->Decimals;
    if (q->decimalsOption() & ctkDoubleSpinBox::DecimalsByKey ||
        q->decimalsOption() & ctkDoubleSpinBox::DecimalsByShortcuts)
      {
      spinBoxDecimals = q->spinBox(i)->decimals();
      }
    if (q->decimalsOption() & ctkDoubleSpinBox::DecimalsByValue)
      {
      spinBoxDecimals = ctkCoordinatesWidgetPrivate::spinBoxSignificantDecimals(
        q->spinBox(i));
      if (spinBoxDecimals == 16)
        {
        spinBoxDecimals = q->spinBox(i)->decimals();
        }
      }
    maxDecimals = qMax(maxDecimals, spinBoxDecimals);
    }
  this->ChangingDecimals = true;
  this->setTemporaryDecimals(maxDecimals);
  this->ChangingDecimals = false;
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidgetPrivate::updateOtherDecimals(int senderDecimals)
{
  Q_Q(ctkCoordinatesWidget);
  if (this->ChangingDecimals)
    {
    return;
    }
  int senderSpinBoxDecimals = ctkCoordinatesWidgetPrivate::spinBoxSignificantDecimals(
    qobject_cast<ctkDoubleSpinBox*>(this->sender()));

  int maxDecimals = senderDecimals;
  for (int i = 0; i < this->Dimension; ++i)
    {
    if (this->sender() == q->spinBox(i))
      {
      continue;
      }
    int spinBoxDecimals = maxDecimals;
    if (q->decimalsOption() & ctkDoubleSpinBox::DecimalsByKey)
      {
      spinBoxDecimals = q->spinBox(i)->decimals();
      }
    if (q->decimalsOption() & ctkDoubleSpinBox::DecimalsByValue)
      {
      spinBoxDecimals = ctkCoordinatesWidgetPrivate::spinBoxSignificantDecimals(
        q->spinBox(i));
      // if the edited spinbox has an undefined number of decimals and the
      // the current spinbox too, then use the new number of decimals otherwise
      // there would be no way to increase/decrease decimals for all the
      // spinboxes.
      if (spinBoxDecimals == 16)
        {
        spinBoxDecimals = (senderSpinBoxDecimals == 16)?
          senderDecimals : q->spinBox(i)->decimals();
        }
      }
    maxDecimals = qMax(maxDecimals, spinBoxDecimals);
    }
  this->ChangingDecimals = true;
  this->setTemporaryDecimals(maxDecimals);
  this->ChangingDecimals = false;
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidgetPrivate::setTemporaryDecimals(int newDecimals)
{
  Q_Q(ctkCoordinatesWidget);
  for (int i = 0; i < this->Dimension; ++i)
    {
    if (this->sender() == q->spinBox(i))
      {
      continue;
      }
    // Increasing the number of decimals might have lost precision.
    double currentValue = q->spinBox(i)->value();
    if (q->spinBox(i)->valueProxy())
      {
      currentValue = q->spinBox(i)->valueProxy()->proxyValueFromValue(currentValue);
      }
    q->spinBox(i)->d_ptr->setValue(currentValue, newDecimals);
    }
}

//------------------------------------------------------------------------------
int ctkCoordinatesWidgetPrivate::spinBoxSignificantDecimals(ctkDoubleSpinBox* spinBox)
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
double ctkCoordinatesWidgetPrivate::normalize(double* coordinates, int dimension)
{
  double den = ctkCoordinatesWidgetPrivate::norm( coordinates, dimension );
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
double ctkCoordinatesWidgetPrivate::norm(double* coordinates, int dimension)
{
  return sqrt(ctkCoordinatesWidgetPrivate::squaredNorm(coordinates, dimension));
}

//------------------------------------------------------------------------------
double ctkCoordinatesWidgetPrivate::squaredNorm(double* coordinates, int dimension)
{
  double sum = 0.;
  for (int i = 0; i < dimension; ++i)
    {
    sum += coordinates[i] * coordinates[i];
    }
  return sum;
}

//----------------------------------------------------------------------------
void ctkCoordinatesWidgetPrivate::onValueProxyAboutToBeModified()
{
  Q_Q(ctkCoordinatesWidget);
  for (int i = 0; i < this->Dimension; ++i)
    {
    q->spinBox(i)->blockSignals(true);
    }
}

//----------------------------------------------------------------------------
void ctkCoordinatesWidgetPrivate::onValueProxyModified()
{
  Q_Q(ctkCoordinatesWidget);
  for (int i = 0; i < this->Dimension; ++i)
    {
    q->spinBox(i)->blockSignals(false);
    }
  // Only decimals (not range/nor value) may have change during a proxy
  // modification.
  this->updateDecimals();
}

//------------------------------------------------------------------------------
ctkCoordinatesWidget::ctkCoordinatesWidget(QWidget* _parent)
  : QWidget(_parent)
  , d_ptr(new ctkCoordinatesWidgetPrivate(*this))
{
  Q_D(ctkCoordinatesWidget);
  d->init();
}

//------------------------------------------------------------------------------
ctkCoordinatesWidget::~ctkCoordinatesWidget()
{
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::setDimension(int dim)
{
  Q_D(ctkCoordinatesWidget);
  if (dim < 1)
    {
    return;
    }
  double* newPos = new double[dim];
  if (dim > d->Dimension)
    {
    memcpy(newPos, d->Coordinates, d->Dimension * sizeof(double));
    for (int i = d->Dimension; i < dim; ++i)
      {
      newPos[i] = 0.;
      d->addSpinBox();
      d->LastUserEditedCoordinates.push_back(i);
      }
    }
  else
    {
    memcpy(newPos, d->Coordinates, dim * sizeof(double));
    for (int i = d->Dimension - 1 ; i >= dim; --i)
      {
      QLayoutItem* item = this->layout()->takeAt(i);
      QWidget* widget = item ? item->widget() : 0;
      delete item;
      delete widget;
      d->LastUserEditedCoordinates.pop_back();
      }
    }
  delete [] d->Coordinates;
  d->Coordinates = newPos;
  d->Dimension = dim;

  this->updateGeometry();

  this->updateCoordinates();
}

//------------------------------------------------------------------------------
int ctkCoordinatesWidget::dimension() const
{
  Q_D(const ctkCoordinatesWidget);
  return d->Dimension;
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::setMinimum(double min)
{
  Q_D(ctkCoordinatesWidget);
  for (int i = 0; i < d->Dimension; ++i)
    {
    this->spinBox(i)->setMinimum(min);
    }
  d->Minimum = min;
}

//------------------------------------------------------------------------------
double ctkCoordinatesWidget::minimum() const
{
  Q_D(const ctkCoordinatesWidget);
  return d->Minimum;
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::setMaximum(double max)
{
  Q_D(ctkCoordinatesWidget);
  for (int i = 0; i < d->Dimension; ++i)
    {
    this->spinBox(i)->setMaximum(max);
    }
  d->Maximum = max;
}

//------------------------------------------------------------------------------
double ctkCoordinatesWidget::maximum() const
{
  Q_D(const ctkCoordinatesWidget);
  return d->Maximum;
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::setRange(double min, double max)
{
  Q_D(ctkCoordinatesWidget);
  for (int i = 0; i < d->Dimension; ++i)
    {
    this->spinBox(i)->setRange(min, max);
    }
  d->Minimum = min;
  d->Maximum = max;
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::setNormalized(bool normalized)
{
  Q_D(ctkCoordinatesWidget);
  d->Normalized = normalized;
  if (d->Normalized)
    {
    double* normalizedCoordinates = new double[d->Dimension];
    memcpy(normalizedCoordinates, d->Coordinates, sizeof(double)*d->Dimension);
    ctkCoordinatesWidgetPrivate::normalize(normalizedCoordinates, d->Dimension);

    this->setMinimum(-1.);
    this->setMaximum(1.);

    this->setCoordinates(normalizedCoordinates);
    delete [] normalizedCoordinates;
    }
}

//------------------------------------------------------------------------------
bool ctkCoordinatesWidget::isNormalized() const
{
  Q_D(const ctkCoordinatesWidget);
  return d->Normalized;
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::setDecimals(int newDecimals)
{
  Q_D(ctkCoordinatesWidget);
  d->Decimals = newDecimals;
  for (int i = 0; i < d->Dimension; ++i)
    {
    this->spinBox(i)->setDecimals(newDecimals);
    }
}

//------------------------------------------------------------------------------
int ctkCoordinatesWidget::decimals() const
{
  Q_D(const ctkCoordinatesWidget);
  return d->Decimals;
}

// --------------------------------------------------------------------------
ctkDoubleSpinBox::DecimalsOptions ctkCoordinatesWidget::decimalsOption()const
{
  Q_D(const ctkCoordinatesWidget);
  return d->DecimalsOption;
}

// --------------------------------------------------------------------------
void ctkCoordinatesWidget
::setDecimalsOption(ctkDoubleSpinBox::DecimalsOptions newDecimalsOption)
{
  Q_D(ctkCoordinatesWidget);
  for (int i = 0; i < d->Dimension; ++i)
    {
    this->spinBox(i)->setDecimalsOption(newDecimalsOption);
    }
  d->DecimalsOption = newDecimalsOption;
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::setSingleStep(double step)
{
  Q_D(ctkCoordinatesWidget);
  for (int i = 0; i < d->Dimension; ++i)
    {
    this->spinBox(i)->setSingleStep(step);
    }
  d->SingleStep = step;
}

//------------------------------------------------------------------------------
double ctkCoordinatesWidget::singleStep() const
{
  Q_D(const ctkCoordinatesWidget);
  return d->SingleStep;
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::setCoordinatesAsString(QString _pos)
{
  Q_D(ctkCoordinatesWidget);
  QStringList posList = _pos.split(',');
  if (posList.count() != d->Dimension)
    {
    return;
    }
  double* newPos = new double[d->Dimension];
  for (int i = 0; i < d->Dimension; ++i)
    {
    newPos[i] = posList[i].toDouble();
    }
  this->setCoordinates(newPos);
  delete [] newPos;
}

//------------------------------------------------------------------------------
QString ctkCoordinatesWidget::coordinatesAsString()const
{
  Q_D(const ctkCoordinatesWidget);
  QString res;
  for (int i = 0; i < d->Dimension; ++i)
    {
    if (i != 0)
      {
      res += ",";
      }
    res += QString::number(d->Coordinates[i]);
    }
  return res;
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::setCoordinates(double* coordinates)
{
  Q_D(ctkCoordinatesWidget);
  for (int i = 0; i < d->Dimension; ++i)
    {
    d->Coordinates[i] = coordinates[i];
    }
  if (d->Normalized)
    {
    d->normalize(d->Coordinates, d->Dimension);
    }
  bool valuesModified = false;
  bool blocked = this->blockSignals(true);
  for (int i = 0; i < d->Dimension; ++i)
    {
    ctkDoubleSpinBox* spinbox = this->spinBox(i);
    if (spinbox)
      {
      // we don't want updateCoordinate() to be called.
      // it could mess with the LastUserEditedCoordinates list.
      bool spinBoxSignalWasBlocked = spinbox->blockSignals(true);
      if (spinbox->value() != d->Coordinates[i])
        {
        valuesModified = true;
        }
      // Still setValue needs to be called to recompute the number of decimals
      // if DecimalsByValue is set.
      spinbox->setValue(d->Coordinates[i]);
      spinbox->blockSignals(spinBoxSignalWasBlocked);
      }
    }
  this->blockSignals(blocked);
  d->updateDecimals();
  if (valuesModified)
    {
    this->updateCoordinates();
    }
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::setCoordinates(double x, double y, double z, double w)
{
  Q_D(ctkCoordinatesWidget);
  double* coordinates = new double[d->Dimension];
  if (d->Dimension >= 1)
    {
    coordinates[0] = x;
    }
  if (d->Dimension >= 2)
    {
    coordinates[1] = y;
    }
  if (d->Dimension >= 3)
    {
    coordinates[2] = z;
    }
  if (d->Dimension >= 4)
    {
    coordinates[3] = w;
    }
  for (int i = 4; i < d->Dimension; ++i)
    {
    coordinates[i] = d->Coordinates[i];
    }
  this->setCoordinates(coordinates);
  delete [] coordinates;
}

//------------------------------------------------------------------------------
double const * ctkCoordinatesWidget::coordinates()const
{
  Q_D(const ctkCoordinatesWidget);
  return d->Coordinates;
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::updateCoordinate(double coordinate)
{
  Q_D(ctkCoordinatesWidget);
  int element = -1;
  for (int i = 0; i < d->Dimension; ++i)
    {
    if ( this->spinBox(i) && this->spinBox(i) == this->sender())
      {
      d->Coordinates[i] = coordinate;
      element = i;
      }
    }
  Q_ASSERT(element != -1);
  // Update the last edited history by push first the element.
  for (int i = d->Dimension -1; i > 0; --i)
    {
    if (d->LastUserEditedCoordinates[i] == element)
      {
      #if (QT_VERSION >= QT_VERSION_CHECK(5,13,0))
        if (i >= 0 && i-1 >= 0 && d->LastUserEditedCoordinates.size() > i && d->LastUserEditedCoordinates.size() > i-1)
          {
          d->LastUserEditedCoordinates.swapItemsAt(i,i-1);
          }
      #else
      d->LastUserEditedCoordinates.swap(i,i-1);
      #endif
      }
    }
  // What is the oldest coordinate to be edited
  int oldestElement = d->LastUserEditedCoordinates.last();

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
        d->Coordinates[oldestElement] != 0.0 &&
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
             d->Coordinates[oldestElement] *
             d->Coordinates[oldestElement])) /
              (d->Coordinates[oldestElement] *
               d->Coordinates[oldestElement]);
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
      else if (d->Dimension > 1)
        {
        mult = false;
        den = sqrt((1. - coordinate*coordinate) / (d->Dimension - 1));
        }
      }
    // Normalize coordinates
    double* normalizedCoordinates = new double[d->Dimension];
    for (int i = 0; i < d->Dimension; ++i)
      {
      if ((i != element && oldestElement == -1) ||
          (i == oldestElement && oldestElement != -1))
        {
        normalizedCoordinates[i] = mult ? d->Coordinates[i] * den : den;
        }
      else
        {
        normalizedCoordinates[i] = d->Coordinates[i];
        }
      }
    this->setCoordinates(normalizedCoordinates);
    delete [] normalizedCoordinates;
    }
  else
    {
    emit coordinatesChanged(d->Coordinates);
    }
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::updateCoordinates()
{
  Q_D(ctkCoordinatesWidget);
  for (int i = 0; i < d->Dimension; ++i)
    {
    d->Coordinates[i] = this->spinBox(i)->value();
    }
  emit coordinatesChanged(d->Coordinates);
}

//------------------------------------------------------------------------------
void ctkCoordinatesWidget::normalize()
{
  Q_D(ctkCoordinatesWidget);
  double* normalizedCoordinates = new double[d->Dimension];
  memcpy(normalizedCoordinates, d->Coordinates,
         sizeof(double) * d->Dimension);
  ctkCoordinatesWidgetPrivate::normalize(normalizedCoordinates, d->Dimension);
  this->setCoordinates(normalizedCoordinates);
  delete [] normalizedCoordinates;
}

//------------------------------------------------------------------------------
double ctkCoordinatesWidget::norm()const
{
  Q_D(const ctkCoordinatesWidget);
  return ctkCoordinatesWidgetPrivate::norm(d->Coordinates, d->Dimension);
}

//------------------------------------------------------------------------------
double ctkCoordinatesWidget::squaredNorm()const
{
  Q_D(const ctkCoordinatesWidget);
  return ctkCoordinatesWidgetPrivate::squaredNorm(d->Coordinates, d->Dimension);
}

//----------------------------------------------------------------------------
void ctkCoordinatesWidget::setSizeHintPolicy(ctkDoubleSpinBox::SizeHintPolicy newSizeHintPolicy)
{
  Q_D(ctkCoordinatesWidget);
  for (int i = 0; i < d->Dimension; ++i)
    {
    this->spinBox(i)->setSizeHintPolicy(newSizeHintPolicy);
    }
  d->SizeHintPolicy = newSizeHintPolicy;
}

//----------------------------------------------------------------------------
ctkDoubleSpinBox::SizeHintPolicy ctkCoordinatesWidget::sizeHintPolicy()const
{
  Q_D(const ctkCoordinatesWidget);
  return d->SizeHintPolicy;
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
  Q_D(ctkCoordinatesWidget);
  if (d->Proxy.data() == proxy)
    {
    return;
    }

  d->onValueProxyAboutToBeModified();

  if (d->Proxy)
    {
    disconnect(d->Proxy.data(), SIGNAL(proxyAboutToBeModified()),
               d, SLOT(onValueProxyAboutToBeModified()));
    disconnect(d->Proxy.data(), SIGNAL(proxyModified()),
               d, SLOT(onValueProxyModified()));
    }

  d->Proxy = proxy;

  if (d->Proxy)
    {
    connect(d->Proxy.data(), SIGNAL(proxyAboutToBeModified()),
            d, SLOT(onValueProxyAboutToBeModified()));
    }

  for (int i = 0; i < d->Dimension; ++i)
    {
    this->spinBox(i)->setValueProxy(d->Proxy.data());
    }

  if (d->Proxy)
    {
    connect(d->Proxy.data(), SIGNAL(proxyModified()),
            d, SLOT(onValueProxyModified()));
    }
  d->onValueProxyModified();
}

//----------------------------------------------------------------------------
ctkValueProxy* ctkCoordinatesWidget::valueProxy() const
{
  Q_D(const ctkCoordinatesWidget);
  return d->Proxy.data();
}

//----------------------------------------------------------------------------
void ctkCoordinatesWidget::setReadOnly(bool readOnly)
{
  Q_D(ctkCoordinatesWidget);
  if (d->ReadOnly == readOnly)
    {
    return;
    }

  d->ReadOnly = readOnly;
  for (int i = 0; i < d->Dimension; ++i)
    {
    this->spinBox(i)->setReadOnly(d->ReadOnly);
    }
}

//------------------------------------------------------------------------------
bool ctkCoordinatesWidget::isReadOnly() const
{
  Q_D(const ctkCoordinatesWidget);
  return d->ReadOnly;
}

//-----------------------------------------------------------------------------
void ctkCoordinatesWidget::setFrame(bool frame)
{
  Q_D(ctkCoordinatesWidget);
  if (d->Frame == frame)
    {
    return;
    }

  d->Frame = frame;
  for (int i = 0; i < d->Dimension; ++i)
    {
    this->spinBox(i)->setFrame(d->Frame);
    }
}

//-----------------------------------------------------------------------------
bool ctkCoordinatesWidget::hasFrame() const
{
  Q_D(const ctkCoordinatesWidget);
  return d->Frame;
}
