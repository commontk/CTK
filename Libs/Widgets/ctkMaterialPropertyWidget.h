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

#ifndef __ctkMaterialPropertyWidget_h
#define __ctkMaterialPropertyWidget_h

// Qt includes
#include <QWidget>

// CTK includes
#include "ctkWidgetsExport.h"

class ctkMaterialPropertyWidgetPrivate;
class QListWidgetItem;

/// \ingroup Widgets
/// ctkMaterialPropertyWidget is a panel to control material properties
/// such as color and lighting coefficients. It contains a preview icon
/// and a list of presets.
/// Anytime a property is modified, the preview icon is updated with the
/// new lighting coefficient.
class CTK_WIDGETS_EXPORT ctkMaterialPropertyWidget : public QWidget
{
  Q_OBJECT
  /// This property holds the color of the material.
  Q_PROPERTY(QColor color  READ color WRITE setColor);
  /// Opacity component of the material property.
  Q_PROPERTY(double opacity READ opacity WRITE setOpacity);
  /// This property holds the ambient lighting coefficient,
  /// it is a nondirectional property.
  /// Its range is [0,1], where 0 means no ambient light, and 1 means
  /// full ambient light
  /// Hint: A range of [0.1,0.5] is more realistic.
  Q_PROPERTY(double ambient READ ambient WRITE setAmbient);
  /// This property holds the diffuse lighting coefficient.
  /// Its range is [0,1], where 0 means no diffuse light, and 1 means
  /// full diffuse light
  Q_PROPERTY(double diffuse READ diffuse WRITE setDiffuse);
  /// This property holds the specular lighting coefficient.
  /// Its range is [0,1], where 0 means no specular light, and 1 means
  /// full specular light
  Q_PROPERTY(double specular READ specular WRITE setSpecular);
  /// This property holds the power of specular lighting coefficient.
  /// Its range is [1,50].
  Q_PROPERTY(double specularPower READ specularPower WRITE setSpecularPower);
  /// This property controls weither backface culling should be enabled or not
  Q_PROPERTY(bool backfaceCulling READ backfaceCulling WRITE setBackfaceCulling);
  /// Control weither the color is shown to the user. Visible by default
  Q_PROPERTY(bool colorVisible READ isColorVisible WRITE setColorVisible);
  /// Control weither the opacity is shown to the user. Visible by default
  Q_PROPERTY(bool opacityVisible READ isOpacityVisible WRITE setOpacityVisible);
  /// Control weither the backface culling is shown to the user. Visible by default
  Q_PROPERTY(bool backfaceCullingVisible READ isBackfaceCullingVisible WRITE setBackfaceCullingVisible);
  
public:
  /// Superclass typedef
  typedef QWidget Superclass;

  /// Constructor
  explicit ctkMaterialPropertyWidget(QWidget* parent = 0);

  /// Destructor
  virtual ~ctkMaterialPropertyWidget();

  QColor color()const;
  double opacity()const;

  double ambient()const;
  double diffuse()const;
  double specular()const;
  double specularPower()const;

  bool backfaceCulling()const;

  /// Add a preset to the preset list. A preview icon will be generated and be
  /// added on the bottom right corner list. If space is needed, a scrollbar
  /// will appear. When the user clicks on the icon representing the preset,
  /// all the preset properties will be applied.
  /// If color is invalid, the preset color is synchronized with the current
  /// color property.
  void addPreset(const QColor& color, double opacity,
                 double ambient, double diffuse,
                 double specular, double power,
                 const QString& label);

  bool isColorVisible()const;
  void setColorVisible(bool show);
  bool isOpacityVisible()const;
  void setOpacityVisible(bool show);
  bool isBackfaceCullingVisible()const;
  void setBackfaceCullingVisible(bool show);

public Q_SLOTS:
  void setColor(const QColor& newColor);
  void setOpacity(double newOpacity);

  void setAmbient(double newAmbient);
  void setDiffuse(double newDiffuse);
  void setSpecular(double newSpecular);
  void setSpecularPower(double newSpecularPower);

  void setBackfaceCulling(bool enable);

Q_SIGNALS:
  void colorChanged(QColor newColor);
  void opacityChanged(double newOpacity);

  void ambientChanged(double newAmbient);
  void diffuseChanged(double newDiffuse);
  void specularChanged(double newSpecular);
  void specularPowerChanged(double newSpecularPower);

  void backfaceCullingChanged(bool newBackfaceCulling);
protected Q_SLOTS:
  virtual void onColorChanged(const QColor& newColor);
  virtual void onOpacityChanged(double newOpacity);

  virtual void onAmbientChanged(double newAmbient);
  virtual void onDiffuseChanged(double newDiffuse);
  virtual void onSpecularChanged(double newSpecular);
  virtual void onSpecularPowerChanged(double newSpecularPower);

  virtual void onBackfaceCullingChanged(bool newBackFaceCulling);

  void selectPreset(QListWidgetItem*);

protected:
  QScopedPointer<ctkMaterialPropertyWidgetPrivate> d_ptr;

  virtual void resizeEvent(QResizeEvent* resize);
private:
  Q_DECLARE_PRIVATE(ctkMaterialPropertyWidget);
  Q_DISABLE_COPY(ctkMaterialPropertyWidget);
};

#endif
