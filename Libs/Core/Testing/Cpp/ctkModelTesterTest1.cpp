/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QAbstractItemModel>
#include <QCoreApplication>
#include <QList>
#include <QModelIndex>
#include <QStandardItem>
#include <QStandardItemModel>

// CTK includes
#include "ctkModelTester.h"

// STL includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
class QAbstractItemModelHelper : public QAbstractItemModel
{
public:
  virtual QModelIndex index(int, int, const QModelIndex&) const { return QModelIndex(); }
  virtual QModelIndex parent(const QModelIndex&) const { return QModelIndex(); }
  virtual int rowCount(const QModelIndex&) const { return 0; }
  virtual int columnCount(const QModelIndex&) const { return 0; }
  virtual QVariant data(const QModelIndex&, int) const { return QVariant(); }
};

//-----------------------------------------------------------------------------
int ctkModelTesterTest1(int argc, char * argv [] )
{
  QCoreApplication app(argc, argv);

  QAbstractItemModelHelper * item = new QAbstractItemModelHelper;
  QObject * object = new QObject; 

  ctkModelTester ctkTester( item, object );

  delete item;

  try
    {
    // as we can infer that QStandardItemModel is correct,
    // ctkModelTester shall not fail for any of the actions on the model.
    // Please note here that takeRow() doesn't delete the items so we end up
    // with mem leaks.
    QStandardItemModel model;
    ctkModelTester treeModelTester(&model);
    QList<QStandardItem*> items;
    items << new QStandardItem("col1") << new QStandardItem("col2");
    model.appendRow(items);
    QList<QStandardItem*> items2  = model.takeRow(0);
    if (items2 != items)
      {
      std::cerr << "Error" << std::endl;
      return EXIT_FAILURE;
      }
    items2.clear();
    model.appendRow(items);
    for (int i = 0; i < 10; ++i)
      {
      model.appendRow(QList<QStandardItem*>() << new QStandardItem("col1") << new QStandardItem("col2"));
      }
    model.takeRow(0);
    model.takeRow(model.rowCount() / 2 );
    model.takeRow(model.rowCount() - 1);
    items2 << new QStandardItem("col1") << new QStandardItem("col2");
    items2[0]->appendRow(QList<QStandardItem*>() << new QStandardItem("subcol1") << new QStandardItem("subcol2"));
    items2[0]->appendRow(QList<QStandardItem*>() << new QStandardItem("subcol1") << new QStandardItem("subcol2"));
    model.setData(model.index(0,0), QString("foo"));
    model.sort(0);
    }
  catch (const char* error)
    {
    std::cerr << error << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

