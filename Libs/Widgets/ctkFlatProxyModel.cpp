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
// QT includes
#include <QDebug>

// CTK includes
#include "ctkFlatProxyModel.h"

// ----------------------------------------------------------------------------
class ctkFlatProxyModelPrivate
{
  Q_DECLARE_PUBLIC(ctkFlatProxyModel);
protected:
  ctkFlatProxyModel* const q_ptr;
public:
  ctkFlatProxyModelPrivate(ctkFlatProxyModel& object);
  void init();
  int indexLevel(const QModelIndex& index)const;
  int levelRowCount(const QModelIndex& index)const;
  int nextLevelRowCount(const QModelIndex& index)const;
  int rowCount(const QModelIndex& sourceIndex, int depth = 0)const;
  QModelIndex sourceParent(const QModelIndex& index)const;
  QModelIndex grandChild(const QModelIndex& parent, int& row, int depth)const;

  int StartFlattenLevel;
  int EndFlattenLevel;
  int HideLevel;
};

// ----------------------------------------------------------------------------
//  Methods ctkFlatProxyModelPrivate

// ----------------------------------------------------------------------------
ctkFlatProxyModelPrivate::ctkFlatProxyModelPrivate(ctkFlatProxyModel &object)
  : q_ptr(&object)
{
  this->StartFlattenLevel = -1;
  this->EndFlattenLevel = -1;
  this->HideLevel = -1;
}

// ----------------------------------------------------------------------------
void ctkFlatProxyModelPrivate::init()
{
}

// ----------------------------------------------------------------------------
int ctkFlatProxyModelPrivate::indexLevel(const QModelIndex& index)const
{
  int level = -1;
  QModelIndex parent = index;
  while (parent.isValid())
    {
    parent = parent.parent();
    ++level;
    }
  return level;
}

// ----------------------------------------------------------------------------
int ctkFlatProxyModelPrivate::levelRowCount(const QModelIndex& sourceIndex)const
{
  Q_Q(const ctkFlatProxyModel);
  if (!sourceIndex.isValid()
      || this->StartFlattenLevel > this->indexLevel(sourceIndex)
      || this->EndFlattenLevel < this->indexLevel(sourceIndex))
    {
    return 0;
    }
  int previousRows = 0;
  for (int row = 0; row != sourceIndex.row() ; ++row)
    {
    previousRows += q->sourceModel()->rowCount(sourceIndex.sibling(row, sourceIndex.column()));
    }
  return previousRows + this->levelRowCount(sourceIndex.parent());
}

// ----------------------------------------------------------------------------
int ctkFlatProxyModelPrivate::nextLevelRowCount(const QModelIndex& sourceIndex)const
{
  Q_Q(const ctkFlatProxyModel);
  if (!sourceIndex.isValid()
      || this->StartFlattenLevel > this->indexLevel(sourceIndex)
      || this->EndFlattenLevel < this->indexLevel(sourceIndex))
    {
    return q->sourceModel()->rowCount(sourceIndex);
    }
  int rowCount = 0;
  QModelIndex sibling = sourceIndex.sibling(0, sourceIndex.column());
  for (int row = 0; sibling.isValid() ; ++row)
    {
    sibling = sourceIndex.sibling(row, sourceIndex.column());
    rowCount += q->sourceModel()->rowCount(sibling);
    }
  return rowCount;
}

// ----------------------------------------------------------------------------
int ctkFlatProxyModelPrivate::rowCount(const QModelIndex& sourceIndex, int depth)const
{
  Q_Q(const ctkFlatProxyModel);
  int rows = 0;
  if (depth < 0)
    {
    rows = 1;
    }
  else
    {
    --depth;
    for (int row = 0; row < q->sourceModel()->rowCount(sourceIndex); ++row)
      {
      QModelIndex child = q->sourceModel()->index(row, 0, sourceIndex);
      rows += this->rowCount(child, depth);
      }
    }
  return rows;
}

// ----------------------------------------------------------------------------
QModelIndex ctkFlatProxyModelPrivate
::sourceParent(const QModelIndex& index)const
{
  Q_Q(const ctkFlatProxyModel);
  QModelIndexList sourceIndexes;
  sourceIndexes << QModelIndex();
  QMap<int, int> rowCountsPerLevel;
  while (!sourceIndexes.isEmpty())
    {
    QModelIndex sourceIndex = sourceIndexes.takeFirst();
    const int rowCount = q->sourceModel()->rowCount(sourceIndex);
    for (int row = 0; row < rowCount; ++row)
      {
      QModelIndex child = q->sourceModel()->index(row, 0, sourceIndex);
      if (child.internalPointer() == index.internalPointer())
        {
        return sourceIndex;
        }
      else
        {
        sourceIndexes << child;
        }
      }
    }
  Q_ASSERT(false);
  return QModelIndex();
}

// ----------------------------------------------------------------------------
QModelIndex ctkFlatProxyModelPrivate
::grandChild(const QModelIndex& parent, int& row, int depth)const
{
  Q_Q(const ctkFlatProxyModel);
  const int rowCount = q->sourceModel()->rowCount(parent);

  if (depth > 0)
    {
    for (int i = 0; i < rowCount; ++i)
      {
      QModelIndex child = q->sourceModel()->index(i, 0, parent);
      QModelIndex found = this->grandChild(child, row, depth - 1);
      if (found.isValid())
        {
        return found;
        }
      }
    }
  else
    {
    if (row < rowCount)
      {
      QModelIndex sourceIndex = q->sourceModel()->index(
        row, 0, parent);
      return sourceIndex;
      }
    else
      {
      row -= rowCount;
      }
    }
  return QModelIndex();
}

// ----------------------------------------------------------------------------
//  Methods ctkFlatProxyModel

// ----------------------------------------------------------------------------
ctkFlatProxyModel::ctkFlatProxyModel(QObject *parentObject)
  : Superclass(parentObject)
  , d_ptr(new ctkFlatProxyModelPrivate(*this))
{
  Q_D(ctkFlatProxyModel);
  d->init();
}

// ----------------------------------------------------------------------------
ctkFlatProxyModel::~ctkFlatProxyModel()
{
}

// ----------------------------------------------------------------------------
int ctkFlatProxyModel::startFlattenLevel() const
{
  Q_D(const ctkFlatProxyModel);
  return d->StartFlattenLevel;
}

// ----------------------------------------------------------------------------
void ctkFlatProxyModel::setStartFlattenLevel(int level)
{
  Q_D(ctkFlatProxyModel);
  d->StartFlattenLevel = level;
  Q_ASSERT( d->StartFlattenLevel <= d->EndFlattenLevel);
}

// ----------------------------------------------------------------------------
int ctkFlatProxyModel::endFlattenLevel() const
{
  Q_D(const ctkFlatProxyModel);
  return d->EndFlattenLevel;
}

// ----------------------------------------------------------------------------
void ctkFlatProxyModel::setEndFlattenLevel(int level)
{
  Q_D(ctkFlatProxyModel);
  d->EndFlattenLevel = level;
  Q_ASSERT( d->EndFlattenLevel >= d->EndFlattenLevel);
}

// ----------------------------------------------------------------------------
int ctkFlatProxyModel::hideLevel()const
{
  Q_D(const ctkFlatProxyModel);
  return d->HideLevel;
}

// ----------------------------------------------------------------------------
void ctkFlatProxyModel::setHideLevel(int level)
{
  Q_D(ctkFlatProxyModel);
  d->HideLevel = level;
}

// ----------------------------------------------------------------------------
QModelIndex ctkFlatProxyModel::mapFromSource( const QModelIndex& sourceIndex ) const
{
  Q_D(const ctkFlatProxyModel);
  if (!sourceIndex.isValid())
    {
    return QModelIndex();
    }
  int level = d->indexLevel(sourceIndex);
  if (d->HideLevel != -1
      && level >= d->HideLevel)
    {
    return QModelIndex();
    }
  if (d->EndFlattenLevel != -1
      && level <= d->EndFlattenLevel)
    {
    return QModelIndex();
    }
  int row = sourceIndex.row();
  if (d->EndFlattenLevel != -1)
    {
    row += d->levelRowCount(sourceIndex.parent());
    }
  return this->createIndex(row, sourceIndex.column(),
                           sourceIndex.internalPointer());
}

// ----------------------------------------------------------------------------
QModelIndex ctkFlatProxyModel::mapToSource( const QModelIndex& proxyIndex ) const
{
  Q_D(const ctkFlatProxyModel);
  if (!proxyIndex.isValid())
    {
    return QModelIndex();
    }
  QModelIndex sourceParent = d->sourceParent(proxyIndex);
  int level = d->indexLevel(sourceParent);
  int levelRowCount = 0;
  if ((d->StartFlattenLevel != -1 || d->EndFlattenLevel != -1) &&
      (d->StartFlattenLevel != -1 || level >= d->StartFlattenLevel) &&
      (d->EndFlattenLevel != -1 || level <= d->EndFlattenLevel))
    {
    levelRowCount = d->levelRowCount(sourceParent);
    }
  QModelIndex sourceIndex = this->sourceModel()->index(
    proxyIndex.row() - levelRowCount, proxyIndex.column(), sourceParent);
  Q_ASSERT(sourceIndex.isValid());
  return sourceIndex;
}

// ----------------------------------------------------------------------------
QModelIndex ctkFlatProxyModel::index(int row, int column, const QModelIndex &parent) const
{
  Q_D(const ctkFlatProxyModel);
  if (row < 0 || column < 0)
    {
    return QModelIndex();
    }

  QModelIndex sourceParent = this->mapToSource(parent); // parent is already mapped at this point
  int sourceRow = row;
  QModelIndex sourceGrandChild = d->grandChild(
    sourceParent, sourceRow, qMax(0, d->EndFlattenLevel - d->indexLevel(sourceParent)));
  return this->createIndex(row, column, sourceGrandChild.internalPointer());
}

// ----------------------------------------------------------------------------
QModelIndex ctkFlatProxyModel::parent(const QModelIndex &child) const
{
  if (!child.isValid())
    {
    return QModelIndex();
    }
  QModelIndex sourceChild = this->mapToSource(child);
  QModelIndex sourceParent = sourceChild.parent();
  QModelIndex proxyParent = this->mapFromSource(sourceParent);
  return proxyParent;
}

// ----------------------------------------------------------------------------
int ctkFlatProxyModel::rowCount(const QModelIndex &parent) const
{
  Q_D(const ctkFlatProxyModel);
  QModelIndex sourceParent = this->mapToSource(parent);
  int sourceParentLevel = d->indexLevel(sourceParent);
  int depth = 0;
  if (sourceParentLevel >= d->StartFlattenLevel &&
      sourceParentLevel <= d->EndFlattenLevel)
    {
    depth = d->EndFlattenLevel - d->StartFlattenLevel;
    }
  return d->rowCount(sourceParent, depth);
}

// ----------------------------------------------------------------------------
int ctkFlatProxyModel::columnCount(const QModelIndex &parent) const
{
  QModelIndex proxyChild = this->index(0, 0, parent);
  if (parent.isValid() && !proxyChild.internalPointer())
    {
    Q_ASSERT(!parent.isValid() || proxyChild.internalPointer());
    }
  QModelIndex sourceChild = this->mapToSource(proxyChild);
  QModelIndex sourceParent = sourceChild.parent();
  return this->sourceModel()->columnCount(sourceParent);
}
