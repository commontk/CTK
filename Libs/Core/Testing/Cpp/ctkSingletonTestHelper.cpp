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

// CTK includes
#include "ctkSingletonTestHelper.h"

//----------------------------------------------------------------------------
class ctkSingletonTestHelperPrivate
{
public:
  ctkSingletonTestHelperPrivate();
  
  int NorthFaceCount;
};

//-----------------------------------------------------------------------------
// ctkSingletonTestHelperPrivate methods

//-----------------------------------------------------------------------------
ctkSingletonTestHelperPrivate::ctkSingletonTestHelperPrivate()
{
  this->NorthFaceCount = 0;
}

//-----------------------------------------------------------------------------
// ctkSingletonTestHelper methods

//-----------------------------------------------------------------------------
ctkSingletonTestHelper::ctkSingletonTestHelper() : d_ptr(new ctkSingletonTestHelperPrivate)
{
}

//-----------------------------------------------------------------------------
ctkSingletonTestHelper::~ctkSingletonTestHelper()
{
}

//-----------------------------------------------------------------------------
ctkSingletonTestHelper* ctkSingletonTestHelper::instance()
{
  return Self::Instance;
}

//-----------------------------------------------------------------------------
void ctkSingletonTestHelper::registerNorthFace()
{
  Q_D(ctkSingletonTestHelper);
  d->NorthFaceCount++;
}

//-----------------------------------------------------------------------------
int ctkSingletonTestHelper::northFaceCount()const
{
  Q_D(const ctkSingletonTestHelper);
  return d->NorthFaceCount;
}

//-----------------------------------------------------------------------------
CTK_SINGLETON_DEFINE(ctkSingletonTestHelper)

