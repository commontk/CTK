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

#ifndef __ctkModelTester_h
#define __ctkModelTester_h

/// Qt includes
#include <QObject> 
#include <QModelIndex>
#include <QPersistentModelIndex>
#include <QList>

/// CTK includes
#include "ctkPimpl.h"
#include "ctkCoreExport.h"

class QAbstractItemModel;
class ctkModelTesterPrivate;

/// \ingroup Core
/// ctkModelTester is a tool that tests any QAbstractItemModel
/// Most of the signals fired by the model set (ctkModelTester::setModel())
/// are connected to the tester that check their consistency with the 
/// model contents.
/// ctkModelTester is typically used when developing a new QAbstractItemModel
/// or during unit tests.
class CTK_CORE_EXPORT ctkModelTester: public QObject
{
  Q_OBJECT
  Q_PROPERTY(bool nestedInserts READ nestedInserts WRITE setNestedInserts);
  Q_PROPERTY(bool testDataEnabled READ testDataEnabled WRITE setTestDataEnabled);
  Q_PROPERTY(bool throwOnError READ throwOnError WRITE setThrowOnError);
  Q_PROPERTY(bool verbose READ verbose WRITE setVerbose);
public:
  ///
  /// Constructor
  /// No model is set by default. To be tested, a model must be set using 
  /// setModel(...)
  explicit ctkModelTester(QObject *parent = 0);

  ///
  /// Constructor that set the model to test.
  /// A new model can later be set using setModel(...)
  /// by default, throwOnError is true,
  ///             nestedInsert is false,
  ///             testDataEnabled is true,
  ///             verbose is true.
  ctkModelTester(QAbstractItemModel *model, QObject *parent = 0);

  ///
  /// Destructor
  virtual ~ctkModelTester();

  ///
  /// Set the model to be tested, the model must remain valid during 
  /// the life ctkModelTester.
  void setModel(QAbstractItemModel* model);
  QAbstractItemModel* model()const;

  ///
  /// Throw an exception when an error is found in the model.
  /// True by default 
  void setThrowOnError(bool throwException);
  bool throwOnError()const;
 
  ///
  /// nestedInserts controls wether the model is allowed to make 
  /// nested row/column insertions ( an insertion signal is fired when an 
  /// insertion a previous insertion was not finished). A row insertion 
  /// consists of 2  signals: rowsAboutToBeInserted and rowsInserted
  /// It also applies for row/column suppressions. 
  void setNestedInserts(bool enable);
  bool nestedInserts()const;

  ///
  /// When TestData is enabled, it checks if the display role of a valid
  /// model index is valid too.
  /// You can disable the test if you are ok with temporary invalid display
  /// roles.
  void setTestDataEnabled(bool enable);
  bool testDataEnabled()const;

  /// When Verbose is enabled, message will be printed to standard or error output.
  void setVerbose(bool enable);
  bool verbose()const;

  ///
  /// Test the data consistency of a QModelIndex.
  /// Note: Only DisplayRole is checked.
  virtual void testData(const QModelIndex& index)const;

  ///
  /// Run all the tests on the model previously set in setModel(...)
  virtual void testModel()const;

  ///
  /// Run a collection of tests on a QModelIndex
  virtual void testModelIndex(const QModelIndex& index)const;

  ///
  /// Check the hierarchy consistency of a QModelIndex 
  /// child/parent/siblings relationships
  virtual void testParent(const QModelIndex& parent)const;

  /// 
  /// Test a persistent model index
  virtual void testPersistentModelIndex(const QPersistentModelIndex& index)const;

protected Q_SLOTS:
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
  /// 
  /// The logic of onColumnsAboutToBeInserted and onRowsAboutToBeInserted is 
  /// gathered in onItemsAboutToBeInserted
  virtual void onItemsAboutToBeInserted(const QModelIndex& parent, Qt::Orientation, int start, int end);

  /// 
  /// The logic of onColumnsAboutToBeRemoved and onRowsAboutToBeRemoved is 
  /// gathered in onItemsAboutToBeRemoved
  virtual void onItemsAboutToBeRemoved(const QModelIndex& parent, Qt::Orientation, int start, int end);

  /// 
  /// The logic of onColumnsInserted and onRowsInserted is gathered in 
  /// onItemsInserted
  virtual void onItemsInserted(const QModelIndex& parent, Qt::Orientation, int start, int end);

  /// 
  /// The logic of onColumnsRemoved and onRowsRemoved is gathered in 
  /// onItemsRemoved
  virtual void onItemsRemoved(const QModelIndex& parent, Qt::Orientation, int start, int end);

  ///
  /// Create a list of persistent index of all the index's children
  QList<QPersistentModelIndex> persistentModelIndexes(const QModelIndex& index)const;

  ///
  /// Utility function that process the result of a test
  virtual void test(bool result, const QString& errorString)const;
  
protected:
  QScopedPointer<ctkModelTesterPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkModelTester);
  Q_DISABLE_COPY(ctkModelTester);
};

#endif
