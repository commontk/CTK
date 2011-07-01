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

#ifndef __ctkThumbnailListWidget_h
#define __ctkThumbnailListWidget_h

// Qt includes 
#include <QWidget>

#include "ctkWidgetsExport.h"

class QModelIndex;
class ctkThumbnailListWidgetPrivate;
class ctkThumbnailWidget;

class Ui_ctkThumbnailListWidget;

class CTK_WIDGETS_EXPORT ctkThumbnailListWidget : public QWidget
{
  Q_OBJECT
public:
  typedef QWidget Superclass;
  explicit ctkThumbnailListWidget(QWidget* parent=0);
  virtual ~ctkThumbnailListWidget();
  
  /// Add multiple thumbnails to the widget
  void addThumbnails(QList<QPixmap> thumbnails);

  /// Select a thumbnail
  void selectThumbnail(int index);

  /// Clear all the thumbnails
  void reset();

  /// Get thumbnails width
  int thumbnailWidth();

protected:
  Ui_ctkThumbnailListWidget* ui;
  QScopedPointer<ctkThumbnailListWidgetPrivate> d_ptr;

  int ThumbnailWidth;

private:
  Q_DECLARE_PRIVATE(ctkThumbnailListWidget);
  Q_DISABLE_COPY(ctkThumbnailListWidget);

signals:
  void selected(const ctkThumbnailWidget& widget);
  void doubleClicked(const ctkThumbnailWidget& widget);

public slots:
  void onThumbnailSelected(const ctkThumbnailWidget& widget);

  /// Set thumbnails width
  void setThumbnailWidth(int width);
};

#endif
