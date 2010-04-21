/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __ctkDICOMQueryRetrieveWidget_h
#define __ctkDICOMQueryRetrieveWidget_h

// Qt includes 
#include <QWidget>

// CTK includes
#include <ctkPimpl.h>

#include "CTKDICOMWidgetsExport.h"

class ctkDICOMQueryRetrieveWidgetPrivate;

class CTK_DICOM_WIDGETS_EXPORT ctkDICOMQueryRetrieveWidget : public QWidget
{
public:
  typedef QWidget Superclass;
  explicit ctkDICOMQueryRetrieveWidget(QWidget* parent=0);
  virtual ~ctkDICOMQueryRetrieveWidget();

private:
  CTK_DECLARE_PRIVATE(ctkDICOMQueryRetrieveWidget);
};

#endif
