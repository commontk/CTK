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

#ifndef __ctkFlatProxyModel_h
#define __ctkFlatProxyModel_h

// QT includes
#include <QAbstractProxyModel>

// CTK includes
#include "ctkWidgetsExport.h"

class ctkFlatProxyModelPrivate;

/// \ingroup Widgets
///
/// ctkFlatProxyModel intends to flatten contiguous hierarchies within a model.
/// For now, it only supports the toplevel hierarchy flatten with the 1 degree
/// children.
/// The items in the levels being flatten don't appear in the model anymore, 
/// however their children will be visible.
class CTK_WIDGETS_EXPORT ctkFlatProxyModel : public QAbstractProxyModel
{
  Q_OBJECT
  /// level for which to start flattening the rows. -1 by default
  /// Not supported yet.
  Q_PROPERTY(int startFlattenLevel READ startFlattenLevel WRITE setStartFlattenLevel)
  /// level for which to stop flattening the rows. -1 by default
  Q_PROPERTY(int endFlattenLevel READ endFlattenLevel WRITE setEndFlattenLevel)
  /// level for which to stop flattening the rows. -1 by default
  /// Not supported yet.
  Q_PROPERTY(int hideLevel READ hideLevel WRITE setHideLevel)

public:
  typedef QAbstractProxyModel Superclass;

  ctkFlatProxyModel(QObject *parentObject = 0);
  virtual ~ctkFlatProxyModel();

  void setStartFlattenLevel(int level);
  int startFlattenLevel() const;

  void setEndFlattenLevel(int level);
  int endFlattenLevel() const;

  void setHideLevel(int level);
  int hideLevel() const;

  virtual QModelIndex mapFromSource( const QModelIndex& sourceIndex ) const;
  virtual QModelIndex mapToSource( const QModelIndex& sourceIndex ) const;

  virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
  virtual QModelIndex parent(const QModelIndex &child) const;
  virtual int rowCount(const QModelIndex &parent) const;
  virtual int columnCount(const QModelIndex &parent) const;

protected:
  QScopedPointer<ctkFlatProxyModelPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkFlatProxyModel);
  Q_DISABLE_COPY(ctkFlatProxyModel);
};

#endif
