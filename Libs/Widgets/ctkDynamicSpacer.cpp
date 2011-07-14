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
#include "ctkDynamicSpacer.h"

// -----------------------------------------------------------------------------
class ctkDynamicSpacerPrivate
{
  Q_DECLARE_PUBLIC(ctkDynamicSpacer);
protected:
  ctkDynamicSpacer* const q_ptr;
public:
  ctkDynamicSpacerPrivate(ctkDynamicSpacer& object);
  void init();

  bool        Enable;
  QSizePolicy ActiveSizePolicy;
  QSizePolicy InactiveSizePolicy;
};

// -----------------------------------------------------------------------------
ctkDynamicSpacerPrivate::ctkDynamicSpacerPrivate(ctkDynamicSpacer& object)
  :q_ptr(&object)
{
  this->Enable = false;
}

// -----------------------------------------------------------------------------
void ctkDynamicSpacerPrivate::init()
{
  Q_Q(ctkDynamicSpacer);
  this->ActiveSizePolicy = q->sizePolicy();
  this->InactiveSizePolicy = q->sizePolicy();
}

// -----------------------------------------------------------------------------
ctkDynamicSpacer::ctkDynamicSpacer(QWidget *_parent)
  :QWidget(_parent)
  , d_ptr(new ctkDynamicSpacerPrivate(*this))
{
  Q_D(ctkDynamicSpacer);
  d->init();
}

// -----------------------------------------------------------------------------
ctkDynamicSpacer::~ctkDynamicSpacer()
{
}

// -----------------------------------------------------------------------------
QSizePolicy ctkDynamicSpacer::activeSizePolicy() const
{
  Q_D(const ctkDynamicSpacer);
  return d->ActiveSizePolicy;
}

// -----------------------------------------------------------------------------
void ctkDynamicSpacer::setActiveSizePolicy(QSizePolicy newActiveSizePolicy)
{
  Q_D(ctkDynamicSpacer);
  d->ActiveSizePolicy = newActiveSizePolicy;
  if (d->Enable)
    {
    this->setSizePolicy(d->ActiveSizePolicy);
    }
}

// -----------------------------------------------------------------------------
QSizePolicy ctkDynamicSpacer::inactiveSizePolicy() const
{
  Q_D(const ctkDynamicSpacer);
  return d->InactiveSizePolicy;
}

// -----------------------------------------------------------------------------
void ctkDynamicSpacer::setInactiveSizePolicy(QSizePolicy newInactiveSizePolicy)
{
  Q_D(ctkDynamicSpacer);
  d->InactiveSizePolicy = newInactiveSizePolicy;
  if (!d->Enable)
    {
    this->setSizePolicy(d->InactiveSizePolicy);
    }
}

// -----------------------------------------------------------------------------
bool ctkDynamicSpacer::isActive()const
{
  Q_D(const ctkDynamicSpacer);
  return d->Enable;
}

// -----------------------------------------------------------------------------
void ctkDynamicSpacer::setActive(bool enableSizePolicy)
{
  Q_D(ctkDynamicSpacer);
  d->Enable = enableSizePolicy;
  this->setSizePolicy(
    d->Enable ? d->ActiveSizePolicy : d->InactiveSizePolicy);
}
