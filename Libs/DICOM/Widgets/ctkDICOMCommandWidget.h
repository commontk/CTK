/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __ctkDICOMCommandWidget_h
#define __ctkDICOMCommandWidget_h

// Qt includes 
#include <QWidget>

// CTK includes
#include <ctkPimpl.h>

#include "CTKDICOMWidgetsExport.h"

class ctkDICOMCommandWidgetPrivate; 

class CTK_DICOM_WIDGETS_EXPORT ctkDICOMCommandWidget : public QWidget
{
public:
  typedef QWidget Superclass;
  explicit ctkDICOMCommandWidget(QWidget* parent=0);
  virtual ~ctkDICOMCommandWidget();

private:
  CTK_DECLARE_PRIVATE(ctkDICOMCommandWidget);
};

#endif
