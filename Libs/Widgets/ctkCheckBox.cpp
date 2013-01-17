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
#include <QApplication>
#include <QStyleOption>

// CTK includes
#include "ctkCheckBox.h"
#include "ctkProxyStyle.h"

// STD includes
#include <iostream>

// ----------------------------------------------------------------------------
class ctkCheckBoxStyle : public ctkProxyStyle
{
public:
  typedef ctkProxyStyle Superclass;
  ctkCheckBoxStyle(QStyle* baseStyle, QObject* parent = 0);
  virtual ~ctkCheckBoxStyle();

  virtual void drawPrimitive(QStyle::PrimitiveElement pe,
                             const QStyleOption * opt,
                             QPainter * p,
                             const QWidget * widget = 0) const;

  virtual int pixelMetric(QStyle::PixelMetric metric,
                          const QStyleOption *option,
                          const QWidget *widget = 0) const;

  QIcon   IndicatorIcon;
  QSize   IndicatorSize;
};

// ----------------------------------------------------------------------------
//  Methods ctkCheckBoxStyle

// ----------------------------------------------------------------------------
ctkCheckBoxStyle::ctkCheckBoxStyle(QStyle *baseStyle, QObject* parent)
  : Superclass(baseStyle, parent)
{
}

// ----------------------------------------------------------------------------
ctkCheckBoxStyle::~ctkCheckBoxStyle()
{
}

// ----------------------------------------------------------------------------
void ctkCheckBoxStyle::drawPrimitive(QStyle::PrimitiveElement pe,
                                     const QStyleOption * opt,
                                     QPainter * p, const QWidget * widget) const
{
  if (pe == QStyle::PE_IndicatorCheckBox)
    {
    const ctkCheckBox* checkBox= qobject_cast<const ctkCheckBox*>(widget);
    if (checkBox && !this->IndicatorIcon.isNull())
      {
      QIcon::Mode mode =
          (opt->state & QStyle::State_MouseOver) == QStyle::State_MouseOver
            ? QIcon::Active : QIcon::Normal;
      mode = (opt->state & QStyle::State_Sunken) == QStyle::State_Sunken
               ? QIcon::Selected : mode;
      mode = (opt->state & QStyle::State_Enabled) == QStyle::State_Enabled
               ? mode : QIcon::Disabled;
      QIcon::State state =
          (opt->state & QStyle::State_On) == QStyle::State_On
            ?  QIcon::Off : QIcon::On;
      state = (opt->state & QStyle::State_Sunken) == QStyle::State_Sunken
              ? QIcon::Off : state ;

      this->drawItemPixmap(p, opt->rect, Qt::AlignHCenter,
                           this->IndicatorIcon.pixmap(
                             opt->rect.width(), opt->rect.height(),
                             mode,
                             state));
      return;
      }
    }
  this->Superclass::drawPrimitive(pe, opt, p, widget);
}

// ----------------------------------------------------------------------------
int ctkCheckBoxStyle::pixelMetric(QStyle::PixelMetric metric,
                                  const QStyleOption *option,
                                  const QWidget *widget) const
{
  const ctkCheckBox* checkBox= qobject_cast<const ctkCheckBox*>(widget);
  if (checkBox && !this->IndicatorIcon.isNull())
    {
    if(metric == QStyle::PM_IndicatorHeight && !this->IndicatorSize.isEmpty())
      {
      return this->IndicatorIcon.actualSize(this->IndicatorSize).height();
      }
    if(metric == QStyle::PM_IndicatorWidth && !this->IndicatorSize.isEmpty())
      {
      return this->IndicatorIcon.actualSize(this->IndicatorSize).width();
      }
    }
  return this->Superclass::pixelMetric(metric, option, widget);
}

// ----------------------------------------------------------------------------
class ctkCheckBoxPrivate
{
  Q_DECLARE_PUBLIC(ctkCheckBox);
protected:
  ctkCheckBox* const q_ptr;
public:
  ctkCheckBoxPrivate(ctkCheckBox& object);
  void init();

  ctkCheckBoxStyle* IconStyle;
};

// ----------------------------------------------------------------------------
//  Methods ctkCheckBoxPrivate

// ----------------------------------------------------------------------------
ctkCheckBoxPrivate::ctkCheckBoxPrivate(ctkCheckBox &object)
  : q_ptr(&object)
{
  this->IconStyle = 0;
}

// ----------------------------------------------------------------------------
void ctkCheckBoxPrivate::init()
{
  Q_Q(ctkCheckBox);
  QWidget* parent = q->parentWidget();
  QStyle* parentStyle = (parent) ? parent->style() : qApp->style();
  this->IconStyle = new ctkCheckBoxStyle(parentStyle, qApp);
  q->setStyle(this->IconStyle);
  this->IconStyle->ensureBaseStyle();
}

// ----------------------------------------------------------------------------
//  Methods ctkCheckBox

// ----------------------------------------------------------------------------
ctkCheckBox::ctkCheckBox(QWidget *_parent)
  :Superclass(_parent)
  , d_ptr(new ctkCheckBoxPrivate(*this))
{
  Q_D(ctkCheckBox);
  d->init();
}

// ----------------------------------------------------------------------------
ctkCheckBox::~ctkCheckBox()
{
}

// ----------------------------------------------------------------------------
void ctkCheckBox::setIndicatorIcon(const QIcon& newIcon)
{
  Q_D(ctkCheckBox);
  d->IconStyle->IndicatorIcon = newIcon;
  this->update();
}

// ----------------------------------------------------------------------------
QIcon ctkCheckBox::indicatorIcon() const
{
  Q_D(const ctkCheckBox);
  return d->IconStyle->IndicatorIcon;
}

// ----------------------------------------------------------------------------
void ctkCheckBox::setIndicatorIconSize(const QSize& newSize)
{
  Q_D(ctkCheckBox);
  d->IconStyle->IndicatorSize = newSize;
  this->update();
}

// ----------------------------------------------------------------------------
QSize ctkCheckBox::indicatorIconSize() const
{
  Q_D(const ctkCheckBox);
  return d->IconStyle->IndicatorSize;
}
