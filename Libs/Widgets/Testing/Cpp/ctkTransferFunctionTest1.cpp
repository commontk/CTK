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

// CTK includes
#include "ctkTransferFunction.h"

// STL includes
#include <cstdlib>
#include <iostream>

class ctkDummyTransferFunction: public ctkTransferFunction
{
public:
  ctkDummyTransferFunction(QObject* parent = 0):ctkTransferFunction(parent){}
  virtual ~ctkDummyTransferFunction(){}

  virtual ctkControlPoint* controlPoint(int index)const
  {
    Q_UNUSED(index);
    return 0;
  }
  virtual QVariant value(qreal pos)const
  {
    Q_UNUSED(pos);
    return 0.;
  }

  virtual int count()const
  {
    return 0;
  }
  virtual bool isDiscrete()const
  {
    return true;
  }
  virtual bool isEditable()const
  {
    return false;
  }

  virtual void range(qreal& minRange, qreal& maxRange)const
  {
    minRange = 0.;
    maxRange = 0.;
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

private:
  Q_DISABLE_COPY(ctkDummyTransferFunction);
};


int ctkTransferFunctionTest1(int argc, char * argv [])
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);
  ctkDummyTransferFunction dummy;

  if (!dummy.isDiscrete())
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkHistogram::isDiscrete"
              <<std::endl;
    return EXIT_FAILURE;
    }

  if (dummy.isEditable())
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkHistogram::isEditable"
              <<std::endl;
    return EXIT_FAILURE;
    }

  qreal defaultIndex = 0.;
  if (dummy.controlPoint(defaultIndex) != 0)
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkHistogram::controlPoint"
              <<std::endl;
    return EXIT_FAILURE;
    }
  if (dummy.count() != 0)
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkHistogram::count"
              <<std::endl;
    return EXIT_FAILURE;
    }
  qreal defaultPos = 0.;
  if (dummy.value(defaultPos) != 0)
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkHistogram::value"
              <<std::endl;
    return EXIT_FAILURE;
    }
  if (dummy.value(0) != 0)
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkHistogram::value"
              <<std::endl;
    return EXIT_FAILURE;
    }
  if (dummy.minValue() != 0)
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkHistogram::minValue"
              <<std::endl;
    return EXIT_FAILURE;
    }
  if (dummy.maxValue() != 0)
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkHistogram::maxValue"
              <<std::endl;
    return EXIT_FAILURE;
    }
  if (dummy.insertControlPoint(defaultPos) != -1)
  {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkHistogram::insertControlPoint"
              <<std::endl;
    return EXIT_FAILURE;
  }
  ctkControlPoint defaultCP;
  if (dummy.insertControlPoint(defaultCP) != -1)
  {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkHistogram::insertControlPoint"
              <<std::endl;
    return EXIT_FAILURE;
  }

  //----------
  QVariant defaultValue;
  dummy.removeControlPoint(defaultPos);
  dummy.setControlPointPos(defaultIndex,defaultPos);
  dummy.setControlPointValue(defaultIndex,defaultValue);
  dummy.representation();

  return EXIT_SUCCESS;
}

