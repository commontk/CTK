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
#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QLayout>
#include <QMouseEvent>
#include <QMenu>
#include <QPainter>
#include <QPointer>
#include <QPushButton>
#include <QStyle>
#include <QStyleOptionButton>
#include <QStylePainter>
#include <QToolBar>

// CTK includes
#include "ctkCheckablePushButton.h"
#include "ctkPushButton_p.h"

//-----------------------------------------------------------------------------
class ctkCheckablePushButtonPrivate: public ctkPushButtonPrivate
{
  Q_DECLARE_PUBLIC(ctkCheckablePushButton);
protected:
  ctkCheckablePushButton* const q_ptr;
public:
  ctkCheckablePushButtonPrivate(ctkCheckablePushButton& object);
  virtual void init();
  virtual QStyleOptionButton drawIcon(QPainter* p);

  // Tuning of the button look&feel
  Qt::ItemFlags CheckBoxFlags;
  Qt::CheckState CheckState;
};

//-----------------------------------------------------------------------------
ctkCheckablePushButtonPrivate::ctkCheckablePushButtonPrivate(ctkCheckablePushButton& object)
  : ctkPushButtonPrivate(object)
  , q_ptr(&object)
{
  this->CheckBoxFlags = Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
  this->CheckState = Qt::Unchecked;
}

//-----------------------------------------------------------------------------
void ctkCheckablePushButtonPrivate::init()
{
  Q_Q(ctkCheckablePushButton);

  QStyleOptionButton opt;
  opt.initFrom(q);

  QSize indicatorSize = QSize(q->style()->pixelMetric(QStyle::PM_IndicatorWidth, &opt, q),
                              q->style()->pixelMetric(QStyle::PM_IndicatorHeight, &opt, q));
  q->setIconSize(indicatorSize);
  this->IconSpacing = q->style()->pixelMetric(QStyle::PM_CheckBoxLabelSpacing, &opt, q);
}

//-----------------------------------------------------------------------------
QStyleOptionButton ctkCheckablePushButtonPrivate::drawIcon(QPainter* p)
{
  Q_Q(ctkCheckablePushButton);

  QStyleOptionButton indicatorOpt;

  indicatorOpt.init(q);
  if (!(this->CheckBoxFlags & Qt::ItemIsUserCheckable))
    {
    indicatorOpt.state &= ~QStyle::State_Enabled;
    }
  if (q->checkBoxControlsButton())
    {
    // Hack: calling setCheckable() instead of setCheckState while being in a
    // control button mode leads to an inconsistent state, we need to make
    // synchronize the 2 properties.
    q->setCheckState(q->isCheckable() ? Qt::Checked : Qt::Unchecked);
    }
  switch (this->CheckState)
    {
    case Qt::Checked:
      indicatorOpt.state |= QStyle::State_On;
      break;
    case Qt::PartiallyChecked:
      indicatorOpt.state |= QStyle::State_NoChange;
      break;
    default:
    case Qt::Unchecked:
      indicatorOpt.state |= QStyle::State_Off;
      break;
    }
  indicatorOpt.rect = this->iconRect();
  q->style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &indicatorOpt, p, 0);
  return indicatorOpt;
}


//-----------------------------------------------------------------------------
ctkCheckablePushButton::ctkCheckablePushButton(QWidget* _parent)
  : ctkPushButton(new ctkCheckablePushButtonPrivate(*this), _parent)
{
  Q_D(ctkCheckablePushButton);
  d->init();
}

//-----------------------------------------------------------------------------
ctkCheckablePushButton::ctkCheckablePushButton(const QString& title, QWidget* _parent)
  : ctkPushButton(new ctkCheckablePushButtonPrivate(*this), _parent)
{
  Q_D(ctkCheckablePushButton);
  d->init();
  this->setText(title);
}

//-----------------------------------------------------------------------------
ctkCheckablePushButton::~ctkCheckablePushButton()
{
}

//-----------------------------------------------------------------------------
void ctkCheckablePushButton::setIndicatorAlignment(Qt::Alignment indicatorAlignment)
{
  this->setIconAlignment(indicatorAlignment);
}

//-----------------------------------------------------------------------------
Qt::Alignment ctkCheckablePushButton::indicatorAlignment()const
{
  return this->iconAlignment();
}

//-----------------------------------------------------------------------------
void ctkCheckablePushButton::setCheckState(Qt::CheckState checkState)
{
  Q_D(ctkCheckablePushButton);
  Qt::CheckState oldCheckState = d->CheckState;
  if (checkState == oldCheckState)
    {
    return;
    }
  d->CheckState = checkState;
  bool emitToggled = false;
  if (d->CheckBoxFlags & Qt::ItemIsEnabled)
    {
    bool wasChecked = this->isChecked();
    // QCheckBox::setCheckable() doesn't emit toggled signal
    this->setCheckable(checkState == Qt::Checked);
    emitToggled = (wasChecked != this->isChecked());
    }
  if (emitToggled)
    {
    emit toggled(this->isChecked());
    }
  emit checkStateChanged(d->CheckState);
  emit checkBoxToggled(d->CheckState == Qt::Checked);
  this->update();
}

//-----------------------------------------------------------------------------
Qt::CheckState ctkCheckablePushButton::checkState()const
{
  Q_D(const ctkCheckablePushButton);
  return d->CheckState;
}

//-----------------------------------------------------------------------------
void ctkCheckablePushButton::setCheckBoxControlsButton(bool b)
{
  Q_D(ctkCheckablePushButton);
  if (b)
    {
    d->CheckBoxFlags |= Qt::ItemIsEnabled;
    // synchronize checkstate with the checkable property.
    this->setCheckState(
      this->isCheckable() ? Qt::Checked : Qt::Unchecked);
    }
  else
    {
    d->CheckBoxFlags &= ~Qt::ItemIsEnabled;
    }
  this->update();
}

//-----------------------------------------------------------------------------
bool ctkCheckablePushButton::checkBoxControlsButton()const
{
  Q_D(const ctkCheckablePushButton);
  return d->CheckBoxFlags & Qt::ItemIsEnabled;
}

//-----------------------------------------------------------------------------
void ctkCheckablePushButton::setCheckBoxUserCheckable(bool b)
{
  Q_D(ctkCheckablePushButton);
  if (b)
    {
    d->CheckBoxFlags |= Qt::ItemIsUserCheckable;
    }
  else
    {
    d->CheckBoxFlags &= ~Qt::ItemIsUserCheckable;
    }
  this->update();
}

//-----------------------------------------------------------------------------
bool ctkCheckablePushButton::isCheckBoxUserCheckable()const
{
  Q_D(const ctkCheckablePushButton);
  return d->CheckBoxFlags & Qt::ItemIsUserCheckable;
}

//-----------------------------------------------------------------------------
bool ctkCheckablePushButton::hitButton(const QPoint & _pos)const
{
  Q_D(const ctkCheckablePushButton);
  return !d->iconRect().contains(_pos)
    && this->QPushButton::hitButton(_pos);
}

//-----------------------------------------------------------------------------
void ctkCheckablePushButton::mousePressEvent(QMouseEvent *e)
{
  Q_D(ctkCheckablePushButton);
  this->QPushButton::mousePressEvent(e);
  if (e->isAccepted())
    {
    return;
    }
  if (d->iconRect().contains(e->pos()) &&
      (d->CheckBoxFlags & Qt::ItemIsUserCheckable))
    {
    Qt::CheckState newCheckState;
    switch (d->CheckState)
      {
      case Qt::Unchecked:
      case Qt::PartiallyChecked:
        newCheckState = Qt::Checked;
        break;
      default:
      case Qt::Checked:
        newCheckState = Qt::Unchecked;
        break;
      }
    this->setCheckState(newCheckState);
    e->accept();
    }
}
