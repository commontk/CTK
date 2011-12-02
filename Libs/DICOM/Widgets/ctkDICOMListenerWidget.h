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

#ifndef __ctkDICOMListenerWidget_h
#define __ctkDICOMListenerWidget_h

// Qt includes 
#include <QWidget>

#include "ctkDICOMWidgetsExport.h"

class ctkDICOMListenerWidgetPrivate;

/// \ingroup DICOM_Widgets
class CTK_DICOM_WIDGETS_EXPORT ctkDICOMListenerWidget : public QWidget
{
public:
  typedef QWidget Superclass;
  explicit ctkDICOMListenerWidget(QWidget* parent=0);
  virtual ~ctkDICOMListenerWidget();

protected:
  QScopedPointer<ctkDICOMListenerWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMListenerWidget);
  Q_DISABLE_COPY(ctkDICOMListenerWidget);
};

#endif
