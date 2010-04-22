/*=========================================================================

  Library:   ctk

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/
/// CTK includes
#include "ctkTransferFunction.h"

//-----------------------------------------------------------------------------
ctkControlPoint::~ctkControlPoint()
{ 
}

//-----------------------------------------------------------------------------
ctkBezierControlPoint::~ctkBezierControlPoint()
{
}

//-----------------------------------------------------------------------------
ctkNonLinearControlPoint::~ctkNonLinearControlPoint()
{
}

//-----------------------------------------------------------------------------
ctkTransferFunction::ctkTransferFunction(QObject* parentObject)
  :QObject(parentObject)
{
}

//-----------------------------------------------------------------------------
ctkTransferFunction::~ctkTransferFunction()
{
  // foreach(ctkControlPoint* cp, this->ControlPoints)
  //   {
  //   delete cp;
  //   }
  // this->ControlPoints->clear();
  // emit changed();
}

ctkBezierControlPoint*  ctkTransferFunction::toto()
{
  return new ctkBezierControlPoint();
}

