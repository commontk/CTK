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

// QT includes
#include <QDebug>
#include <QEvent>
#include <QIcon>
#include <QStyle>

// CTK includes
#include "ctkMoreButton.h"

class ctkMoreButtonPrivate
{
  Q_DECLARE_PUBLIC(ctkMoreButton);
protected:
  ctkMoreButton* const q_ptr;
public:
  ctkMoreButtonPrivate(ctkMoreButton& object);
  void init();
  bool customBehavior;
};

//-----------------------------------------------------------------------------
ctkMoreButtonPrivate::ctkMoreButtonPrivate(ctkMoreButton &object)
  : q_ptr(&object)
{
  this->customBehavior = false;
}

//-----------------------------------------------------------------------------
void ctkMoreButtonPrivate::init()
{
  Q_Q(ctkMoreButton);
  q->setIcon(QIcon(":/Icons/more-right.png"));
  q->setIconSize(
      QSize(q->style()->pixelMetric(QStyle::PM_ToolBarExtensionExtent),
            q->style()->pixelMetric(QStyle::PM_ToolBarExtensionExtent)));
  q->setFlat(true);
  q->setCheckable(true);
  q->setFixedWidth(q->style()->pixelMetric(QStyle::PM_ToolBarExtensionExtent));
  q->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Minimum);
}

//-----------------------------------------------------------------------------
// ctkMoreButton methods

//-----------------------------------------------------------------------------
ctkMoreButton::ctkMoreButton(QWidget *_parent)
  :Superclass(_parent)
  , d_ptr(new ctkMoreButtonPrivate(*this))
{
  Q_D(ctkMoreButton);
  d->init();
}

//-----------------------------------------------------------------------------
ctkMoreButton::~ctkMoreButton()
{
}

//-----------------------------------------------------------------------------
void ctkMoreButton::setCustomBehavior(bool newState)
{
  Q_D(ctkMoreButton);
  d->customBehavior = newState;
}

//-----------------------------------------------------------------------------
bool ctkMoreButton::customBehavior() const
{
  Q_D(const ctkMoreButton);
  return d->customBehavior;
}

//-----------------------------------------------------------------------------
bool ctkMoreButton::event(QEvent *e)
{
  Q_D(ctkMoreButton);
  if (e->type() == QEvent::Leave)
    {
    this->setFlat(true);
    }
  if (e->type() == QEvent::Enter)
    {
    this->setFlat(false);
    }
  if (d->customBehavior &&
      e->type() == QEvent::MouseButtonPress)
    {
    QIcon icon = this->isChecked() ? QIcon(":/Icons/more-right.png") :
                 QIcon(":/Icons/more-left.png");
    this->setIcon(icon);
    this->setIconSize(
        QSize(this->style()->pixelMetric(QStyle::PM_ToolBarExtensionExtent),
              this->style()->pixelMetric(QStyle::PM_ToolBarExtensionExtent)));
    }

  return this->Superclass::event(e);
}
