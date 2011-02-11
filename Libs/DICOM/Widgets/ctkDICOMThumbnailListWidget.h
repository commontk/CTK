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

#ifndef __ctkDICOMThumbnailsListWidget_h
#define __ctkDICOMThumbnailsListWidget_h

// Qt includes 
#include <QWidget>

#include "ctkDICOMWidgetsExport.h"

class QModelIndex;
class ctkDICOMThumbnailListWidgetPrivate;
class ctkDICOMThumbnailWidget;

class CTK_DICOM_WIDGETS_EXPORT ctkDICOMThumbnailListWidget : public QWidget
{
  Q_OBJECT
public:
  typedef QWidget Superclass;
  explicit ctkDICOMThumbnailListWidget(QWidget* parent=0);
  virtual ~ctkDICOMThumbnailListWidget();

  void setThumbnailFiles(const QStringList& thumbnailFiles);
  
protected:
  QScopedPointer<ctkDICOMThumbnailListWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMThumbnailListWidget);
  Q_DISABLE_COPY(ctkDICOMThumbnailListWidget);

signals:
  void selected(const ctkDICOMThumbnailWidget& widget);
};

#endif
