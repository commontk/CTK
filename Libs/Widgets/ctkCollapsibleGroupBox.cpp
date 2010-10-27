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
#include <QDebug>
#include <QChildEvent>
#include <QMouseEvent>
#include <QStylePainter>
#include <QStyleOptionGroupBox>
#include <QStyle>

// CTK includes
#include "ctkCollapsibleGroupBox.h"

#if QT_VERSION >= 0x040600
#include <QProxyStyle>

//-----------------------------------------------------------------------------
class ctkCollapsibleGroupBoxStyle:public QProxyStyle
{
  public:

  virtual void drawPrimitive(PrimitiveElement pe, const QStyleOption * opt, QPainter * p, const QWidget * widget = 0) const
  {
    if (pe == QStyle::PE_IndicatorCheckBox)
      {
      const QGroupBox* groupBox= qobject_cast<const QGroupBox*>(widget);
      if (groupBox)
        {
        this->QProxyStyle::drawPrimitive(groupBox->isChecked() ? QStyle::PE_IndicatorArrowUp : QStyle::PE_IndicatorArrowDown, opt, p, widget);
        return;
        }
      }
    this->QProxyStyle::drawPrimitive(pe, opt, p, widget);
  }
};
#else
  
#endif

//-----------------------------------------------------------------------------
ctkCollapsibleGroupBox::ctkCollapsibleGroupBox(QWidget* _parent)
  :QGroupBox(_parent)
{
  this->init();
}

//-----------------------------------------------------------------------------
ctkCollapsibleGroupBox::ctkCollapsibleGroupBox(const QString& title, QWidget* _parent)
  :QGroupBox(title, _parent)
{
  this->init();
}

//-----------------------------------------------------------------------------
ctkCollapsibleGroupBox::~ctkCollapsibleGroupBox()
{

}

//-----------------------------------------------------------------------------
void ctkCollapsibleGroupBox::init()
{
  this->setCheckable(true);
  connect(this, SIGNAL(toggled(bool)), this, SLOT(expand(bool)));

  this->MaxHeight = this->maximumHeight();
#if QT_VERSION >= 0x040600
  this->setStyle(new ctkCollapsibleGroupBoxStyle);
#else
  this->setStyleSheet(
    "ctkCollapsibleGroupBox::indicator:checked{"
    "image: url(:/Icons/expand-up.png);}"
    "ctkCollapsibleGroupBox::indicator:unchecked{"
    "image: url(:/Icons/expand-down.png);}");
#endif
}

//-----------------------------------------------------------------------------
void ctkCollapsibleGroupBox::expand(bool _expand)
{
  if (!_expand)
    {
    this->OldSize = this->size();
    }

  QObjectList childList = this->children();
  for (int i = 0; i < childList.size(); ++i) 
    {
    QObject *o = childList.at(i);
    if (o && o->isWidgetType()) 
      {
      QWidget *w = static_cast<QWidget *>(o);
      if ( w )
        {
        w->setVisible(_expand);
        }
      }
    }
  
  if (_expand)
    {
    this->setMaximumHeight(this->MaxHeight);
    this->resize(this->OldSize);
    }
  else
    {
    this->MaxHeight = this->maximumHeight();
    this->setMaximumHeight(22);
    }
}

//-----------------------------------------------------------------------------
void ctkCollapsibleGroupBox::childEvent(QChildEvent* c)
{
  if(c && c->type() == QEvent::ChildAdded)
    {
    if (c->child() && c->child()->isWidgetType())
      {
      QWidget *w = static_cast<QWidget*>(c->child());
      w->setVisible(this->isChecked());
      }
    }
  QGroupBox::childEvent(c);
}

#if QT_VERSION < 0x040600
//-----------------------------------------------------------------------------
void ctkCollapsibleGroupBox::paintEvent(QPaintEvent* e)
{
  this->QGroupBox::paintEvent(e);
  
  QStylePainter paint(this);
  QStyleOptionGroupBox option;
  initStyleOption(&option);
  option.activeSubControls &= !QStyle::SC_GroupBoxCheckBox;
  paint.drawComplexControl(QStyle::CC_GroupBox, option);
  
}

//-----------------------------------------------------------------------------
void ctkCollapsibleGroupBox::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        event->ignore();
        return;
    }
    // no animation
}

//-----------------------------------------------------------------------------
void ctkCollapsibleGroupBox::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        event->ignore();
        return;
    }

    QStyleOptionGroupBox box;
    initStyleOption(&box);
    box.activeSubControls &= !QStyle::SC_GroupBoxCheckBox;
    QStyle::SubControl released = style()->hitTestComplexControl(QStyle::CC_GroupBox, &box,
                                                                 event->pos(), this);
    bool toggle = this->isCheckable() && (released == QStyle::SC_GroupBoxLabel
                                   || released == QStyle::SC_GroupBoxCheckBox);
    if (toggle)
      {
      this->setChecked(!this->isChecked());
      }
}

#endif

//-----------------------------------------------------------------------------
QSize ctkCollapsibleGroupBox::minimumSizeHint() const
{
  //qDebug() << "ctkCollapsibleGroupBox::minimumSizeHint::" << this->QGroupBox::minimumSizeHint() ;
  return this->QGroupBox::minimumSizeHint();
}

//-----------------------------------------------------------------------------
QSize ctkCollapsibleGroupBox::sizeHint() const
{
  //qDebug() << "ctkCollapsibleGroupBox::sizeHint::" << this->QGroupBox::sizeHint() ;
  return this->QGroupBox::sizeHint();
}

//-----------------------------------------------------------------------------
int ctkCollapsibleGroupBox::heightForWidth(int w) const
{
  //qDebug() << "ctkCollapsibleGroupBox::heightForWidth::" << this->QGroupBox::heightForWidth(w) ;
  return this->QGroupBox::heightForWidth(w);
}

//-----------------------------------------------------------------------------
void ctkCollapsibleGroupBox::resizeEvent ( QResizeEvent * _event )
{
  //qDebug() << "ctkCollapsibleGroupBox::resizeEvent::" << _event->oldSize() << _event->size() ;
  return this->QGroupBox::resizeEvent(_event);
}
