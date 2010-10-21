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

#ifndef __ctkVTKScalarsToColorsWidget_h
#define __ctkVTKScalarsToColorsWidget_h

//Qt includes
#include <QWidget>

// CTK includes
#include <ctkPimpl.h>
#include "ctkVisualizationVTKWidgetsExport.h"
class ctkVTKScalarsToColorsView;
class ctkVTKScalarsToColorsWidgetPrivate;

// VTK includes
#include <QVTKWidget.h>

class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKScalarsToColorsWidget : public QWidget
{
  Q_OBJECT

public:
  ctkVTKScalarsToColorsWidget(QWidget* parent = 0);
  virtual ~ctkVTKScalarsToColorsWidget();

  ctkVTKScalarsToColorsView* view()const;
protected:
  QScopedPointer<ctkVTKScalarsToColorsWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkVTKScalarsToColorsWidget);
  Q_DISABLE_COPY(ctkVTKScalarsToColorsWidget);
};

#endif
