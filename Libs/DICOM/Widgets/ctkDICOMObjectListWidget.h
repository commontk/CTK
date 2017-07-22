/*=========================================================================

  Library:   CTK

  Copyright (c)  Brigham and Women's Hospital (BWH).

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

#ifndef __ctkDICOMObjectListWidget_h
#define __ctkDICOMObjectListWidget_h

// Qt includes
#include <QItemSelection>
#include <QWidget>

#include "ctkDICOMWidgetsExport.h"

class ctkDICOMObjectListWidgetPrivate;

/// \ingroup DICOM_Widgets
class CTK_DICOM_WIDGETS_EXPORT ctkDICOMObjectListWidget : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(QString currentFile READ currentFile WRITE setCurrentFile)
  Q_PROPERTY(QStringList fileList READ fileList WRITE setFileList)
  Q_PROPERTY(QString filterExpression READ filterExpression WRITE setFilterExpression)

public:
  typedef QWidget Superclass;
  explicit ctkDICOMObjectListWidget(QWidget* parent=0);
  virtual ~ctkDICOMObjectListWidget();

  QString currentFile();
  QStringList fileList();

  /// Filter displayed metadata based on content in Tag, Attribute, and Value columns.
  /// Simple search : enter any text to show only those items that contains the text.
  /// Use ? and * wildcards to represent any single character or sequence of characters.
  /// Regular expression search: Enter regexp: followed by a regular expression.
  /// For example, show 3 specific tags, enter : regexp : 0010, 0010 | 0010, 0020 | 0010, 0030
  QString filterExpression();

  /// Get metadata tree as plain text
  QString metadataAsText(bool allFiles = false);

  /// Open DICOM tag definition in a web browser
  void openLookupUrl(QString tag);

protected:
  QScopedPointer<ctkDICOMObjectListWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMObjectListWidget);
  Q_DISABLE_COPY(ctkDICOMObjectListWidget);

Q_SIGNALS:
  void doubleClicked(const QModelIndex&);

public Q_SLOTS:
  void setCurrentFile(const QString& newFileName);
  void setFileList(const QStringList& fileList);
  void setFilterExpression(const QString& expr);

protected Q_SLOTS:
  void itemDoubleClicked(const QModelIndex&);
  void onFilterChanged();
  void updateWidget();
  void copyPath();
  void copyMetadata();
  void copyAllFilesMetadata();
};

#endif
