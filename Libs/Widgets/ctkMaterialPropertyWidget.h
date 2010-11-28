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

#ifndef __ctkMaterialPropertyWidget_h
#define __ctkMaterialPropertyWidget_h

// Qt includes
#include <QWidget>

// CTK includes
#include "ctkWidgetsExport.h"

class ctkMaterialPropertyWidgetPrivate;
class QListWidgetItem;

class CTK_WIDGETS_EXPORT ctkMaterialPropertyWidget : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(QColor color  READ color WRITE setColor);
  Q_PROPERTY(double opacity READ opacity WRITE setOpacity);
  Q_PROPERTY(double ambient READ ambient WRITE setAmbient);
  Q_PROPERTY(double diffuse READ diffuse WRITE setDiffuse);
  Q_PROPERTY(double specular READ specular WRITE setSpecular);
  Q_PROPERTY(double specularPower READ specularPower WRITE setSpecularPower);
  Q_PROPERTY(bool backfaceCulling READ backfaceCulling WRITE setBackfaceCulling);
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
  
  void addPreset(const QColor& color, double opacity,
                 double ambient, double diffuse,
                 double specular, double power,
                 const QString& label);

public slots:
  void setColor(const QColor& newColor);
  void setOpacity(double newOpacity);

  void setAmbient(double newAmbient);
  void setDiffuse(double newDiffuse);
  void setSpecular(double newSpecular);
  void setSpecularPower(double newSpecularPower);

  void setBackfaceCulling(bool enable);

signals:
  void colorChanged(QColor newColor);
  void opacityChanged(double newOpacity);

  void ambientChanged(double newAmbient);
  void diffuseChanged(double newDiffuse);
  void specularChanged(double newSpecular);
  void specularPowerChanged(double newSpecularPower);

  void backfaceCullingChanged(bool newBackfaceCulling);
protected slots:
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
