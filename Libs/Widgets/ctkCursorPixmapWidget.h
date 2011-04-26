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

#ifndef __ctkCursorPixmapWidget_h
#define __ctkCursorPixmapWidget_h

// QT includes
#include <QLabel>
#include <QPen>
class QColor;
class QSize;

// CTK includes
#include "ctkWidgetsExport.h"

class ctkCursorPixmapWidgetPrivate;

/// Draws a cursor onto a QLabel.  This widget is designed to be used to show
/// a cursor overlaid onto an image (the QLabel's pixmap).
/// Since painting must be done in discrete pixels, this widget looks best
/// when the widget size and the cursor line width are both either even or odd.
/// If using a bulls-eye cursor, this widget looks best if the cursor line
/// width and the bullsEyeWidth are both either even or odd.

class CTK_WIDGETS_EXPORT ctkCursorPixmapWidget : public QLabel
{
  Q_OBJECT
  Q_FLAGS(CursorType CursorTypes)
  Q_PROPERTY(bool showCursor READ showCursor WRITE setShowCursor)
  // QT designer does not yet support QPen properties, so we provide the
  // temporary properties cursorColor and lineWidth.
  Q_PROPERTY(QPen cursorPen READ cursorPen WRITE setCursorPen DESIGNABLE false)
  Q_PROPERTY(QColor cursorColor READ cursorColor WRITE setCursorColor)
  Q_PROPERTY(int lineWidth READ lineWidth WRITE setLineWidth)
  Q_PROPERTY(CursorTypes cursorType READ cursorType WRITE setCursorType)
  Q_PROPERTY(QColor marginColor READ marginColor WRITE setMarginColor)
  Q_PROPERTY(int bullsEyeWidth READ bullsEyeWidth WRITE setBullsEyeWidth)

public:
  /// Constructors
  typedef QLabel Superclass;
  explicit ctkCursorPixmapWidget(QWidget* parent = 0);
  virtual ~ctkCursorPixmapWidget();

  /// Enumeration over types of cursors
  enum CursorType {
    CrossHairCursor = 0,
    BullsEyeCursor
  };
  Q_DECLARE_FLAGS(CursorTypes, CursorType)

  /// Set/get whether or not to draw the cursor.  Default True.
  bool showCursor() const;
  void setShowCursor(bool newShow);

  /// Set/get the pen used to draw the cursor.  Default color is from the
  /// widget's palette's Highlight role, default width is 0 (which draws
  /// at 1 pixel wide regardless of painter transformation).  Since painting
  /// must be done in discrete pixels, this widget looks best when the widget
  /// size and the cursor pen width are both either even or odd.
  QPen cursorPen() const;
  void setCursorPen(const QPen& newPen);

  /// Temporary: Set/get the cursor color (via QPen).  This will be removed once
  /// Qt Designer supports QPen properties.
  QColor cursorColor() const;
  void setCursorColor(const QColor& newColor);

  /// Temporary: Set/get the cursor line width (via QPen).  This will be removed
  /// once Qt Designer supports QPen properties.  Since painting
  /// must be done in discrete pixels, this widget looks best when the widget
  /// size and the cursor pen width are both either even or odd.
  int lineWidth() const;
  void setLineWidth(int newWidth);

  /// Set/get the cursor type.  Default CursorType_CrossHair.
  CursorTypes cursorType() const;
  void setCursorType(const CursorTypes& newType);

  /// Set/get color to set the widget to when not magnifying or when label
  /// size is larger than pixmap size.  Default is the color from the widget's
  /// palette Window role.  Note that the Window role is overridden if you
  /// set this property.  Does not support transparent colors.
  QColor marginColor() const;
  void setMarginColor(const QColor& newColor);

  /// Set/get the width of the crosshair when in BullsEye mode.
  /// Looks best when the BullsEye width and the widget size are both either
  /// even or odd, preferably odd.  Default 15.  Since painting must be done
  /// in discrete pixels, this widget looks best if the cursor pen width and
  /// the bullsEyeWidth are both either even or odd.
  int bullsEyeWidth() const;
  void setBullsEyeWidth(int newWidth);

  /// Size hints
  virtual QSize minimumSizeHint()const;
  virtual QSize sizeHint()const;
  virtual bool hasHeightForWidth()const;
  virtual int heightForWidth(int width)const;

protected:
  QScopedPointer<ctkCursorPixmapWidgetPrivate> d_ptr;

  // Draws the cursor on top of the widget.
  virtual void paintEvent(QPaintEvent * event);

private:
  Q_DECLARE_PRIVATE(ctkCursorPixmapWidget);
  Q_DISABLE_COPY(ctkCursorPixmapWidget);
}; 

#endif
