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

#ifndef __ctkVTKScalarBarWidget_h
#define __ctkVTKScalarBarWidget_h

// Qt includes
#include <QWidget>

// CTK includes
#include <ctkPimpl.h>
#include "ctkVisualizationVTKWidgetsExport.h"
#include "ctkVTKObject.h"

class ctkVTKScalarBarWidgetPrivate;
class vtkScalarBarWidget;

/// \ingroup Visualization_VTK_Widgets
class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKScalarBarWidget : public QWidget
{
  Q_OBJECT;
  QVTK_OBJECT;

public:
  /// Constructors
  ctkVTKScalarBarWidget(QWidget* parentWidget = 0);
  ctkVTKScalarBarWidget(vtkScalarBarWidget* scalarBar, QWidget* parentWidget = 0);
  virtual ~ctkVTKScalarBarWidget();

  vtkScalarBarWidget* scalarBarWidget()const;

  bool display()const;
  /// Returns -1 if not scalarbar is set
  int maxNumberOfColors()const;
  int numberOfLabels()const;
  QString title()const;
  QString labelsFormat()const;

public Q_SLOTS:
  void setScalarBarWidget(vtkScalarBarWidget* scalarBar);

  void setDisplay(bool visible);
  void setMaxNumberOfColors(int colorCount);
  void setNumberOfLabels(int labelCount);
  void setTitle(const QString& title);
  void setLabelsFormat(const QString& format);

Q_SIGNALS:
  void modified();

protected Q_SLOTS:
  void onScalarBarModified();

protected:
  QScopedPointer<ctkVTKScalarBarWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkVTKScalarBarWidget);
  Q_DISABLE_COPY(ctkVTKScalarBarWidget);
};

#endif
