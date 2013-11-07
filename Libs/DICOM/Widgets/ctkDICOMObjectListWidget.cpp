/*=========================================================================

  Library:   CTK

  Copyright (c) Brigham and Women's Hospital (BWH).

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

// ctkDICOMWidgets includes
#include "ctkDICOMObjectListWidget.h"
#include "ui_ctkDICOMObjectListWidget.h"

// STD includes
#include <iostream>

// Qt includes
#include <QString>
#include <QStringList>
#include <QTimer>

//CTK includes
#include <ctkDICOMObjectModel.h>
#include <ctkLogger.h>
static ctkLogger logger("org.commontk.DICOM.Widgets.ctkDICOMObjectListWidget");

//----------------------------------------------------------------------------
class ctkDICOMObjectListWidgetPrivate: public Ui_ctkDICOMObjectListWidget
{
public:
  ctkDICOMObjectListWidgetPrivate();
  ~ctkDICOMObjectListWidgetPrivate();
  void populateDICOMObjectTreeView(const QString& fileName);

  QString currentFile;
  QStringList fileList;
};

//----------------------------------------------------------------------------
// ctkDICOMObjectListWidgetPrivate methods

//----------------------------------------------------------------------------
ctkDICOMObjectListWidgetPrivate::ctkDICOMObjectListWidgetPrivate()
{

}

//----------------------------------------------------------------------------
ctkDICOMObjectListWidgetPrivate::~ctkDICOMObjectListWidgetPrivate()
{

}

//----------------------------------------------------------------------------
void ctkDICOMObjectListWidgetPrivate::populateDICOMObjectTreeView(const QString& fileName)
{
  //TODO: Check memory management
  ctkDICOMObjectModel* dcmObjModel = new ctkDICOMObjectModel;
  dcmObjModel->setFile(fileName);
  this->dcmObjectTreeView->reset();
  this->dcmObjectTreeView->setModel(dcmObjModel);
  this->dcmObjectTreeView->expandAll();
}

//----------------------------------------------------------------------------
// ctkDICOMObjectListWidget methods

//----------------------------------------------------------------------------
ctkDICOMObjectListWidget::ctkDICOMObjectListWidget(QWidget* _parent):Superclass(_parent), 
  d_ptr(new ctkDICOMObjectListWidgetPrivate)
{
  Q_D(ctkDICOMObjectListWidget);

  d->setupUi(this);
  d->currentPathLineEdit->setReadOnly(true);
  connect(d->fileSlider, SIGNAL(valueChanged(int)), this, SLOT(updateWidget()));
}

//----------------------------------------------------------------------------
ctkDICOMObjectListWidget::~ctkDICOMObjectListWidget()
{
}

//----------------------------------------------------------------------------
void ctkDICOMObjectListWidget::setCurrentFile(const QString& newFileName)
{
  Q_D(ctkDICOMObjectListWidget);
  d->currentPathLineEdit->setText(newFileName);
}

// --------------------------------------------------------------------------
void ctkDICOMObjectListWidget::setFileList(const QStringList& fileList)
{
  Q_D(ctkDICOMObjectListWidget);
  d->fileList = fileList;
  if (d-> fileList.size()> 0)
    {
    d->currentFile = d->fileList[0];
    d->currentPathLineEdit->setText(d->currentFile );
    d->populateDICOMObjectTreeView(d->currentFile );
    d->fileSlider->setMaximum(d->fileList.size()-1);
    }
}

// --------------------------------------------------------------------------
QString ctkDICOMObjectListWidget::currentFile()
{
  Q_D(const ctkDICOMObjectListWidget);
  return d->currentFile;
}

// --------------------------------------------------------------------------
QStringList ctkDICOMObjectListWidget::fileList()
{
  Q_D(const ctkDICOMObjectListWidget);
  return d->fileList;
}

// --------------------------------------------------------------------------
void ctkDICOMObjectListWidget::updateWidget()
{
  Q_D(ctkDICOMObjectListWidget);
  int fileNumber = d->fileSlider->value();
  d->currentFile = d->fileList[fileNumber];
  d->currentPathLineEdit->setText(d->currentFile);
  d->populateDICOMObjectTreeView(d->currentFile);
}
