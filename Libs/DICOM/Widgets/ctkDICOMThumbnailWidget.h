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

#ifndef __ctkDICOMThumbnailWidget_h
#define __ctkDICOMThumbnailWidget_h

// Qt includes 
#include <QWidget>

#include "ctkDICOMWidgetsExport.h"

class ctkDICOMThumbnailWidgetPrivate;

class CTK_DICOM_WIDGETS_EXPORT ctkDICOMThumbnailWidget : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(QString text READ text WRITE setText)
  Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap)
public:
  typedef QWidget Superclass;
  explicit ctkDICOMThumbnailWidget(QWidget* parent=0);
  virtual ~ctkDICOMThumbnailWidget();

  void setText(const QString& text);
  QString text()const;

  void setPixmap(const QPixmap& pixmap);
  const QPixmap* pixmap()const;

protected:
  QScopedPointer<ctkDICOMThumbnailWidgetPrivate> d_ptr;

  virtual void mousePressEvent(QMouseEvent* event);

private:
  Q_DECLARE_PRIVATE(ctkDICOMThumbnailWidget);
  Q_DISABLE_COPY(ctkDICOMThumbnailWidget);

signals:
  void selected(const ctkDICOMThumbnailWidget& widget);
};

#endif
