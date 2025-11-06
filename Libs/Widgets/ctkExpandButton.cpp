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
#include <QIcon>
#include <QStyleOption>

// CTK includes
#include "ctkExpandButton.h"

class ctkExpandButtonPrivate
{
  Q_DECLARE_PUBLIC(ctkExpandButton);
protected:
  ctkExpandButton* const q_ptr;
public:
  ctkExpandButtonPrivate(ctkExpandButton& object);
  void                 init();
  bool                 mirrorOnExpand;
  QPixmap              defaultPixmap;
  Qt::Orientation      orientation;
};

//-----------------------------------------------------------------------------
ctkExpandButtonPrivate::ctkExpandButtonPrivate(ctkExpandButton &object)
  : q_ptr(&object)
{
  this->mirrorOnExpand  = false;
  this->orientation   = Qt::Horizontal;
}

//-----------------------------------------------------------------------------
void ctkExpandButtonPrivate::init()
{
  Q_Q(ctkExpandButton);
  q->setAutoRaise(true);
  q->setOrientation(Qt::Horizontal);
  q->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
  q->setCheckable(true);

  QObject::connect(q, SIGNAL(toggled(bool)), q, SLOT(updateIcon()));
}

//-----------------------------------------------------------------------------
// ctkExpandButton methods

//-----------------------------------------------------------------------------
ctkExpandButton::ctkExpandButton(QWidget *_parent)
  :Superclass(_parent)
  , d_ptr(new ctkExpandButtonPrivate(*this))
{
  Q_D(ctkExpandButton);
  d->init();
}

//-----------------------------------------------------------------------------
ctkExpandButton::~ctkExpandButton()
{
}

//-----------------------------------------------------------------------------
void ctkExpandButton::setMirrorOnExpand(bool newBehavior)
{
  Q_D(ctkExpandButton);
  d->mirrorOnExpand = newBehavior;
  this->updateIcon();
}

//-----------------------------------------------------------------------------
bool ctkExpandButton::mirrorOnExpand() const
{
  Q_D(const ctkExpandButton);
  return d->mirrorOnExpand;
}

//-----------------------------------------------------------------------------
QSize ctkExpandButton::sizeHint() const
{
  int ext = this->style()->pixelMetric(QStyle::PM_ToolBarExtensionExtent);
  return QSize(ext, ext);
}

//-----------------------------------------------------------------------------
void ctkExpandButton::setOrientation(Qt::Orientation newOrientation)
{
  Q_D(ctkExpandButton);
  QStyleOption opt;
  opt.initFrom(this);
  if(newOrientation == Qt::Horizontal)
  {
    d->defaultPixmap = this->style()->standardPixmap(
        QStyle::SP_ToolBarHorizontalExtensionButton, &opt);
    d->orientation = Qt::Horizontal;
  }
  else
  {
    d->defaultPixmap = this->style()->standardPixmap(
        QStyle::SP_ToolBarVerticalExtensionButton, &opt);
    d->orientation = Qt::Vertical;
  }
  this->updateIcon();
}

//-----------------------------------------------------------------------------
Qt::Orientation ctkExpandButton::orientation() const
{
  Q_D(const ctkExpandButton);
  return d->orientation;
}

//-----------------------------------------------------------------------------
void ctkExpandButton::updateIcon()
{
  Q_D(ctkExpandButton);
  // If the orientation is vertical, UpToBottom is LeftToRight and
  // BottomToUp is RightToLeft. Rotate 90' clockwise.
  if(!d->mirrorOnExpand || !this->isChecked())
  {
    this->setIcon(QIcon(d->defaultPixmap));
  }
  else
  {
    QImage mirrorImage =
        d->defaultPixmap.toImage().mirrored(d->orientation == Qt::Horizontal,
                                            d->orientation == Qt::Vertical);
    this->setIcon(QIcon(QPixmap::fromImage(mirrorImage)));
  }
}
