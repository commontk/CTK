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

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

=========================================================================*/

// Qt includes
#include <QFileInfo>
#include <QHBoxLayout>
#include <QListView>
#include <QStandardItemModel>
#include <QApplication>

// QtGUI includes
#include "ctkPathListWidget.h"

// --------------------------------------------------------------------------
// ctkPathListWidgetPrivate

//-----------------------------------------------------------------------------
class ctkPathListWidgetPrivate
{
  Q_DECLARE_PUBLIC(ctkPathListWidget)

protected:
  ctkPathListWidget* const q_ptr;

public:

  enum PathType {
    Unknown,
    File,
    Directory
  };

  ctkPathListWidgetPrivate(ctkPathListWidget& object);

  void _q_emitPathClicked(const QModelIndex &index);
  void _q_emitPathDoubleClicked(const QModelIndex &index);
  void _q_emitPathActivated(const QModelIndex &index);
  void _q_emitCurrentPathChanged(const QModelIndex &current, const QModelIndex &previous);

  bool addPath(const QString& path);
  bool removePath(const QString& path);

  void fileOptionsChanged();
  void directoryOptionsChanged();

  PathType pathType(const QString& absolutePath) const;

  bool isValidPath(const QString& absoluteFilePath, PathType pathType) const;
  bool isValidFile(const QString& absoluteFilePath) const;
  bool isValidDir(const QString& absoluteDirPath) const;

  QStandardItemModel PathListModel;
  ctkPathListWidget::Mode Mode;
  ctkPathListWidget::PathOptions FileOptions;
  ctkPathListWidget::PathOptions DirectoryOptions;
  QIcon FileIcon;
  QIcon DirectoryIcon;
};

// --------------------------------------------------------------------------
// ctkPathListWidgetPrivate methods

#include "moc_ctkPathListWidget.cpp"

// --------------------------------------------------------------------------
ctkPathListWidgetPrivate::ctkPathListWidgetPrivate(ctkPathListWidget& object)
  : q_ptr(&object)
  , Mode(ctkPathListWidget::Any)
  , FileOptions(ctkPathListWidget::Exists | ctkPathListWidget::Readable)
  , DirectoryOptions(ctkPathListWidget::Exists | ctkPathListWidget::Readable)
{
}

// --------------------------------------------------------------------------
void ctkPathListWidgetPrivate::_q_emitPathClicked(const QModelIndex &index)
{
  Q_Q(ctkPathListWidget);
  emit q->pathClicked(this->PathListModel.data(index, ctkPathListWidget::AbsolutePathRole).toString());
}

// --------------------------------------------------------------------------
void ctkPathListWidgetPrivate::_q_emitPathDoubleClicked(const QModelIndex &index)
{
  Q_Q(ctkPathListWidget);
  emit q->pathDoubleClicked(this->PathListModel.data(index, ctkPathListWidget::AbsolutePathRole).toString());
}

// --------------------------------------------------------------------------
void ctkPathListWidgetPrivate::_q_emitPathActivated(const QModelIndex &index)
{
  Q_Q(ctkPathListWidget);
  emit q->pathActivated(this->PathListModel.data(index, ctkPathListWidget::AbsolutePathRole).toString());
}

// --------------------------------------------------------------------------
void ctkPathListWidgetPrivate::_q_emitCurrentPathChanged(const QModelIndex &current, const QModelIndex &previous)
{
  Q_Q(ctkPathListWidget);
  QString currentPath = this->PathListModel.data(current, ctkPathListWidget::AbsolutePathRole).toString();
  QString previousPath = this->PathListModel.data(previous, ctkPathListWidget::AbsolutePathRole).toString();
  emit q->currentPathChanged(currentPath, previousPath);
}

// --------------------------------------------------------------------------
bool ctkPathListWidgetPrivate::addPath(const QString& path)
{
  Q_Q(ctkPathListWidget);
  QString absolutePath = QFileInfo(path).absoluteFilePath();
  if (q->contains(absolutePath))
  {
    return false;
  }

  PathType pathType = this->pathType(absolutePath);
  if (!this->isValidPath(absolutePath, pathType))
  {
    return false;
  }
  QStandardItem * item = new QStandardItem(path);
  item->setData(QVariant(absolutePath), Qt::ToolTipRole);
  item->setData(QVariant(absolutePath), ctkPathListWidget::AbsolutePathRole);
  if (pathType == File && !this->FileIcon.isNull())
  {
    item->setData(this->FileIcon, Qt::DecorationRole);
  }
  else if (pathType == Directory && !this->DirectoryIcon.isNull())
  {
    item->setData(this->DirectoryIcon, Qt::DecorationRole);
  }
  this->PathListModel.appendRow(item);
  return true;
}

// --------------------------------------------------------------------------
bool ctkPathListWidgetPrivate::removePath(const QString& path)
{
  QString absolutePath = QFileInfo(path).absoluteFilePath();
  QModelIndexList foundIndices = this->PathListModel.match(this->PathListModel.index(0, 0),
                                                           ctkPathListWidget::AbsolutePathRole,
                                                           absolutePath);
  Q_ASSERT(foundIndices.count() < 2);
  if (!foundIndices.isEmpty())
  {
    this->PathListModel.removeRow(foundIndices.front().row());
    return true;
  }
  return false;
}

// --------------------------------------------------------------------------
void ctkPathListWidgetPrivate::fileOptionsChanged()
{
  QStringList removedPaths;
  for(int i = 0; i < this->PathListModel.rowCount();)
  {
    QModelIndex index = this->PathListModel.index(i, 0);
    QString filePath = this->PathListModel.data(index, ctkPathListWidget::AbsolutePathRole).toString();
    if (!this->isValidFile(filePath))
    {
      this->PathListModel.removeRow(i);
      removedPaths << filePath;
    }
    else
    {
      ++i;
    }
  }

  if (!removedPaths.empty())
  {
    Q_Q(ctkPathListWidget);
    emit q->pathsChanged(QStringList(), removedPaths);
  }
}

// --------------------------------------------------------------------------
void ctkPathListWidgetPrivate::directoryOptionsChanged()
{
  QStringList removedPaths;
  for(int i = 0; i < this->PathListModel.rowCount();)
  {
    QModelIndex index = this->PathListModel.index(i, 0);
    QString dirPath = this->PathListModel.data(index, ctkPathListWidget::AbsolutePathRole).toString();
    if (!this->isValidDir(dirPath))
    {
      this->PathListModel.removeRow(i);
      removedPaths << dirPath;
    }
    else
    {
      ++i;
    }
  }

  if (!removedPaths.empty())
  {
    Q_Q(ctkPathListWidget);
    emit q->pathsChanged(QStringList(), removedPaths);
  }
}

// --------------------------------------------------------------------------
ctkPathListWidgetPrivate::PathType ctkPathListWidgetPrivate::pathType(const QString& absolutePath) const
{
  QFileInfo fileInfo(absolutePath);
  if (fileInfo.exists())
  {
    if (fileInfo.isFile())
    {
      return File;
    }
    else if (fileInfo.isDir())
    {
      return Directory;
    }
    return Unknown;
  }
  // Check if path is a file or directory by looking for a trailing slash
  else if (absolutePath.endsWith('/'))
  {
    return Directory;
  }
  else
  {
    return File;
  }
}

// --------------------------------------------------------------------------
bool ctkPathListWidgetPrivate::isValidPath(const QString& absoluteFilePath, PathType pathType) const
{
  switch (pathType)
  {
  case Unknown:
    if (this->Mode == ctkPathListWidget::Any)
    {
      return true;
    }
    return false;
  case File:
    return this->isValidFile(absoluteFilePath);
  case Directory:
    return this->isValidDir(absoluteFilePath);
  default:
    return false;
  }
}

// --------------------------------------------------------------------------
bool ctkPathListWidgetPrivate::isValidFile(const QString& absoluteFilePath) const
{
  if (this->Mode == ctkPathListWidget::DirectoriesOnly)
  {
    return false;
  }

  if (this->FileOptions.testFlag(ctkPathListWidget::None))
  {
    return true;
  }
  else
  {
    QFileInfo fileInfo(absoluteFilePath);
    if (fileInfo.exists())
    {
      if (!fileInfo.isFile())
      {
        return false;
      }
      if (this->FileOptions.testFlag(ctkPathListWidget::Readable) &&
          !fileInfo.isReadable())
      {
        return false;
      }
      if (this->FileOptions.testFlag(ctkPathListWidget::Writable) &&
          !fileInfo.isWritable())
      {
        return false;
      }
      if (this->FileOptions.testFlag(ctkPathListWidget::Executable) &&
          !fileInfo.isExecutable())
      {
        return false;
      }
      return true;
    }
    else
    {
      return !this->FileOptions.testFlag(ctkPathListWidget::Exists);
    }
  }
}

// --------------------------------------------------------------------------
bool ctkPathListWidgetPrivate::isValidDir(const QString& absoluteDirPath) const
{
  if (this->Mode == ctkPathListWidget::FilesOnly)
  {
    return false;
  }

  if (this->DirectoryOptions.testFlag(ctkPathListWidget::None))
  {
    return true;
  }
  else
  {
    QFileInfo fileInfo(absoluteDirPath);
    if (fileInfo.exists())
    {
      if (!fileInfo.isDir())
      {
        return false;
      }
      if (this->DirectoryOptions.testFlag(ctkPathListWidget::Readable) &&
          !fileInfo.isReadable())
      {
        return false;
      }
      if (this->DirectoryOptions.testFlag(ctkPathListWidget::Writable) &&
          !fileInfo.isWritable())
      {
        return false;
      }
      if (this->DirectoryOptions.testFlag(ctkPathListWidget::Executable) &&
          !fileInfo.isExecutable())
      {
        return false;
      }
      return true;
    }
    else
    {
      return !this->FileOptions.testFlag(ctkPathListWidget::Exists);
    }
  }
}

// --------------------------------------------------------------------------
// ctkPathListWidget methods

// --------------------------------------------------------------------------
ctkPathListWidget::ctkPathListWidget(QWidget* _parent)
  : Superclass(_parent)
  , d_ptr(new ctkPathListWidgetPrivate(*this))
{
  Q_D(ctkPathListWidget);

  this->setSelectionBehavior(QAbstractItemView::SelectRows);
  this->setSelectionMode(QAbstractItemView::ExtendedSelection);
  this->setEditTriggers(QAbstractItemView::NoEditTriggers);

  this->unsetFileIcon();
  this->unsetDirectoryIcon();

  QListView::setModel(&d->PathListModel);

  // signals
  this->connect(this, SIGNAL(clicked(QModelIndex)), SLOT(_q_emitPathClicked(QModelIndex)));
  this->connect(this, SIGNAL(doubleClicked(QModelIndex)), SLOT(_q_emitPathDoubleClicked(QModelIndex)));
  this->connect(this, SIGNAL(activated(QModelIndex)), SLOT(_q_emitPathActivated(QModelIndex)));
  this->connect(this->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                SLOT(_q_emitCurrentPathChanged(QModelIndex,QModelIndex)));
}

// --------------------------------------------------------------------------
ctkPathListWidget::~ctkPathListWidget()
{
}

// --------------------------------------------------------------------------
ctkPathListWidget::Mode ctkPathListWidget::mode() const
{
  Q_D(const ctkPathListWidget);
  return d->Mode;
}

// --------------------------------------------------------------------------
QIcon ctkPathListWidget::fileIcon() const
{
  Q_D(const ctkPathListWidget);
  return d->FileIcon;
}

// --------------------------------------------------------------------------
void ctkPathListWidget::setFileIcon(const QIcon& icon)
{
  Q_D(ctkPathListWidget);
  d->FileIcon = icon;
}

// --------------------------------------------------------------------------
void ctkPathListWidget::unsetFileIcon()
{
  Q_D(ctkPathListWidget);
  d->FileIcon = QApplication::style()->standardIcon(QStyle::SP_FileIcon);
}

// --------------------------------------------------------------------------
QIcon ctkPathListWidget::directoryIcon() const
{
  Q_D(const ctkPathListWidget);
  return d->DirectoryIcon;
}

// --------------------------------------------------------------------------
void ctkPathListWidget::setDirectoryIcon(const QIcon& icon)
{
  Q_D(ctkPathListWidget);
  d->DirectoryIcon = icon;
}

// --------------------------------------------------------------------------
void ctkPathListWidget::unsetDirectoryIcon()
{
  Q_D(ctkPathListWidget);
  d->DirectoryIcon = QApplication::style()->standardIcon(QStyle::SP_DirIcon);
}

// --------------------------------------------------------------------------
ctkPathListWidget::PathOptions ctkPathListWidget::fileOptions() const
{
  Q_D(const ctkPathListWidget);
  return d->FileOptions;
}

// --------------------------------------------------------------------------
void ctkPathListWidget::setFileOptions(PathOptions fileOptions)
{
  Q_D(ctkPathListWidget);
  if (d->FileOptions != fileOptions)
  {
    d->FileOptions = fileOptions;
    d->fileOptionsChanged();
  }
}

// --------------------------------------------------------------------------
ctkPathListWidget::PathOptions ctkPathListWidget::directoryOptions() const
{
  Q_D(const ctkPathListWidget);
  return d->DirectoryOptions;
}

// --------------------------------------------------------------------------
void ctkPathListWidget::setDirectoryOptions(PathOptions directoryOptions)
{
  Q_D(ctkPathListWidget);
  if (d->DirectoryOptions != directoryOptions)
  {
    d->DirectoryOptions = directoryOptions;
    d->directoryOptionsChanged();
  }
}

// --------------------------------------------------------------------------
QStringList ctkPathListWidget::files(bool absolutePath) const
{
  Q_D(const ctkPathListWidget);
  QStringList fileList;
  int role = Qt::DisplayRole;
  if (absolutePath)
  {
    role = ctkPathListWidget::AbsolutePathRole;
  }
  for(int i = 0; i < d->PathListModel.rowCount(); ++i)
  {
    QString filePath = d->PathListModel.data(d->PathListModel.index(i, 0), role).toString();
    if (d->pathType(filePath) == ctkPathListWidgetPrivate::File)
    {
      fileList << filePath;
    }
  }
  return fileList;
}

// --------------------------------------------------------------------------
QStringList ctkPathListWidget::directories(bool absolutePath) const
{
  Q_D(const ctkPathListWidget);
  QStringList pathList;
  int role = Qt::DisplayRole;
  if (absolutePath)
  {
    role = ctkPathListWidget::AbsolutePathRole;
  }
  for(int i = 0; i < d->PathListModel.rowCount(); ++i)
  {
    QString dirPath = d->PathListModel.data(d->PathListModel.index(i, 0), role).toString();
    if (d->pathType(dirPath) == ctkPathListWidgetPrivate::Directory)
    {
      pathList << dirPath;
    }
  }
  return pathList;
}

// --------------------------------------------------------------------------
QStringList ctkPathListWidget::paths(bool absolutePath)const
{
  Q_D(const ctkPathListWidget);
  QStringList pathList;
  int role = Qt::DisplayRole;
  if (absolutePath)
    {
    role = ctkPathListWidget::AbsolutePathRole;
    }
  for(int i = 0; i < d->PathListModel.rowCount(); ++i)
    {
    pathList << d->PathListModel.data(d->PathListModel.index(i, 0), role).toString();
    }
  return pathList;
}

// --------------------------------------------------------------------------
QStringList ctkPathListWidget::selectedPaths(bool absolutePath)const
{
  Q_D(const ctkPathListWidget);
  QStringList pathList;
  int role = Qt::DisplayRole;
  if (absolutePath)
    {
    role = ctkPathListWidget::AbsolutePathRole;
    }
  QModelIndexList selectedIndexes = this->selectionModel()->selectedRows();
  foreach(const QModelIndex& index, selectedIndexes)
    {
    pathList << d->PathListModel.data(index, role).toString();
    }
  return pathList;
}

// --------------------------------------------------------------------------
QString ctkPathListWidget::currentPath(bool absolutePath) const
{
  Q_D(const ctkPathListWidget);

  QModelIndex currentIndex = this->currentIndex();
  if (!currentIndex.isValid())
  {
    return QString();
  }

  int role = absolutePath ? static_cast<int>(AbsolutePathRole)
                          : static_cast<int>(Qt::DisplayRole);
  return d->PathListModel.data(currentIndex, role).toString();
}

// --------------------------------------------------------------------------
int ctkPathListWidget::count() const
{
  return this->model()->rowCount();
}

// --------------------------------------------------------------------------
QString ctkPathListWidget::path(int row) const
{
  Q_D(const ctkPathListWidget);
  if (row < 0 || row >= count())
  {
    return QString();
  }
  return d->PathListModel.data(d->PathListModel.index(row, 0), AbsolutePathRole).toString();
}

// --------------------------------------------------------------------------
QStandardItem* ctkPathListWidget::item(int row) const
{
  Q_D(const ctkPathListWidget);
  return d->PathListModel.item(row);
}

// --------------------------------------------------------------------------
QStandardItem *ctkPathListWidget::item(const QString &absolutePath) const
{
  Q_D(const ctkPathListWidget);
  QModelIndexList result = d->PathListModel.match(d->PathListModel.index(0,0), AbsolutePathRole,
                                                  absolutePath, 1, Qt::MatchExactly);
  Q_ASSERT(result.count() < 2);
  if (result.isEmpty())
  {
    return NULL;
  }
  else
  {
    return d->PathListModel.item(result.front().row());
  }
}

// --------------------------------------------------------------------------
QString ctkPathListWidget::pathAt(const QPoint& point) const
{
  Q_D(const ctkPathListWidget);
  return d->PathListModel.data(indexAt(point), AbsolutePathRole).toString();
}

// --------------------------------------------------------------------------
QStandardItem* ctkPathListWidget::itemAt(const QPoint &point) const
{
  Q_D(const ctkPathListWidget);
  QModelIndex index = this->indexAt(point);
  if (index.isValid())
  {
    return d->PathListModel.item(index.row());
  }
  return NULL;
}

// --------------------------------------------------------------------------
int ctkPathListWidget::row(const QString& path) const
{
  Q_D(const ctkPathListWidget);
  QModelIndexList result = d->PathListModel.match(d->PathListModel.index(0,0), AbsolutePathRole,
                                                  QFileInfo(path).absoluteFilePath(), 1, Qt::MatchExactly);
  Q_ASSERT(result.count() < 2);
  if (!result.isEmpty())
  {
    return result.front().row();
  }
  return -1;
}

// --------------------------------------------------------------------------
bool ctkPathListWidget::editPath(const QString &oldPath, const QString &newPath)
{
  Q_D(ctkPathListWidget);

  QString oldAbsolutePath = QFileInfo(oldPath).absoluteFilePath();
  QModelIndexList matched = d->PathListModel.match(d->PathListModel.index(0,0), AbsolutePathRole,
                                                   oldAbsolutePath, 1, Qt::MatchExactly);
  Q_ASSERT(matched.size() < 2);
  if (matched.isEmpty())
  {
    return false;
  }
  return this->editPath(matched.front(), newPath);
}

// --------------------------------------------------------------------------
bool ctkPathListWidget::editPath(const QModelIndex &index, const QString &newPath)
{
  Q_D(ctkPathListWidget);

  if (!index.isValid())
  {
    return false;
  }

  QString oldAbsolutePath = d->PathListModel.data(index, AbsolutePathRole).toString();
  ctkPathListWidgetPrivate::PathType oldPathType = d->pathType(oldAbsolutePath);
  ctkPathListWidgetPrivate::PathType newPathType = d->pathType(newPath);
  if (oldPathType != newPathType)
  {
    return false;
  }

  if (!d->isValidPath(newPath, newPathType))
  {
    return false;
  }

  QString newAbsolutePath = QFileInfo(newPath).absoluteFilePath();
  d->PathListModel.setData(index, newPath, Qt::DisplayRole);
  d->PathListModel.setData(index, newAbsolutePath, AbsolutePathRole);

  emit this->pathsChanged(QStringList(newAbsolutePath), QStringList(oldAbsolutePath));
  return true;
}

// --------------------------------------------------------------------------
bool ctkPathListWidget::isFile(const QString &path) const
{
  Q_D(const ctkPathListWidget);
  return d->pathType(path) == ctkPathListWidgetPrivate::File;
}

// --------------------------------------------------------------------------
bool ctkPathListWidget::isDirectory(const QString &path) const
{
  Q_D(const ctkPathListWidget);
  return d->pathType(path) == ctkPathListWidgetPrivate::Directory;
}

// --------------------------------------------------------------------------
void ctkPathListWidget::setMode(ctkPathListWidget::Mode mode)
{
  Q_D(ctkPathListWidget);
  d->Mode = mode;
}

// --------------------------------------------------------------------------
bool ctkPathListWidget::contains(const QString& path)const
{
  Q_D(const ctkPathListWidget);
  QString absolutePath = QFileInfo(path).absoluteFilePath();
  QModelIndexList foundIndexes = d->PathListModel.match(
        d->PathListModel.index(0, 0), ctkPathListWidget::AbsolutePathRole,
        QVariant(absolutePath), 1, Qt::MatchExactly);
  Q_ASSERT(foundIndexes.size() < 2);
  return (foundIndexes.size() != 0);
}

// --------------------------------------------------------------------------
bool ctkPathListWidget::addPath(const QString& path)
{
  return !this->addPaths(QStringList() << path).empty();
}

// --------------------------------------------------------------------------
QStringList ctkPathListWidget::addPaths(const QStringList &paths)
{
  Q_D(ctkPathListWidget);

  QStringList addedPaths;
  foreach(const QString& path, paths)
  {
    if (d->addPath(path))
    {
      addedPaths << QFileInfo(path).absoluteFilePath();
    }
  }

  if (!addedPaths.empty())
  {
    emit this->pathsChanged(addedPaths, QStringList());
  }
  return addedPaths;
}

// --------------------------------------------------------------------------
bool ctkPathListWidget::removePath(const QString& path)
{
  return !this->removePaths(QStringList() << path).empty();
}

// --------------------------------------------------------------------------
QStringList ctkPathListWidget::removePaths(const QStringList &paths)
{
  Q_D(ctkPathListWidget);

  QStringList removedPaths;
  foreach(const QString& path, paths)
  {
    if (d->removePath(path))
    {
      removedPaths << QFileInfo(path).absoluteFilePath();
    }
  }

  if (!removedPaths.empty())
  {
    emit this->pathsChanged(QStringList(), removedPaths);
  }
  return removedPaths;
}

// --------------------------------------------------------------------------
void ctkPathListWidget::removeSelectedPaths()
{
  Q_D(ctkPathListWidget);

  QModelIndexList selectedIndexes = this->selectionModel()->selectedRows();
  if (selectedIndexes.empty()) return;

  QStringList removedPaths;
  while(selectedIndexes.count() > 0)
  {
    removedPaths << d->PathListModel.data(selectedIndexes.front(), AbsolutePathRole).toString();
    d->PathListModel.removeRow(selectedIndexes.front().row());
    selectedIndexes = this->selectionModel()->selectedRows();
  }

  emit this->pathsChanged(QStringList(), removedPaths);
}

// --------------------------------------------------------------------------
void ctkPathListWidget::clear()
{
  Q_D(ctkPathListWidget);
  if (d->PathListModel.rowCount() == 0) return;

  QStringList removedPaths = this->paths(true);
  d->PathListModel.clear();
  emit this->pathsChanged(QStringList(), removedPaths);
}

// --------------------------------------------------------------------------
void ctkPathListWidget::setPaths(const QStringList& paths)
{
  Q_D(ctkPathListWidget);

  QStringList addedPaths;
  QStringList removedPaths;

  QStringList absolutePaths;
  foreach(const QString& path, paths)
  {
    absolutePaths << QFileInfo(path).absoluteFilePath();
  }

  foreach(const QString& path, this->paths(true))
  {
    if (!absolutePaths.contains(path) && d->removePath(path))
    {
      removedPaths << path;
    }
  }

  for(int i = 0; i < paths.count(); ++i)
  {
    if (!this->contains(paths[i]) && d->addPath(paths[i]))
    {
      addedPaths << absolutePaths[i];
    }
  }

  if (addedPaths.isEmpty() && removedPaths.empty())
  {
    return;
  }

  emit this->pathsChanged(addedPaths, removedPaths);
}

// --------------------------------------------------------------------------
void ctkPathListWidget::setModel(QAbstractItemModel*)
{
  Q_ASSERT(!"ctkPathListWidget::setModel() - Changing the model of the ctkPathListWidget is not allowed.");
}
