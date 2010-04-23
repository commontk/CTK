/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

// CTK includes
#include "ctkDynamicSpacer.h"

// -----------------------------------------------------------------------------
class ctkDynamicSpacerPrivate : public ctkPrivate<ctkDynamicSpacer>
{
  CTK_DECLARE_PUBLIC(ctkDynamicSpacer);
public:
  void init();

  QSizePolicy ActiveSizePolicy;
  QSizePolicy InactiveSizePolicy;
};

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
}

// -----------------------------------------------------------------------------
void ctkDynamicSpacer::activate(bool enableSizePolicy)
{
  CTK_D(ctkDynamicSpacer);
  this->setSizePolicy(
    enableSizePolicy ? d->ActiveSizePolicy : d->InactiveSizePolicy);
}
