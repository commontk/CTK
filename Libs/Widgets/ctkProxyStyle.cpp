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
#include <QPointer>
#include <QStyleFactory>

// CTK includes
#include "ctkProxyStyle.h"

// ----------------------------------------------------------------------------
class ctkProxyStylePrivate
{
  Q_DECLARE_PUBLIC(ctkProxyStyle)
protected:
  ctkProxyStyle* const q_ptr;
public:
  void setProxyStyle(QProxyStyle* proxy, QStyle *style)const;
  void setBaseStyle(QProxyStyle* proxyStyle, QStyle* baseStyle)const;
private:
  ctkProxyStylePrivate(ctkProxyStyle& object);
  mutable QPointer <QStyle> baseStyle;
  mutable bool ensureBaseStyleInProgress;
};

// ----------------------------------------------------------------------------
ctkProxyStylePrivate::ctkProxyStylePrivate(ctkProxyStyle& object)
  : q_ptr(&object)
  , ensureBaseStyleInProgress(false)
{
}

// ----------------------------------------------------------------------------
void ctkProxyStylePrivate::setProxyStyle(QProxyStyle* proxy, QStyle *style)const
{
  if (style->proxy() == proxy)
    {
    return;
    }
  this->setBaseStyle(proxy, style);
}
// ----------------------------------------------------------------------------
void ctkProxyStylePrivate::setBaseStyle(QProxyStyle* proxy, QStyle *style)const
{
  if (proxy->baseStyle() == style &&
      style->proxy() == proxy)
    {
    return;
    }
  QObject* parent = style->parent();
  QStyle* oldStyle = proxy->baseStyle();
  QObject* oldParent = oldStyle ? oldStyle->parent() : 0;
  if (oldParent == proxy)
    {
    oldStyle->setParent(0);// make sure setBaseStyle doesn't delete baseStyle
    }
  proxy->setBaseStyle(style);
  style->setParent(parent);
  if (oldParent == proxy)
    {
    oldStyle->setParent(oldParent);
    }
}

// ----------------------------------------------------------------------------
ctkProxyStyle::ctkProxyStyle(QStyle *style, QObject* parent)
  : d_ptr(new ctkProxyStylePrivate(*this))
{
  Q_D(ctkProxyStyle);
  d->baseStyle = style;
  this->setBaseStyle(style);
  this->setParent(parent);
}

// ----------------------------------------------------------------------------
ctkProxyStyle::~ctkProxyStyle()
{
  Q_D(ctkProxyStyle);
  if (!QApplication::closingDown() && d->baseStyle == QApplication::style())
    {
    d->baseStyle->setParent(qApp); // don't delete the application style.
    }
}

// ----------------------------------------------------------------------------
void ctkProxyStyle::ensureBaseStyle() const
{
  Q_D(const ctkProxyStyle);
  if (d->ensureBaseStyleInProgress)
  {
    // avoid infinite loop
    return;
  }
  d->ensureBaseStyleInProgress = true;  
  d->baseStyle = this->baseStyle();
  // Set the proxy to the entire hierarchy.
  QProxyStyle* proxyStyle = const_cast<QProxyStyle*>(qobject_cast<const QProxyStyle*>(
    this->proxy() ? this->proxy() : this));
  QStyle* proxyBaseStyle = proxyStyle->baseStyle(); // calls ensureBaseStyle
  QStyle* baseStyle = proxyBaseStyle;
  while (baseStyle)
    {
    d->setProxyStyle(proxyStyle, baseStyle);// set proxy on itself to all children
    QProxyStyle* proxy = qobject_cast<QProxyStyle*>(baseStyle);
    baseStyle = proxy ? proxy->baseStyle() : 0;
    }
  d->setBaseStyle(proxyStyle, proxyBaseStyle);
  d->ensureBaseStyleInProgress = false;
}

// ----------------------------------------------------------------------------
void ctkProxyStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    Q_D(const ctkProxyStyle);
    this->ensureBaseStyle();
    d->baseStyle->drawPrimitive(element, option, painter, widget);
}

// ----------------------------------------------------------------------------
void ctkProxyStyle::drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    Q_D(const ctkProxyStyle);
    this->ensureBaseStyle();
    d->baseStyle->drawControl(element, option, painter, widget);
}

// ----------------------------------------------------------------------------
void ctkProxyStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    Q_D(const ctkProxyStyle);
    this->ensureBaseStyle();
    d->baseStyle->drawComplexControl(control, option, painter, widget);
}

// ----------------------------------------------------------------------------
void ctkProxyStyle::drawItemText(QPainter *painter, const QRect &rect, int flags, const QPalette &pal, bool enabled,
                               const QString &text, QPalette::ColorRole textRole) const
{
    Q_D(const ctkProxyStyle);
    this->ensureBaseStyle();
    d->baseStyle->drawItemText(painter, rect, flags, pal, enabled, text, textRole);
}

// ----------------------------------------------------------------------------
void ctkProxyStyle::drawItemPixmap(QPainter *painter, const QRect &rect, int alignment, const QPixmap &pixmap) const
{
    Q_D(const ctkProxyStyle);
    this->ensureBaseStyle();
    d->baseStyle->drawItemPixmap(painter, rect, alignment, pixmap);
}

// ----------------------------------------------------------------------------
QSize ctkProxyStyle::sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const
{
    Q_D(const ctkProxyStyle);
    this->ensureBaseStyle();
    return d->baseStyle->sizeFromContents(type, option, size, widget);
}

// ----------------------------------------------------------------------------
QRect ctkProxyStyle::subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const
{
    Q_D(const ctkProxyStyle);
    this->ensureBaseStyle();
    return d->baseStyle->subElementRect(element, option, widget);
}

// ----------------------------------------------------------------------------
QRect ctkProxyStyle::subControlRect(ComplexControl cc, const QStyleOptionComplex *option, SubControl sc, const QWidget *widget) const
{
    Q_D(const ctkProxyStyle);
    this->ensureBaseStyle();
    return d->baseStyle->subControlRect(cc, option, sc, widget);
}

// ----------------------------------------------------------------------------
QRect ctkProxyStyle::itemTextRect(const QFontMetrics &fm, const QRect &r, int flags, bool enabled, const QString &text) const
{
    Q_D(const ctkProxyStyle);
    this->ensureBaseStyle();
    return d->baseStyle->itemTextRect(fm, r, flags, enabled, text);
}

// ----------------------------------------------------------------------------
QRect ctkProxyStyle::itemPixmapRect(const QRect &r, int flags, const QPixmap &pixmap) const
{
    Q_D(const ctkProxyStyle);
    this->ensureBaseStyle();
    return d->baseStyle->itemPixmapRect(r, flags, pixmap);
}

// ----------------------------------------------------------------------------
QStyle::SubControl ctkProxyStyle::hitTestComplexControl(ComplexControl control, const QStyleOptionComplex *option, const QPoint &pos, const QWidget *widget) const
{
    Q_D(const ctkProxyStyle);
    this->ensureBaseStyle();
    return d->baseStyle->hitTestComplexControl(control, option, pos, widget);
}

// ----------------------------------------------------------------------------
int ctkProxyStyle::styleHint(StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const
{
    Q_D(const ctkProxyStyle);
    this->ensureBaseStyle();
    return d->baseStyle->styleHint(hint, option, widget, returnData);
}

// ----------------------------------------------------------------------------
int ctkProxyStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    Q_D(const ctkProxyStyle);
    this->ensureBaseStyle();
    return d->baseStyle->pixelMetric(metric, option, widget);
}

// ----------------------------------------------------------------------------
QPixmap ctkProxyStyle::standardPixmap(StandardPixmap standardPixmap, const QStyleOption *opt, const QWidget *widget) const
{
    Q_D(const ctkProxyStyle);
    this->ensureBaseStyle();
    return d->baseStyle->standardPixmap(standardPixmap, opt, widget);
}

// ----------------------------------------------------------------------------
QPixmap ctkProxyStyle::generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap, const QStyleOption *opt) const
{
    Q_D(const ctkProxyStyle);
    this->ensureBaseStyle();
    return d->baseStyle->generatedIconPixmap(iconMode, pixmap, opt);
}

// ----------------------------------------------------------------------------
QPalette ctkProxyStyle::standardPalette() const
{
    Q_D(const ctkProxyStyle);
    this->ensureBaseStyle();
    return d->baseStyle->standardPalette();
}

// ----------------------------------------------------------------------------
void ctkProxyStyle::polish(QWidget *widget)
{
    Q_D(const ctkProxyStyle);
    this->ensureBaseStyle();
    d->baseStyle->polish(widget);
}

// ----------------------------------------------------------------------------
void ctkProxyStyle::polish(QPalette &pal)
{
    Q_D(const ctkProxyStyle);
    this->ensureBaseStyle();
    d->baseStyle->polish(pal);
}

// ----------------------------------------------------------------------------
void ctkProxyStyle::polish(QApplication *app)
{
    Q_D(const ctkProxyStyle);
    this->ensureBaseStyle();
    d->baseStyle->polish(app);
}

// ----------------------------------------------------------------------------
void ctkProxyStyle::unpolish(QWidget *widget)
{
    Q_D(const ctkProxyStyle);
    this->ensureBaseStyle();
    d->baseStyle->unpolish(widget);
}

// ----------------------------------------------------------------------------
void ctkProxyStyle::unpolish(QApplication *app)
{
    Q_D(const ctkProxyStyle);
    this->ensureBaseStyle();
    d->baseStyle->unpolish(app);
}

// ----------------------------------------------------------------------------
bool ctkProxyStyle::event(QEvent *e)
{
    Q_D(const ctkProxyStyle);
    if (e->type() != QEvent::ParentChange &&
        e->type() != QEvent::ChildRemoved &&
        e->type() != QEvent::ChildAdded)
      {
      this->ensureBaseStyle();
      }
    return !d->baseStyle.isNull() ? d->baseStyle->event(e) : false;
}

// ----------------------------------------------------------------------------
QIcon ctkProxyStyle::standardIconImplementation(StandardPixmap standardIcon,
                                              const QStyleOption *option,
                                              const QWidget *widget) const
{
    Q_D(const ctkProxyStyle);
    this->ensureBaseStyle();
    return d->baseStyle->standardIcon(standardIcon, option, widget);
}

// ----------------------------------------------------------------------------
int ctkProxyStyle::layoutSpacingImplementation(QSizePolicy::ControlType control1,
                                             QSizePolicy::ControlType control2,
                                             Qt::Orientation orientation,
                                             const QStyleOption *option,
                                             const QWidget *widget) const
{
    Q_D(const ctkProxyStyle);
    this->ensureBaseStyle();
    return d->baseStyle->layoutSpacing(control1, control2, orientation, option, widget);
}
