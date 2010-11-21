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

class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKScalarBarWidget : public QWidget
{
  Q_OBJECT;
  QVTK_OBJECT;

public:
  /// Constructors
  ctkVTKScalarBarWidget(QWidget* parentWidget);
  ctkVTKScalarBarWidget(vtkScalarBarWidget* scalarBar, QWidget* parentWidget);
  virtual ~ctkVTKScalarBarWidget();

  vtkScalarBarWidget* scalarBarWidget()const;

public slots:
  void setScalarBarWidget(vtkScalarBarWidget* scalarBar);
  void setDisplay(bool visible);
  void setMaxNumberOfColors(int colorCount);
  void setNumberOfLabels(int labelCount);
  void setTitle(const QString& title);
  void setLabelsFormat(const QString& format);

signals:
  void modified();

protected slots:
  void updateFromScalarBarWidget();

protected:
  QScopedPointer<ctkVTKScalarBarWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkVTKScalarBarWidget);
  Q_DISABLE_COPY(ctkVTKScalarBarWidget);
};

#endif
