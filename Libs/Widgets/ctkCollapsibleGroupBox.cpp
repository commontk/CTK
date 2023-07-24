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
#include <QChildEvent>
#include <QMouseEvent>
#include <QStylePainter>
#include <QStyleOptionGroupBox>
#include <QStyle>

// CTK includes
#include "ctkCollapsibleGroupBox.h"

#include "ctkProxyStyle.h"

//-----------------------------------------------------------------------------
class ctkCollapsibleGroupBoxStyle:public ctkProxyStyle
{
public:
  typedef ctkProxyStyle Superclass;
  ctkCollapsibleGroupBoxStyle(QStyle* style = 0, QObject* parent =0)
    : Superclass(style, parent)
  {
  }
  virtual void drawPrimitive(PrimitiveElement pe, const QStyleOption * opt, QPainter * p, const QWidget * widget = 0) const
  {
    if (pe == QStyle::PE_IndicatorCheckBox)
      {
      const ctkCollapsibleGroupBox* groupBox= qobject_cast<const ctkCollapsibleGroupBox*>(widget);
      if (groupBox)
        {
        this->Superclass::drawPrimitive(groupBox->isChecked() ? QStyle::PE_IndicatorArrowUp : QStyle::PE_IndicatorArrowDown, opt, p, widget);
        return;
        }
      }
    this->Superclass::drawPrimitive(pe, opt, p, widget);
  }
  virtual int pixelMetric(PixelMetric metric, const QStyleOption * option, const QWidget * widget) const
  {
    if (metric == QStyle::PM_IndicatorHeight)
      {
      const ctkCollapsibleGroupBox* groupBox= qobject_cast<const ctkCollapsibleGroupBox*>(widget);
      if (groupBox)
        {
        return groupBox->fontMetrics().height();
        }
      }
    return this->Superclass::pixelMetric(metric, option, widget);
  }
};

//-----------------------------------------------------------------------------
class ctkCollapsibleGroupBoxPrivate
{
  Q_DECLARE_PUBLIC(ctkCollapsibleGroupBox);
protected:
  ctkCollapsibleGroupBox* const q_ptr;
public:
  ctkCollapsibleGroupBoxPrivate(ctkCollapsibleGroupBox& object);
  void init();
  void setChildVisibility(QWidget* childWidget);

  /// Size of the widget for collapsing
  QSize OldSize;
  /// Maximum allowed height
  int   MaxHeight;
  int   CollapsedHeight;

  /// We change the visibility of the children in setChildrenVisibility
  /// and we track when the visibility is changed to force it back to possibly
  /// force the child to be hidden. To prevent infinite loop we need to know
  /// who is changing children's visibility.
  bool     ForcingVisibility;
  /// Sometimes the creation of the widget is not done inside setVisible,
  /// as we need to do special processing the first time the groupBox is
  /// setVisible, we track its created state with the variable
  bool     IsStateCreated;

  /// Pointer to keep track of the proxy style
  ctkCollapsibleGroupBoxStyle* GroupBoxStyle;
};

//-----------------------------------------------------------------------------
ctkCollapsibleGroupBoxPrivate::ctkCollapsibleGroupBoxPrivate(
  ctkCollapsibleGroupBox& object)
  :q_ptr(&object)
{
  this->ForcingVisibility = false;
  this->IsStateCreated = false;
  this->MaxHeight = 0;
  this->CollapsedHeight = 14;
  this->GroupBoxStyle = 0;
}

//-----------------------------------------------------------------------------
void ctkCollapsibleGroupBoxPrivate::init()
{
  Q_Q(ctkCollapsibleGroupBox);
  q->setCheckable(true);
  QObject::connect(q, SIGNAL(toggled(bool)), q, SLOT(expand(bool)));

  this->MaxHeight = q->maximumHeight();
  QWidget* parent = q->parentWidget();
  QStyle* parentStyle = (parent) ? parent->style() : qApp->style();
  this->GroupBoxStyle = new ctkCollapsibleGroupBoxStyle(parentStyle, qApp);
  q->setStyle(this->GroupBoxStyle);
  this->GroupBoxStyle->ensureBaseStyle();
}
//-----------------------------------------------------------------------------
void ctkCollapsibleGroupBoxPrivate::setChildVisibility(QWidget* childWidget)
{
  Q_Q(ctkCollapsibleGroupBox);
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

  bool visible= !q->collapsed();
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
  // to allow any children to be explicitly hidden by the user.
  if ((!childWidget->property("visibilityToParent").isValid() ||
      childWidget->property("visibilityToParent").toBool()))
    {
    childWidget->setAttribute(Qt::WA_WState_ExplicitShowHide, false);
    }
  this->ForcingVisibility = false;
}

//-----------------------------------------------------------------------------
ctkCollapsibleGroupBox::ctkCollapsibleGroupBox(QWidget* _parent)
  :QGroupBox(_parent)
  , d_ptr(new ctkCollapsibleGroupBoxPrivate(*this))
{
  Q_D(ctkCollapsibleGroupBox);
  d->init();
}

//-----------------------------------------------------------------------------
ctkCollapsibleGroupBox::ctkCollapsibleGroupBox(const QString& title, QWidget* _parent)
  :QGroupBox(title, _parent)
  , d_ptr(new ctkCollapsibleGroupBoxPrivate(*this))
{
  Q_D(ctkCollapsibleGroupBox);
  d->init();
}

//-----------------------------------------------------------------------------
ctkCollapsibleGroupBox::~ctkCollapsibleGroupBox()
{

}

//-----------------------------------------------------------------------------
void ctkCollapsibleGroupBox::setCollapsedHeight(int heightInPixels)
{
  Q_D(ctkCollapsibleGroupBox);
  d->CollapsedHeight = heightInPixels;
}

//-----------------------------------------------------------------------------
int ctkCollapsibleGroupBox::collapsedHeight()const
{
  Q_D(const ctkCollapsibleGroupBox);
  return d->CollapsedHeight;
}

//-----------------------------------------------------------------------------
void ctkCollapsibleGroupBox::expand(bool _expand)
{
  Q_D(ctkCollapsibleGroupBox);
  if (!_expand)
    {
    d->OldSize = this->size();
    }

  // Update the visibility of all the children
  // We can't use findChildren as it would return the grandchildren
  foreach(QObject* childObject, this->children())
    {
    if (childObject->isWidgetType())
      {
      d->setChildVisibility(qobject_cast<QWidget*>(childObject));
      }
    }


  if (_expand)
    {
    this->setMaximumHeight(d->MaxHeight);
    this->resize(d->OldSize);
    }
  else
    {
    d->MaxHeight = this->maximumHeight();
    QStyleOptionGroupBox option;
    this->initStyleOption(&option);
    QRect labelRect = this->style()->subControlRect(
      QStyle::CC_GroupBox, &option, QStyle::SC_GroupBoxLabel, this);
    this->setMaximumHeight(labelRect.height() + d->CollapsedHeight);
    }
}

//-----------------------------------------------------------------------------
void ctkCollapsibleGroupBox::childEvent(QChildEvent* c)
{
  Q_D(ctkCollapsibleGroupBox);
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
  this->QGroupBox::childEvent(c);
}

//-----------------------------------------------------------------------------
void ctkCollapsibleGroupBox::setVisible(bool show)
{
  Q_D(ctkCollapsibleGroupBox);
  // calling QWidget::setVisible() on ctkCollapsibleGroupBox will eventually
  // call QWidget::showChildren() or hideChildren() which will generate
  // ShowToParent/HideToParent events but we want to ignore that case in
  // eventFilter().
  d->ForcingVisibility = true;
  this->QGroupBox::setVisible(show);
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
bool ctkCollapsibleGroupBox::eventFilter(QObject* child, QEvent* e)
{
  Q_D(ctkCollapsibleGroupBox);
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
  return this->QGroupBox::eventFilter(child, e);
}
