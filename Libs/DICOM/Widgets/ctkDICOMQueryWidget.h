/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __ctkDICOMQueryWidget_h
#define __ctkDICOMQueryWidget_h

// Qt includes 
#include <QWidget>

// CTK includes
#include <ctkPimpl.h>

#include "CTKDICOMWidgetsExport.h"

class ctkDICOMQueryWidgetPrivate;

class CTK_DICOM_WIDGETS_EXPORT ctkDICOMQueryWidget : public QWidget
{
public:
  typedef QWidget Superclass;
  explicit ctkDICOMQueryWidget(QWidget* parent=0);
  virtual ~ctkDICOMQueryWidget();

private:
  CTK_DECLARE_PRIVATE(ctkDICOMQueryWidget);
};

#endif
