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
#include <QSortFilterProxyModel>
#include <QFileSystemModel>
#include <QFileInfo>
#include <QMessageBox>

// CTK includes
#include "ctkPathListButtonsWidget.h"
#include "ctkPathListButtonsWidget_p.h"

//-----------------------------------------------------------------------------
// ctkPathListButtonsWidgetPrivate methods

//-----------------------------------------------------------------------------
ctkPathListButtonsWidgetPrivate::~ctkPathListButtonsWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
ctkPathListButtonsWidgetPrivate::ctkPathListButtonsWidgetPrivate(ctkPathListButtonsWidget& object)
  : QObject(&object)
  , q_ptr(&object)
  , PathListWidget(NULL)
{
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidgetPrivate::init()
{
  Q_Q(ctkPathListButtonsWidget);
  this->setupUi(q);

  q->unsetIconAddFilesButton();
  q->unsetIconAddDirectoryButton();
  q->unsetIconRemoveButton();
  q->unsetIconEditButton();
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidgetPrivate::setupUi(QWidget * widget)
{
  this->Ui_ctkPathListButtonsWidget::setupUi(widget);

  connect(this->AddFilesButton, SIGNAL(clicked()), SLOT(on_AddFilesButton_clicked()));
  connect(this->AddDirectoryButton, SIGNAL(clicked()), SLOT(on_AddDirButton_clicked()));
  connect(this->RemoveButton, SIGNAL(clicked()), SLOT(on_RemoveButton_clicked()));
  connect(this->EditButton, SIGNAL(clicked()), SLOT(on_EditButton_clicked()));
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidgetPrivate::on_AddFilesButton_clicked()
{
  if (!this->PathListWidget) return;

  QStringList paths = this->openAddFilesDialog(true);
  this->addPathsWithWarningMessage(paths);
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidgetPrivate::on_AddDirButton_clicked()
{
  if (!this->PathListWidget) return;

  QStringList paths = this->openAddDirDialog();
  this->addPathsWithWarningMessage(paths);
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidgetPrivate::on_RemoveButton_clicked()
{
  if (!this->PathListWidget) return;

  this->PathListWidget->removeSelectedPaths();
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidgetPrivate::on_EditButton_clicked()
{
  Q_Q(ctkPathListButtonsWidget);

  if (!this->PathListWidget) return;

  QString currentPath = this->PathListWidget->currentPath(true);

  QStringList paths;
  if (this->PathListWidget->isFile(currentPath))
  {
    paths = this->openAddFilesDialog(false);
  }
  else
  {
    paths = this->openAddDirDialog();
  }

  if (!paths.isEmpty())
  {
    if (!this->PathListWidget->editPath(currentPath, paths.front()))
    {
      QMessageBox::information(q, tr("Editing the path failed"),
                               QString(tr("Failed to change path:\n\n%1\n\nto path\n\n%2\n\nPlease check your permissions."))
                               .arg(currentPath).arg(paths.front()));
    }
  }
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidgetPrivate::on_PathListWidget_selectionChanged(const QItemSelection &selected,
                                                                         const QItemSelection &deselected)
{
  Q_UNUSED(selected)
  Q_UNUSED(deselected)

  bool hasSelection = this->PathListWidget->selectionModel()->hasSelection();
  this->EditButton->setEnabled(hasSelection);
  this->RemoveButton->setEnabled(hasSelection);
}

//-----------------------------------------------------------------------------
QStringList ctkPathListButtonsWidgetPrivate::openAddFilesDialog(bool multiple)
{
  Q_Q(ctkPathListButtonsWidget);

  if (!this->PathListWidget) return QStringList();

  QString caption;
  if (multiple)
  {
    caption = tr("Select one or more files");
  }
  else
  {
    caption = tr("Select a file");
  }

  QFileDialog fileDialog(q, caption);
  fileDialog.setOption(QFileDialog::ReadOnly, true);

  if (multiple)
  {
    fileDialog.setFileMode(QFileDialog::ExistingFiles);
  }
  else
  {
    fileDialog.setFileMode(QFileDialog::ExistingFile);
  }

  QString currentPath = this->PathListWidget->currentPath(true);
  currentPath = currentPath.left(currentPath.lastIndexOf('/') + 1);
  if (!currentPath.isEmpty())
  {
    fileDialog.setDirectory(currentPath);
  }

  // We use a proxy model as a workaround for the broken QFileDialog::setFilter() method.
  // See for example https://bugreports.qt-project.org/browse/QTBUG-10244
  class FileFilterProxyModel : public QSortFilterProxyModel
  {
  public:
    FileFilterProxyModel(ctkPathListWidget::PathOptions fileOptions)
      : FileOptions(fileOptions)
    {}

  protected:
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const
    {
      QModelIndex sourceIndex = sourceModel()->index(sourceRow, 0, sourceParent);
      QFileSystemModel* fileModel = qobject_cast<QFileSystemModel*>(sourceModel());

      QFileInfo fileInfo = fileModel->fileInfo(sourceIndex);

      if(fileInfo.isFile())
      {
        if (FileOptions.testFlag(ctkPathListWidget::Readable) &&
            !fileInfo.isReadable())
        {
          return false;
        }
        if (FileOptions.testFlag(ctkPathListWidget::Writable) &&
            !fileInfo.isWritable())
        {
          return false;
        }
        if (FileOptions.testFlag(ctkPathListWidget::Executable)&&
            !fileInfo.isExecutable())
        {
          return false;
        }
        return true;
      }
      else
      {
        // Show all readable directories
        return fileInfo.isReadable();
      }
    }

  private:
    ctkPathListWidget::PathOptions FileOptions;
  };

  fileDialog.setProxyModel(new FileFilterProxyModel(this->PathListWidget->fileOptions()));

  if (fileDialog.exec() == QDialog::Accepted)
  {
    return fileDialog.selectedFiles();
  }
  return QStringList();
}

//-----------------------------------------------------------------------------
QStringList ctkPathListButtonsWidgetPrivate::openAddDirDialog()
{
  Q_Q(ctkPathListButtonsWidget);

  if (!this->PathListWidget) return QStringList();

  QString caption = tr("Select a directory");
  QFileDialog fileDialog(q, caption);

  fileDialog.setFileMode(QFileDialog::Directory);
  fileDialog.setOption(QFileDialog::ShowDirsOnly);

  QString currentPath = this->PathListWidget->currentPath(true);
  if (!currentPath.isEmpty())
  {
    fileDialog.setDirectory(currentPath);
  }

  // We use a proxy model as a workaround for the broken QFileDialog::setFilter() method.
  // See for example https://bugreports.qt-project.org/browse/QTBUG-10244
  class DirFilterProxyModel : public QSortFilterProxyModel
  {
  public:
    DirFilterProxyModel(ctkPathListWidget::PathOptions dirOptions)
      : DirOptions(dirOptions)
    {}

  protected:
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const
    {
      QModelIndex sourceIndex = sourceModel()->index(sourceRow, 0, sourceParent);
      QFileSystemModel* fileModel = qobject_cast<QFileSystemModel*>(sourceModel());

      QFileInfo fileInfo = fileModel->fileInfo(sourceIndex);

      if (DirOptions.testFlag(ctkPathListWidget::Readable) &&
          !fileInfo.isReadable())
      {
        return false;
      }
      // Do not check for the Writable flag, since it makes navigation from
      // non-writable folders to writable sub-folders hard.
//      if (DirOptions.testFlag(ctkPathListWidget::Writable) &&
//          !fileInfo.isWritable())
//      {
//        return false;
//      }
      return true;
    }

  private:
    ctkPathListWidget::PathOptions DirOptions;
  };

  fileDialog.setProxyModel(new DirFilterProxyModel(this->PathListWidget->directoryOptions()));

  if (fileDialog.exec() == QDialog::Accepted)
  {
    return fileDialog.selectedFiles();
  }
  return QStringList();
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidgetPrivate::addPathsWithWarningMessage(const QStringList& paths)
{
  Q_Q(ctkPathListButtonsWidget);

  QStringList addedPaths = this->PathListWidget->addPaths(paths);
  if (addedPaths != paths)
  {
    QString problematicPaths;
    foreach(const QString& path, paths)
    {
      if (!addedPaths.contains(path) && !this->PathListWidget->contains(path))
      {
        problematicPaths += path + '\n';
      }
    }
    if (!problematicPaths.isEmpty())
    {
      QMessageBox::information(q, tr("Adding paths failed"),
                               QString(tr("Failed to add the following paths:\n\n%1\nPlease check your permissions."))
                               .arg(problematicPaths));
    }
  }
}


//-----------------------------------------------------------------------------
// ctkPathListButtonsWidget methods

//-----------------------------------------------------------------------------
ctkPathListButtonsWidget::~ctkPathListButtonsWidget()
{
}

void ctkPathListButtonsWidget::init(ctkPathListWidget *pathListWidget)
{
  Q_D(ctkPathListButtonsWidget);
  d->PathListWidget = pathListWidget;

  if (d->PathListWidget->selectionModel()->selectedIndexes().isEmpty())
  {
    d->RemoveButton->setEnabled(false);
    d->EditButton->setEnabled(false);
  }

  switch(d->PathListWidget->mode())
  {
  case ctkPathListWidget::FilesOnly:
    d->AddDirectoryButton->setVisible(false);
    break;
  case ctkPathListWidget::DirectoriesOnly:
    d->AddFilesButton->setVisible(false);
    break;
  default:
    break;
  }

  connect(d->PathListWidget->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          d, SLOT(on_PathListWidget_selectionChanged(QItemSelection,QItemSelection)));
  connect(d->PathListWidget, SIGNAL(pathActivated(QString)), d, SLOT(on_EditButton_clicked()));
}

//-----------------------------------------------------------------------------
ctkPathListButtonsWidget::ctkPathListButtonsWidget(QWidget* newParent)
  : Superclass(newParent)
  , d_ptr(new ctkPathListButtonsWidgetPrivate(*this))
{
  Q_D(ctkPathListButtonsWidget);
  d->init();
}

//-----------------------------------------------------------------------------
bool ctkPathListButtonsWidget::isAddFilesButtonVisible() const
{
  Q_D(const ctkPathListButtonsWidget);
  return d->AddFilesButton->isVisible();
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidget::setAddFilesButtonVisible(bool visible)
{
  Q_D(ctkPathListButtonsWidget);
  d->AddFilesButton->setVisible(visible);
}

//-----------------------------------------------------------------------------
bool ctkPathListButtonsWidget::isAddDirectoryButtonVisible() const
{
  Q_D(const ctkPathListButtonsWidget);
  return d->AddDirectoryButton->isVisible();
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidget::setAddDirectoryButtonVisible(bool visible)
{
  Q_D(ctkPathListButtonsWidget);
  d->AddDirectoryButton->setVisible(visible);
}

//-----------------------------------------------------------------------------
bool ctkPathListButtonsWidget::isRemoveButtonVisible() const
{
  Q_D(const ctkPathListButtonsWidget);
  return d->RemoveButton->isVisible();
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidget::setRemoveButtonVisible(bool visible)
{
  Q_D(ctkPathListButtonsWidget);
  d->RemoveButton->setVisible(visible);
}

//-----------------------------------------------------------------------------
bool ctkPathListButtonsWidget::isEditButtonVisible() const
{
  Q_D(const ctkPathListButtonsWidget);
  return d->EditButton->isVisible();
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidget::setEditButtonVisible(bool visible)
{
  Q_D(ctkPathListButtonsWidget);
  d->EditButton->setVisible(visible);
}

//-----------------------------------------------------------------------------
QString ctkPathListButtonsWidget::textAddFilesButton() const
{
  Q_D(const ctkPathListButtonsWidget);
  return d->AddFilesButton->text();
}

//-----------------------------------------------------------------------------
QString ctkPathListButtonsWidget::textAddDirectoryButton() const
{
  Q_D(const ctkPathListButtonsWidget);
  return d->AddDirectoryButton->text();
}

//-----------------------------------------------------------------------------
QString ctkPathListButtonsWidget::textRemoveButton() const
{
  Q_D(const ctkPathListButtonsWidget);
  return d->RemoveButton->text();
}

//-----------------------------------------------------------------------------
QString ctkPathListButtonsWidget::textEditButton() const
{
  Q_D(const ctkPathListButtonsWidget);
  return d->EditButton->text();
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidget::setTextAddFilesButton(const QString& text)
{
  Q_D(ctkPathListButtonsWidget);
  d->AddFilesButton->setText(text);
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidget::setTextAddDirectoryButton(const QString& text)
{
  Q_D(ctkPathListButtonsWidget);
  d->AddDirectoryButton->setText(text);
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidget::setTextRemoveButton(const QString& text)
{
  Q_D(ctkPathListButtonsWidget);
  d->RemoveButton->setText(text);
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidget::setTextEditButton(const QString& text)
{
  Q_D(ctkPathListButtonsWidget);
  d->EditButton->setText(text);
}

//-----------------------------------------------------------------------------
QString ctkPathListButtonsWidget::toolTipAddFilesButton() const
{
  Q_D(const ctkPathListButtonsWidget);
  return d->AddFilesButton->toolTip();
}

//-----------------------------------------------------------------------------
QString ctkPathListButtonsWidget::toolTipAddDirectoryButton() const
{
  Q_D(const ctkPathListButtonsWidget);
  return d->AddDirectoryButton->toolTip();
}

//-----------------------------------------------------------------------------
QString ctkPathListButtonsWidget::toolTipRemoveButton() const
{
  Q_D(const ctkPathListButtonsWidget);
  return d->RemoveButton->toolTip();
}

//-----------------------------------------------------------------------------
QString ctkPathListButtonsWidget::toolTipEditButton() const
{
  Q_D(const ctkPathListButtonsWidget);
  return d->EditButton->toolTip();
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidget::setToolTipAddFilesButton(const QString& toolTip)
{
  Q_D(ctkPathListButtonsWidget);
  d->AddFilesButton->setToolTip(toolTip);
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidget::setToolTipAddDirectoryButton(const QString& toolTip)
{
  Q_D(ctkPathListButtonsWidget);
  d->AddDirectoryButton->setToolTip(toolTip);
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidget::setToolTipRemoveButton(const QString& toolTip)
{
  Q_D(ctkPathListButtonsWidget);
  d->RemoveButton->setToolTip(toolTip);
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidget::setToolTipEditButton(const QString& toolTip)
{
  Q_D(ctkPathListButtonsWidget);
  d->EditButton->setToolTip(toolTip);
}

//-----------------------------------------------------------------------------
QIcon ctkPathListButtonsWidget::iconAddFilesButton() const
{
  Q_D(const ctkPathListButtonsWidget);
  return d->AddFilesButton->icon();
}

//-----------------------------------------------------------------------------
QIcon ctkPathListButtonsWidget::iconAddDirectoryButton() const
{
  Q_D(const ctkPathListButtonsWidget);
  return d->AddDirectoryButton->icon();
}

//-----------------------------------------------------------------------------
QIcon ctkPathListButtonsWidget::iconRemoveButton() const
{
  Q_D(const ctkPathListButtonsWidget);
  return d->RemoveButton->icon();
}

//-----------------------------------------------------------------------------
QIcon ctkPathListButtonsWidget::iconEditButton() const
{
  Q_D(const ctkPathListButtonsWidget);
  return d->EditButton->icon();
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidget::setIconAddFilesButton(const QIcon& icon)
{
  Q_D(ctkPathListButtonsWidget);
  d->AddFilesButton->setIcon(icon);
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidget::setIconAddDirectoryButton(const QIcon& icon)
{
  Q_D(ctkPathListButtonsWidget);
  d->AddDirectoryButton->setIcon(icon);
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidget::setIconRemoveButton(const QIcon& icon)
{
  Q_D(ctkPathListButtonsWidget);
  d->RemoveButton->setIcon(icon);
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidget::setIconEditButton(const QIcon& icon)
{
  Q_D(ctkPathListButtonsWidget);
  d->EditButton->setIcon(icon);
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidget::unsetIconAddFilesButton()
{
  Q_D(ctkPathListButtonsWidget);
  d->AddFilesButton->setIcon(QIcon(":/Icons/plus.png"));
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidget::unsetIconAddDirectoryButton()
{
  Q_D(ctkPathListButtonsWidget);
  d->AddDirectoryButton->setIcon(QIcon(":/Icons/plus.png"));
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidget::unsetIconRemoveButton()
{
  Q_D(ctkPathListButtonsWidget);
  d->RemoveButton->setIcon(QIcon(":/Icons/minus.png"));
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidget::unsetIconEditButton()
{
  Q_D(ctkPathListButtonsWidget);
  d->EditButton->setIcon(QIcon(":/Icons/edit.png"));
}

//-----------------------------------------------------------------------------
bool ctkPathListButtonsWidget::isButtonsAutoRaise() const
{
  Q_D(const ctkPathListButtonsWidget);
  return d->AddFilesButton->autoRaise();
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidget::setButtonsAutoRaise(bool autoRaise)
{
  Q_D(ctkPathListButtonsWidget);
  d->AddFilesButton->setAutoRaise(autoRaise);
  d->AddDirectoryButton->setAutoRaise(autoRaise);
  d->RemoveButton->setAutoRaise(autoRaise);
  d->EditButton->setAutoRaise(autoRaise);
}

//-----------------------------------------------------------------------------
int ctkPathListButtonsWidget::buttonSpacing() const
{
  return this->layout()->spacing();
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidget::setButtonSpacing(int spacing)
{
  this->layout()->setSpacing(spacing);
}

//-----------------------------------------------------------------------------
Qt::Orientation ctkPathListButtonsWidget::orientation() const
{
  return qobject_cast<QVBoxLayout*>(this->layout()) ? Qt::Vertical : Qt::Horizontal;
}

//-----------------------------------------------------------------------------
void ctkPathListButtonsWidget::setOrientation(Qt::Orientation orientation)
{
  QVBoxLayout* verticalLayout = qobject_cast<QVBoxLayout*>(this->layout());
  if (verticalLayout && orientation == Qt::Vertical)
  {
    return;
  }

  QLayout* oldLayout = this->layout();
  QLayout* newLayout = NULL;
  if (orientation == Qt::Vertical)
  {
    newLayout = new QVBoxLayout;
  }
  else
  {
    newLayout = new QHBoxLayout;
  }
  newLayout->setContentsMargins(0,0,0,0);
  newLayout->setSpacing(oldLayout->spacing());

  QLayoutItem* item = 0;
  while((item = oldLayout->takeAt(0)))
  {
    if (item->widget())
    {
      newLayout->addWidget(item->widget());
    }
  }
  delete oldLayout;
  this->setLayout(newLayout);
}

//-----------------------------------------------------------------------------
QToolButton *ctkPathListButtonsWidget::buttonAddFiles() const
{
  Q_D(const ctkPathListButtonsWidget);
  return d->AddFilesButton;
}

//-----------------------------------------------------------------------------
QToolButton *ctkPathListButtonsWidget::buttonAddDirectory() const
{
  Q_D(const ctkPathListButtonsWidget);
  return d->AddDirectoryButton;
}

//-----------------------------------------------------------------------------
QToolButton *ctkPathListButtonsWidget::buttonEdit() const
{
  Q_D(const ctkPathListButtonsWidget);
  return d->EditButton;
}

//-----------------------------------------------------------------------------
QToolButton *ctkPathListButtonsWidget::buttonRemove() const
{
  Q_D(const ctkPathListButtonsWidget);
  return d->RemoveButton;
}
