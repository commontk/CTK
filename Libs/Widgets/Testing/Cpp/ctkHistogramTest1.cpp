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
#include <QVariant>

// CTK includes
#include <ctkHistogram.h>
#include <ctkTransferFunction.h>

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
class ctkDummyHistogram: public ctkHistogram
{
public:
  ctkDummyHistogram(QObject* parent = 0):ctkHistogram(parent){}
  virtual ~ctkDummyHistogram(){}

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

  virtual void range(qreal& minRange, qreal& maxRange)const
  {
    Q_UNUSED(minRange);
    Q_UNUSED(maxRange);
    minRange = 0.;
    maxRange = 0.;
  }

  virtual QVariant minValue()const
  {
    return 0;
  }

  virtual QVariant maxValue()const
  {
    return 0;
  }

  virtual void removeControlPoint( qreal pos )
  {
    Q_UNUSED(pos);
  }

  virtual void build()
  {
  }

protected:

};

int ctkHistogramTest1(int argc, char * argv [] )
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);
  ctkDummyHistogram dummyHistogram;

  if (!dummyHistogram.isDiscrete())
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkHistogram::isDiscrete"
              <<std::endl;
    return EXIT_FAILURE;
    }

  if (dummyHistogram.isEditable())
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkHistogram::isEditable"
              <<std::endl;
    return EXIT_FAILURE;
    }

  qreal defaultIndex = 0.;
  if (dummyHistogram.controlPoint(defaultIndex) != 0)
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkHistogram::controlPoint"
              <<std::endl;
    return EXIT_FAILURE;
    }
  if (dummyHistogram.count() != 0)
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkHistogram::count"
              <<std::endl;
    return EXIT_FAILURE;
    }
  qreal defaultPos = 0.;
  if (dummyHistogram.value(defaultPos) != 0)
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkHistogram::value"
              <<std::endl;
    return EXIT_FAILURE;
    }
  if (dummyHistogram.minValue() != 0)
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkHistogram::minValue"
              <<std::endl;
    return EXIT_FAILURE;
    }
  if (dummyHistogram.maxValue() != 0)
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkHistogram::maxValue"
              <<std::endl;
    return EXIT_FAILURE;
    }
  if (dummyHistogram.insertControlPoint(defaultPos) != -1)
  {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkHistogram::insertControlPoint"
              <<std::endl;
    return EXIT_FAILURE;
  }
  ctkControlPoint defaultCP;
  if (dummyHistogram.insertControlPoint(defaultCP) != -1)
  {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkHistogram::insertControlPoint"
              <<std::endl;
    return EXIT_FAILURE;
  }

  //----------
  QVariant defaultValue;
  dummyHistogram.removeControlPoint(defaultPos);
  dummyHistogram.setControlPointPos(defaultIndex,defaultPos);
  dummyHistogram.setControlPointValue(defaultIndex,defaultValue);
  dummyHistogram.build();

  return EXIT_SUCCESS;
}

