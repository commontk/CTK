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

// QtGUI includes
#include "ctkDirectoryListView.h"

// --------------------------------------------------------------------------
// ctkDirectoryListViewPrivate

//-----------------------------------------------------------------------------
class ctkDirectoryListViewPrivate
{
  Q_DECLARE_PUBLIC(ctkDirectoryListView);
protected:
  ctkDirectoryListView* const q_ptr;

public:
  ctkDirectoryListViewPrivate(ctkDirectoryListView& object);
  void init();

  void addDirectory(const QString& path);

  enum
    {
    AbsolutePathRole = Qt::UserRole + 1
    };

  QListView*         ListView;
  QStandardItemModel DirectoryListModel;
};

// --------------------------------------------------------------------------
// ctkDirectoryListViewPrivate methods

// --------------------------------------------------------------------------
ctkDirectoryListViewPrivate::ctkDirectoryListViewPrivate(ctkDirectoryListView& object)
  :q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void ctkDirectoryListViewPrivate::init()
{
  Q_Q(ctkDirectoryListView);

  this->ListView = new QListView();
  this->ListView->setSelectionBehavior(QAbstractItemView::SelectRows);
  this->ListView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  this->ListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  QHBoxLayout * layout = new QHBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(this->ListView);
  q->setLayout(layout);

  this->ListView->setModel(&this->DirectoryListModel);
}

// --------------------------------------------------------------------------
void ctkDirectoryListViewPrivate::addDirectory(const QString& path)
{
  Q_Q(ctkDirectoryListView);
  QString absolutePath = QFileInfo(path).absoluteFilePath();
  if (!QFile::exists(absolutePath) || q->hasDirectory(absolutePath))
    {
    return;
    }
  QStandardItem * item = new QStandardItem(path);
  item->setData(QVariant(absolutePath), Qt::ToolTipRole);
  item->setData(QVariant(absolutePath), ctkDirectoryListViewPrivate::AbsolutePathRole);
  this->DirectoryListModel.appendRow(item);
}

// --------------------------------------------------------------------------
// ctkDirectoryListView methods

// --------------------------------------------------------------------------
ctkDirectoryListView::ctkDirectoryListView(QWidget* _parent)
  : Superclass(_parent)
  , d_ptr(new ctkDirectoryListViewPrivate(*this))
{
  Q_D(ctkDirectoryListView);
  d->init();
}

// --------------------------------------------------------------------------
ctkDirectoryListView::~ctkDirectoryListView()
{
}

// --------------------------------------------------------------------------
QStringList ctkDirectoryListView::directoryList(bool absolutePath)const
{
  Q_D(const ctkDirectoryListView);
  QStringList directoryList;
  int role = Qt::DisplayRole;
  if (absolutePath)
    {
    role = ctkDirectoryListViewPrivate::AbsolutePathRole;
    }
  for(int i = 0; i < d->DirectoryListModel.rowCount(); ++i)
    {
    directoryList << d->DirectoryListModel.data(d->DirectoryListModel.index(i, 0), role).toString();
    }
  return directoryList;
}

// --------------------------------------------------------------------------
QStringList ctkDirectoryListView::selectedDirectoryList(bool absolutePath)const
{
  Q_D(const ctkDirectoryListView);
  QStringList directoryList;
  int role = Qt::DisplayRole;
  if (absolutePath)
    {
    role = ctkDirectoryListViewPrivate::AbsolutePathRole;
    }
  QModelIndexList selectedIndexes = d->ListView->selectionModel()->selectedRows();
  foreach(const QModelIndex& index, selectedIndexes)
    {
    directoryList << d->DirectoryListModel.data(index, role).toString();
    }
  return directoryList;
}

// --------------------------------------------------------------------------
bool ctkDirectoryListView::hasDirectory(const QString& path)const
{
  Q_D(const ctkDirectoryListView);
  QString absolutePath = QFileInfo(path).absoluteFilePath();
  QModelIndexList foundIndexes = d->DirectoryListModel.match(
        d->DirectoryListModel.index(0, 0), ctkDirectoryListViewPrivate::AbsolutePathRole,
        QVariant(absolutePath));
  Q_ASSERT(foundIndexes.size() < 2);
  return (foundIndexes.size() != 0);
}

// --------------------------------------------------------------------------
void ctkDirectoryListView::addDirectory(const QString& path)
{
  Q_D(ctkDirectoryListView);
  d->addDirectory(path);
  emit this->directoryListChanged();
}

// --------------------------------------------------------------------------
void ctkDirectoryListView::removeDirectory(const QString& path)
{
  Q_D(ctkDirectoryListView);
  QList<QStandardItem*> foundItems = d->DirectoryListModel.findItems(path);
  Q_ASSERT(foundItems.count() < 2);
  if (foundItems.count() == 1)
    {
    d->DirectoryListModel.removeRow(foundItems.at(0)->row());
    emit this->directoryListChanged();
    }
}

// --------------------------------------------------------------------------
void ctkDirectoryListView::removeSelectedDirectories()
{
  Q_D(ctkDirectoryListView);

  QModelIndexList selectedIndexes = d->ListView->selectionModel()->selectedRows();
  bool selectedCount = selectedIndexes.count();
  while(selectedIndexes.count() > 0)
    {
    d->DirectoryListModel.removeRow(selectedIndexes.at(0).row());
    selectedIndexes = d->ListView->selectionModel()->selectedRows();
    }
  if (selectedCount)
    {
    emit this->directoryListChanged();
    }
}

// --------------------------------------------------------------------------
void ctkDirectoryListView::selectAllDirectories()
{
  Q_D(ctkDirectoryListView);
  d->ListView->selectAll();
}

// --------------------------------------------------------------------------
void ctkDirectoryListView::clearDirectorySelection()
{
  Q_D(ctkDirectoryListView);
  d->ListView->clearSelection();
}

// --------------------------------------------------------------------------
void ctkDirectoryListView::setDirectoryList(const QStringList& paths)
{
  Q_D(ctkDirectoryListView);

  if (paths.count() == this->directoryList().count())
    {
    int found = 0;
    foreach(const QString& path, paths)
      {
      if (this->hasDirectory(path))
        {
        ++found;
        }
      }
    if (found == paths.count())
      {
      return;
      }
    }

  d->DirectoryListModel.removeRows(0, d->DirectoryListModel.rowCount());

  foreach(const QString& path, paths)
    {
    d->addDirectory(path);
    }
  emit this->directoryListChanged();
}

