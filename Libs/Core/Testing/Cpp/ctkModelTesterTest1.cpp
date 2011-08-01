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

    //------ Test on row--------
    QList<QStandardItem*> items;
    items << new QStandardItem("col1") << new QStandardItem("col2");
    model.appendRow(items);
    QList<QStandardItem*> items2  = model.takeRow(0);
    if (items2 != items)
      {
      std::cerr << "Line : " << __LINE__ << "Error" << std::endl;
      return EXIT_FAILURE;
      }
    model.appendRow(items);
    for (int i = 0; i < 10; ++i)
      {
      model.appendRow(QList<QStandardItem*>() << new QStandardItem("col1") << new QStandardItem("col2"));
      }
    model.takeRow(0);
    model.setHeaderData(0, Qt::Vertical, QString("ID"));
    model.takeRow(model.rowCount() / 2 );
    model.takeRow(model.rowCount() - 1);
    items2.clear();
    items2 << new QStandardItem("col1") << new QStandardItem("col2");
    items2[0]->appendRow(QList<QStandardItem*>() << new QStandardItem("subcol1") << new QStandardItem("subcol2"));
    items2[0]->appendRow(QList<QStandardItem*>() << new QStandardItem("subcol1") << new QStandardItem("subcol2"));

    model.appendRow(items2);
    items2[0]->child(0,0)->setText("foo");
    model.sort(0);

    //------ Test on Column-----
    QStandardItemModel model2;
    ctkModelTester treeModelTester2(&model2);

    QList<QStandardItem*> itemsCol;
    itemsCol << new QStandardItem("row1") << new QStandardItem("row2");
    model2.appendColumn(itemsCol);
    QList<QStandardItem*> itemsCol2  = model2.takeColumn(0);
    if (itemsCol2 != itemsCol)
      {
      std::cerr << "Line : " << __LINE__ << "Error" << std::endl;
      return EXIT_FAILURE;
      }
    model2.appendColumn(itemsCol);
    for (int i = 0; i < 10; ++i)
      {
      model2.appendColumn(QList<QStandardItem*>() << new QStandardItem("row1") << new QStandardItem("row2"));
      }
    model2.takeColumn(0);
    model2.takeColumn(model2.columnCount() / 2 );
    model2.takeColumn(model2.columnCount() - 1);
    itemsCol2 << new QStandardItem("row1") << new QStandardItem("row2");
    itemsCol2[0]->appendColumn(QList<QStandardItem*>() << new QStandardItem("subrow1") << new QStandardItem("subrow2"));
    itemsCol2[0]->appendColumn(QList<QStandardItem*>() << new QStandardItem("subrow1") << new QStandardItem("subrow2"));

    model2.setData(model2.index(0,0), QString("foo"));
    model2.sort(0);
    model2.clear();

    //------ Test setDataHeader ----------
    for (int i = 0; i < 10; ++i)
      {
      QList<QStandardItem*> columns;
      columns << new QStandardItem("row1") << new QStandardItem("row2");
      model2.appendColumn(columns);
      model2.setHorizontalHeaderItem(i,new QStandardItem(QString("Column %1").arg(i)));
      }
    model2.setHeaderData(0, Qt::Horizontal, QString("ID"));
    }
  catch (const char* error)
    {
    std::cerr << error << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

