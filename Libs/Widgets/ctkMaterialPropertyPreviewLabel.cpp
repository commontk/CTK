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

//Qt includes
#include <QColor>
#include <QDebug>
#include <QImage>
#include <QPainter>
#include <QVector3D>

//CTK includes
#include "ctkMaterialPropertyPreviewLabel.h"

// STD includes
#include <cmath>

//ctkMaterialPropertyPreviewLabelPrivate
//-----------------------------------------------------------------------------
class ctkMaterialPropertyPreviewLabelPrivate
{
  Q_DECLARE_PUBLIC(ctkMaterialPropertyPreviewLabel);
protected:
  ctkMaterialPropertyPreviewLabel* const q_ptr;
public:
  ctkMaterialPropertyPreviewLabelPrivate(ctkMaterialPropertyPreviewLabel& object);

  QColor Color;
  double Opacity;

  double Ambient;
  double Diffuse;
  double Specular;
  double SpecularPower;

  double GridOpacity;
};

//-----------------------------------------------------------------------------
ctkMaterialPropertyPreviewLabelPrivate::ctkMaterialPropertyPreviewLabelPrivate(ctkMaterialPropertyPreviewLabel& object)
  :q_ptr(&object)
{
  this->Color = Qt::white;
  this->Opacity = 1.;
  this->Ambient = 0.0;
  this->Diffuse = 1.0;
  this->Specular = 0.0;
  this->SpecularPower = 1;

  this->GridOpacity = 0.6;
}

//ctkMaterialPropertyPreviewLabel
//-----------------------------------------------------------------------------
ctkMaterialPropertyPreviewLabel::ctkMaterialPropertyPreviewLabel(QWidget *newParent)
  : QFrame(newParent)
  , d_ptr(new ctkMaterialPropertyPreviewLabelPrivate(*this))
{
}

//-----------------------------------------------------------------------------
ctkMaterialPropertyPreviewLabel::ctkMaterialPropertyPreviewLabel(
  const QColor& color, double opacity,
  double ambient, double diffuse,
  double specular, double specularPower,
  QWidget *newParent)
  : QFrame(newParent)
  , d_ptr(new ctkMaterialPropertyPreviewLabelPrivate(*this))
{
  Q_D(ctkMaterialPropertyPreviewLabel);
  d->Color = color;
  d->Opacity = opacity;
  d->Ambient = ambient;
  d->Diffuse = diffuse;
  d->Specular = specular;
  d->SpecularPower = specularPower;
}

//-----------------------------------------------------------------------------
ctkMaterialPropertyPreviewLabel::~ctkMaterialPropertyPreviewLabel()
{
}

//-----------------------------------------------------------------------------
void ctkMaterialPropertyPreviewLabel::setAmbient(double newAmbient)
{
  Q_D(ctkMaterialPropertyPreviewLabel);
  d->Ambient = newAmbient;
  this->update();
}

//-----------------------------------------------------------------------------
double ctkMaterialPropertyPreviewLabel::ambient()const
{
  Q_D(const ctkMaterialPropertyPreviewLabel);
  return d->Ambient;
}

//-----------------------------------------------------------------------------
void ctkMaterialPropertyPreviewLabel::setDiffuse(double newDiffuse)
{
  Q_D(ctkMaterialPropertyPreviewLabel);
  d->Diffuse = newDiffuse;
  this->update();
}

//-----------------------------------------------------------------------------
double ctkMaterialPropertyPreviewLabel::diffuse()const
{
  Q_D(const ctkMaterialPropertyPreviewLabel);
  return d->Diffuse;
}

//-----------------------------------------------------------------------------
void ctkMaterialPropertyPreviewLabel::setSpecular(double newSpecular)
{
  Q_D(ctkMaterialPropertyPreviewLabel);
  d->Specular = newSpecular;
  this->update();
}

//-----------------------------------------------------------------------------
double ctkMaterialPropertyPreviewLabel::specular()const
{
  Q_D(const ctkMaterialPropertyPreviewLabel);
  return d->Specular;
}

//-----------------------------------------------------------------------------
void ctkMaterialPropertyPreviewLabel::setSpecularPower(double newSpecularPower)
{
  Q_D(ctkMaterialPropertyPreviewLabel);
  d->SpecularPower = newSpecularPower;
  this->update();
}

//-----------------------------------------------------------------------------
double ctkMaterialPropertyPreviewLabel::specularPower()const
{
  Q_D(const ctkMaterialPropertyPreviewLabel);
  return d->SpecularPower;
}

//-----------------------------------------------------------------------------
void ctkMaterialPropertyPreviewLabel::setColor(const QColor& newColor)
{
  Q_D(ctkMaterialPropertyPreviewLabel);
  d->Color = newColor;
  this->update();
}

//-----------------------------------------------------------------------------
QColor ctkMaterialPropertyPreviewLabel::color()const
{
  Q_D(const ctkMaterialPropertyPreviewLabel);
  return d->Color;
}

//-----------------------------------------------------------------------------
void ctkMaterialPropertyPreviewLabel::setOpacity(double newOpacity)
{
  Q_D(ctkMaterialPropertyPreviewLabel);
  d->Opacity = newOpacity;
  this->update();
}

//-----------------------------------------------------------------------------
double ctkMaterialPropertyPreviewLabel::opacity()const
{
  Q_D(const ctkMaterialPropertyPreviewLabel);
  return d->Opacity;
}

//-----------------------------------------------------------------------------
void ctkMaterialPropertyPreviewLabel::setGridOpacity(double newGridOpacity)
{
  Q_D(ctkMaterialPropertyPreviewLabel);
  d->GridOpacity = newGridOpacity;
  this->update();
}

//-----------------------------------------------------------------------------
double ctkMaterialPropertyPreviewLabel::gridOpacity()const
{
  Q_D(const ctkMaterialPropertyPreviewLabel);
  return d->GridOpacity;
}

//-----------------------------------------------------------------------------
int ctkMaterialPropertyPreviewLabel::heightForWidth(int w)const
{
  return w;
}

//-----------------------------------------------------------------------------
QSize ctkMaterialPropertyPreviewLabel::sizeHint()const
{
  return QSize(30,30);
}

//-----------------------------------------------------------------------------
void ctkMaterialPropertyPreviewLabel::paintEvent(QPaintEvent* event)
{
  Q_UNUSED(event);
  QRect cr = this->contentsRect();
  QImage image(cr.size(), QImage::Format_ARGB32_Premultiplied);
  this->draw(image);

  QPainter widgetPainter(this);
  this->drawFrame(&widgetPainter);
  widgetPainter.drawImage(cr.left(), cr.top(), image);
}

//-----------------------------------------------------------------------------
//From vtkKWMaterialPropertyWidget::CreateImage
void ctkMaterialPropertyPreviewLabel::draw(QImage& image)
{
  Q_D(ctkMaterialPropertyPreviewLabel);
  qreal opacity = d->Opacity;
  qreal ambient = d->Ambient;
  qreal diffuse = d->Diffuse;
  qreal specular = d->Specular;
  qreal specular_power = d->SpecularPower;

  int size = qMin(image.width(), image.height());
  int size8 = qMax(size / 8, 1);
  qreal size2 = 0.5 * size;
  qreal radius2 = size2*size2 - 1;

  QRgb rgba;
  for (int i = 0; i < image.width(); ++i)
    {
    for (int j = 0; j < image.height(); ++j)
      {
      int iGrid = i / size8;
      int jGrid = j / size8;

      if (((iGrid / 2) * 2 == iGrid &&
           (jGrid / 2) * 2 == jGrid) ||
          ((iGrid / 2) * 2 != iGrid &&
           (jGrid / 2) * 2 != jGrid))
        {
        rgba = qRgba(0, 0, 0, d->GridOpacity * 255);
        }
      else
        {
        rgba = qRgba(255. * d->GridOpacity,
                     255. * d->GridOpacity,
                     255. * d->GridOpacity,
                     255. * d->GridOpacity);
        }
      qreal dist = static_cast<qreal>((i-size2)*(i-size2) + (j-size2)*(j-size2));
      if (dist <= radius2)
        {
        QVector3D pt;
        pt.setX( (i-size2) / (size2-1) );
        pt.setY( (j-size2) / (size2-1) );
        pt.setZ( sqrt(qMax(1. - pt.x()*pt.x() - pt.y()*pt.y(), 0.)) );

        QVector3D normal = pt;
        normal.normalize();

        QVector3D light;
        light.setX(-5 - pt.x());
        light.setY(-5 - pt.y());
        light.setZ( 5 - pt.z());
        light.normalize();

        QVector3D view;
        view.setX(-pt.x());
        view.setY(-pt.y());
        view.setZ(5 - pt.z());
        view.normalize();

        qreal dot = QVector3D::dotProduct(normal, light);
        QVector3D ref;
        ref.setX( 2.*normal.x()*dot - light.x());
        ref.setY( 2.*normal.y()*dot - light.y());
        ref.setZ( 2.*normal.z()*dot - light.z());
        ref.normalize();

        qreal diffuseComp = qMax(diffuse * dot, static_cast<qreal>(0.));

        qreal specularDot = qMax(static_cast<qreal>(QVector3D::dotProduct(ref, view)), static_cast<qreal>(0));

        qreal specularComp = specular*pow(specularDot, specular_power);

        QVector3D intensity;
        intensity.setX( qMin(static_cast<qreal>((ambient + diffuseComp)*d->Color.redF() + specularComp), static_cast<qreal>(1.)));
        intensity.setY( qMin(static_cast<qreal>((ambient + diffuseComp)*d->Color.greenF() + specularComp), static_cast<qreal>(1.)));
        intensity.setZ( qMin(static_cast<qreal>((ambient + diffuseComp)*d->Color.blueF() + specularComp), static_cast<qreal>(1.)));

        if (opacity == 1.)
          {
          rgba = qRgba(static_cast<unsigned char>(255. * intensity.x() * opacity),
                       static_cast<unsigned char>(255. * intensity.y() * opacity),
                       static_cast<unsigned char>(255. * intensity.z() * opacity),
                       static_cast<unsigned char>(255. * opacity));
          }
        else
          {
          rgba = qRgba(static_cast<unsigned char>(qMin(255. * intensity.x() * opacity + qRed(rgba)*(1. - opacity), static_cast<qreal>(255.))),
                       static_cast<unsigned char>(qMin(255. * intensity.y() * opacity + qGreen(rgba)*(1. - opacity), static_cast<qreal>(255.))),
                       static_cast<unsigned char>(qMin(255. * intensity.z() * opacity + qBlue(rgba)*(1. - opacity), static_cast<qreal>(255.))),
                       static_cast<unsigned char>(qMin(255. * opacity + qAlpha(rgba)*(1. - opacity), static_cast<qreal>(255.))));
          }
        }
      image.setPixel(i,j,rgba);
      }
    }
}

