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
#include <QCleanlooksStyle>
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

//-----------------------------------------------------------------------------
class ctkCheckablePushButtonPrivate
{
  Q_DECLARE_PUBLIC(ctkCheckablePushButton);
protected:
  ctkCheckablePushButton* const q_ptr;
public:
  ctkCheckablePushButtonPrivate(ctkCheckablePushButton& object);
  void init();

  QRect checkboxRect() const;
  QSize buttonSizeHint()const;

  // Tuning of the button look&feel
  Qt::Alignment TextAlignment;
  Qt::Alignment IndicatorAlignment;
  Qt::ItemFlags CheckBoxFlags;
  Qt::CheckState CheckState;
};

//-----------------------------------------------------------------------------
ctkCheckablePushButtonPrivate::ctkCheckablePushButtonPrivate(ctkCheckablePushButton& object)
  :q_ptr(&object)
{
  this->TextAlignment = Qt::AlignLeft | Qt::AlignVCenter;
  this->IndicatorAlignment = Qt::AlignLeft | Qt::AlignVCenter;
  this->CheckBoxFlags = Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
  this->CheckState = Qt::Unchecked;
}

//-----------------------------------------------------------------------------
void ctkCheckablePushButtonPrivate::init()
{
}

//-----------------------------------------------------------------------------
QRect ctkCheckablePushButtonPrivate::checkboxRect()const
{
  Q_Q(const ctkCheckablePushButton);
  QRect rect;
  QStyleOptionButton opt;
  q->initStyleOption(&opt);

  QSize indicatorSize = QSize(q->style()->pixelMetric(QStyle::PM_IndicatorWidth, &opt, q),
                              q->style()->pixelMetric(QStyle::PM_IndicatorHeight, &opt, q));
  int buttonHeight = opt.rect.height();
  uint tf = this->TextAlignment;
  if (q->style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, q))
    {
    tf |= Qt::TextShowMnemonic;
    }
  else
    {
    tf |= Qt::TextHideMnemonic;
    }
  int textWidth = opt.fontMetrics.boundingRect(opt.rect, tf, opt.text).width();
  int indicatorSpacing = q->style()->pixelMetric(QStyle::PM_CheckBoxLabelSpacing, &opt, q);
  int buttonMargin = q->style()->pixelMetric(QStyle::PM_ButtonMargin, &opt, q);
  if (this->IndicatorAlignment & Qt::AlignLeft)
    {
    rect = QRect((buttonHeight - indicatorSize.width()) / 2,
                 (buttonHeight - indicatorSize.height()) / 2,
                 indicatorSize.width(), indicatorSize.height());
    }
  else if (this->IndicatorAlignment & Qt::AlignHCenter)
    {
    int w = indicatorSize.width();
    if (!opt.text.isEmpty() && (this->TextAlignment & Qt::AlignHCenter))
      {
      w += textWidth + indicatorSpacing;
      }
    rect = QRect(opt.rect.x()+ opt.rect.width() /2 - w / 2,
                 (buttonHeight - indicatorSize.height()) / 2,
                 indicatorSize.width(), indicatorSize.height());
    if (this->TextAlignment & Qt::AlignLeft &&
        rect.left() < opt.rect.x() + buttonMargin + textWidth)
      {
      rect.moveLeft(opt.rect.x() + buttonMargin + textWidth);
      }
    else if (this->TextAlignment & Qt::AlignRight &&
             rect.right() > opt.rect.right() - buttonMargin - textWidth)
      {
      rect.moveRight(opt.rect.right() - buttonMargin - textWidth);
      }
    }
  else if (this->IndicatorAlignment & Qt::AlignRight)
    {
    rect = QRect(opt.rect.width() - (buttonHeight - indicatorSize.width()) / 2
                                  - indicatorSize.width(),
                 (buttonHeight - indicatorSize.height()) / 2,
                 indicatorSize.width(), indicatorSize.height());
    }
  return rect;
}

//-----------------------------------------------------------------------------
QSize ctkCheckablePushButtonPrivate::buttonSizeHint()const
{
  Q_Q(const ctkCheckablePushButton);
  int w = 0, h = 0;

  QStyleOptionButton opt;
  opt.initFrom(q);
  
  // indicator
  QSize indicatorSize = QSize(q->style()->pixelMetric(QStyle::PM_IndicatorWidth, &opt, q),
                              q->style()->pixelMetric(QStyle::PM_IndicatorHeight, &opt, q));
  int indicatorSpacing = q->style()->pixelMetric(QStyle::PM_CheckBoxLabelSpacing, &opt, q);
  int ih = indicatorSize.height();
  int iw = indicatorSize.width() + indicatorSpacing;
  w += iw;
  h = qMax(h, ih);
  
  // text 
  QString string(q->text());
  bool empty = string.isEmpty();
  if (empty)
    {
    string = QString::fromLatin1("XXXX");
    }
  QFontMetrics fm = q->fontMetrics();
  QSize sz = fm.size(Qt::TextShowMnemonic, string);
  if(!empty || !w)
    {
    w += sz.width();
    }
  h = qMax(h, sz.height());
  //opt.rect.setSize(QSize(w, h)); // PM_MenuButtonIndicator depends on the height
  QSize buttonSize = (q->style()->sizeFromContents(QStyle::CT_PushButton, &opt, QSize(w, h), q).
                      expandedTo(QApplication::globalStrut()));
  return buttonSize;
}

//-----------------------------------------------------------------------------
ctkCheckablePushButton::ctkCheckablePushButton(QWidget* _parent)
  :QPushButton(_parent)
  , d_ptr(new ctkCheckablePushButtonPrivate(*this))
{
  Q_D(ctkCheckablePushButton);
  d->init();
}

//-----------------------------------------------------------------------------
ctkCheckablePushButton::ctkCheckablePushButton(const QString& title, QWidget* _parent)
  :QPushButton(title, _parent)
  , d_ptr(new ctkCheckablePushButtonPrivate(*this))
{
}

//-----------------------------------------------------------------------------
ctkCheckablePushButton::~ctkCheckablePushButton()
{
}

//-----------------------------------------------------------------------------
void ctkCheckablePushButton::setButtonTextAlignment(Qt::Alignment textAlignment)
{
  Q_D(ctkCheckablePushButton);
  d->TextAlignment = textAlignment;
  this->update();
}

//-----------------------------------------------------------------------------
Qt::Alignment ctkCheckablePushButton::buttonTextAlignment()const
{
  Q_D(const ctkCheckablePushButton);
  return d->TextAlignment;
}

//-----------------------------------------------------------------------------
void ctkCheckablePushButton::setIndicatorAlignment(Qt::Alignment indicatorAlignment)
{
  Q_D(ctkCheckablePushButton);
  d->IndicatorAlignment = indicatorAlignment;
  this->update();
}

//-----------------------------------------------------------------------------
Qt::Alignment ctkCheckablePushButton::indicatorAlignment()const
{
  Q_D(const ctkCheckablePushButton);
  return d->IndicatorAlignment;
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
  if (d->CheckBoxFlags & Qt::ItemIsEnabled)
    {
    this->setCheckable(checkState == Qt::Checked);
    }
  this->update();
  emit checkStateChanged(d->CheckState);
  emit checkBoxToggled(d->CheckState == Qt::Checked);
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
QSize ctkCheckablePushButton::minimumSizeHint()const
{
  Q_D(const ctkCheckablePushButton);
  return d->buttonSizeHint();
}

//-----------------------------------------------------------------------------
QSize ctkCheckablePushButton::sizeHint()const
{
  return this->minimumSizeHint();
}

//-----------------------------------------------------------------------------
void ctkCheckablePushButton::paintEvent(QPaintEvent * _event)
{
  Q_UNUSED(_event);
  Q_D(ctkCheckablePushButton);

  QPainter p(this);
  // Draw Button
  QStyleOptionButton opt;
  this->initStyleOption(&opt);

  // Checkbox size
  QSize indicatorSize = QSize(style()->pixelMetric(QStyle::PM_IndicatorWidth, &opt, this),
                              style()->pixelMetric(QStyle::PM_IndicatorHeight, &opt, this));
  // Replace the icon size by the checkbox size
  opt.iconSize = indicatorSize;
  // Draw the panel of the button (no text, no icon)
  style()->drawControl(QStyle::CE_PushButtonBevel, &opt, &p, this);
  // TBD is PE_PanelButtonCommand better ?
  //style()->drawPrimitive(QStyle::PE_PanelButtonCommand, &opt, &p, this);
  //int buttonHeight = opt.rect.height();
  uint tf = d->TextAlignment;
  if (this->style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, this))
    {
    tf |= Qt::TextShowMnemonic;
    }
  else
    {
    tf |= Qt::TextHideMnemonic;
    }
  int textWidth = opt.fontMetrics.boundingRect(opt.rect, tf, opt.text).width();
  // Spacing between the text and the checkbox
  int indicatorSpacing = this->style()->pixelMetric(QStyle::PM_CheckBoxLabelSpacing, &opt, this);
  int buttonMargin = this->style()->pixelMetric(QStyle::PM_ButtonMargin, &opt, this);
  // Draw Indicator
  QStyleOptionButton indicatorOpt;
  indicatorOpt.init(this);
  if (!(d->CheckBoxFlags & Qt::ItemIsUserCheckable))
    {
    indicatorOpt.state &= ~QStyle::State_Enabled;
    }
  if (this->checkBoxControlsButton())
    {
    // Hack: calling setCheckable() instead of setCheckState while being in a
    // control button mode leads to an inconsistent state, we need to make
    // synchronize the 2 properties.
    this->setCheckState(this->isCheckable() ? Qt::Checked : Qt::Unchecked);
    }
  switch (d->CheckState)
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
  indicatorOpt.rect = d->checkboxRect();
  this->style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &indicatorOpt, &p, 0);

  // Draw Text
  if (d->TextAlignment & Qt::AlignLeft)
    {
    if (d->IndicatorAlignment & Qt::AlignLeft)
      {
      opt.rect.setLeft(indicatorOpt.rect.right() + indicatorSpacing);
      }
    else
      {
      opt.rect.setLeft(opt.rect.x() + buttonMargin);
      }
    }
  else if (d->TextAlignment & Qt::AlignHCenter)
    {
    if (d->IndicatorAlignment & Qt::AlignHCenter)
      {
      opt.rect.setLeft(indicatorOpt.rect.right() + indicatorSpacing);
      }
    else
      {
      opt.rect.setLeft(opt.rect.x() + opt.rect.width() / 2 - textWidth / 2);
      if (d->IndicatorAlignment & Qt::AlignLeft)
        {
        opt.rect.setLeft( qMax(indicatorOpt.rect.right() + indicatorSpacing, opt.rect.left()) );
        }
      }
    }
  else if (d->TextAlignment & Qt::AlignRight)
    {
    if (d->IndicatorAlignment & Qt::AlignRight)
      {
      opt.rect.setLeft(indicatorOpt.rect.left() - indicatorSpacing - textWidth);
      }
    else
      {
      opt.rect.setLeft(opt.rect.right() - buttonMargin - textWidth);
      }
    }
  // all the computations have been made infering the text would be left oriented
  tf &= ~Qt::AlignHCenter & ~Qt::AlignRight;
  tf |= Qt::AlignLeft;
  this->style()->drawItemText(&p, opt.rect, tf, opt.palette, (opt.state & QStyle::State_Enabled),
                        opt.text, QPalette::ButtonText);
}

//-----------------------------------------------------------------------------
bool ctkCheckablePushButton::hitButton(const QPoint & _pos)const
{
  Q_D(const ctkCheckablePushButton);
  return !d->checkboxRect().contains(_pos) 
    && this->QPushButton::hitButton(_pos);
}

//-----------------------------------------------------------------------------
void ctkCheckablePushButton::initStyleOption(QStyleOptionButton* option)const
{
  this->QPushButton::initStyleOption(option);
  option->iconSize = QSize(this->style()->pixelMetric(QStyle::PM_IndicatorWidth, option, this),
                           this->style()->pixelMetric(QStyle::PM_IndicatorHeight, option, this));
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
  if (d->checkboxRect().contains(e->pos()) &&
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
