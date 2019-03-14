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

#ifndef __ctkVTKSurfaceMaterialPropertyWidget_h
#define __ctkVTKSurfaceMaterialPropertyWidget_h

// CTK includes
#include <ctkMaterialPropertyWidget.h>
#include "ctkVTKObject.h"
#include "ctkVisualizationVTKWidgetsExport.h"

class ctkVTKSurfaceMaterialPropertyWidgetPrivate;
class vtkProperty;

/// \ingroup Visualization_VTK_Widgets
class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKSurfaceMaterialPropertyWidget
  : public ctkMaterialPropertyWidget
{
  Q_OBJECT;
  QVTK_OBJECT;
  typedef ctkMaterialPropertyWidget Superclass;

public:
  /// Constructors
  ctkVTKSurfaceMaterialPropertyWidget(QWidget* parentWidget);
  ctkVTKSurfaceMaterialPropertyWidget(vtkProperty* property, QWidget* parentWidget);
  ~ctkVTKSurfaceMaterialPropertyWidget() override;

  vtkProperty* property()const;

public Q_SLOTS:
  void setProperty(vtkProperty* property);

protected Q_SLOTS:
  void updateFromProperty();

protected:
  QScopedPointer<ctkVTKSurfaceMaterialPropertyWidgetPrivate> d_ptr;

  void onColorChanged(const QColor& newColor) override;
  void onOpacityChanged(double newOpacity) override;
  void onAmbientChanged(double newAmbient) override;
  void onDiffuseChanged(double newDiffuse) override;
  void onSpecularChanged(double newSpecular) override;
  void onSpecularPowerChanged(double newSpecularPower) override;
  void onBackfaceCullingChanged(bool newBackfaceCulling) override;

private:
  Q_DECLARE_PRIVATE(ctkVTKSurfaceMaterialPropertyWidget);
  Q_DISABLE_COPY(ctkVTKSurfaceMaterialPropertyWidget);
};

#endif
