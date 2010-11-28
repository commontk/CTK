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
  Q_PROPERTY(double ambient READ ambient WRITE setAmbient);
  Q_PROPERTY(double diffuse READ diffuse WRITE setDiffuse);
  Q_PROPERTY(double specular READ specular WRITE setSpecular);
  Q_PROPERTY(double specularPower READ specularPower WRITE setSpecularPower);
public:
  /// Superclass typedef
  typedef QWidget Superclass;

  /// Constructor
  explicit ctkMaterialPropertyWidget(QWidget* parent = 0);
  
  /// Destructor
  virtual ~ctkMaterialPropertyWidget();

  double ambient()const;
  double diffuse()const;
  double specular()const;
  double specularPower()const;
  
  void addPreset(double ambient, double diffuse, double specular, double power, const QString& label);

public slots:
  void setAmbient(double newAmbient);
  void setDiffuse(double newDiffuse);
  void setSpecular(double newSpecular);
  void setSpecularPower(double newSpecularPower);

signals:
  void ambientChanged(double newAmbient);
  void diffuseChanged(double newDiffuse);
  void specularChanged(double newSpecular);
  void specularPowerChanged(double newSpecularPower);
  
protected slots:
  void selectPreset(QListWidgetItem*);

protected:
  QScopedPointer<ctkMaterialPropertyWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkMaterialPropertyWidget);
  Q_DISABLE_COPY(ctkMaterialPropertyWidget);
};

#endif
