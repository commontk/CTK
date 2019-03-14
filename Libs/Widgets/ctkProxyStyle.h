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
  ctkProxyStyle(QStyle *baseStyle = nullptr, QObject* parent = nullptr);
  ~ctkProxyStyle() override;

  void ensureBaseStyle()const;

  void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = nullptr) const override;
  void drawItemText(QPainter *painter, const QRect &rect, int flags, const QPalette &pal, bool enabled,
                            const QString &text, QPalette::ColorRole textRole = QPalette::NoRole) const override;
  void drawItemPixmap(QPainter *painter, const QRect &rect, int alignment, const QPixmap &pixmap) const override;

  QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const override;

  QRect subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const override;
  QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc, const QWidget *widget) const override;
  QRect itemTextRect(const QFontMetrics &fm, const QRect &r, int flags, bool enabled, const QString &text) const override;
  QRect itemPixmapRect(const QRect &r, int flags, const QPixmap &pixmap) const override;

  SubControl hitTestComplexControl(ComplexControl control, const QStyleOptionComplex *option, const QPoint &pos, const QWidget *widget = nullptr) const override;
  int styleHint(StyleHint hint, const QStyleOption *option = nullptr, const QWidget *widget = nullptr, QStyleHintReturn *returnData = nullptr) const override;
  int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override;

  QPixmap standardPixmap(StandardPixmap standardPixmap, const QStyleOption *opt, const QWidget *widget = nullptr) const override;
  QPixmap generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap, const QStyleOption *opt) const override;
  QPalette standardPalette() const override;

  void polish(QWidget *widget) override;
  void polish(QPalette &pal) override;
  void polish(QApplication *app) override;

  void unpolish(QWidget *widget) override;
  void unpolish(QApplication *app) override;

protected:
  QScopedPointer<ctkProxyStylePrivate> d_ptr;
  bool event(QEvent *e) override;

protected Q_SLOTS:
  virtual QIcon standardIconImplementation(StandardPixmap standardIcon, const QStyleOption *option, const QWidget *widget) const;
  virtual int layoutSpacingImplementation(QSizePolicy::ControlType control1, QSizePolicy::ControlType control2,
                                  Qt::Orientation orientation, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const;
private:
  Q_DISABLE_COPY(ctkProxyStyle)
  Q_DECLARE_PRIVATE(ctkProxyStyle)
};

#endif
