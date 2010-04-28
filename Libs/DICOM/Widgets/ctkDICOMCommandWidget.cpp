/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 
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


