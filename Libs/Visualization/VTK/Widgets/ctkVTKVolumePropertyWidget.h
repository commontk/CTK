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

#ifndef __ctkVTKVolumePropertyWidget_h
#define __ctkVTKVolumePropertyWidget_h

//Qt includes
#include <QWidget>

// CTK includes
#include <ctkVTKObject.h>
#include "ctkVisualizationVTKWidgetsExport.h"
class ctkVTKVolumePropertyWidgetPrivate;

// VTK includes
class vtkVolumeProperty;

class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKVolumePropertyWidget
  : public QWidget
{
  Q_OBJECT
  QVTK_OBJECT

public:
  ctkVTKVolumePropertyWidget(QWidget* parent = 0);
  virtual ~ctkVTKVolumePropertyWidget();

  vtkVolumeProperty* volumeProperty()const;

public slots:
  void setVolumeProperty(vtkVolumeProperty* volumeProperty);

protected slots:
  void updateFromVolumeProperty();

  void setInterpolationMode(int mode);
  void setShade(bool);
  void setAmbient(double value);
  void setDiffuse(double value);
  void setSpecular(double value);
  void setSpecularPower(double value);

protected:
  QScopedPointer<ctkVTKVolumePropertyWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkVTKVolumePropertyWidget);
  Q_DISABLE_COPY(ctkVTKVolumePropertyWidget);
};

#endif
