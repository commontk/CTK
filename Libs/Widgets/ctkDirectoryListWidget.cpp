/*=========================================================================

  Library:   CTK

  Copyright (c) University College London.

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

#include <iostream>

// Qt includes
#include <QDebug>
#include <QFileDialog>

// CTK includes
#include "ctkDirectoryListWidget.h"
#include "ctkDirectoryListWidget_p.h"

//-----------------------------------------------------------------------------
// ctkDirectoryListWidgetPrivate methods

//-----------------------------------------------------------------------------
ctkDirectoryListWidgetPrivate::~ctkDirectoryListWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
ctkDirectoryListWidgetPrivate::ctkDirectoryListWidgetPrivate(ctkDirectoryListWidget& object)
  : QObject(&object), q_ptr(&object)
{
}

//-----------------------------------------------------------------------------
void ctkDirectoryListWidgetPrivate::init()
{
  Q_Q(ctkDirectoryListWidget);
  this->setupUi(q);
}

//-----------------------------------------------------------------------------
void ctkDirectoryListWidgetPrivate::setupUi(QWidget * widget)
{
  this->Ui_ctkDirectoryListWidget::setupUi(widget);

  this->ExpandButton->setChecked(false);
  this->ExpandButton->setMirrorOnExpand(true);
  this->GroupBox->hide();

  QObject::connect(this->AddButton, SIGNAL(clicked()),
                   this, SLOT(onAddClicked()));
  QObject::connect(this->RemoveButton, SIGNAL(clicked()),
                   this, SLOT(onRemoveClicked()));
  QObject::connect(this->ExpandButton, SIGNAL(clicked(bool)),
                   this, SLOT(onExpandClicked(bool)));
  QObject::connect(this->DirectoryList, SIGNAL(directoryListChanged()),
                   this, SLOT(onDirectoryListChanged()));
}

//-----------------------------------------------------------------------------
void ctkDirectoryListWidgetPrivate::setDirectoryList(const QStringList& list)
{
  this->DirectoryList->setDirectoryList(list);
}

//-----------------------------------------------------------------------------
QStringList ctkDirectoryListWidgetPrivate::directoryList() const
{
  return this->DirectoryList->directoryList(true); // true for absolute path.
}

//-----------------------------------------------------------------------------
void ctkDirectoryListWidgetPrivate::onAddClicked()
{
  QString path = QFileDialog::getExistingDirectory(
        this->DirectoryList, tr("Select folder"),
        QString(""));
  // An empty directory means that the user cancelled the dialog.
  if (path.isEmpty())
    {
    return;
    }
  this->DirectoryList->addDirectory(path);
}

//-----------------------------------------------------------------------------
void ctkDirectoryListWidgetPrivate::onRemoveClicked()
{
  this->DirectoryList->removeSelectedDirectories();
}

//-----------------------------------------------------------------------------
void ctkDirectoryListWidgetPrivate::onExpandClicked(bool state)
{
  this->GroupBox->setVisible(state);
}

//-----------------------------------------------------------------------------
void ctkDirectoryListWidgetPrivate::onDirectoryListChanged()
{
  Q_Q(ctkDirectoryListWidget);
  emit (q->directoryListChanged(this->DirectoryList->directoryList()));
}

//-----------------------------------------------------------------------------
// ctkDirectoryListWidget methods

//-----------------------------------------------------------------------------
ctkDirectoryListWidget::~ctkDirectoryListWidget()
{
}

//-----------------------------------------------------------------------------
ctkDirectoryListWidget::ctkDirectoryListWidget(QWidget* newParent)
  : Superclass(newParent)
  , d_ptr(new ctkDirectoryListWidgetPrivate(*this))
{
  Q_D(ctkDirectoryListWidget);
  d->init();
}

//-----------------------------------------------------------------------------
void ctkDirectoryListWidget::setDirectoryList(const QStringList& list)
{
  Q_D(ctkDirectoryListWidget);
  d->setDirectoryList(list);
}

//-----------------------------------------------------------------------------
QStringList ctkDirectoryListWidget::directoryList() const
{
  Q_D(const ctkDirectoryListWidget);
  return d->directoryList();
}

