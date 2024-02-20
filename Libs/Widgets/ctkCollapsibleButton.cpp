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
  void setChildVisibility(QWidget* childWidget);

  bool     Collapsed;

  // Contents frame
  QFrame::Shape  ContentsFrameShape;
  QFrame::Shadow ContentsFrameShadow;
  int            ContentsLineWidth;
  int            ContentsMidLineWidth;

  int      CollapsedHeight;
  bool     Flat;
  bool     ExclusiveMouseOver;
  bool     LookOffWhenChecked;

  /// We change the visibility of the children in setChildrenVisibility
  /// and we track when the visibility is changed to force it back to possibly
  /// force the child to be hidden. To prevent infinite loop we need to know
  /// who is changing children's visibility.
  bool     ForcingVisibility;
  /// Sometimes the creation of the widget is not done inside setVisible,
  /// as we need to do special processing the first time the button is
  /// setVisible, we track its created state with the variable
  bool     IsStateCreated;

  int      MaximumHeight;  // use carefully

  // Tuning of the button look&feel
  Qt::Alignment TextAlignment;
  Qt::Alignment IndicatorAlignment;
};

//-----------------------------------------------------------------------------
ctkCollapsibleButtonPrivate::ctkCollapsibleButtonPrivate(ctkCollapsibleButton& object)
  :q_ptr(&object)
{
  this->Flat = false;
  this->ForcingVisibility = false;
  this->IsStateCreated = false;
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

  this->CollapsedHeight = 14;
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
void ctkCollapsibleButtonPrivate::setChildVisibility(QWidget* childWidget)
{
  Q_Q(ctkCollapsibleButton);
  // Don't hide children while the widget is not yet created (before show() is
  // called). If we hide them (but don't set ExplicitShowHide), they would be
  // shown anyway when they will be created (because ExplicitShowHide is not set).
  // If we set ExplicitShowHide, then calling setVisible(false) on them would
  // be a no (because they are already hidden and ExplicitShowHide is set).
  // So we don't hide/show the children until the widget is created.
  if (!q->testAttribute(Qt::WA_WState_Created))
    {
    return;
    }
  this->ForcingVisibility = true;

  bool visible= !this->Collapsed;
  // if the widget has been explicitly hidden, then hide it.
  if (childWidget->property("visibilityToParent").isValid()
      && !childWidget->property("visibilityToParent").toBool())
    {
    visible = false;
    }

  // Setting Qt::WA_WState_Visible to true during child construction can have
  // undesirable side effects.
  if (childWidget->testAttribute(Qt::WA_WState_Created) ||
      !visible)
    {
    childWidget->setVisible(visible);
    }

  // setVisible() has set the ExplicitShowHide flag, restore it as we don't want
  // to make it like it was an explicit visible set because we want
  // to allow the children to be explicitly hidden by the user.
  if ((!childWidget->property("visibilityToParent").isValid() ||
      childWidget->property("visibilityToParent").toBool()))
    {
    childWidget->setAttribute(Qt::WA_WState_ExplicitShowHide, false);
    }
  this->ForcingVisibility = false;
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
  if (d->Flat)
    {
    option->features |= QStyleOptionButton::Flat;
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
void ctkCollapsibleButton::setFlat(bool flat)
{
  Q_D(ctkCollapsibleButton);
  d->Flat = flat;
  this->update();
}

//-----------------------------------------------------------------------------
bool ctkCollapsibleButton::isFlat()const
{
  Q_D(const ctkCollapsibleButton);
  return d->Flat;
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
void ctkCollapsibleButton::collapse(bool collapsed)
{
  Q_D(ctkCollapsibleButton);
  if (collapsed == d->Collapsed)
    {
    return;
    }

  d->Collapsed = collapsed;

  // we do that here as setVisible calls will correctly refresh the widget
  if (collapsed)
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

  // Update the visibility of all the children
  // We can't use findChildren as it would return the grandchildren
  foreach(QObject* child, this->children())
    {
    QWidget* childWidget = qobject_cast<QWidget*>(child);
    if (childWidget)
      {
      d->setChildVisibility(childWidget);
      }
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
  emit contentsCollapsed(collapsed);
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
  // all the computations have been made inferring the text would be left oriented
  tf &= ~Qt::AlignHCenter & ~Qt::AlignRight;
  tf |= Qt::AlignLeft;
  style()->drawItemText(&p, opt.rect, tf, opt.palette, (opt.state & QStyle::State_Enabled),
                        opt.text, QPalette::ButtonText);

  // Draw Frame around contents
  QStyleOptionFrame fopt;
  fopt.init(this);
  // HACK: on some styles, the frame doesn't exactly touch the button.
  // this is because the button has some kind of extra border.
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
  QObject* child = c->child();
  if (c && c->type() == QEvent::ChildAdded &&
      child && child->isWidgetType())
    {
    QWidget *childWidget = qobject_cast<QWidget*>(c->child());
    // Handle the case where the child has already it's visibility set before
    // being added to the widget
    if (childWidget->testAttribute(Qt::WA_WState_ExplicitShowHide) &&
        childWidget->testAttribute(Qt::WA_WState_Hidden))
      {
      // if the widget has explicitly set to hidden, then mark it as such
      childWidget->setProperty("visibilityToParent", false);
      }
    // We want to catch all the child's Show/Hide events.
    child->installEventFilter(this);
    // If the child is added while ctkCollapsibleButton is collapsed, then we
    // need to hide the child.
    d->setChildVisibility(childWidget);
    }
  this->QAbstractButton::childEvent(c);
}

//-----------------------------------------------------------------------------
void ctkCollapsibleButton::setVisible(bool show)
{
  Q_D(ctkCollapsibleButton);
  // calling QWidget::setVisible() on ctkCollapsibleButton will eventually
  // call QWidget::showChildren() or hideChildren() which will generate
  // ShowToParent/HideToParent events but we want to ignore that case in
  // eventFilter().
  d->ForcingVisibility = true;
  this->QWidget::setVisible(show);
  d->ForcingVisibility = false;
  // We have been ignoring setChildVisibility() while the collapsible button
  // is not yet created, now that it is created, ensure that the children
  // are correctly shown/hidden depending on their explicit visibility and
  // the collapsed property of the button.
  if (!d->IsStateCreated && this->testAttribute(Qt::WA_WState_Created))
    {
    d->IsStateCreated = true;
    foreach(QObject* child, this->children())
      {
      QWidget* childWidget = qobject_cast<QWidget*>(child);
      if (childWidget)
        {
        d->setChildVisibility(childWidget);
        }
      }
    }
}

//-----------------------------------------------------------------------------
bool ctkCollapsibleButton::eventFilter(QObject* child, QEvent* e)
{
  Q_D(ctkCollapsibleButton);
  Q_ASSERT(child && e);
  // Make sure the Show/QHide events are not generated by one of our
  // ctkCollapsibleButton function.
  if (d->ForcingVisibility)
    {
    return false;
    }
  // When we are here, it's because somewhere (not in ctkCollapsibleButton),
  // someone explicitly called setVisible() on a child widget.
  // If the collapsible button is collapsed/closed, then even if someone
  // request the widget to be visible, we force it back to be hidden because
  // they meant to be hidden to its parent, the collapsible button. However the
  // child will later be shown when the button will be expanded/opened.
  // On the other hand, if the user explicitly hide the child when the button
  // is collapsed/closed, then we want to keep it hidden next time the
  // collapsible button is expanded/opened.
  if (e->type() == QEvent::ShowToParent)
    {
    child->setProperty("visibilityToParent", true);
    Q_ASSERT(qobject_cast<QWidget*>(child));
    // force the widget to be hidden if the button is collapsed.
    d->setChildVisibility(qobject_cast<QWidget*>(child));
    }
  else if(e->type() == QEvent::HideToParent)
    {
    // we don't need to force the widget to be visible here.
    child->setProperty("visibilityToParent", false);
    }
  return this->QWidget::eventFilter(child, e);
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
