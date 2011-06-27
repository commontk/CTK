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
#include <QWidget>

#include "ctkDICOMWidgetsExport.h"

class ctkDICOMAppWidgetPrivate;
class ctkDICOMThumbnailWidget;
class QModelIndex;

class CTK_DICOM_WIDGETS_EXPORT ctkDICOMAppWidget : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(QString databaseDirectory READ databaseDirectory WRITE setDatabaseDirectory)

public:
  typedef QWidget Superclass;
  explicit ctkDICOMAppWidget(QWidget* parent=0);
  virtual ~ctkDICOMAppWidget();

  QString databaseDirectory() const;

public slots:
  void setDatabaseDirectory(const QString& directory);
  void onAddToDatabase();

  void openImportDialog();
  void openExportDialog();
  void openQueryDialog();

signals:
  /// Emited when directory is changed
  void databaseDirectoryChanged(const QString&);

protected:
    QScopedPointer<ctkDICOMAppWidgetPrivate> d_ptr;
protected slots:
    void onImportDirectory(QString directory);
    void onModelSelected(const QModelIndex& index);

    /// To be called when a thumbnail in thumbnail list widget is selected
    void onThumbnailSelected(const ctkDICOMThumbnailWidget& widget);

    /// To be called when a thumbnail in thumbnail list widget is double-clicked
    void onThumbnailDoubleClicked(const ctkDICOMThumbnailWidget& widget);

    /// To be called when previous and next buttons are clicked
    void onNextImage();
    void onPreviousImage();
    void onNextSeries();
    void onPreviousSeries();
    void onNextStudy();
    void onPreviousStudy();

    /// To be called when an entry of the tree list is collapsed
    void onTreeCollapsed(const QModelIndex& index);

    /// To be called when an entry of the tree list is expanded
    void onTreeExpanded(const QModelIndex& index);

    /// To be called when auto-play checkbox state changed
    void onAutoPlayCheckboxStateChanged(int state);


    void onAutoPlayTimer();

private:
  Q_DECLARE_PRIVATE(ctkDICOMAppWidget);
  Q_DISABLE_COPY(ctkDICOMAppWidget);
};

#endif
