/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QAbstractItemModel>
#include <QCoreApplication>
#include <QStandardItemModel>
#include <QModelIndex>

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
  void emitInvalidHeaderDataChanged()
  { emit this->headerDataChanged(Qt::Vertical, 10, 10);}
};

//-----------------------------------------------------------------------------
int ctkModelTesterTest2(int argc, char * argv [] )
{
  QCoreApplication app(argc, argv);

  QObject * object = new QObject; 

  ctkModelTester ctkTester( object );

  //---------------------------------------------------
  if (    ctkTester.model() != 0
      ||  ctkTester.throwOnError() != true
      ||  ctkTester.nestedInserts() != false
      ||  ctkTester.testDataEnabled() != true
      ||  ctkTester.verbose() != true)
    {
    std::cerr << "Line : " << __LINE__
              << " - Error in ctkModelTester::ctkModelTester" << std::endl;
    return EXIT_FAILURE;
    }

  ctkTester.setThrowOnError(false);
  ctkTester.setNestedInserts(true);
  ctkTester.setTestDataEnabled(false);
  ctkTester.setVerbose(false);

  if (    ctkTester.throwOnError() != false
      ||  ctkTester.nestedInserts() != true
      ||  ctkTester.testDataEnabled() != false
      ||  ctkTester.verbose() != false)
    {
    std::cerr << "Line : " << __LINE__
              << " - Error " << std::endl;
    return EXIT_FAILURE;
    }

  QModelIndex defaultModelIndex;
  ctkTester.testModelIndex(defaultModelIndex);

  ctkTester.setThrowOnError(true);
  ctkTester.setNestedInserts(false);
  ctkTester.setTestDataEnabled(true);
  ctkTester.setVerbose(true);

  ctkTester.testModel();

  QAbstractItemModelHelper helper;
  ctkTester.setModel(&helper);
  bool errorThrown = false;
  try
    {
    helper.emitInvalidHeaderDataChanged();
    }
  catch (...)
    {
    errorThrown = true;
    }
  if (!errorThrown)
    {
    std::cerr << "ThrowOnError failed" << std::endl;
    return EXIT_FAILURE;
    }

  QStandardItemModel model;
  ctkTester.setModel(&model);
  ctkTester.setModel(0);
  ctkTester.setModel(&model);

  ctkTester.testModelIndex(defaultModelIndex);


  return EXIT_SUCCESS;
}

