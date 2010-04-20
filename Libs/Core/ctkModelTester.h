/*=========================================================================

  Library:   ctk

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __ctkModelTester_h
#define __ctkModelTester_h

/// Qt includes
#include <QObject> 
#include <QModelIndex>
#include <QPersistentModelIndex>
#include <QList>

/// CTK includes
#include "ctkPimpl.h"
#include "CTKCoreExport.h"

class QAbstractItemModel;
class ctkModelTesterPrivate;

class CTK_CORE_EXPORT ctkModelTester: public QObject
{
  Q_OBJECT
  Q_PROPERTY(bool nestedInserts READ nestedInserts WRITE setNestedInserts);
public:
  explicit ctkModelTester(QObject *parent = 0);
  ctkModelTester(QAbstractItemModel *model, QObject *parent = 0);

  void setModel(QAbstractItemModel* model);
  QAbstractItemModel* model()const;

  void setThrowOnError(bool throwException);
  bool throwOnError()const;
 
  void setNestedInserts(bool enable);
  bool nestedInserts()const;

  virtual void testData(const QModelIndex& index)const;
  virtual void testModel()const;
  virtual void testModelIndex(const QModelIndex& index)const;
  virtual void testParent(const QModelIndex& parent)const;
  virtual void testPersistentModelIndex(const QPersistentModelIndex& index)const;

protected slots:
  void onColumnsAboutToBeInserted(const QModelIndex & parent, int start, int end);
  void onColumnsAboutToBeRemoved(const QModelIndex & parent, int start, int end);
  void onColumnsInserted(const QModelIndex & parent, int start, int end);
  void onColumnsRemoved(const QModelIndex & parent, int start, int end);
  void onDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight);
  void onHeaderDataChanged(Qt::Orientation orientation, int first, int last);
  void onLayoutAboutToBeChanged();
  void onLayoutChanged();
  void onModelAboutToBeReset();
  void onModelReset();
  void onRowsAboutToBeInserted(const QModelIndex &parent, int start, int end);
  void onRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
  void onRowsInserted(const QModelIndex & parent, int start, int end);
  void onRowsRemoved(const QModelIndex & parent, int start, int end);
  
protected:
  virtual void onItemsAboutToBeInserted(const QModelIndex& parent, Qt::Orientation, int start, int end);
  virtual void onItemsAboutToBeRemoved(const QModelIndex& parent, Qt::Orientation, int start, int end);
  virtual void onItemsInserted(const QModelIndex& parent, Qt::Orientation, int start, int end);
  virtual void onItemsRemoved(const QModelIndex& parent, Qt::Orientation, int start, int end);
  QList<QPersistentModelIndex> persistentModelIndexes(const QModelIndex& index)const;
  virtual void test(bool result, const QString& errorString)const;
  
private:
  CTK_DECLARE_PRIVATE(ctkModelTester);
};

#endif
