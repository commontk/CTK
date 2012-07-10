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
// Freely inspired from KWWidgets: vtkKWMaterialPropertyWidget

#ifndef __ctkMaterialPropertyPreviewLabel_h
#define __ctkMaterialPropertyPreviewLabel_h

// Qt includes
#include <QFrame>

// CTK includes
#include "ctkWidgetsExport.h"

class ctkMaterialPropertyPreviewLabelPrivate;

/// \ingroup Widgets
class CTK_WIDGETS_EXPORT ctkMaterialPropertyPreviewLabel : public QFrame
{
  Q_OBJECT
  Q_PROPERTY(double ambient READ ambient WRITE setAmbient)
  Q_PROPERTY(double diffuse READ diffuse WRITE setDiffuse)
  Q_PROPERTY(double specular READ specular WRITE setSpecular)
  Q_PROPERTY(double specularPower READ specularPower WRITE setSpecularPower)

  Q_PROPERTY(QColor color READ color WRITE setColor)
  Q_PROPERTY(double opacity READ opacity WRITE setOpacity)
  Q_PROPERTY(double gridOpacity READ gridOpacity WRITE setGridOpacity)
public : 

  ctkMaterialPropertyPreviewLabel(QWidget *parent = 0);
  ctkMaterialPropertyPreviewLabel(const QColor& color, double opacity,
    double ambient, double diffuse, double specular, double specularPower,
    QWidget *parent = 0);
  virtual ~ctkMaterialPropertyPreviewLabel();
  
  double ambient()const;
  double diffuse()const;
  double specular()const;
  double specularPower()const;
  
  QColor color()const;
  double opacity()const;
  double gridOpacity()const;

  /// Reimplemented to make it square
  virtual int heightForWidth(int w)const;
  
  virtual QSize sizeHint()const;
public Q_SLOTS:
  /// Valid range: [0,1]
  void setAmbient(double newAbient);
  /// Valid range: [0,1]
  void setDiffuse(double newDiffuse);
  /// Valid range: [0,1]
  void setSpecular(double newSpecular);
  /// Valid range: [1,inf[
  void setSpecularPower(double newSpecularPower);
  
  void setColor(const QColor& newColor);
  /// Valid range: [0, 1]
  void setOpacity(double newOpacity);
  void setGridOpacity(double newGridOpacity);

protected: 
  void paintEvent(QPaintEvent *);
  void draw(QImage& image);

  QScopedPointer<ctkMaterialPropertyPreviewLabelPrivate> d_ptr;
private :
  Q_DECLARE_PRIVATE(ctkMaterialPropertyPreviewLabel);
  Q_DISABLE_COPY(ctkMaterialPropertyPreviewLabel);
};

#endif

