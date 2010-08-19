/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

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

// Qt includes
#include <QDebug>

// CTK includes
#include "ctkDynamicSpacer.h"

// -----------------------------------------------------------------------------
class ctkDynamicSpacerPrivate : public ctkPrivate<ctkDynamicSpacer>
{
  CTK_DECLARE_PUBLIC(ctkDynamicSpacer);
public:
  ctkDynamicSpacerPrivate();
  void init();

  bool        Enable;
  QSizePolicy ActiveSizePolicy;
  QSizePolicy InactiveSizePolicy;
};

// -----------------------------------------------------------------------------
ctkDynamicSpacerPrivate::ctkDynamicSpacerPrivate()
{
  this->Enable = false;
}

// -----------------------------------------------------------------------------
void ctkDynamicSpacerPrivate::init()
{
  CTK_P(ctkDynamicSpacer);
  this->ActiveSizePolicy = p->sizePolicy();
  this->InactiveSizePolicy = p->sizePolicy();
}

// -----------------------------------------------------------------------------
ctkDynamicSpacer::ctkDynamicSpacer(QWidget *_parent)
  :QWidget(_parent)
{
  CTK_INIT_PRIVATE(ctkDynamicSpacer);
  ctk_d()->init();
}

// -----------------------------------------------------------------------------
ctkDynamicSpacer::~ctkDynamicSpacer()
{
}

// -----------------------------------------------------------------------------
QSizePolicy ctkDynamicSpacer::activeSizePolicy() const
{
  CTK_D(const ctkDynamicSpacer);
  return d->ActiveSizePolicy;
}

// -----------------------------------------------------------------------------
void ctkDynamicSpacer::setActiveSizePolicy(QSizePolicy newActiveSizePolicy)
{
  CTK_D(ctkDynamicSpacer);
  d->ActiveSizePolicy = newActiveSizePolicy;
  if (d->Enable)
    {
    this->setSizePolicy(d->ActiveSizePolicy);
    }
}

// -----------------------------------------------------------------------------
QSizePolicy ctkDynamicSpacer::inactiveSizePolicy() const
{
  CTK_D(const ctkDynamicSpacer);
  return d->InactiveSizePolicy;
}

// -----------------------------------------------------------------------------
void ctkDynamicSpacer::setInactiveSizePolicy(QSizePolicy newInactiveSizePolicy)
{
  CTK_D(ctkDynamicSpacer);
  d->InactiveSizePolicy = newInactiveSizePolicy;
  if (!d->Enable)
    {
    this->setSizePolicy(d->InactiveSizePolicy);
    }
}

// -----------------------------------------------------------------------------
bool ctkDynamicSpacer::isActivated()const
{
  CTK_D(const ctkDynamicSpacer);
  return d->Enable;
}

// -----------------------------------------------------------------------------
void ctkDynamicSpacer::activate(bool enableSizePolicy)
{
  CTK_D(ctkDynamicSpacer);
  qDebug() << "Activate " << enableSizePolicy << (enableSizePolicy ? "Active" : "Inactive") << "size policy";
  d->Enable = enableSizePolicy;
  this->setSizePolicy(
    d->Enable ? d->ActiveSizePolicy : d->InactiveSizePolicy);
}
