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

#ifndef __ctkThumbnailListWidget_h
#define __ctkThumbnailListWidget_h

// Qt includes 
#include <QWidget>

#include "ctkWidgetsExport.h"

class QModelIndex;
class ctkThumbnailListWidgetPrivate;
class ctkThumbnailLabel;

/// \ingroup Widgets
class CTK_WIDGETS_EXPORT ctkThumbnailListWidget : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(int currentThumbnail READ currentThumbnail WRITE setCurrentThumbnail)
  Q_PROPERTY(QSize thumbnailSize READ thumbnailSize WRITE setThumbnailSize)
public:
  typedef QWidget Superclass;
  explicit ctkThumbnailListWidget(QWidget* parent=0);
  virtual ~ctkThumbnailListWidget();
  
  /// Add multiple thumbnails to the widget
  void addThumbnails(QList<QPixmap> thumbnails);

  /// Set current thumbnail
  void setCurrentThumbnail(int index);

  /// Get current thumbnail
  int currentThumbnail();

  /// Clear all the thumbnails
  void clearThumbnails();

  /// Get thumbnail width
  QSize thumbnailSize()const;

public Q_SLOTS:
  /// Set thumbnail width
  void setThumbnailSize(QSize size);

Q_SIGNALS:
  void selected(const ctkThumbnailLabel& widget);
  void doubleClicked(const ctkThumbnailLabel& widget);

protected Q_SLOTS:
  void onThumbnailSelected(const ctkThumbnailLabel& widget);

protected:
  explicit ctkThumbnailListWidget(ctkThumbnailListWidgetPrivate* ptr, QWidget* parent=0);
  ctkThumbnailListWidgetPrivate* d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkThumbnailListWidget);
  Q_DISABLE_COPY(ctkThumbnailListWidget);

};

#endif
