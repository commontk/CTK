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
#include <QDebug>
#include <QApplication>
#include <QFocusEvent>
#include <QTableView>
#include <QFileSystemModel>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTimer>

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
    QList<QStandardItem*> row0;
    row0 << new QStandardItem << new QStandardItem << new QStandardItem;
    row0[0]->setText("not user checkable");
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
#endif

  QTableView table;
  table.setModel(&model);

  model.setHeaderData(0, Qt::Horizontal, Qt::Checked, Qt::CheckStateRole);

  QHeaderView* previousHeaderView = table.horizontalHeader();
  bool oldClickable = previousHeaderView->isClickable();

  ctkCheckableHeaderView* headerView = new ctkCheckableHeaderView(Qt::Horizontal, &table);  
  headerView->setClickable(oldClickable);
  headerView->setMovable(previousHeaderView->isMovable());
  headerView->setHighlightSections(previousHeaderView->highlightSections());
  headerView->setPropagateToItems(true);

  // sets the model to the headerview
  table.setHorizontalHeader(headerView);
  
  if (headerView->isClickable() != oldClickable)
    {
    std::cerr << "ctkCheckableHeaderView::setClickable() failed: "
              << headerView->isClickable() << std::endl;
    return EXIT_FAILURE;
    }
  if (headerView->checkState(0) == Qt::Unchecked)
    {
    std::cerr << "ctkCheckableHeaderView::checkstate() failed: "
              << headerView->checkState(0) << std::endl;
    return EXIT_FAILURE;
    }
  Qt::CheckState checkstate;
  if (!headerView->checkState(0, checkstate))
    {
    std::cerr << "ctkCheckableHeaderView::checkstate() failed: "
              << headerView->checkState(0, checkstate) << std::endl;
    return EXIT_FAILURE;
    }
  QFocusEvent focus(QEvent::FocusIn,Qt::TabFocusReason);
  headerView->eventFilter(headerView, &focus);
  
  if (!headerView->propagateToItems())
    {
    std::cerr << "ctkCheckableHeaderView::propagateToItems() failed: "
              << headerView->propagateToItems() << std::endl;
    return EXIT_FAILURE;
    }
  headerView->setPropagateToItems(false);
  if (headerView->propagateToItems())
    {
    std::cerr << "ctkCheckableHeaderView::propagateToItems() failed: "
              << headerView->propagateToItems() << std::endl;
    return EXIT_FAILURE;
    }

  // uncheck the header
  headerView->toggleCheckState(0);
  
  if (headerView->checkState(0) != Qt::Unchecked)
    {
    std::cerr << "ctkCheckableHeaderView::toggleCheckState() failed: "
              << headerView->checkState(0) << std::endl;
    return EXIT_FAILURE;
    }
    
  // make sure it didn't uncheck the checkable items
  if (row0[0]->checkState() != Qt::Checked ||
      row1[0]->checkState() != Qt::Checked ||
      row2[0]->checkState() != Qt::Checked)
    {
    std::cerr << "ctkCheckableHeaderView::toggleCheckState() failed: "
              << row0[0]->checkState() << " "
              << row1[0]->checkState() << " "
              << row2[0]->checkState() << std::endl;
    return EXIT_FAILURE;
    }

  row0[0]->setCheckState(Qt::Unchecked);
  // make sure it didn't uncheck the checkable items
  if (headerView->checkState(0) != Qt::Unchecked ||
      row0[0]->checkState() != Qt::Unchecked ||
      row1[0]->checkState() != Qt::Checked ||
      row2[0]->checkState() != Qt::Checked)
    {
    std::cerr << "QStandardItem::setCheckState() failed: "
              << headerView->checkState(0) << " "
              << row0[0]->checkState() << " "
              << row1[0]->checkState() << " "
              << row2[0]->checkState() << std::endl;
    return EXIT_FAILURE;
    }

  headerView->setPropagateToItems(true);

  if (!headerView->propagateToItems() ||
      headerView->checkState(0) != Qt::PartiallyChecked ||
      row0[0]->checkState() != Qt::Unchecked ||
      row1[0]->checkState() != Qt::Checked ||
      row2[0]->checkState() != Qt::Checked)
    {
    std::cerr << "ctkCheckableHeaderView::setPropagateToItems() failed: "
              << headerView->checkState(0) << " "
              << row0[0]->checkState() << " "
              << row1[0]->checkState() << " "
              << row2[0]->checkState() << std::endl;
    return EXIT_FAILURE;
    }

  row0[0]->setCheckState(Qt::Checked);
  
  if (headerView->checkState(0) != Qt::Checked ||
      row0[0]->checkState() != Qt::Checked ||
      row1[0]->checkState() != Qt::Checked ||
      row2[0]->checkState() != Qt::Checked)
    {
    std::cerr << "QStandardItem::setCheckState() failed: "
              << headerView->checkState(0) << " "
              << row0[0]->checkState() << " "
              << row1[0]->checkState() << " "
              << row2[0]->checkState() << std::endl;
    return EXIT_FAILURE;
    }

  row1[0]->setCheckState(Qt::Unchecked);
  
    // make sure it didn't uncheck the checkable items
  if (headerView->checkState(0) != Qt::PartiallyChecked ||
      row0[0]->checkState() != Qt::Checked ||
      row1[0]->checkState() != Qt::Unchecked ||
      row2[0]->checkState() != Qt::Checked)
    {
    std::cerr << "QStandardItem::setCheckState() failed: "
              << headerView->checkState(0) << " "
              << row0[0]->checkState() << " "
              << row1[0]->checkState() << " "
              << row2[0]->checkState() << std::endl;
    return EXIT_FAILURE;
    }

  row1[0]->setCheckState(Qt::Unchecked);
  
  // make sure it didn't uncheck the checkable items
  if (headerView->checkState(0) != Qt::PartiallyChecked ||
      row0[0]->checkState() != Qt::Checked ||
      row1[0]->checkState() != Qt::Unchecked ||
      row2[0]->checkState() != Qt::Checked)
    {
    std::cerr << "QStandardItem::setCheckState() failed: "
              << headerView->checkState(0) << " "
              << row0[0]->checkState() << " "
              << row1[0]->checkState() << " "
              << row2[0]->checkState() << std::endl;
    return EXIT_FAILURE;
    }

  row0[0]->setCheckState(Qt::Unchecked);
  row2[0]->setCheckState(Qt::Unchecked);

  // make sure the header is now unchecked
  if (headerView->checkState(0) != Qt::Unchecked ||
      row0[0]->checkState() != Qt::Unchecked ||
      row1[0]->checkState() != Qt::Unchecked ||
      row2[0]->checkState() != Qt::Unchecked)
    {
    std::cerr << "QStandardItem::setCheckState() failed: "
              << headerView->checkState(0) << " "
              << row0[0]->checkState() << " "
              << row1[0]->checkState() << " "
              << row2[0]->checkState() << std::endl;
    return EXIT_FAILURE;
    }

  headerView->setCheckState(0, Qt::Checked);
    
  if (headerView->checkState(0) != Qt::Checked ||
      row0[0]->checkState() != Qt::Checked ||
      row1[0]->checkState() != Qt::Checked ||
      row2[0]->checkState() != Qt::Checked)
    {
    std::cerr << "ctkCheckableHeaderView::setCheckState() failed: "
              << headerView->checkState(0) << " "
              << row0[0]->checkState() << " "
              << row1[0]->checkState() << " "
              << row2[0]->checkState() << std::endl;
    return EXIT_FAILURE;
    }

  table.show();
  //table.raise();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(500, &app, SLOT(quit()));
    }
  
  return app.exec();
}
