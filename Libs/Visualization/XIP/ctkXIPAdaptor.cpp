/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

// Qt includes
#include <QDebug>

// ctkXIPAdaptor includes
#include "ctkXIPAdaptor.h"

//----------------------------------------------------------------------------
class ctkXIPAdaptorPrivate: public ctkPrivate<ctkXIPAdaptor>
{
public:
  ctkXIPAdaptorPrivate();
};

//----------------------------------------------------------------------------
// ctkXIPAdaptorPrivate methods

//------------------------------------------------------------------------------
ctkXIPAdaptorPrivate::ctkXIPAdaptorPrivate()
{
}

//----------------------------------------------------------------------------
// ctkXIPAdaptorWidget methods

//------------------------------------------------------------------------------
ctkXIPAdaptor::ctkXIPAdaptor(QObject* _parent): Superclass(_parent)
{
  CTK_INIT_PRIVATE(ctkXIPAdaptor);
}

//----------------------------------------------------------------------------
ctkXIPAdaptor::~ctkXIPAdaptor()
{
}
