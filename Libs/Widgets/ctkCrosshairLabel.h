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

#ifndef __ctkCrosshairLabel_h
#define __ctkCrosshairLabel_h

// QT includes
#include <QLabel>
#include <QPen>
class QColor;
class QSize;

// CTK includes
#include "ctkWidgetsExport.h"

class ctkCrosshairLabelPrivate;

/// \ingroup Widgets
/// Draws a crosshair onto a QLabel.  This widget is designed to be used to show
/// a crosshair overlaid onto an image (the QLabel's pixmap).
/// Since painting must be done in discrete pixels, this widget looks best
/// when the widget size and the crosshair line width are both either even or odd.
/// If using a bulls-eye crosshair, this widget looks best if the crosshair line
/// width and the bullsEyeWidth are both either even or odd.

class CTK_WIDGETS_EXPORT ctkCrosshairLabel : public QLabel
{
  Q_OBJECT
  Q_FLAGS(CrosshairType CrosshairTypes)
  Q_PROPERTY(bool showCrosshair READ showCrosshair WRITE setShowCrosshair)
  // QT designer does not yet support QPen properties, so we provide the
  // temporary properties crosshairColor and lineWidth.
  Q_PROPERTY(QPen crosshairPen READ crosshairPen WRITE setCrosshairPen
             DESIGNABLE false)
  Q_PROPERTY(QColor crosshairColor READ crosshairColor WRITE setCrosshairColor)
  Q_PROPERTY(int lineWidth READ lineWidth WRITE setLineWidth)
  Q_PROPERTY(CrosshairTypes crosshairType READ crosshairType WRITE setCrosshairType)
  Q_PROPERTY(QColor marginColor READ marginColor WRITE setMarginColor)
  Q_PROPERTY(int bullsEyeWidth READ bullsEyeWidth WRITE setBullsEyeWidth)

public:
  /// Constructors
  typedef QLabel Superclass;
  explicit ctkCrosshairLabel(QWidget* parent = 0);
  virtual ~ctkCrosshairLabel();

  /// Enumeration over types of crosshairs
  enum CrosshairType {
    SimpleCrosshair = 0,
    BullsEyeCrosshair
  };
  Q_DECLARE_FLAGS(CrosshairTypes, CrosshairType)

  /// Set/get whether or not to draw the crosshair.  Default True.
  bool showCrosshair() const;
  void setShowCrosshair(bool newShow);

  /// Set/get the pen used to draw the crosshair.  Default color is from the
  /// widget's palette's Highlight role, default width is 0 (which draws
  /// at 1 pixel wide regardless of painter transformation).  Since painting
  /// must be done in discrete pixels, this widget looks best when the widget
  /// size and the crosshair pen width are both either even or odd.
  QPen crosshairPen() const;
  void setCrosshairPen(const QPen& newPen);

  /// Temporary: Set/get the crosshair color (via QPen).  This will be removed once
  /// Qt Designer supports QPen properties.
  QColor crosshairColor() const;
  void setCrosshairColor(const QColor& newColor);

  /// Temporary: Set/get the crosshair line width (via QPen).  This will be removed
  /// once Qt Designer supports QPen properties.  Since painting
  /// must be done in discrete pixels, this widget looks best when the widget
  /// size and the crosshair pen width are both either even or odd.
  int lineWidth() const;
  void setLineWidth(int newWidth);

  /// Set/get the crosshair type.  Default SimpleCrosshair.
  CrosshairTypes crosshairType() const;
  void setCrosshairType(const CrosshairTypes& newType);

  /// Set/get color to set the widget to when not magnifying or when label
  /// size is larger than pixmap size.  Default is the color from the widget's
  /// palette Window role.  Note that the Window role is overridden if you
  /// set this property.  Does not support transparent colors.
  QColor marginColor() const;
  void setMarginColor(const QColor& newColor);

  /// Set/get the width of the crosshair when in BullsEye mode.
  /// Looks best when the BullsEye width and the widget size are both either
  /// even or odd, preferably odd.  Default 15.  Since painting must be done
  /// in discrete pixels, this widget looks best if the crosshair pen width and
  /// the bullsEyeWidth are both either even or odd.
  int bullsEyeWidth() const;
  void setBullsEyeWidth(int newWidth);

  /// Size hints
  virtual QSize minimumSizeHint()const;
  virtual QSize sizeHint()const;
  virtual bool hasHeightForWidth()const;
  virtual int heightForWidth(int width)const;

protected:
  QScopedPointer<ctkCrosshairLabelPrivate> d_ptr;

  // Draws the crosshair on top of the widget.
  virtual void paintEvent(QPaintEvent * event);

private:
  Q_DECLARE_PRIVATE(ctkCrosshairLabel)
  Q_DISABLE_COPY(ctkCrosshairLabel)
}; 

#endif
