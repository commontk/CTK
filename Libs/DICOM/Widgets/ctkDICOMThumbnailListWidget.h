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

#ifndef __ctkDICOMThumbnailListWidget_h
#define __ctkDICOMThumbnailListWidget_h

#include "ctkDICOMWidgetsExport.h"
#include "ctkThumbnailListWidget.h"

class QModelIndex;
class ctkDICOMThumbnailListWidgetPrivate;
class ctkThumbnailWidget;

/// \ingroup DICOM_Widgets
class CTK_DICOM_WIDGETS_EXPORT ctkDICOMThumbnailListWidget : public ctkThumbnailListWidget
{
  Q_OBJECT
public:
  typedef ctkThumbnailListWidget Superclass;
  explicit ctkDICOMThumbnailListWidget(QWidget* parent=0);
  virtual ~ctkDICOMThumbnailListWidget();

  void setDatabaseDirectory(const QString& directory);

  void selectThumbnailFromIndex(const QModelIndex& index);

private:
  Q_DECLARE_PRIVATE(ctkDICOMThumbnailListWidget);
  Q_DISABLE_COPY(ctkDICOMThumbnailListWidget);

public Q_SLOTS:
  void addThumbnails(const QModelIndex& index);
};

#endif
