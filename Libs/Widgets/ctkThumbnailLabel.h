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

#ifndef __ctkThumbnailLabel_h
#define __ctkThumbnailLabel_h

// Qt includes 
#include <QWidget>
#include <QModelIndex>

#include "ctkWidgetsExport.h"

class ctkThumbnailLabelPrivate;

/// \ingroup Widgets
/// ctkThumbnailLabel is an advanced label that gives control over
/// the pixmap size and text location.
/// If a pixmap is set, it is resized to fit the available space but its
/// original width/height ratio is always respected.
/// The widget can be selected or not (a rectangle is drawn around)
class CTK_WIDGETS_EXPORT ctkThumbnailLabel : public QWidget
{
  Q_OBJECT
  /// If the text is empty, the space allocated for the text is hidden
  /// Empty by default
  Q_PROPERTY(QString text READ text WRITE setText)
  /// Position of the text relative to the pixmap.
  /// Qt::AlignTop | Qt::AlignHCenter by default.
  /// For now, if the alignment is HCenter|VCenter (same location than the
  /// pixmap), no text is shown.
  Q_PROPERTY(Qt::Alignment textPosition READ textPosition WRITE setTextPosition)
  /// Optional pixmap for the label.
  /// No pixmap by default
  Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap)
  /// Controls the quality of the resizing of the pixmap.
  /// Qt::FastTransformation by default
  Q_PROPERTY(Qt::TransformationMode transformationMode READ transformationMode WRITE setTransformationMode)
  /// Control whether or not the label is selected. When selected, a rectangle
  /// is drawn around the widget with the \a selectedColor color.
  /// Not selected by default
  Q_PROPERTY(bool selected READ isSelected WRITE setSelected)
  /// Color of the selected rectangle.
  /// Palette highlight color by default
  Q_PROPERTY(QColor selectedColor READ selectedColor WRITE setSelectedColor)
public:
  typedef QWidget Superclass;
  explicit ctkThumbnailLabel(QWidget* parent=0);
  virtual ~ctkThumbnailLabel();

  void setText(const QString& text);
  QString text()const;

  void setTextPosition(const Qt::Alignment& alignment);
  Qt::Alignment textPosition()const;

  void setPixmap(const QPixmap& pixmap);
  const QPixmap* pixmap()const;

  Qt::TransformationMode transformationMode()const;
  void setTransformationMode(Qt::TransformationMode mode);

  void setSelected(bool selected);
  bool isSelected()const;

  void setSelectedColor(const QColor& color);
  QColor selectedColor()const;

  virtual QSize minimumSizeHint()const;
  virtual QSize sizeHint()const;
  virtual int heightForWidth(int width)const;

protected:
  QScopedPointer<ctkThumbnailLabelPrivate> d_ptr;

  virtual void paintEvent(QPaintEvent* event);
  virtual void mousePressEvent(QMouseEvent* event);
  virtual void mouseDoubleClickEvent(QMouseEvent* event);

  virtual void resizeEvent(QResizeEvent* event);

private:
  Q_DECLARE_PRIVATE(ctkThumbnailLabel);
  Q_DISABLE_COPY(ctkThumbnailLabel);

Q_SIGNALS:
  void selected(const ctkThumbnailLabel& widget);
  void doubleClicked(const ctkThumbnailLabel& widget);
};

#endif
