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
#include <QLayout>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionButton>
#include <QStylePainter>

// CTK includes
#include "ctkPushButton_p.h"

//-----------------------------------------------------------------------------
ctkPushButtonPrivate::ctkPushButtonPrivate(ctkPushButton& object)
  :q_ptr(&object)
{
  this->ButtonTextAlignment = Qt::AlignHCenter | Qt::AlignVCenter;
  this->IconAlignment = Qt::AlignLeft | Qt::AlignVCenter;
  this->IconSpacing = 4;
}

//-----------------------------------------------------------------------------
ctkPushButtonPrivate::~ctkPushButtonPrivate()
{
}

//-----------------------------------------------------------------------------
void ctkPushButtonPrivate::init()
{
}

//-----------------------------------------------------------------------------
QRect ctkPushButtonPrivate::iconRect()const
{
  Q_Q(const ctkPushButton);
  QRect rect;
  QStyleOptionButton opt;
  q->initStyleOption(&opt);

  QSize iconSize = q->iconSize();
  int buttonHeight = opt.rect.height();
  uint tf = this->ButtonTextAlignment;
  if (q->style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, q))
    {
    tf |= Qt::TextShowMnemonic;
    }
  else
    {
    tf |= Qt::TextHideMnemonic;
    }
  int textWidth = opt.fontMetrics.boundingRect(opt.rect, tf, opt.text).width();
  int iconSpacing = this->IconSpacing;
  int buttonMargin = q->style()->pixelMetric(QStyle::PM_ButtonMargin, &opt, q);
  if (this->IconAlignment & Qt::AlignLeft)
    {
    rect = QRect((buttonHeight - iconSize.width()) / 2,
                 (buttonHeight - iconSize.height()) / 2,
                 iconSize.width(), iconSize.height());
    }
  else if (this->IconAlignment & Qt::AlignHCenter)
    {
    int w = iconSize.width();
    if (!opt.text.isEmpty() && (this->ButtonTextAlignment & Qt::AlignHCenter))
      {
      w += textWidth + iconSpacing;
      }
    rect = QRect(opt.rect.x()+ opt.rect.width() /2 - w / 2,
                 (buttonHeight - iconSize.height()) / 2,
                 iconSize.width(), iconSize.height());
    if (this->ButtonTextAlignment & Qt::AlignLeft &&
        rect.left() < opt.rect.x() + buttonMargin + textWidth)
      {
      rect.moveLeft(opt.rect.x() + buttonMargin + textWidth);
      }
    else if (this->ButtonTextAlignment & Qt::AlignRight &&
             rect.right() > opt.rect.right() - buttonMargin - textWidth)
      {
      rect.moveRight(opt.rect.right() - buttonMargin - textWidth);
      }
    }
  else if (this->IconAlignment & Qt::AlignRight)
    {
    rect = QRect(opt.rect.width() - (buttonHeight - iconSize.width()) / 2
                                  - iconSize.width(),
                 (buttonHeight - iconSize.height()) / 2,
                 iconSize.width(), iconSize.height());
    }
  return rect;
}

//-----------------------------------------------------------------------------
QSize ctkPushButtonPrivate::buttonSizeHint()const
{
  Q_Q(const ctkPushButton);
  int w = 0, h = 0;

  QStyleOptionButton opt;
  opt.initFrom(q);

  // icon
  QSize iconSize = q->iconSize();
  int ih = iconSize.height();
  int iw = iconSize.width() + this->IconSpacing;
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
QStyleOptionButton ctkPushButtonPrivate::drawIcon(QPainter* p)
{
  Q_Q(ctkPushButton);
  QStyleOptionButton iconOpt;
  iconOpt.init(q);
  iconOpt.rect = this->iconRect();
  if (q->icon().isNull())
    {
    iconOpt.rect.setWidth(0);
    return iconOpt;
    }
  QIcon::Mode mode = iconOpt.state & QStyle::State_Enabled ? QIcon::Normal : QIcon::Disabled;
  if (mode == QIcon::Normal && iconOpt.state & QStyle::State_HasFocus)
    {
    mode = QIcon::Active;
    }
  QIcon::State state = QIcon::Off;
  if (iconOpt.state & QStyle::State_On)
    {
    state = QIcon::On;
    }

  QPixmap pixmap = q->icon().pixmap(iconOpt.rect.size(), mode, state);
  p->drawPixmap(iconOpt.rect, pixmap);
  return iconOpt;
}

//-----------------------------------------------------------------------------
ctkPushButton::ctkPushButton(QWidget* _parent)
  : QPushButton(_parent)
  , d_ptr(new ctkPushButtonPrivate(*this))
{
  Q_D(ctkPushButton);
  d->init();
}

//-----------------------------------------------------------------------------
ctkPushButton::ctkPushButton(const QString& title, QWidget* _parent)
  : QPushButton(title, _parent)
  , d_ptr(new ctkPushButtonPrivate(*this))
{
  Q_D(ctkPushButton);
  d->init();
}

//-----------------------------------------------------------------------------
ctkPushButton::ctkPushButton(const QIcon& icon, const QString& title,
                             QWidget* _parent)
  : QPushButton(icon, title, _parent)
  , d_ptr(new ctkPushButtonPrivate(*this))
{
  Q_D(ctkPushButton);
  d->init();
}

//-----------------------------------------------------------------------------
ctkPushButton::ctkPushButton(ctkPushButtonPrivate* pimpl, QWidget* _parent)
  : QPushButton(_parent)
  , d_ptr(pimpl)
{
}

//-----------------------------------------------------------------------------
ctkPushButton::~ctkPushButton()
{
}

//-----------------------------------------------------------------------------
void ctkPushButton::setButtonTextAlignment(Qt::Alignment newButtonTextAlignment)
{
  Q_D(ctkPushButton);
  d->ButtonTextAlignment = newButtonTextAlignment;
  this->update();
}

//-----------------------------------------------------------------------------
Qt::Alignment ctkPushButton::buttonTextAlignment()const
{
  Q_D(const ctkPushButton);
  return d->ButtonTextAlignment;
}

//-----------------------------------------------------------------------------
void ctkPushButton::setIconAlignment(Qt::Alignment newIconAlignment)
{
  Q_D(ctkPushButton);
  d->IconAlignment = newIconAlignment;
  this->update();
}

//-----------------------------------------------------------------------------
Qt::Alignment ctkPushButton::iconAlignment()const
{
  Q_D(const ctkPushButton);
  return d->IconAlignment;
}

//-----------------------------------------------------------------------------
QSize ctkPushButton::minimumSizeHint()const
{
  Q_D(const ctkPushButton);
  return d->buttonSizeHint();
}

//-----------------------------------------------------------------------------
QSize ctkPushButton::sizeHint()const
{
  return this->minimumSizeHint();
}

//-----------------------------------------------------------------------------
void ctkPushButton::paintEvent(QPaintEvent * _event)
{
  Q_UNUSED(_event);
  Q_D(ctkPushButton);

  QPainter p(this);
  // Draw Button
  QStyleOptionButton opt;
  this->initStyleOption(&opt);

  // Checkbox size
  QSize iconSize = this->iconSize();
  // Replace the icon size by the checkbox size
  opt.iconSize = iconSize;
  // Draw the panel of the button (no text, no icon)
  style()->drawControl(QStyle::CE_PushButtonBevel, &opt, &p, this);
  // TBD is PE_PanelButtonCommand better ?
  //style()->drawPrimitive(QStyle::PE_PanelButtonCommand, &opt, &p, this);
  //int buttonHeight = opt.rect.height();
  uint tf = d->ButtonTextAlignment;
  if (this->style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, this))
    {
    tf |= Qt::TextShowMnemonic;
    }
  else
    {
    tf |= Qt::TextHideMnemonic;
    }
  int textWidth = opt.fontMetrics.boundingRect(opt.rect, tf, opt.text).width();
  int buttonMargin = this->style()->pixelMetric(QStyle::PM_ButtonMargin, &opt, this);
  // Draw Icon
  QStyleOptionButton iconOpt = d->drawIcon(&p);
  // Spacing between the text and the checkbox
  int iconSpacing = d->IconSpacing;

  // Draw Text
  if (d->ButtonTextAlignment & Qt::AlignLeft)
    {
    if (d->IconAlignment & Qt::AlignLeft)
      {
      opt.rect.setLeft(iconOpt.rect.right() + iconSpacing);
      }
    else
      {
      opt.rect.setLeft(opt.rect.x() + buttonMargin);
      }
    }
  else if (d->ButtonTextAlignment & Qt::AlignHCenter)
    {
    if (d->IconAlignment & Qt::AlignHCenter)
      {
      opt.rect.setLeft(iconOpt.rect.right() + iconSpacing);
      }
    else
      {
      opt.rect.setLeft(opt.rect.x() + opt.rect.width() / 2 - textWidth / 2);
      if (d->IconAlignment & Qt::AlignLeft)
        {
        opt.rect.setLeft( qMax(iconOpt.rect.right() + iconSpacing, opt.rect.left()) );
        }
      }
    }
  else if (d->ButtonTextAlignment & Qt::AlignRight)
    {
    if (d->IconAlignment & Qt::AlignRight)
      {
      opt.rect.setLeft(iconOpt.rect.left() - iconSpacing - textWidth);
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
