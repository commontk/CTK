/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __ctkDICOMListenerWidget_h
#define __ctkDICOMListenerWidget_h

// Qt includes 
#include <QWidget>

// CTK includes
#include <ctkPimpl.h>

#include "CTKDICOMWidgetsExport.h"

class ctkDICOMListenerWidgetPrivate;

class CTK_DICOM_WIDGETS_EXPORT ctkDICOMListenerWidget : public QWidget
{
public:
  typedef QWidget Superclass;
  explicit ctkDICOMListenerWidget(QWidget* parent=0);
  virtual ~ctkDICOMListenerWidget();

private:
  CTK_DECLARE_PRIVATE(ctkDICOMListenerWidget);
};

#endif
