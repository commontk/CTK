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

#ifndef __ctkVTKPropertyWidget_h
#define __ctkVTKPropertyWidget_h

// Qt includes
#include <QWidget>

// CTK includes
#include "ctkColorPickerButton.h"
#include "ctkVTKObject.h"
#include "ctkVisualizationVTKWidgetsExport.h"

class ctkVTKPropertyWidgetPrivate;
class vtkProperty;

/// \ingroup Visualization_VTK_Widgets
class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKPropertyWidget
  : public QWidget
{
  Q_OBJECT;
  QVTK_OBJECT;
  typedef QWidget Superclass;

  Q_FLAGS(GroupsState)

  /// Show/Hide, expand/collapse groups in the widget.
  /// Groups are visible by default and Color is the only expanded group.
  /// \sa groupsState(), setGroupsState()
  Q_PROPERTY(GroupsState groupsState READ groupsState WRITE setGroupsState )

  /// This property controls the color dialog option of the Color and EdgeColor
  /// color picker buttons.
  /// Same default as ctkColorPickerbutton
  Q_PROPERTY(ctkColorPickerButton::ColorDialogOptions colorDialogOptions READ colorDialogOptions WRITE setColorDialogOptions)

public:
  enum GroupState {
    RepresentationVisible = 0x00001,
    RepresentationCollapsed = 0x00002,
    ColorVisible          = 0x00010,
    ColorCollapsed        = 0x00020,
    LightingVisible       = 0x00100,
    LightingCollapsed     = 0x00200,
    MaterialVisible       = 0x01000,
    MaterialCollapsed     = 0x02000,
    AllVisible            = RepresentationVisible | ColorVisible | LightingVisible | MaterialVisible,
    AllCollapsed          = RepresentationCollapsed | ColorCollapsed | LightingCollapsed | MaterialCollapsed
  };
  Q_DECLARE_FLAGS(GroupsState, GroupState)

  /// Construct a ctkVTKPropertyWidget with a default vtkProperty.
  ctkVTKPropertyWidget(QWidget* parentWidget);

  /// Construct a ctkVTKPropertyWidget with the given vtkProperty.
  ctkVTKPropertyWidget(vtkProperty* property, QWidget* parentWidget);
  virtual ~ctkVTKPropertyWidget();

  vtkProperty* property()const;

  /// \sa groupsState, setGroupsState()
  ctkVTKPropertyWidget::GroupsState groupsState()const;
  /// \sa groupsState, groupsState()
  void setGroupsState(ctkVTKPropertyWidget::GroupsState state);

  /// \sa colorDialogOptions, setColorDialogOptions()
  ctkColorPickerButton::ColorDialogOptions colorDialogOptions()const;
  /// \sa colorDialogOptions, colorDialogOptions()
  void setColorDialogOptions(ctkColorPickerButton::ColorDialogOptions options);

  virtual int representation()const;
  virtual double pointSize()const;
  virtual double lineWidth()const;
  virtual bool frontfaceCulling()const;
  virtual bool backfaceCulling()const;
  virtual QColor color()const;
  virtual double opacity()const;
  virtual bool edgeVisibility()const;
  virtual QColor edgeColor()const;
  virtual bool lighting()const;
  virtual int interpolation()const;
  virtual bool shading()const;

public Q_SLOTS:
  virtual void setProperty(vtkProperty* property);

  virtual void setRepresentation(int newRepresentation);
  virtual void setPointSize(double newPointSize);
  virtual void setLineWidth(double newLineWidth);
  virtual void setFrontfaceCulling(bool newFrontfaceCulling);
  virtual void setBackfaceCulling(bool newBackfaceCulling);
  virtual void setColor(const QColor& newColor);
  virtual void setOpacity(double newOpacity);
  virtual void setEdgeVisibility(bool newEdgeVisibility);
  virtual void setEdgeColor(const QColor& newColor);
  virtual void setLighting(bool newLighting);
  virtual void setInterpolation(int newInterpolation);
  virtual void setShading(bool newShading);

protected Q_SLOTS:
  void updateWidgetFromProperty();

protected:
  QScopedPointer<ctkVTKPropertyWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkVTKPropertyWidget);
  Q_DISABLE_COPY(ctkVTKPropertyWidget);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ctkVTKPropertyWidget::GroupsState)

#endif
