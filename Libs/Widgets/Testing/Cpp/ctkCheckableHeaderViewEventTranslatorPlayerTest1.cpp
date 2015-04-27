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
#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QSignalSpy>
#include <QStandardItemModel>
#include <QTableView>
#include <QTimer>
#include <QTreeView>

// CTK includes
#include "ctkCallback.h"
#include "ctkConfig.h"
#include "ctkCheckableHeaderView.h"
#include "ctkCheckableHeaderViewEventPlayer.h"
#include "ctkCheckableHeaderViewEventTranslator.h"
#include "ctkEventTranslatorPlayerWidget.h"

// QtTesting includes
#include "pqTestUtility.h"

// STD includes
#include <cstdlib>
#include <iostream>

namespace
{
QSignalSpy* Spy1;
//-----------------------------------------------------------------------------
void checkFinalWidgetState(void* data)
  {
  QWidget* parentWidget = reinterpret_cast<QWidget*>(data);
  QList<ctkCheckableHeaderView*> widget = parentWidget->findChildren<ctkCheckableHeaderView*>();

  if( widget.count() > 0)
    {
    CTKCOMPARE(widget[0]->checkState(0), Qt::PartiallyChecked);
    CTKCOMPARE(widget[0]->checkState(1), Qt::Unchecked);
    CTKCOMPARE(Spy1->count(), 8);
    }
  else
    {
    QApplication::exit(EXIT_FAILURE);
    }
  }
}

//-----------------------------------------------------------------------------
int ctkCheckableHeaderViewEventTranslatorPlayerTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QString xmlDirectory = CTK_SOURCE_DIR "/Libs/Widgets/Testing/Cpp/";

  // ------------------------
  ctkEventTranslatorPlayerWidget etpWidget;
  pqTestUtility* testUtility = new pqTestUtility(&etpWidget);
  etpWidget.setTestUtility(testUtility);
  etpWidget.addWidgetEventTranslator(new ctkCheckableHeaderViewEventTranslator);
  etpWidget.addWidgetEventPlayer(new ctkCheckableHeaderViewEventPlayer);

  // Test case 1
  QStandardItemModel model;
  QStringList headers;
  headers << "Title 1" << "Title 2" << "Title 3";
  model.setHorizontalHeaderLabels(headers);
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

  QTableView table;
  table.setModel(&model);

  // Header is checked by default
  model.setHeaderData(0, Qt::Horizontal, Qt::Checked, Qt::CheckStateRole);

  QHeaderView* previousHeaderView = table.horizontalHeader();
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
  bool oldClickable = previousHeaderView->isClickable();
#else
  bool oldClickable = previousHeaderView->sectionsClickable();
#endif
  ctkCheckableHeaderView* headerView =
    new ctkCheckableHeaderView(Qt::Horizontal, &table);
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
  headerView->setClickable(oldClickable);
  headerView->setMovable(previousHeaderView->isMovable());
#else
  headerView->setSectionsClickable(oldClickable);
  headerView->setSectionsMovable(previousHeaderView->sectionsMovable());
#endif
  headerView->setHighlightSections(previousHeaderView->highlightSections());
  // propagatetoitems is true by default
  //headerView->setPropagateToItems(true);

  // sets the model to the headerview
  table.setHorizontalHeader(headerView);

  // Header is checked by default
  model.setHeaderData(1, Qt::Horizontal, Qt::Checked, Qt::CheckStateRole);

  QSignalSpy spy1(headerView, SIGNAL(sectionPressed(int)));

  Spy1 = &spy1;

  etpWidget.addTestCase(&table,
                        xmlDirectory + "ctkCheckableHeaderViewEventTranslatorPlayerTest1.xml",
                        &checkFinalWidgetState);

  // ------------------------
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(0, &etpWidget, SLOT(play()));
    }

  etpWidget.show();
  return app.exec();
}

