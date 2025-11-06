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

#ifndef __ctkDICOMMetadataDialog_h
#define __ctkDICOMMetadataDialog_h

// Qt includes
#include <QDialog>
#include <QStringList>

// CTK includes
#include "ctkDICOMWidgetsExport.h"

class ctkDICOMObjectListWidget;

/// \ingroup DICOM_Widgets
class CTK_DICOM_WIDGETS_EXPORT ctkDICOMMetadataDialog : public QDialog
{
  Q_OBJECT

public:
  /// Constructor
  explicit ctkDICOMMetadataDialog(QWidget* parent = nullptr);

  /// Destructor
  virtual ~ctkDICOMMetadataDialog();

  /// Set the list of DICOM files to display metadata for
  void setFileList(const QStringList& fileList);

  /// Handle close event by hiding the dialog instead of closing it
  void closeEvent(QCloseEvent* evt) override;

  /// Handle show event to restore previous geometry
  void showEvent(QShowEvent* event) override;

  /// Handle hide event to save current geometry
  void hideEvent(QHideEvent* event) override;

protected:
  ctkDICOMObjectListWidget* tagListWidget;
  QByteArray savedGeometry;

private:
  Q_DISABLE_COPY(ctkDICOMMetadataDialog)
};

#endif
