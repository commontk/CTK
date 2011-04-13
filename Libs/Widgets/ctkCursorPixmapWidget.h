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
class QColor;
class QSize;

// CTK includes
#include "ctkWidgetsExport.h"
class ctkCursorPixmapWidgetPrivate;

// Draws a cursor onto a pixmap

class CTK_WIDGETS_EXPORT ctkCursorPixmapWidget : public QLabel
{
  Q_OBJECT
  Q_FLAGS(CursorType CursorTypes)
  Q_PROPERTY(bool showCursor READ showCursor WRITE setShowCursor)
  Q_PROPERTY(QColor cursorColor READ cursorColor WRITE setCursorColor)
  Q_PROPERTY(CursorTypes cursorType READ cursorType WRITE setCursorType)
  Q_PROPERTY(QColor marginColor READ marginColor WRITE setMarginColor);
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

  /// Set/get whether or not to draw the cursor (default true)
  bool showCursor() const;
  void setShowCursor(bool newShow);

  /// Set/get the color of the cursor.  Default is the color from the widget's
  /// palette Highlight role.  Note that the Highlight role is overridden if
  /// you set this property.
  QColor cursorColor() const;
  void setCursorColor(const QColor& newColor);

  /// Set/get the cursor type (default CursorType_CrossHair)
  const CursorTypes& cursorType() const;
  void setCursorType(const CursorTypes& newType);

  /// Set/get color to set the widget to when not magnifying or when label
  /// size is larger than pixmap size.  Default is the color from the widget's
  /// palette Window role.  Note that the Window role is overridden if you
  /// set this property.
  QColor marginColor() const;
  void setMarginColor(const QColor& newColor);

  /// Set/get the width of the crosshair when in BullsEye mode (default 15)
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
