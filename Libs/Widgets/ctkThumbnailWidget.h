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

#ifndef __ctkThumbnailWidget_h
#define __ctkThumbnailWidget_h

// Qt includes 
#include <QWidget>
#include <QModelIndex>

#include "ctkWidgetsExport.h"

class ctkThumbnailWidgetPrivate;

class CTK_WIDGETS_EXPORT ctkThumbnailWidget : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(QString text READ text WRITE setText)
  Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap)
  Q_PROPERTY(bool selected READ isSelected WRITE setSelected)
public:
  typedef QWidget Superclass;
  explicit ctkThumbnailWidget(QWidget* parent=0);
  virtual ~ctkThumbnailWidget();

  void setText(const QString& text);
  QString text()const;

  void setPixmap(const QPixmap& pixmap);
  const QPixmap* pixmap()const;

  void setSelected(bool selected);
  bool isSelected();

protected:
  QScopedPointer<ctkThumbnailWidgetPrivate> d_ptr;

  virtual void mousePressEvent(QMouseEvent* event);
  virtual void mouseDoubleClickEvent(QMouseEvent* event);

  virtual void resizeEvent(QResizeEvent* event);

private:
  Q_DECLARE_PRIVATE(ctkThumbnailWidget);
  Q_DISABLE_COPY(ctkThumbnailWidget);

signals:
  void selected(const ctkThumbnailWidget& widget);
  void doubleClicked(const ctkThumbnailWidget& widget);
};

#endif
