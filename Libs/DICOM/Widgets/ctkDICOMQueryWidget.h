/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

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

#ifndef __ctkDICOMQueryWidget_h
#define __ctkDICOMQueryWidget_h

// Qt includes 
#include <QWidget>

#include "ctkDICOMWidgetsExport.h"

class ctkDICOMQueryWidgetPrivate;

class CTK_DICOM_WIDGETS_EXPORT ctkDICOMQueryWidget : public QWidget
{
public:
  typedef QWidget Superclass;
  explicit ctkDICOMQueryWidget(QWidget* parent=0);
  virtual ~ctkDICOMQueryWidget();
  
  /// set properties of query based on current widget state
  void populateQuery (/*ctkDICOMQuery &query*/);

protected:
  QScopedPointer<ctkDICOMQueryWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMQueryWidget);
  Q_DISABLE_COPY(ctkDICOMQueryWidget);
};

#endif
