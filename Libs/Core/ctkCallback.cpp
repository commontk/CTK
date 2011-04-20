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

// CTK includes
#include "ctkCallback.h"

// --------------------------------------------------------------------------
// ctkCallback methods

// --------------------------------------------------------------------------
ctkCallback::ctkCallback(QObject * parentObject) : QObject(parentObject)
{
  this->setCallback(0);
}

// --------------------------------------------------------------------------
ctkCallback::ctkCallback(void (*callback)(), QObject * parentObject) : QObject(parentObject)
{
  this->setCallback(callback);
}

// --------------------------------------------------------------------------
ctkCallback::~ctkCallback()
{
}

// --------------------------------------------------------------------------
void (*ctkCallback::callback())()const
{
  return this->Callback;
}
  
// --------------------------------------------------------------------------
void ctkCallback::setCallback(void (*callback)())
{
  this->Callback = callback;
}

// --------------------------------------------------------------------------
void ctkCallback::invoke()
{
  if (!this->Callback)
    {
    return;
    }
  (*this->Callback)();
}


