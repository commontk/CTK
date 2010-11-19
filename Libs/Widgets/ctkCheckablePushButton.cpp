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
};

//-----------------------------------------------------------------------------
ctkCheckablePushButtonPrivate::ctkCheckablePushButtonPrivate(ctkCheckablePushButton& object)
  :q_ptr(&object)
{
  this->TextAlignment = Qt::AlignLeft | Qt::AlignVCenter;
  this->IndicatorAlignment = Qt::AlignLeft | Qt::AlignVCenter;
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

  QSize indicatorSize = QSize(style()->pixelMetric(QStyle::PM_IndicatorWidth, &opt, this),
                              style()->pixelMetric(QStyle::PM_IndicatorHeight, &opt, this));
  opt.iconSize = indicatorSize;
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
  int indicatorSpacing = this->style()->pixelMetric(QStyle::PM_CheckBoxLabelSpacing, &opt, this);
  int buttonMargin = this->style()->pixelMetric(QStyle::PM_ButtonMargin, &opt, this);
  // Draw Indicator
  QStyleOption indicatorOpt;
  indicatorOpt.init(this);
  if (this->isCheckable())
    {
    indicatorOpt.state |= QStyle::State_On;
    }
  else
    {
    indicatorOpt.state |= QStyle::State_Off;
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
  if (d->checkboxRect().contains(e->pos()))
    {
    //check the checkbox
    this->setCheckable(!this->isCheckable());
    this->update();
    e->accept();
    }
}
