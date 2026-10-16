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

  This file was originally developed by Davide Punzo, punzodavide@hotmail.it,
  and development was supported by the Program for Intelligent Image-Guided Interventions (PI3).

=========================================================================*/

#ifndef __ctkDICOMVisualBrowserColors_p_h
#define __ctkDICOMVisualBrowserColors_p_h

//
//  W A R N I N G
//  -------------
//
// This file is not part of the CTK API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

// Qt includes
#include <QApplication>
#include <QBrush>
#include <QColor>
#include <QIcon>
#include <QPainter>
#include <QPalette>
#include <QPixmap>
#include <QString>
#include <QWidget>

/// \ingroup DICOM_Widgets
/// Colors of the visual DICOM browser: the cards drawn by its delegates, and the
/// style sheets of its scroll bars, menus and buttons.
///
/// They are derived from the palette of the widgets, so that the browser follows the
/// light and dark styles of the application: the base color, shifted toward the text
/// color for the cards and their borders, and the highlight color for the selection.
/// Nothing is white or black on its own.
namespace ctkDICOMVisualBrowserColors
{

/// The palette to derive the colors from. A style sheet that makes a view transparent
/// ("background: transparent", as SlicerHIVEApp does for the views of the browser,
/// or "selection-background-color: transparent") makes the matching colors of its
/// palette transparent as well: those come from the palette of the application.
inline QPalette palette(const QPalette& widgetPalette, const QWidget* widget)
{
  QPalette result = widgetPalette;
  const QPalette applicationPalette = widget ? QApplication::palette(widget) : QApplication::palette();
  const QList<QPalette::ColorRole> roles = QList<QPalette::ColorRole>()
    << QPalette::Window << QPalette::Base << QPalette::Text << QPalette::WindowText
    << QPalette::Highlight << QPalette::HighlightedText << QPalette::Shadow;
  const QList<QPalette::ColorGroup> groups = QList<QPalette::ColorGroup>()
    << QPalette::Active << QPalette::Inactive << QPalette::Disabled;
  for (QPalette::ColorGroup group : groups)
  {
    for (QPalette::ColorRole role : roles)
    {
      // "transparent" leaves either a transparent color or no brush at all, whose
      // color is an opaque black
      const QBrush& brush = result.brush(group, role);
      if (brush.style() == Qt::NoBrush || brush.color().alpha() == 0)
      {
        result.setColor(group, role, applicationPalette.color(group, role));
      }
    }
  }
  return result;
}

/// Palette of a widget of the browser, see palette()
inline QPalette palette(const QWidget* widget)
{
  return widget ? palette(widget->palette(), widget) : QApplication::palette();
}

/// Blend \a from toward \a to: 0 gives \a from, 1 gives \a to.
inline QColor blend(const QColor& from, const QColor& to, qreal amount)
{
  return QColor::fromRgbF(from.redF() + (to.redF() - from.redF()) * amount,
                          from.greenF() + (to.greenF() - from.greenF()) * amount,
                          from.blueF() + (to.blueF() - from.blueF()) * amount,
                          from.alphaF() + (to.alphaF() - from.alphaF()) * amount);
}

/// \a color with the given alpha (0-255)
inline QColor withAlpha(QColor color, int alpha)
{
  color.setAlpha(alpha);
  return color;
}

inline bool isDark(const QPalette& palette)
{
  return palette.color(QPalette::Base).lightness() < 128;
}

/// Background of the cards
inline QColor card(const QPalette& palette)
{
  return blend(palette.color(QPalette::Base), palette.color(QPalette::Text), isDark(palette) ? 0.05 : 0.02);
}

/// Background of the inner areas of a card (e.g. the header of a patient)
inline QColor cardInner(const QPalette& palette)
{
  return blend(palette.color(QPalette::Base), palette.color(QPalette::Text), isDark(palette) ? 0.09 : 0.035);
}

inline QColor border(const QPalette& palette)
{
  return blend(palette.color(QPalette::Base), palette.color(QPalette::Text), isDark(palette) ? 0.18 : 0.14);
}

/// Border of a hovered card
inline QColor borderStrong(const QPalette& palette)
{
  return blend(palette.color(QPalette::Base), palette.color(QPalette::Text), isDark(palette) ? 0.32 : 0.30);
}

inline QColor text(const QPalette& palette)
{
  return palette.color(QPalette::Text);
}

inline QColor secondaryText(const QPalette& palette)
{
  return blend(palette.color(QPalette::Text), palette.color(QPalette::Base), 0.40);
}

/// Selection, current item, progress
inline QColor accent(const QPalette& palette)
{
  return palette.color(QPalette::Highlight);
}

/// Tint of a selected (strong) or hovered card
inline QColor accentSoft(const QPalette& palette, bool strong)
{
  return withAlpha(accent(palette), strong ? (isDark(palette) ? 70 : 55) : (isDark(palette) ? 40 : 28));
}

/// Background of a small button drawn on a card: transparent until hovered or pressed
inline QColor buttonBackground(const QPalette& palette, bool hovered, bool pressed)
{
  if (pressed)
  {
    return withAlpha(palette.color(QPalette::Text), 60);
  }
  if (hovered)
  {
    return withAlpha(palette.color(QPalette::Text), 30);
  }
  return withAlpha(palette.color(QPalette::Base), 200);
}

/// Background of a small button drawn over a thumbnail, which can be of any
/// brightness: mostly opaque, so that the icon reads on any image
inline QColor overlayButtonBackground(const QPalette& palette, bool hovered, bool pressed)
{
  const QColor base = palette.color(QPalette::Base);
  const QColor text = palette.color(QPalette::Text);
  if (pressed)
  {
    return withAlpha(blend(base, text, 0.35), 230);
  }
  if (hovered)
  {
    return withAlpha(blend(base, text, 0.20), 230);
  }
  return withAlpha(base, 220);
}

/// Color of the icons of the browser: the color of the icon files
inline QColor icon()
{
  return QColor(0x68, 0x84, 0xa8);
}

/// Name of a color usable in a style sheet, alpha included
inline QString styleSheetColor(const QColor& color)
{
  return QString("rgba(%1, %2, %3, %4)").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha());
}

/// Thin vertical scroll bar of the browser views
inline QString scrollBarStyleSheet(const QPalette& palette)
{
  return QString(R"(
    QScrollBar:vertical {
      border: none;
      width: 10px;
      margin: 0px 0px 0px 0px;
      background: none;
    }
    QScrollBar::handle:vertical {
      min-height: 50px;
      margin: 4px 0px 4px 0px;
      width: 10px;
      border-width: 1px;
      border-radius: 2px;
      background-color: %1;
    }
    QScrollBar::handle:vertical:hover {
      background-color: %2;
    }
    QScrollBar::handle:vertical:pressed {
      background-color: %3;
    }
    QScrollBar::add-line:vertical {
      height: 0px;
      subcontrol-position: bottom;
      subcontrol-origin: margin;
    }
    QScrollBar::sub-line:vertical {
      height: 0px;
      subcontrol-position: top;
      subcontrol-origin: margin;
    }
  )")
    .arg(styleSheetColor(blend(palette.color(QPalette::Base), palette.color(QPalette::Text), 0.35)))
    .arg(styleSheetColor(blend(palette.color(QPalette::Base), palette.color(QPalette::Text), 0.55)))
    .arg(styleSheetColor(accent(palette)));
}

/// Context menus of the patients, studies and series
inline QString menuStyleSheet(const QPalette& palette)
{
  return QString(
    "QMenu {"
    "  background-color: %1;"
    "  border-radius: 8px;"
    "  border: 1px solid %2;"
    "  padding: 4px;"
    "}"
    "QMenu::item {"
    "  padding: 8px 32px 8px 16px;"
    "  border-radius: 4px;"
    "  color: %3;"
    "  background-color: transparent;"
    "  margin: 2px 4px;"
    "}"
    "QMenu::item:selected {"
    "  background-color: %4;"
    "  color: %5;"
    "}"
    "QMenu::item:disabled {"
    "  color: %6;"
    "}"
    "QMenu::separator {"
    "  height: 1px;"
    "  background: %2;"
    "  margin: 4px 8px;"
    "}")
    .arg(styleSheetColor(card(palette)))
    .arg(styleSheetColor(border(palette)))
    .arg(styleSheetColor(text(palette)))
    .arg(styleSheetColor(blend(card(palette), accent(palette), 0.12)))
    .arg(styleSheetColor(blend(accent(palette), text(palette), isDark(palette) ? 0.25 : 0.0)))
    .arg(styleSheetColor(secondaryText(palette)));
}

/// Floating button of the main action (load), in the highlight color
inline QString actionButtonStyleSheet(const QPalette& palette)
{
  const QColor background = accent(palette);
  const QColor foreground = palette.color(QPalette::HighlightedText);
  return QString(R"(
    QPushButton {
      background: %1;
      border: none;
      border-radius: 24px;
      font-size: 14px;
      font-weight: 600;
      color: %2;
      padding: 14px 32px;
      min-width: 90px;
    }
    QPushButton:hover {
      background: %3;
    }
    QPushButton:pressed {
      background: %4;
      padding: 15px 31px 13px 33px;
    }
  )")
    .arg(styleSheetColor(withAlpha(background, 235)))
    .arg(styleSheetColor(foreground))
    .arg(styleSheetColor(blend(background, foreground, 0.12)))
    .arg(styleSheetColor(blend(background, palette.color(QPalette::Shadow), 0.25)));
}

/// \a icon repainted in a single \a color, e.g. to read on a colored button
inline QIcon tintedIcon(const QIcon& icon, const QColor& color, const QSize& size)
{
  QIcon tinted;
  for (int scale = 1; scale <= 2; ++scale)
  {
    QPixmap pixmap = icon.pixmap(size * scale);
    if (pixmap.isNull())
    {
      continue;
    }
    QPainter painter(&pixmap);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(pixmap.rect(), color);
    painter.end();
    tinted.addPixmap(pixmap);
  }
  return tinted;
}

/// Banner of a warning: a tint of amber over the base color
inline QString warningStyleSheet(const QPalette& palette)
{
  const QColor amber(234, 165, 63);
  return QString("color: %1; background-color: %2;")
    .arg(styleSheetColor(text(palette)))
    .arg(styleSheetColor(blend(palette.color(QPalette::Base), amber, isDark(palette) ? 0.25 : 0.35)));
}

}

#endif
