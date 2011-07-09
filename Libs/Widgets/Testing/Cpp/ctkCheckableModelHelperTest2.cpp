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
#include <QFocusEvent>
#include <QTreeView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTimer>

// CTK includes
#include "ctkCheckableModelHelper.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkCheckableModelHelperTest2(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QStringList headers;
  headers << "Title 1" << "Title 2" << "Title 3";
  QStandardItemModel model;
  model.setHorizontalHeaderLabels(headers);
  QList<QStandardItem*> row0;
  row0 << new QStandardItem << new QStandardItem << new QStandardItem;
  row0[0]->setText("forced checkability");
  model.appendRow(row0);
  QList<QStandardItem*> row1;
  row1 << new QStandardItem << new QStandardItem << new QStandardItem;
  row1[0]->setCheckable(true);
  row1[0]->setText("checkable");
  model.appendRow(row1);
  QList<QStandardItem*> row2;
  row2 << new QStandardItem << new QStandardItem << new QStandardItem;
  row2[0]->setCheckable(true);
  row2[0]->setText("checkable");
  model.appendRow(row2);

  QTreeView view;
  view.setModel(&model);

  model.setHeaderData(0, Qt::Horizontal, Qt::Checked, Qt::CheckStateRole);

  ctkCheckableModelHelper headerView(Qt::Horizontal);
  headerView.setPropagateDepth(-1);
  headerView.setForceCheckability(true);
  headerView.setDefaultCheckState(Qt::Checked);
  headerView.setModel(&model);

  
  QList<QStandardItem*> subRow2;
  subRow2 << new QStandardItem << new QStandardItem << new QStandardItem;
//  subRow2[0]->setCheckable(true);
  subRow2[0]->setText("checkable");
  row2[0]->insertRow(0, subRow2);
  
  headers << "4";
  model.setHorizontalHeaderLabels(headers);
  view.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(500, &app, SLOT(quit()));
    }
  
  return app.exec();
}
