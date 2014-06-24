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
#include <QTableView>
#include <QFileSystemModel>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTimer>

// CTK includes
#include "ctkCheckableHeaderView.h"
#include <ctkCheckableModelHelper.h>

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkCheckableHeaderViewTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

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

  // items are unchecked by default
  if (row0[0]->checkState() != Qt::Unchecked ||
      row1[0]->checkState() != Qt::Unchecked ||
      row2[0]->checkState() != Qt::Unchecked)
    {
    std::cerr << "QStandardItem default failed: "
        << static_cast<int>(row0[0]->checkState()) << " "
              << static_cast<int>(row1[0]->checkState()) << " "
              << static_cast<int>(row2[0]->checkState()) << std::endl;
    return EXIT_FAILURE;
    }

  QTableView table;
  table.setModel(&model);

  // Header is checked by default
  model.setHeaderData(0, Qt::Horizontal, Qt::Checked, Qt::CheckStateRole);

  QHeaderView* previousHeaderView = table.horizontalHeader();
#if (QT_VERSION >= 0x50000)
  bool oldClickable = previousHeaderView->sectionsClickable();
#else
  bool oldClickable = previousHeaderView->isClickable();
#endif

  ctkCheckableHeaderView* headerView =
    new ctkCheckableHeaderView(Qt::Horizontal, &table);
#if (QT_VERSION >= 0x50000)
  headerView->setSectionsClickable(oldClickable);
  headerView->setSectionsMovable(previousHeaderView->sectionsMovable());
#else
  headerView->setClickable(oldClickable);
  headerView->setMovable(previousHeaderView->isMovable());
#endif
  headerView->setHighlightSections(previousHeaderView->highlightSections());
  // propagatetoitems is true by default
  //headerView->setPropagateToItems(true);

  // sets the model to the headerview
  table.setHorizontalHeader(headerView);

#if (QT_VERSION >= 0x50000)
  if (headerView->sectionsClickable() != oldClickable)
    {
    std::cerr << "ctkCheckableHeaderView::setSectionClickable() failed: "
              << headerView->sectionsClickable() << std::endl;
    return EXIT_FAILURE;
    }
#else
  if (headerView->isClickable() != oldClickable)
    {
    std::cerr << "ctkCheckableHeaderView::setClickable() failed: "
              << headerView->isClickable() << std::endl;
    return EXIT_FAILURE;
    }
#endif
  // As propagateToItems is true, once the model is set to the headerview,
  // the checkable header is updated from the check state of all the items
  // all the items are unchecked by default, so the header becomes unchecked
  if (headerView->checkState(0) != Qt::Unchecked ||
      row0[0]->checkState() != Qt::Unchecked ||
      row1[0]->checkState() != Qt::Unchecked ||
      row2[0]->checkState() != Qt::Unchecked)
    {
    std::cerr << "ctkCheckableHeaderView::checkstate() failed: "
              << static_cast<int>(headerView->checkState(0)) << " "
        << static_cast<int>(row0[0]->checkState()) << " "
              << static_cast<int>(row1[0]->checkState()) << " "
              << static_cast<int>(row2[0]->checkState()) << std::endl;
    return EXIT_FAILURE;
    }
  // Retrieve checkstate of the header
  Qt::CheckState checkstate;
  if (!headerView->checkState(0, checkstate))
    {
    std::cerr << "ctkCheckableHeaderView::checkstate() failed: "
              << static_cast<int>(checkstate) << std::endl;
    return EXIT_FAILURE;
    }

  QFocusEvent focus(QEvent::FocusIn,Qt::TabFocusReason);
  headerView->eventFilter(headerView, &focus);

  if (headerView->checkableModelHelper()->propagateDepth() == 0)
    {
    std::cerr << "ctkCheckableHeaderView::propagateDepth() failed: "
              << headerView->checkableModelHelper()->propagateDepth() << std::endl;
    return EXIT_FAILURE;
    }
  headerView->checkableModelHelper()->setPropagateDepth(0);
  if (headerView->checkableModelHelper()->propagateDepth() != 0)
    {
    std::cerr << "ctkCheckableHeaderView::propagateDepth() failed: "
              << headerView->checkableModelHelper()->propagateDepth() << std::endl;
    return EXIT_FAILURE;
    }
  if (headerView->checkState(0) != Qt::Unchecked ||
      row0[0]->checkState() != Qt::Unchecked ||
      row1[0]->checkState() != Qt::Unchecked ||
      row2[0]->checkState() != Qt::Unchecked)
    {
    std::cerr << "ctkCheckableHeaderView::propagateToItems() failed: "
              << static_cast<int>(headerView->checkState(0)) << " "
        << static_cast<int>(row0[0]->checkState()) << " "
              << static_cast<int>(row1[0]->checkState()) << " "
              << static_cast<int>(row2[0]->checkState()) << std::endl;
    return EXIT_FAILURE;
    }

  // check the header
  headerView->checkableModelHelper()->toggleHeaderCheckState(0);

  // make sure it didn't uncheck the checkable items
  if (headerView->checkState(0) != Qt::Checked ||
      row0[0]->checkState() != Qt::Unchecked ||
      row1[0]->checkState() != Qt::Unchecked ||
      row2[0]->checkState() != Qt::Unchecked)
    {
    std::cerr << __LINE__ << " ctkCheckableHeaderView::toggleCheckState() failed: "
              << static_cast<int>(headerView->checkState(0)) << " "
        << static_cast<int>(row0[0]->checkState()) << " "
              << static_cast<int>(row1[0]->checkState()) << " "
              << static_cast<int>(row2[0]->checkState()) << std::endl;
    return EXIT_FAILURE;
    }

  row0[0]->setCheckState(Qt::Checked);
  // make sure it didn't uncheck the checkable items
  if (headerView->checkState(0) != Qt::Checked ||
      row0[0]->checkState() != Qt::Checked ||
      row1[0]->checkState() != Qt::Unchecked ||
      row2[0]->checkState() != Qt::Unchecked)
    {
    std::cerr << "QStandardItem::setCheckState() failed: "
              << static_cast<int>(headerView->checkState(0)) << " "
              << static_cast<int>(row0[0]->checkState()) << " "
              << static_cast<int>(row1[0]->checkState()) << " "
              << static_cast<int>(row2[0]->checkState()) << std::endl;
    return EXIT_FAILURE;
    }

  // The checkable header gets updated with the item check states
  headerView->checkableModelHelper()->setPropagateDepth(-1);

  if (headerView->checkableModelHelper()->propagateDepth() == 0 ||
      headerView->checkState(0) != Qt::PartiallyChecked ||
      row0[0]->checkState() != Qt::Checked ||
      row1[0]->checkState() != Qt::Unchecked ||
      row2[0]->checkState() != Qt::Unchecked)
    {
    std::cerr << "ctkCheckableHeaderView::setPropagateToItems() failed: "
              << static_cast<int>(headerView->checkState(0)) << " "
              << static_cast<int>(row0[0]->checkState()) << " "
              << static_cast<int>(row1[0]->checkState()) << " "
              << static_cast<int>(row2[0]->checkState()) << std::endl;
    return EXIT_FAILURE;
    }

  row0[0]->setCheckState(Qt::Unchecked);

  if (headerView->checkState(0) != Qt::Unchecked ||
      row0[0]->checkState() != Qt::Unchecked ||
      row1[0]->checkState() != Qt::Unchecked ||
      row2[0]->checkState() != Qt::Unchecked)
    {
    std::cerr << "QStandardItem::setCheckState() failed: "
              << static_cast<int>(headerView->checkState(0)) << " "
              << static_cast<int>(row0[0]->checkState()) << " "
              << static_cast<int>(row1[0]->checkState()) << " "
              << static_cast<int>(row2[0]->checkState()) << std::endl;
    return EXIT_FAILURE;
    }

  row1[0]->setCheckState(Qt::Checked);

    // make sure it didn't uncheck the checkable items
  if (headerView->checkState(0) != Qt::PartiallyChecked ||
      row0[0]->checkState() != Qt::Unchecked ||
      row1[0]->checkState() != Qt::Checked ||
      row2[0]->checkState() != Qt::Unchecked)
    {
    std::cerr << "QStandardItem::setCheckState() failed: "
              << static_cast<int>(headerView->checkState(0)) << " "
              << static_cast<int>(row0[0]->checkState()) << " "
              << static_cast<int>(row1[0]->checkState()) << " "
              << static_cast<int>(row2[0]->checkState()) << std::endl;
    return EXIT_FAILURE;
    }

  row1[0]->setCheckState(Qt::Checked);

  // make sure it didn't check the checkable items
  if (headerView->checkState(0) != Qt::PartiallyChecked ||
      row0[0]->checkState() != Qt::Unchecked ||
      row1[0]->checkState() != Qt::Checked ||
      row2[0]->checkState() != Qt::Unchecked)
    {
    std::cerr << "QStandardItem::setCheckState() failed: "
              << static_cast<int>(headerView->checkState(0)) << " "
              << static_cast<int>(row0[0]->checkState()) << " "
              << static_cast<int>(row1[0]->checkState()) << " "
              << static_cast<int>(row2[0]->checkState()) << std::endl;
    return EXIT_FAILURE;
    }

  row0[0]->setCheckState(Qt::Checked);
  row2[0]->setCheckState(Qt::Checked);

  // make sure the header is now checked
  if (headerView->checkState(0) != Qt::Checked ||
      row0[0]->checkState() != Qt::Checked ||
      row1[0]->checkState() != Qt::Checked ||
      row2[0]->checkState() != Qt::Checked)
    {
    std::cerr << "QStandardItem::setCheckState() failed: "
              << static_cast<int>(headerView->checkState(0)) << " "
              << static_cast<int>(row0[0]->checkState()) << " "
              << static_cast<int>(row1[0]->checkState()) << " "
              << static_cast<int>(row2[0]->checkState()) << std::endl;
    return EXIT_FAILURE;
    }

  headerView->setCheckState(0, Qt::Unchecked);

  if (headerView->checkState(0) != Qt::Unchecked ||
      row0[0]->checkState() != Qt::Unchecked ||
      row1[0]->checkState() != Qt::Unchecked ||
      row2[0]->checkState() != Qt::Unchecked)
    {
    std::cerr << "ctkCheckableHeaderView::setCheckState() failed: "
              << static_cast<int>(headerView->checkState(0)) << " "
              << static_cast<int>(row0[0]->checkState()) << " "
              << static_cast<int>(row1[0]->checkState()) << " "
              << static_cast<int>(row2[0]->checkState()) << std::endl;
    return EXIT_FAILURE;
    }

  table.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(500, &app, SLOT(quit()));
    }

  return app.exec();
}
