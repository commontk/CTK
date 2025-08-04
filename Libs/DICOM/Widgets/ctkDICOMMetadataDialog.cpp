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

// Qt includes
#include <QApplication>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QHideEvent>
#include <QShowEvent>
#include <QStringList>
#include <QVBoxLayout>

// ctkDICOMWidgets includes
#include "ctkDICOMMetadataDialog.h"
#include "ctkDICOMObjectListWidget.h"

//----------------------------------------------------------------------------
ctkDICOMMetadataDialog::ctkDICOMMetadataDialog(QWidget* parent)
  : QDialog(parent)
{
  this->setWindowFlags(Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint | Qt::Window);
  // Set to non-modal to allow interaction with other windows for easier debugging and comparison
  this->setModal(false);
  this->setSizeGripEnabled(true);
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setMargin(0);
  this->tagListWidget = new ctkDICOMObjectListWidget();
  layout->addWidget(this->tagListWidget);
}

//----------------------------------------------------------------------------
ctkDICOMMetadataDialog::~ctkDICOMMetadataDialog()
{
}

//----------------------------------------------------------------------------
void ctkDICOMMetadataDialog::setFileList(const QStringList& fileList)
{
  this->tagListWidget->setFileList(fileList);
}

//----------------------------------------------------------------------------
void ctkDICOMMetadataDialog::closeEvent(QCloseEvent* evt)
{
  // just hide the window when close button is clicked
  evt->ignore();
  this->hide();
}

//----------------------------------------------------------------------------
void ctkDICOMMetadataDialog::showEvent(QShowEvent* event)
{
  QDialog::showEvent(event);
  // QDialog would reset window position and size when shown.
  // Restore its previous size instead (user may look at metadata
  // of different series one after the other and would be inconvenient to
  // set the desired size manually each time).
  if (!this->savedGeometry.isEmpty())
  {
    this->restoreGeometry(this->savedGeometry);
    if (this->isMaximized())
    {
      this->setGeometry(QApplication::desktop()->availableGeometry(this));
    }
  }
}

//----------------------------------------------------------------------------
void ctkDICOMMetadataDialog::hideEvent(QHideEvent* event)
{
  this->savedGeometry = this->saveGeometry();
  QDialog::hideEvent(event);
}
