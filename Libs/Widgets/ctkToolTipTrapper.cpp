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
/*=========================================================================

   Program: ParaView
   Module:    ctkToolTipTrapper.cxx

   Copyright (c) 2005-2008 Sandia Corporation, Kitware Inc.
   All rights reserved.

   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2. 

   See License_v1.2.txt for the full ParaView license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

// Qt includes
#include <QCoreApplication>

// CTK includes
#include "ctkToolTipTrapper.h"

class ctkToolTipTrapperPrivate
{
  Q_DECLARE_PUBLIC(ctkToolTipTrapper);
protected:
  ctkToolTipTrapper* const q_ptr;
public:
  ctkToolTipTrapperPrivate(ctkToolTipTrapper& object);

  bool Enabled;
};

// --------------------------------------------------------------------------
ctkToolTipTrapperPrivate::ctkToolTipTrapperPrivate(ctkToolTipTrapper& object)
  : q_ptr(&object)
{
  this->Enabled = false;
}

//------------------------------------------------------------------------------
ctkToolTipTrapper::ctkToolTipTrapper(QObject * newParent)
  : Superclass(newParent)
  , d_ptr(new ctkToolTipTrapperPrivate(*this))
{
  this->setEnabled(true);
}

//------------------------------------------------------------------------------
ctkToolTipTrapper::ctkToolTipTrapper(bool enable, QObject * newParent)
  : Superclass(newParent)
  , d_ptr(new ctkToolTipTrapperPrivate(*this))
{
  this->setEnabled(enable);
}

//------------------------------------------------------------------------------
ctkToolTipTrapper::~ctkToolTipTrapper()
{
}

//------------------------------------------------------------------------------
bool ctkToolTipTrapper::isEnabled()const
{
  Q_D(const ctkToolTipTrapper);
  return d->Enabled;
}

//------------------------------------------------------------------------------
void ctkToolTipTrapper::setEnabled(bool enable)
{
  Q_D(ctkToolTipTrapper);
  if (enable == d->Enabled)
    {
    return;
    }
  d->Enabled = enable;
  if (enable)
    {
    QCoreApplication::instance()->installEventFilter(this);
    }
  else
    {
    QCoreApplication::instance()->removeEventFilter(this);
    }
}

//------------------------------------------------------------------------------
bool ctkToolTipTrapper::eventFilter(QObject* watched, QEvent* input_event)
{
  Q_UNUSED(watched);
  if(input_event->type() == QEvent::ToolTip)
    {
    return true;
    }
  return false;
}
