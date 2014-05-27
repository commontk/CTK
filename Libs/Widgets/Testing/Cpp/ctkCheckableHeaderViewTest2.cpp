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
#include "ctkCheckableHeaderView.h"
#include <ctkCheckableModelHelper.h>

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkCheckableHeaderViewTest2(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QStringList headers;
  headers << "Title 1" << "Title 2" << "Title 3";
  QStandardItemModel model;
  model.setHorizontalHeaderLabels(headers);
  QList<QStandardItem*> row0;
  row0 << new QStandardItem << new QStandardItem << new QStandardItem;
  row0[0]->setText("not user checkable");
  model.appendRow(row0);
  QList<QStandardItem*> row1;
  row1 << new QStandardItem << new QStandardItem << new QStandardItem;
  row1[0]->setCheckable(true);
  row1[0]->setText("checkable1");
  model.appendRow(row1);
  QList<QStandardItem*> row2;
  row2 << new QStandardItem << new QStandardItem << new QStandardItem;
  row2[0]->setCheckable(true);
  row2[0]->setText("checkable2");
  model.appendRow(row2);

  QTreeView view;
  view.setModel(&model);

  model.setHeaderData(0, Qt::Horizontal, Qt::Checked, Qt::CheckStateRole);

  QHeaderView* previousHeaderView = view.header();
#if (QT_VERSION >= 0x50000)
  bool oldClickable = previousHeaderView->sectionsClickable();
#else
  bool oldClickable = previousHeaderView->isClickable();
#endif

  ctkCheckableHeaderView* headerView = new ctkCheckableHeaderView(Qt::Horizontal, &view);
#if (QT_VERSION >= 0x50000)
  headerView->setSectionsClickable(oldClickable);
  headerView->setSectionsMovable(previousHeaderView->sectionsMovable());
#else
  headerView->setClickable(oldClickable);
  headerView->setMovable(previousHeaderView->isMovable());
#endif
  headerView->setHighlightSections(previousHeaderView->highlightSections());
  headerView->checkableModelHelper()->setPropagateDepth(-1);
  headerView->checkableModelHelper()->setForceCheckability(true);

  // sets the model to the headerview
  view.setHeader(headerView);
  headers << "4";
  model.setHorizontalHeaderLabels(headers);
  view.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(500, &app, SLOT(quit()));
    }

  return app.exec();
}
