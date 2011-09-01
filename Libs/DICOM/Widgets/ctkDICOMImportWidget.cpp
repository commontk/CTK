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

//ctkDICOMCore includes
#include "ctkDICOMDatabase.h"

// ctkDICOMWidgets includes
#include "ctkDICOMImportWidget.h"
#include "ui_ctkDICOMImportWidget.h"

// STD includes
#include <iostream>

//Qt includes
#include <QFileSystemModel>

//----------------------------------------------------------------------------
class ctkDICOMImportWidgetPrivate: public Ui_ctkDICOMImportWidget
{
public:
  ctkDICOMImportWidgetPrivate(){}

  QFileSystemModel* FileSystemModel;

  QSharedPointer<ctkDICOMDatabase> DICOMDatabase;

};

//----------------------------------------------------------------------------
// ctkDICOMImportWidgetPrivate methods

//----------------------------------------------------------------------------
// ctkDICOMImportWidget methods

//----------------------------------------------------------------------------
ctkDICOMImportWidget::ctkDICOMImportWidget(QWidget* parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new ctkDICOMImportWidgetPrivate)
{
  Q_D(ctkDICOMImportWidget);

  d->setupUi(this);

  d->DirectoryWidget->setDirectory(QDir::homePath());

  d->FileSystemModel = new QFileSystemModel(this);
  d->FileSystemModel->setRootPath(QDir::homePath());
  d->directoryList->setModel(d->FileSystemModel);

  //connect signals and slots
  connect(d->DirectoryWidget, SIGNAL(directoryChanged(QString)),
          this, SLOT(onTopDirectoryChanged(QString)));
  this->onTopDirectoryChanged(d->DirectoryWidget->directory());
}

//----------------------------------------------------------------------------
ctkDICOMImportWidget::~ctkDICOMImportWidget()
{
  Q_D(ctkDICOMImportWidget);

  d->FileSystemModel->deleteLater();
}

//----------------------------------------------------------------------------
void ctkDICOMImportWidget::setTopDirectory(const QString& path)
{
  Q_D(ctkDICOMImportWidget);
  d->DirectoryWidget->setDirectory(path);
}

//----------------------------------------------------------------------------
void ctkDICOMImportWidget::onTopDirectoryChanged(const QString& path)
{
  Q_D(ctkDICOMImportWidget);
  d->directoryList->setRootIndex(d->FileSystemModel->index(path));
}

//----------------------------------------------------------------------------
void ctkDICOMImportWidget
::setDICOMDatabase(QSharedPointer<ctkDICOMDatabase> database)
{
  Q_D(ctkDICOMImportWidget);

  d->DICOMDatabase = database;
}
