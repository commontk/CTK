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

#ifndef __ctkProxyStyle_h
#define __ctkProxyStyle_h

// QT includes
#include <QProxyStyle>

// CTK includes
#include "ctkWidgetsExport.h"

class ctkProxyStylePrivate;

/// \ingroup Widgets
/// ctkProxyStyle fixes some issues with QProxyStyle

class CTK_WIDGETS_EXPORT ctkProxyStyle : public QProxyStyle
{
  Q_OBJECT
public:
  ctkProxyStyle(QStyle *baseStyle = 0, QObject* parent = 0);
  virtual ~ctkProxyStyle();

  void ensureBaseStyle()const;

  virtual void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = 0) const;
  virtual void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = 0) const;
  virtual void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = 0) const;
  virtual void drawItemText(QPainter *painter, const QRect &rect, int flags, const QPalette &pal, bool enabled,
                            const QString &text, QPalette::ColorRole textRole = QPalette::NoRole) const;
  virtual void drawItemPixmap(QPainter *painter, const QRect &rect, int alignment, const QPixmap &pixmap) const;

  virtual QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const;

  virtual QRect subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const;
  virtual QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc, const QWidget *widget) const;
  virtual QRect itemTextRect(const QFontMetrics &fm, const QRect &r, int flags, bool enabled, const QString &text) const;
  virtual QRect itemPixmapRect(const QRect &r, int flags, const QPixmap &pixmap) const;

  virtual SubControl hitTestComplexControl(ComplexControl control, const QStyleOptionComplex *option, const QPoint &pos, const QWidget *widget = 0) const;
  virtual int styleHint(StyleHint hint, const QStyleOption *option = 0, const QWidget *widget = 0, QStyleHintReturn *returnData = 0) const;
  virtual int pixelMetric(PixelMetric metric, const QStyleOption *option = 0, const QWidget *widget = 0) const;

  virtual QPixmap standardPixmap(StandardPixmap standardPixmap, const QStyleOption *opt, const QWidget *widget = 0) const;
  virtual QPixmap generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap, const QStyleOption *opt) const;
  virtual QPalette standardPalette() const;

  virtual void polish(QWidget *widget);
  virtual void polish(QPalette &pal);
  virtual void polish(QApplication *app);

  virtual void unpolish(QWidget *widget);
  virtual void unpolish(QApplication *app);

protected:
  QScopedPointer<ctkProxyStylePrivate> d_ptr;
  virtual bool event(QEvent *e);

protected Q_SLOTS:
  virtual QIcon standardIconImplementation(StandardPixmap standardIcon, const QStyleOption *option, const QWidget *widget) const;
  virtual int layoutSpacingImplementation(QSizePolicy::ControlType control1, QSizePolicy::ControlType control2,
                                  Qt::Orientation orientation, const QStyleOption *option = 0, const QWidget *widget = 0) const;
private:
  Q_DISABLE_COPY(ctkProxyStyle)
  Q_DECLARE_PRIVATE(ctkProxyStyle)
};

#endif
