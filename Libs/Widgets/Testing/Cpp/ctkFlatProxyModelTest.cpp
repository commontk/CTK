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
#include <QDebug>
#include <QApplication>
#include <QEventLoop>
#include <QTreeView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTimer>

// CTK includes
#include "ctkFlatProxyModel.h"
#include "ctkModelTester.h"
#include "ctkTest.h"

#if QT_VERSION < 0x040700
Q_DECLARE_METATYPE(QVariant)
#endif

// ----------------------------------------------------------------------------
class ctkFlatProxyModelTester: public QObject
{
  Q_OBJECT
private slots:
  void testModel();
  void testModel_data();
private:
  QStandardItem* createItem(const QString& name, QVariant& children)const;
};

// ----------------------------------------------------------------------------
void ctkFlatProxyModelTester::testModel_data()
{
  QTest::addColumn<QVariant >("hashModel");
  QTest::addColumn<int>("startHideLevel");
  QTest::addColumn<int>("endFlattenLevel");
  QTest::addColumn<int>("level0ExpectedRowCount");
  QTest::addColumn<int>("level1ExpectedRowCount");

  // -\ top
  //  +--\ item1
  //  |  +--\ subItem1
  //  |  |  +--\ subSubItem1
  //  |  |  |  +-- leaf1
  //  |  |  +--\ subSubItem2
  //  |  |  |  +-- leaf2
  //  |  |  +--\ subSubItem3
  //  |  |     +-- leaf3
  //  |  +--\ subItem2
  //  |  |  +--\ subSubItem1
  //  |  |  |  +-- leaf1
  //  |  |  +--\ subSubItem2
  //  |  |  |  +-- leaf2
  //  |  |  +--\ subSubItem3
  //  |  |     +-- leaf3
  //  |  +--\ subItem3
  //  |     +--\ subSubItem1
  //  |     |  +-- leaf1
  //  |     +--\ subSubItem2
  //  |     |  +-- leaf2
  //  |     +--\ subSubItem3
  //  |        +-- leaf3
  //  +--\ item2
  //  |  +--\ subItem1
  //  |  |  +--\ subSubItem1
  //  |  |  |  +-- leaf1
  //  |  |  +--\ subSubItem2
  //  |  |  |  +-- leaf2
  //  |  |  +--\ subSubItem3
  //  |  |     +-- leaf3
  //  |  +--\ subItem2
  //  |  |  +--\ subSubItem1
  //  |  |  |  +-- leaf1
  //  |  |  +--\ subSubItem2
  //  |  |  |  +-- leaf2
  //  |  |  +--\ subSubItem3
  //  |  |     +-- leaf3
  //  |  +--\ subItem3
  //  |     +--\ subSubItem1
  //  |     |  +-- leaf1
  //  |     +--\ subSubItem2
  //  |     |  +-- leaf2
  //  |     +--\ subSubItem3
  //  |        +-- leaf3
  //  +--\ item3
  //     +--\ subItem1
  //     |  +--\ subSubItem1
  //     |  |  +-- leaf1
  //     |  +--\ subSubItem2
  //     |  |  +-- leaf2
  //     |  +--\ subSubItem3
  //     |     +-- leaf3
  //     +--\ subItem2
  //     |  +--\ subSubItem1
  //     |  |  +-- leaf1
  //     |  +--\ subSubItem2
  //     |  |  +-- leaf2
  //     |  +--\ subSubItem3
  //     |     +-- leaf3
  //     +--\ subItem3
  //        +--\ subSubItem1
  //        |  +-- leaf1
  //        +--\ subSubItem2
  //        |  +-- leaf2
  //        +--\ subSubItem3
  //           +-- leaf3
  QMap<QString, QVariant> subSubModel;
  subSubModel["subSubItem1"] = QString("leaf1");
  subSubModel["subSubItem2"] = QString("leaf2");
  QMap<QString, QVariant> subModel;
  subModel["subItem1"] = subSubModel;
  subModel["subItem2"] = subSubModel;
  subModel["subItem3"] = subSubModel;
  QMap<QString, QVariant> model;
  model["item1"] = subModel;
  model["item2"] = subModel;
  model["item3"] = subModel;
  model["item4"] = subModel;

  // -\ top
  //  +--\ subItem1 (item1)
  //  |  +--\ subSubItem1
  //  |  |  +-- leaf1
  //  |  +--\ subSubItem2
  //  |  |  +-- leaf2
  //  |  +--\ subSubItem3
  //  |     +-- leaf3
  //  +--\ subItem2 (item1)
  //  |  +--\ subSubItem1
  //  |  |  +-- leaf1
  //  |  +--\ subSubItem2
  //  |  |  +-- leaf2
  //  |  +--\ subSubItem3
  //  |     +-- leaf3
  //  +--\ subItem3 (item1)
  //  |  +--\ subSubItem1
  //  |  |  +-- leaf1
  //  |  +--\ subSubItem2
  //  |  |  +-- leaf2
  //  |  +--\ subSubItem3
  //  |     +-- leaf3
  //  +--\ subItem1 (item2)
  //  |  +--\ subSubItem1
  //  |  |  +-- leaf1
  //  |  +--\ subSubItem2
  //  |  |  +-- leaf2
  //  |  +--\ subSubItem3
  //  |     +-- leaf3
  //  +--\ subItem2 (item2)
  //  |  +--\ subSubItem1
  //  |  |  +-- leaf1
  //  |  +--\ subSubItem2
  //  |  |  +-- leaf2
  //  |  +--\ subSubItem3
  //  |     +-- leaf3
  //  +--\ subItem3 (item2)
  //  |  +--\ subSubItem1
  //  |  |  +-- leaf1
  //  |  +--\ subSubItem2
  //  |  |  +-- leaf2
  //  |  +--\ subSubItem3
  //  |     +-- leaf3
  //  +--\ subItem1 (item3)
  //  |  +--\ subSubItem1
  //  |  |  +-- leaf1
  //  |  +--\ subSubItem2
  //  |  |  +-- leaf2
  //  |  +--\ subSubItem3
  //  |     +-- leaf3
  //  +--\ subItem2 (item3)
  //  |  +--\ subSubItem1
  //  |  |  +-- leaf1
  //  |  +--\ subSubItem2
  //  |  |  +-- leaf2
  //  |  +--\ subSubItem3
  //  |     +-- leaf3
  //  +--\ subItem3 (item3)
  //     +--\ subSubItem1
  //     |  +-- leaf1
  //     +--\ subSubItem2
  //     |  +-- leaf2
  //     +--\ subSubItem3
  //        +-- leaf3
  QTest::newRow("flatten level 0") << QVariant(model) << -1 << 0 << 12 << 2;
  // Don't work yet.
  //QTest::newRow("flatten level 1") << QVariant(model) << -1<< 1 <<  24 << 1;
  //QTest::newRow("flatten level 0, hide 2") << QVariant(model) << 0 << 0 << 12 << 2;
  //QTest::newRow("flatten level 1, hide 2") << QVariant(model) << 0 << 1 << 12 << 2;
}

// ----------------------------------------------------------------------------
QStandardItem* ctkFlatProxyModelTester
::createItem(const QString& name, QVariant& children)const
{
  QStandardItem* item = new QStandardItem(name);
  if (children.canConvert<QString>())
    {
    QStandardItem* leaf = new QStandardItem(children.toString());
    item->appendRow(leaf);
    return item;
    }
  QMap<QString, QVariant> hash = children.toMap();
  QMap<QString, QVariant>::iterator i = hash.begin();
  for ( ; i != hash.end(); ++i)
    {
    QStandardItem* childItem = this->createItem(i.key(), i.value());
    item->appendRow(childItem);
    }
  return item;
}

// ----------------------------------------------------------------------------
void ctkFlatProxyModelTester::testModel()
{
  QStandardItemModel model;

  QFETCH(QVariant, hashModel);
  QFETCH(int, endFlattenLevel);
  QFETCH(int, startHideLevel);
  QFETCH(int, level0ExpectedRowCount);
  QFETCH(int, level1ExpectedRowCount);

  QMap<QString, QVariant> hash = hashModel.toMap();
  QMap<QString, QVariant>::iterator i = hash.begin();
  for ( ; i != hash.end(); ++i)
    {
    QStandardItem* childItem = this->createItem(i.key(), i.value());
    model.appendRow(childItem);
    }

  ctkFlatProxyModel flattenModel;
  flattenModel.setEndFlattenLevel(endFlattenLevel);
  flattenModel.setHideLevel(startHideLevel);
  flattenModel.setSourceModel(&model);

  QCOMPARE( flattenModel.rowCount(QModelIndex()), level0ExpectedRowCount);
  QModelIndex subIndex1 = flattenModel.index(0,0, QModelIndex());
  QCOMPARE( flattenModel.rowCount(subIndex1), level1ExpectedRowCount);

  ctkModelTester tester;
  tester.setTestDataEnabled(false);
  tester.setModel(&flattenModel);

  /*
  QTreeView view(0);
  view.setModel(&flattenModel);
  view.show();

  QEventLoop eventLoop;
  eventLoop.exec();
  */
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkFlatProxyModelTest)
#include "moc_ctkFlatProxyModelTest.cpp"
