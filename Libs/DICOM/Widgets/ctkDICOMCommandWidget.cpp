/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

// ctkDICOMWidgets includes
#include "ctkDICOMCommandWidget.h"
#include "ui_ctkDICOMCommandWidget.h"

//----------------------------------------------------------------------------
class ctkDICOMCommandWidgetPrivate: public ctkPrivate<ctkDICOMCommandWidget>,
                                    public Ui_ctkDICOMCommandWidget
{
public:
  ctkDICOMCommandWidgetPrivate(){}
};

//----------------------------------------------------------------------------
// ctkDICOMCommandWidgetPrivate methods


//----------------------------------------------------------------------------
// ctkDICOMCommandWidget methods

//----------------------------------------------------------------------------
ctkDICOMCommandWidget::ctkDICOMCommandWidget(QWidget* _parent):Superclass(_parent)
{
  CTK_INIT_PRIVATE(ctkDICOMCommandWidget);
  CTK_D(ctkDICOMCommandWidget);
  
  d->setupUi(this);
}

//----------------------------------------------------------------------------
ctkDICOMCommandWidget::~ctkDICOMCommandWidget()
{
}


