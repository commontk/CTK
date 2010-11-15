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
#include <QLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QStyle>
#include <QStyleOptionButton>
#include <QStyleOptionFrameV3>

// CTK includes
#include "ctkCollapsibleButton.h"

//-----------------------------------------------------------------------------
class ctkCollapsibleButtonPrivate
{
  Q_DECLARE_PUBLIC(ctkCollapsibleButton);
protected:
  ctkCollapsibleButton* const q_ptr;
public:
  ctkCollapsibleButtonPrivate(ctkCollapsibleButton& object);
  void init();

  bool     Collapsed;

  // Contents frame
  QFrame::Shape  ContentsFrameShape;
  QFrame::Shadow ContentsFrameShadow;
  int            ContentsLineWidth;
  int            ContentsMidLineWidth;

  int      CollapsedHeight;
  bool     ExclusiveMouseOver;
  bool     LookOffWhenChecked;

  int      MaximumHeight;  // use carefully

  // Tuning of the button look&feel
  Qt::Alignment TextAlignment;
  Qt::Alignment IndicatorAlignment;
};

//-----------------------------------------------------------------------------
ctkCollapsibleButtonPrivate::ctkCollapsibleButtonPrivate(ctkCollapsibleButton& object)
  :q_ptr(&object)
{
}

//-----------------------------------------------------------------------------
void ctkCollapsibleButtonPrivate::init()
{
  Q_Q(ctkCollapsibleButton);
  q->setCheckable(true);
  // checked and Collapsed are synchronized: checked != Collapsed
  q->setChecked(true);

  this->Collapsed = false;

  this->ContentsFrameShape = QFrame::NoFrame;
  this->ContentsFrameShadow = QFrame::Plain;
  this->ContentsLineWidth = 1;
  this->ContentsMidLineWidth = 0;

  this->CollapsedHeight = 10;
  this->ExclusiveMouseOver = false;  
  this->LookOffWhenChecked = true; // set as a prop ?
  
  this->MaximumHeight = q->maximumHeight();

  this->TextAlignment = Qt::AlignLeft | Qt::AlignVCenter;
  this->IndicatorAlignment = Qt::AlignLeft | Qt::AlignVCenter;

  q->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,
                               QSizePolicy::Preferred, 
                               QSizePolicy::DefaultType));
  q->setContentsMargins(0, q->buttonSizeHint().height(),0 , 0);
  // by default QAbstractButton changed the background role to Button
  // we want a regular background 
  q->setBackgroundRole(QPalette::Window);

  QObject::connect(q, SIGNAL(toggled(bool)),
                   q, SLOT(onToggled(bool)));
}

//-----------------------------------------------------------------------------
void ctkCollapsibleButton::initStyleOption(QStyleOptionButton* option)const
{
  Q_D(const ctkCollapsibleButton);
  if (option == 0)
    {
    return;
    }
  option->initFrom(this);

  if (this->isDown() )
    {
    option->state |= QStyle::State_Sunken;
    }
  if (this->isChecked() && !d->LookOffWhenChecked)
    {
    option->state |= QStyle::State_On;
    }
  if (!this->isDown())
    {
    option->state |= QStyle::State_Raised;
    }

  option->text = this->text();
  option->icon = this->icon();
  option->iconSize = QSize(this->style()->pixelMetric(QStyle::PM_IndicatorWidth, option, this),
                       this->style()->pixelMetric(QStyle::PM_IndicatorHeight, option, this));
  int buttonHeight = this->buttonSizeHint().height();//qMax(this->fontMetrics().height(), option->iconSize.height());
  option->rect.setHeight(buttonHeight);
}

//-----------------------------------------------------------------------------
ctkCollapsibleButton::ctkCollapsibleButton(QWidget* _parent)
  :QAbstractButton(_parent)
  , d_ptr(new ctkCollapsibleButtonPrivate(*this))
{
  Q_D(ctkCollapsibleButton);
  d->init();
}

//-----------------------------------------------------------------------------
ctkCollapsibleButton::ctkCollapsibleButton(const QString& title, QWidget* _parent)
  :QAbstractButton(_parent)
  , d_ptr(new ctkCollapsibleButtonPrivate(*this))
{
  Q_D(ctkCollapsibleButton);
  d->init();
  this->setText(title);
}

//-----------------------------------------------------------------------------
ctkCollapsibleButton::~ctkCollapsibleButton()
{
}

//-----------------------------------------------------------------------------
void ctkCollapsibleButton::setCollapsed(bool c)
{
  if (!this->isCheckable())
    {
    // not sure if one should handle this case...
    this->collapse(c);
    return;
    }
  this->setChecked(!c);
}

//-----------------------------------------------------------------------------
bool ctkCollapsibleButton::collapsed()const
{
  Q_D(const ctkCollapsibleButton);
  return d->Collapsed;
}

//-----------------------------------------------------------------------------
void ctkCollapsibleButton::setCollapsedHeight(int h)
{
  Q_D(ctkCollapsibleButton);
  d->CollapsedHeight = h;
  this->updateGeometry();
}

//-----------------------------------------------------------------------------
int ctkCollapsibleButton::collapsedHeight()const
{
  Q_D(const ctkCollapsibleButton);
  return d->CollapsedHeight;
}

//-----------------------------------------------------------------------------
void ctkCollapsibleButton::onToggled(bool checked)
{
  if (this->isCheckable())
    {
    this->collapse(!checked);
    }
}

//-----------------------------------------------------------------------------
void ctkCollapsibleButton::collapse(bool c)
{
  Q_D(ctkCollapsibleButton);
  if (c == d->Collapsed)
    {
    return;
    }

  d->Collapsed = c;

  // we do that here as setVisible calls will correctly refresh the widget
  if (c)
    {
    d->MaximumHeight = this->maximumHeight();
    this->setMaximumHeight(this->sizeHint().height());
    //this->updateGeometry();
    }
  else
    {
    // restore maximumheight
    this->setMaximumHeight(d->MaximumHeight);
    this->updateGeometry();
    }

  QObjectList childList = this->children();
  for (int i = 0; i < childList.size(); ++i)
    {
    QObject *o = childList.at(i);
    if (!o->isWidgetType())
      {
      continue;
      }
    QWidget *w = static_cast<QWidget *>(o);
    w->setVisible(!c);
    }
  // this might be too many updates...
  QWidget* _parent = this->parentWidget();
  if (!d->Collapsed && (!_parent || !_parent->layout()))
    {
    this->resize(this->sizeHint());
    }
  else
    {
    this->updateGeometry();
    }
  //this->update(QRect(QPoint(0,0), this->sizeHint()));
  //this->repaint(QRect(QPoint(0,0), this->sizeHint()));
  emit contentsCollapsed(c);
}

//-----------------------------------------------------------------------------
QFrame::Shape ctkCollapsibleButton::contentsFrameShape() const
{
  Q_D(const ctkCollapsibleButton);
  return d->ContentsFrameShape;
}

//-----------------------------------------------------------------------------
void ctkCollapsibleButton::setContentsFrameShape(QFrame::Shape s)
{
  Q_D(ctkCollapsibleButton);
  d->ContentsFrameShape = s;
}

//-----------------------------------------------------------------------------
QFrame::Shadow ctkCollapsibleButton::contentsFrameShadow() const
{
  Q_D(const ctkCollapsibleButton);
  return d->ContentsFrameShadow;
}

//-----------------------------------------------------------------------------
void ctkCollapsibleButton::setContentsFrameShadow(QFrame::Shadow s)
{
  Q_D(ctkCollapsibleButton);
  d->ContentsFrameShadow = s;
}

//-----------------------------------------------------------------------------
int ctkCollapsibleButton:: contentsLineWidth() const
{
  Q_D(const ctkCollapsibleButton);
  return d->ContentsLineWidth;
}

//-----------------------------------------------------------------------------
void ctkCollapsibleButton::setContentsLineWidth(int w)
{
  Q_D(ctkCollapsibleButton);
  d->ContentsLineWidth = w;
}

//-----------------------------------------------------------------------------
int ctkCollapsibleButton::contentsMidLineWidth() const
{
  Q_D(const ctkCollapsibleButton);
  return d->ContentsMidLineWidth;
}

//-----------------------------------------------------------------------------
void ctkCollapsibleButton::setContentsMidLineWidth(int w)
{
  Q_D(ctkCollapsibleButton);
  d->ContentsMidLineWidth = w;
}

//-----------------------------------------------------------------------------
void ctkCollapsibleButton::setButtonTextAlignment(Qt::Alignment textAlignment)
{
  Q_D(ctkCollapsibleButton);
  d->TextAlignment = textAlignment;
  this->update();
}

//-----------------------------------------------------------------------------
Qt::Alignment ctkCollapsibleButton::buttonTextAlignment()const
{
  Q_D(const ctkCollapsibleButton);
  return d->TextAlignment;
}

//-----------------------------------------------------------------------------
void ctkCollapsibleButton::setIndicatorAlignment(Qt::Alignment indicatorAlignment)
{
  Q_D(ctkCollapsibleButton);
  d->IndicatorAlignment = indicatorAlignment;
  this->update();
}

//-----------------------------------------------------------------------------
Qt::Alignment ctkCollapsibleButton::indicatorAlignment()const
{
  Q_D(const ctkCollapsibleButton);
  return d->IndicatorAlignment;
}

//-----------------------------------------------------------------------------
QSize ctkCollapsibleButton::buttonSizeHint()const
{
  int w = 0, h = 0;

  QStyleOptionButton opt;
  opt.initFrom(this);
  
  // indicator
  QSize indicatorSize = QSize(style()->pixelMetric(QStyle::PM_IndicatorWidth, &opt, this),
                              style()->pixelMetric(QStyle::PM_IndicatorHeight, &opt, this));
  int indicatorSpacing = style()->pixelMetric(QStyle::PM_CheckBoxLabelSpacing, &opt, this);
  int ih = indicatorSize.height();
  int iw = indicatorSize.width() + indicatorSpacing;
  w += iw;
  h = qMax(h, ih);
  
  // text 
  QString string(this->text());
  bool empty = string.isEmpty();
  if (empty)
    {
    string = QString::fromLatin1("XXXX");
    }
  QFontMetrics fm = this->fontMetrics();
  QSize sz = fm.size(Qt::TextShowMnemonic, string);
  if(!empty || !w)
    {
    w += sz.width();
    }
  h = qMax(h, sz.height());
  //opt.rect.setSize(QSize(w, h)); // PM_MenuButtonIndicator depends on the height
  QSize buttonSize = (style()->sizeFromContents(QStyle::CT_PushButton, &opt, QSize(w, h), this).
                      expandedTo(QApplication::globalStrut()));
  return buttonSize;
}

//-----------------------------------------------------------------------------
QSize ctkCollapsibleButton::minimumSizeHint()const
{
  Q_D(const ctkCollapsibleButton);
  QSize buttonSize = this->buttonSizeHint();
  if (d->Collapsed)
    {
    return buttonSize + QSize(0,d->CollapsedHeight);
    }
  // open
  if (this->layout() == 0)
    {// no layout, means the button is empty ?
    return buttonSize;
    }
  QSize s = this->QAbstractButton::minimumSizeHint(); 
  return s.expandedTo(buttonSize);
}

//-----------------------------------------------------------------------------
QSize ctkCollapsibleButton::sizeHint()const
{
  Q_D(const ctkCollapsibleButton);
  QSize buttonSize = this->buttonSizeHint();
  if (d->Collapsed)
    {
    return buttonSize + QSize(0,d->CollapsedHeight);
    }
  // open
  // QAbstractButton works well only if a layout is set
  QSize s = this->QAbstractButton::sizeHint(); 
  return s.expandedTo(buttonSize + QSize(0, d->CollapsedHeight));
}

//-----------------------------------------------------------------------------
void ctkCollapsibleButton::paintEvent(QPaintEvent * _event)
{
  Q_D(ctkCollapsibleButton);

  QPainter p(this);
  // Draw Button
  QStyleOptionButton opt;
  this->initStyleOption(&opt);

  // We don't want to have the highlight effect on the button when mouse is
  // over a child. We want the highlight effect only when the mouse is just
  // over itself.
  // same as this->underMouse()
  bool exclusiveMouseOver = false;
  if (opt.state & QStyle::State_MouseOver)
    {
    QRect buttonRect = opt.rect;
    QList<QWidget*> _children = this->findChildren<QWidget*>();
    QList<QWidget*>::ConstIterator it;
    for (it = _children.constBegin(); it != _children.constEnd(); ++it ) 
      {
      if ((*it)->underMouse())
        {
        // the mouse has been moved from the collapsible button to one 
        // of its children. The paint event rect is the child rect, this
        // is why we have to request another paint event to redraw the 
        // button to remove the highlight effect.
        if (!_event->rect().contains(buttonRect))
          {// repaint the button rect.
          this->update(buttonRect);
          }
        opt.state &= ~QStyle::State_MouseOver;
        exclusiveMouseOver = true;
        break;
        }
      }
    if (d->ExclusiveMouseOver && !exclusiveMouseOver)
      {
      // the mouse is over the widget, but not over the children. As it 
      // has been de-highlighted in the past, we should refresh the button
      // rect to re-highlight the button.
      if (!_event->rect().contains(buttonRect))
        {// repaint the button rect.
        this->update(buttonRect);
        }
      }
    }
  d->ExclusiveMouseOver = exclusiveMouseOver;
  QSize indicatorSize = QSize(style()->pixelMetric(QStyle::PM_IndicatorWidth, &opt, this),
                              style()->pixelMetric(QStyle::PM_IndicatorHeight, &opt, this));
  opt.iconSize = indicatorSize;
  style()->drawControl(QStyle::CE_PushButtonBevel, &opt, &p, this);
  // TBD is PE_PanelButtonCommand better ?
  //style()->drawPrimitive(QStyle::PE_PanelButtonCommand, &opt, &p, this);
  int buttonHeight = opt.rect.height();
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
  if (d->IndicatorAlignment & Qt::AlignLeft)
    {
    indicatorOpt.rect = QRect((buttonHeight - indicatorSize.width()) / 2,
                              (buttonHeight - indicatorSize.height()) / 2,
                              indicatorSize.width(), indicatorSize.height());
    }
  else if (d->IndicatorAlignment & Qt::AlignHCenter)
    {
    int w = indicatorSize.width();
    if (!opt.text.isEmpty() && (d->TextAlignment & Qt::AlignHCenter))
      {
      w += textWidth + indicatorSpacing;
      }
    indicatorOpt.rect = QRect(opt.rect.x()+ opt.rect.width() /2 - w / 2,
                              (buttonHeight - indicatorSize.height()) / 2,
                              indicatorSize.width(), indicatorSize.height());
    if (d->TextAlignment & Qt::AlignLeft &&
        indicatorOpt.rect.left() < opt.rect.x() + buttonMargin + textWidth)
      {
      indicatorOpt.rect.moveLeft(opt.rect.x() + buttonMargin + textWidth);
      }
    else if (d->TextAlignment & Qt::AlignRight &&
             indicatorOpt.rect.right() > opt.rect.right() - buttonMargin - textWidth)
      {
      indicatorOpt.rect.moveRight(opt.rect.right() - buttonMargin - textWidth);
      }
    }
  else if (d->IndicatorAlignment & Qt::AlignRight)
    {
    indicatorOpt.rect = QRect(opt.rect.width() - (buttonHeight - indicatorSize.width()) / 2
                                - indicatorSize.width(),
                              (buttonHeight - indicatorSize.height()) / 2,
                              indicatorSize.width(), indicatorSize.height());
    }
  if (d->Collapsed)
    {
    style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &indicatorOpt, &p, this);
    }
  else
    {
    style()->drawPrimitive(QStyle::PE_IndicatorArrowUp, &indicatorOpt, &p, this);
    }

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
  style()->drawItemText(&p, opt.rect, tf, opt.palette, (opt.state & QStyle::State_Enabled),
                        opt.text, QPalette::ButtonText);

  // Draw Frame around contents
  QStyleOptionFrameV3 fopt;
  fopt.init(this);
  // HACK: on some styles, the frame doesn't exactly touch the button.
  // this is because the button has some kind of extra border.
  if (qobject_cast<QCleanlooksStyle*>(this->style()) != 0)
    {
    fopt.rect.setTop(buttonHeight - 1);
    }
  else
    {
    fopt.rect.setTop(buttonHeight);
    }
  fopt.frameShape = d->ContentsFrameShape;
  switch (d->ContentsFrameShadow)
    {
    case QFrame::Sunken:
      fopt.state |= QStyle::State_Sunken;
      break;
    case QFrame::Raised:
      fopt.state |= QStyle::State_Raised;
      break;
    default:
    case QFrame::Plain:
      break;
    }
  fopt.lineWidth = d->ContentsLineWidth;
  fopt.midLineWidth = d->ContentsMidLineWidth;
  style()->drawControl(QStyle::CE_ShapedFrame, &fopt, &p, this);
}

//-----------------------------------------------------------------------------
bool ctkCollapsibleButton::hitButton(const QPoint & _pos)const
{
  QStyleOptionButton opt;
  this->initStyleOption(&opt);
  return opt.rect.contains(_pos);
}

//-----------------------------------------------------------------------------
void ctkCollapsibleButton::childEvent(QChildEvent* c)
{
  Q_D(ctkCollapsibleButton);
  if(c && c->type() == QEvent::ChildAdded)
    {
    if (c->child() && c->child()->isWidgetType())
      {
      QWidget *w = static_cast<QWidget*>(c->child());
      w->setVisible(!d->Collapsed);
      }
    }
  QWidget::childEvent(c);
}

//-----------------------------------------------------------------------------
bool ctkCollapsibleButton::event(QEvent *event)
{
  if (event->type() == QEvent::StyleChange
      || event->type() == QEvent::FontChange
#ifdef Q_WS_MAC
      || event->type() == QEvent::MacSizeChange
#endif
      )
    {
    this->setContentsMargins(0, this->buttonSizeHint().height(),0 , 0);
    if (this->collapsed())
      {
      this->setMaximumHeight(this->sizeHint().height());
      }
    }
  return QAbstractButton::event(event);
}
