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

#ifndef __ctkDICOMAppWidget_h
#define __ctkDICOMAppWidget_h

// Qt includes 
#include <QMainWindow>

#include "ctkDICOMWidgetsExport.h"

class QModelIndex;
class ctkDICOMAppWidgetPrivate;
class ctkDICOMThumbnailWidget;

class CTK_DICOM_WIDGETS_EXPORT ctkDICOMAppWidget : public QWidget
{
Q_OBJECT;
public:
  typedef QWidget Superclass;
  explicit ctkDICOMAppWidget(QWidget* parent=0);
  virtual ~ctkDICOMAppWidget();

public slots:
    void setDatabaseDirectory(const QString& directory);
    void onAddToDatabase();

    void onDICOMModelSelected(const QModelIndex& index);
    void onThumbnailSelected(const ctkDICOMThumbnailWidget& widget);

    void openImportDialog();
    void openExportDialog();
    void openQueryDialog();

protected:
    QScopedPointer<ctkDICOMAppWidgetPrivate> d_ptr;
protected slots:
    void onImportDirectory(QString directory);


private:
  Q_DECLARE_PRIVATE(ctkDICOMAppWidget);
  Q_DISABLE_COPY(ctkDICOMAppWidget);
};

#endif
