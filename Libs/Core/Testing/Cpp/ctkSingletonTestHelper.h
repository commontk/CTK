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

#ifndef __ctkSingletonTestHelper_h
#define __ctkSingletonTestHelper_h

// Qt includes
#include <QtGlobal> // Q_DISABLE_COPY, Q_DECLARE_PRIVATE
#include <QScopedPointer>

// CTK includes
#include "ctkSingleton.h"

class ctkSingletonTestHelperPrivate;

class ctkSingletonTestHelper
{
public:
  ctkSingletonTestHelper();
  
  static ctkSingletonTestHelper* instance();
  
  // Add singleton method here ...
  void registerNorthFace();
  int northFaceCount()const;
  
protected:
  CTK_SINGLETON_DECLARE(ctkSingletonTestHelper);
  QScopedPointer<ctkSingletonTestHelperPrivate> d_ptr;
  
private:
  Q_DISABLE_COPY(ctkSingletonTestHelper);
  Q_DECLARE_PRIVATE(ctkSingletonTestHelper);
};

//-----------------------------------------------------------------------------
CTK_SINGLETON_DECLARE_INITIALIZER(,ctkSingletonTestHelper)

#endif
