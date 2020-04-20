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
  Qt::CheckState CheckState;

  bool CheckBoxControlsButton;
  bool CheckBoxUserCheckable;
  bool CheckBoxControlsButtonToggleState;
};

//-----------------------------------------------------------------------------
ctkCheckablePushButtonPrivate::ctkCheckablePushButtonPrivate(ctkCheckablePushButton& object)
  : ctkPushButtonPrivate(object)
  , q_ptr(&object)
{
  this->CheckBoxControlsButton = true;
  this->CheckBoxUserCheckable = true;
  this->CheckState = Qt::Unchecked;
  this->CheckBoxControlsButtonToggleState = false;
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
  if (!this->CheckBoxUserCheckable)
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
  if (d->CheckBoxControlsButton)
    {
    bool wasChecked = this->isChecked();
    this->setCheckable(checkState == Qt::Checked);
    // QCheckBox::setCheckable() doesn't emit toggled signal
    if (wasChecked != this->isChecked())
      {
      emit toggled(this->isChecked());
      }
    if (d->CheckBoxControlsButtonToggleState)
      {
      if (this->isChecked() != (checkState == Qt::Checked))
        {
        this->setChecked(checkState == Qt::Checked);
        }
      }
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
  d->CheckBoxControlsButton = b;
  if (b)
    {
    // synchronize checkstate with the checkable property.
    this->setCheckState(
      this->isCheckable() ? Qt::Checked : Qt::Unchecked);
    }
  this->update();
}

//-----------------------------------------------------------------------------
bool ctkCheckablePushButton::checkBoxControlsButton()const
{
  Q_D(const ctkCheckablePushButton);
  return d->CheckBoxControlsButton;
}

//-----------------------------------------------------------------------------
void ctkCheckablePushButton::setCheckBoxControlsButtonToggleState(bool b)
{
  Q_D(ctkCheckablePushButton);
  if (d->CheckBoxControlsButtonToggleState == b)
    {
    return;
    }
  d->CheckBoxControlsButtonToggleState = b;
  if (d->CheckBoxControlsButtonToggleState)
    {
    // We have just enabled sync between toggle state and checkbox.
    // If checkbox is enabled then make the button toggled.
    if (this->checkState() && !this->isChecked())
      {
      this->setChecked(true);
      }
    }
  this->update();
}

//-----------------------------------------------------------------------------
bool ctkCheckablePushButton::checkBoxControlsButtonToggleState()const
{
  Q_D(const ctkCheckablePushButton);
  return d->CheckBoxControlsButtonToggleState;
}

//-----------------------------------------------------------------------------
void ctkCheckablePushButton::setCheckBoxUserCheckable(bool b)
{
  Q_D(ctkCheckablePushButton);
  d->CheckBoxUserCheckable = b;
  this->update();
}

//-----------------------------------------------------------------------------
bool ctkCheckablePushButton::isCheckBoxUserCheckable()const
{
  Q_D(const ctkCheckablePushButton);
  return d->CheckBoxUserCheckable;
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
      (d->CheckBoxUserCheckable))
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

//-----------------------------------------------------------------------------
void ctkCheckablePushButton::checkStateSet()
{
  Q_D(ctkCheckablePushButton);
  this->ctkPushButton::checkStateSet();
  if (d->CheckBoxControlsButtonToggleState)
    {
    // Uncheck the checkbox if button is untoggled
    if (!this->isChecked() && this->checkState())
      {
      this->setCheckState(Qt::Unchecked);
      }
    }
}

//-----------------------------------------------------------------------------
void ctkCheckablePushButton::nextCheckState()
{
  Q_D(ctkCheckablePushButton);
  this->ctkPushButton::nextCheckState();
  if (d->CheckBoxControlsButtonToggleState)
    {
    // Uncheck the checkbox if button is untoggled
    if (!this->isChecked() && this->checkState() == Qt::Checked)
      {
      this->setCheckState(Qt::Unchecked);
      }
    }
}
