/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QCoreApplication>
#include <QColor>
#include <QScopedPointer>

// CTK includes
#include "ctkTransferFunction.h"
#include "ctkTransferFunctionRepresentation.h"

// STL includes
#include <cstdlib>
#include <iostream>

class ctkForcedTransferFunction: public ctkTransferFunction
{
public:
  typedef ctkTransferFunction Superclass;
  ctkForcedTransferFunction(QObject* parent = 0) : Superclass(parent)
  {
    this->Discrete = true;
  }

  // --------------------------------------------------------------------------
  virtual ~ctkForcedTransferFunction(){}

  // attributes test
  bool Discrete;

  // --------------------------------------------------------------------------
  virtual ctkControlPoint* controlPoint(int index)const
  {
    QColor rgb = QColor::fromRgbF(55, 56, 57);
    ctkControlPoint* cp = new ctkControlPoint();
    cp->P.X = index;
    cp->P.Value = rgb;
    return cp;
  }

  // --------------------------------------------------------------------------
  virtual QVariant value(qreal pos)const
  {
    QColor rgb = QColor::fromRgbF(55, static_cast<int>(pos), 57);
    return rgb;
  }

  // --------------------------------------------------------------------------
  virtual int count()const { return 2; }
  virtual bool isDiscrete()const { return Discrete; }
  virtual bool isEditable()const { return false; }

  // --------------------------------------------------------------------------
  virtual void range(qreal& minRange, qreal& maxRange)const
  {
    minRange = 0.;
    maxRange = 1.;
  }

  // --------------------------------------------------------------------------
  virtual QVariant minValue()const { return 0.; }
  virtual QVariant maxValue()const { return 0.; }
  virtual int insertControlPoint(const ctkControlPoint& /*cp*/) { return -1; }
  virtual int insertControlPoint(qreal /*pos*/){ return -1; }
  virtual void removeControlPoint(qreal /*pos*/) {}
  virtual void setControlPointPos(int /*index*/, qreal /*pos*/) {}
  virtual void setControlPointValue(int /*index*/, const QVariant& /*value*/){}
};

// --------------------------------------------------------------------------
int ctkTransferFunctionRepresentationTest2( int argc, char * argv [])
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);

  QScopedPointer<ctkForcedTransferFunction> dummy(new ctkForcedTransferFunction());
  ctkTransferFunctionRepresentation representation;

  representation.setTransferFunction(dummy.data());
  representation.computeCurve();
  representation.computeGradient();

  dummy->Discrete = false;
  representation.setTransferFunction(dummy.data());
  representation.computeCurve();
  representation.computeGradient();

  return EXIT_SUCCESS;
}
