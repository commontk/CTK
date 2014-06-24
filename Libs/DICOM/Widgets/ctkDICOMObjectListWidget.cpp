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

// Qt includes
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QString>
#include <QStringList>
#include <QUrl>

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
  void setPathLabel(const QString& currentFile);

  QString currentFile;
  QStringList fileList;
  ctkDICOMObjectModel* dicomObjectModel;
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
  this->dicomObjectModel->setFile(fileName);
  this->dcmObjectTreeView->setModel(this->dicomObjectModel);
  this->dcmObjectTreeView->expandAll();
}

// --------------------------------------------------------------------------
void ctkDICOMObjectListWidgetPrivate::setPathLabel(const QString& currentFile)
{
  currentPathLabel->setText(currentFile);
}

//----------------------------------------------------------------------------
// ctkDICOMObjectListWidget methods

//----------------------------------------------------------------------------
ctkDICOMObjectListWidget::ctkDICOMObjectListWidget(QWidget* _parent):Superclass(_parent), 
  d_ptr(new ctkDICOMObjectListWidgetPrivate)
{
  Q_D(ctkDICOMObjectListWidget);

  d->setupUi(this);
  d->dicomObjectModel = new ctkDICOMObjectModel(this);

  d->fileSliderWidget->setPageStep(1);

  d->currentPathLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
  connect(d->fileSliderWidget, SIGNAL(valueChanged(double)), this, SLOT(updateWidget()));
  connect(d->dcmObjectTreeView, SIGNAL(doubleClicked(const QModelIndex&))
                               ,this, SLOT(openLookupUrl(const QModelIndex&)));
  connect(d->copyPathPushButton , SIGNAL(clicked(bool)),this, SLOT(copyPath()));
}

//----------------------------------------------------------------------------
ctkDICOMObjectListWidget::~ctkDICOMObjectListWidget()
{
  Q_D(ctkDICOMObjectListWidget);
  d->dicomObjectModel->deleteLater();
}

//----------------------------------------------------------------------------
void ctkDICOMObjectListWidget::setCurrentFile(const QString& newFileName)
{
  Q_D(ctkDICOMObjectListWidget);
  d->setPathLabel(newFileName);
}

// --------------------------------------------------------------------------
void ctkDICOMObjectListWidget::setFileList(const QStringList& fileList)
{
  Q_D(ctkDICOMObjectListWidget);
  d->fileList = fileList;
  if (d-> fileList.size()> 0)
    {
    d->currentFile = d->fileList[0];
    d->setPathLabel(d->currentFile );
    d->currentPathLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    d->populateDICOMObjectTreeView(d->currentFile );
    d->fileSliderWidget->setMaximum(fileList.size()-1);
    }
}
// --------------------------------------------------------------------------
QString ctkDICOMObjectListWidget::currentFile()
{
  Q_D(ctkDICOMObjectListWidget);
  return d->currentFile;
}

// --------------------------------------------------------------------------
QStringList ctkDICOMObjectListWidget::fileList()
{
  Q_D(ctkDICOMObjectListWidget);
  return d->fileList;
}
// --------------------------------------------------------------------------

void ctkDICOMObjectListWidget::openLookupUrl(const QModelIndex& index)
{
  if (index.column() == 0)
  {
    QVariant  data = index.data();
    QString lookupUrl = "http://dicomlookup.com/lookup.asp?sw=Tnumber&q="+data.toString();
    QUrl url(lookupUrl);
    QDesktopServices::openUrl(url);
  }
}
// --------------------------------------------------------------------------

void ctkDICOMObjectListWidget::updateWidget()
{
  Q_D(ctkDICOMObjectListWidget);
  d->currentFile = d->fileList[static_cast<int>(d->fileSliderWidget->value())];
  d->setPathLabel(d->currentFile);
  d->populateDICOMObjectTreeView(d->currentFile);
 }
// --------------------------------------------------------------------------

void ctkDICOMObjectListWidget::copyPath()
{
  Q_D(ctkDICOMObjectListWidget);
  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(d->currentFile);
}
