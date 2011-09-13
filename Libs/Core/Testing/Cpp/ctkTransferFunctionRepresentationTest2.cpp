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
#include <QCoreApplication>
#include <QColor>

// CTK includes
#include "ctkTransferFunction.h"
#include "ctkTransferFunctionRepresentation.h"

// STL includes
#include <cstdlib>
#include <iostream>

class ctkForcedTransferFunction: public ctkTransferFunction
{
public:
  ctkForcedTransferFunction(QObject* parent = 0):ctkTransferFunction(parent), _discrete(true) {}
  virtual ~ctkForcedTransferFunction(){}

  // attributes test
  bool _discrete;

  virtual ctkControlPoint* controlPoint(int index)const
  {
    QColor rgb = QColor::fromRgbF(55, 56, 57);
    ctkControlPoint* cp = new ctkControlPoint();
    cp->P.X = index;
    cp->P.Value = rgb;
    return cp;
  }
  virtual QVariant value(qreal pos)const
  {
    QColor rgb = QColor::fromRgbF(55, static_cast<int>(pos), 57);
    return rgb;
  }

  virtual int count()const
  {
    return 2;
  }
  virtual bool isDiscrete()const
  {
    return _discrete;
  }
  virtual bool isEditable()const
  {
    return false;
  }

  virtual void range(qreal& minRange, qreal& maxRange)const
  {
    minRange = 0.;
    maxRange = 1.;
  }
  virtual QVariant minValue()const
  {
    return 0.;
  }
  virtual QVariant maxValue()const
  {
    return 0.;
  }
  virtual int insertControlPoint(const ctkControlPoint& cp)
  {
    Q_UNUSED(cp);
    return -1;
  }

  virtual int insertControlPoint(qreal pos)
  {
    Q_UNUSED(pos);
    return -1;
  }

  virtual void removeControlPoint( qreal pos )
  {
    Q_UNUSED(pos);
  }

  virtual void setControlPointPos(int index, qreal pos)
  {
    Q_UNUSED(pos);
    Q_UNUSED(index);
  }
  virtual void setControlPointValue(int index, const QVariant& value)
  {
    Q_UNUSED(index);
    Q_UNUSED(value);
  }
};

int ctkTransferFunctionRepresentationTest2( int argc, char * argv [])
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);

  ctkForcedTransferFunction* dummy = new ctkForcedTransferFunction();
  ctkTransferFunctionRepresentation representation;

  representation.setTransferFunction(dummy);
  representation.computeCurve();
  representation.computeGradient();

  dummy->_discrete = false;
  representation.setTransferFunction(dummy);
  representation.computeCurve();
  representation.computeGradient();

  delete dummy;
  return EXIT_SUCCESS;
}
