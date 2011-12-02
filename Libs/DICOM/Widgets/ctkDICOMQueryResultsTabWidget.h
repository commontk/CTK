/*=========================================================================

  Library:   CTK

  Copyright (c) Isomics Inc.

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

#ifndef __ctkDICOMQueryResultsTabWidget_h
#define __ctkDICOMQueryResultsTabWidget_h

// Qt includes 
#include <QWidget>
#include <QTabWidget>


#include "ctkDICOMWidgetsExport.h"

/// \ingroup DICOM_Widgets
class CTK_DICOM_WIDGETS_EXPORT ctkDICOMQueryResultsTabWidget : public QTabWidget
{
public:
  typedef QTabWidget Superclass;
  explicit ctkDICOMQueryResultsTabWidget(QWidget* parent=0);
  virtual ~ctkDICOMQueryResultsTabWidget();

  void disableCloseOnTab(int index);

private:
};

#endif
