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

#include <iostream>

/// CTK includes
#include <ctkCheckableHeaderView.h>

// ctkDICOMWidgets includes
#include "ctkDICOMServerNodeWidget.h"
#include "ui_ctkDICOMServerNodeWidget.h"

//----------------------------------------------------------------------------
class ctkDICOMServerNodeWidgetPrivate: public ctkPrivate<ctkDICOMServerNodeWidget>,
                                       public Ui_ctkDICOMServerNodeWidget
{
public:
  ctkDICOMServerNodeWidgetPrivate(){}
};

//----------------------------------------------------------------------------
// ctkDICOMServerNodeWidgetPrivate methods


//----------------------------------------------------------------------------
// ctkDICOMServerNodeWidget methods

//----------------------------------------------------------------------------
ctkDICOMServerNodeWidget::ctkDICOMServerNodeWidget(QWidget* _parent):Superclass(_parent)
{
  CTK_INIT_PRIVATE(ctkDICOMServerNodeWidget);
  CTK_D(ctkDICOMServerNodeWidget);
  
  d->setupUi(this);

  // checkable headers.
  d->nodeTable->model()->setHeaderData(0, Qt::Horizontal, Qt::Unchecked, Qt::CheckStateRole);
  QHeaderView* previousHeaderView = d->nodeTable->horizontalHeader();
  ctkCheckableHeaderView* headerView = new ctkCheckableHeaderView(Qt::Horizontal, d->nodeTable);
  headerView->setClickable(previousHeaderView->isClickable());
  headerView->setMovable(previousHeaderView->isMovable());
  headerView->setHighlightSections(previousHeaderView->highlightSections());
  //headerView->setModel(previousHeaderView->model());
  //headerView->setSelectionModel(previousHeaderView->selectionModel());
  headerView->setPropagateToItems(true);
  d->nodeTable->setHorizontalHeader(headerView);

  d->removeButton->setEnabled(false);

  connect(d->addButton,SIGNAL(clicked()), this, SLOT(addNode()));
  connect(d->nodeTable,SIGNAL(cellActivated(int,int)), this, SLOT(updateState(int,int)));


}

//----------------------------------------------------------------------------
ctkDICOMServerNodeWidget::~ctkDICOMServerNodeWidget()
{
}


//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget::addNode()
{
  CTK_D(ctkDICOMServerNodeWidget);

  std::cerr << "add server node\n";
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget::updateState(int row, int column)
{
  CTK_D(ctkDICOMServerNodeWidget);

  d->removeButton->setEnabled(true);
}

//----------------------------------------------------------------------------
void ctkDICOMServerNodeWidget::populateQuery(/*ctkDICOMQuery &query*/)
{
  CTK_D(ctkDICOMServerNodeWidget);

  std::cerr << "server node populate\n";
}


