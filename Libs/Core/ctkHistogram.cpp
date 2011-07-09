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
/// CTK includes
#include "ctkHistogram.h"

//-----------------------------------------------------------------------------
ctkHistogram::ctkHistogram(QObject* parentObject)
  :ctkTransferFunction(parentObject)
{
}

//-----------------------------------------------------------------------------
ctkHistogram::~ctkHistogram()
{
}

//-----------------------------------------------------------------------------
bool ctkHistogram::isDiscrete()const
{
  return true;
}

//-----------------------------------------------------------------------------
bool ctkHistogram::isEditable()const
{
  return false;
}

//-----------------------------------------------------------------------------
int ctkHistogram::insertControlPoint(const ctkControlPoint& cp)
{
  Q_UNUSED(cp);
  return -1;
}

//-----------------------------------------------------------------------------
int ctkHistogram::insertControlPoint(qreal pos)
{
  Q_UNUSED(pos);
  return -1;
}

//-----------------------------------------------------------------------------
void ctkHistogram::setControlPointPos(int index, qreal pos)
{
  Q_UNUSED(index);
  Q_UNUSED(pos);
}

//-----------------------------------------------------------------------------
void ctkHistogram::setControlPointValue(int index, const QVariant& value)
{
  Q_UNUSED(index);
  Q_UNUSED(value);
}
