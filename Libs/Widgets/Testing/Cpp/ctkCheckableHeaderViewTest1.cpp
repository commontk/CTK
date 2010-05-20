/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

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
#include <QDebug>
#include <QApplication>
#include <QTableView>
#include <QFileSystemModel>
#include <QStandardItem>
#include <QStandardItemModel>

// CTK includes
#include "ctkCheckableHeaderView.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkCheckableHeaderViewTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

#if 0
    QFileSystemModel model;
    model.setRootPath(QDir::currentPath());
#else
    QStandardItemModel model;
    QStandardItem item;
    item.insertColumns(0, 5);
    item.insertRows(0, 7);
    model.insertRow(0, &item);
#endif

  QTableView table;
  table.setModel(&model);


  model.setHeaderData(0, Qt::Horizontal, Qt::Checked, Qt::CheckStateRole);
  QHeaderView* previousHeaderView = table.horizontalHeader();
  ctkCheckableHeaderView* headerView = new ctkCheckableHeaderView(Qt::Horizontal, &table);
  qDebug() << previousHeaderView->isClickable();
  headerView->setClickable(previousHeaderView->isClickable());
  headerView->setMovable(previousHeaderView->isMovable());
  headerView->setHighlightSections(previousHeaderView->highlightSections());
  headerView->setPropagateToItems(true);

  table.setHorizontalHeader(headerView);
  qDebug() << headerView->isCheckable(0);

  table.show();
  table.raise();

  //app.exec();

  return EXIT_SUCCESS;
}
