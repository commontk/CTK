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

//Qt includes
#include <QDebug>
#include <QBrush>
#include <QGridLayout>
#include <QLine>
#include <QMouseEvent>
#include <QPainter>

//CTK includes
#include "ctkAxesWidget.h"

//Test purposes
#include <QPushButton>

#include <cmath>
#include <math.h>

static const double goldenRatio = 1.6180339887;
static const double PI = 3.14159265358979323846;

//ctkAxesWidgetPrivate
//-----------------------------------------------------------------------------
class ctkAxesWidgetPrivate
{
  Q_DECLARE_PUBLIC(ctkAxesWidget);
protected:
  ctkAxesWidget* const q_ptr;
public:
  ctkAxesWidgetPrivate(ctkAxesWidget& object);
  QList<QPoint> extremities(QPoint center, int radius)const;
  QList<QRect> labelRects(const QList<QPoint>& extremities, QSize offset)const;

  ctkAxesWidget::Axis CurrentAxis;
  int Offset;
  bool HighlightCurrentAxis;
  
  QStringList AxesLabels;
  QVector<double> AxesAngles;
  
};

//-----------------------------------------------------------------------------
ctkAxesWidgetPrivate::ctkAxesWidgetPrivate(ctkAxesWidget& object)
  :q_ptr(&object)
{
  this->CurrentAxis = ctkAxesWidget::None;
  this->Offset = 10;
  this->HighlightCurrentAxis = true;

  this->AxesLabels << "R" << "L" << "S" << "I" << "A" << "P";
  this->AxesAngles << 0 <<  3.14159265 << 1.57079633 <<  4.71238898 << 5.49778714 << 2.35619449;
}

//-----------------------------------------------------------------------------
QList<QPoint> ctkAxesWidgetPrivate::extremities(QPoint center, int radius)const
{
  QList<QPoint> pos;
  for (int i = 0; i < 6 ; ++i)
    {
    pos << center + QPoint(radius * cos(this->AxesAngles[i]),
                           -radius * sin(this->AxesAngles[i]));
    }
  return pos;
}

//-----------------------------------------------------------------------------
QList<QRect> ctkAxesWidgetPrivate::labelRects(const QList<QPoint>& extremities, QSize offset)const
{
  Q_Q(const ctkAxesWidget);
  QFontMetrics fm = q->fontMetrics();
  QSize letterSize = fm.size(Qt::TextShowMnemonic, "X") + QSize(1,1);
  QSize halfLetterSize = letterSize / 2;
  QList<QRect> rects;
  for (int i = 0; i < 6; ++i)
    {
    rects << QRect(extremities[i]
                   + QPoint(cos(this->AxesAngles[i]) * (offset.width()+halfLetterSize.width()),
                            -sin(this->AxesAngles[i]) * (offset.height()+halfLetterSize.height()))
                   - QPoint(halfLetterSize.width(), halfLetterSize.height()),
                   letterSize);
    }
  return rects;
}      

//ctkAxesWidget
//-----------------------------------------------------------------------------
ctkAxesWidget::ctkAxesWidget(QWidget *newParent)
  : QWidget(newParent)
  , d_ptr(new ctkAxesWidgetPrivate(*this))
{
}

//-----------------------------------------------------------------------------
ctkAxesWidget::~ctkAxesWidget()
{
}

// ----------------------------------------------------------------------------
ctkAxesWidget::Axis ctkAxesWidget::currentAxis() const
{
  Q_D(const ctkAxesWidget);
  return d->CurrentAxis;
}

//-----------------------------------------------------------------------------
void ctkAxesWidget::setCurrentAxis(ctkAxesWidget::Axis newAxis)
{
  Q_D(ctkAxesWidget);

  if (newAxis == d->CurrentAxis)
    {
    return;
    }
  d->CurrentAxis = newAxis;
  this->update();
  emit currentAxisChanged(d->CurrentAxis);
}

//-----------------------------------------------------------------------------
void ctkAxesWidget::paintEvent(QPaintEvent *)
{
  Q_D(ctkAxesWidget);

  // init
  QPainter painter(this);
  
  //painter.setRenderHint(QPainter::Antialiasing);
  
  QPoint center = QPoint(this->width(), this->height())  / 2;
  int length = qMin(this->width(), this->height());
  int diameter = length / goldenRatio;
  int radius = diameter / 2;

  QStringList axesLabels;
  
  QList<QPoint> positions = d->extremities(center, radius);
  
  
  QFontMetrics fm = this->fontMetrics();
  QSize letterSize = fm.size(Qt::TextShowMnemonic, "X") + QSize(1,1);
  QSize halfLetterSize = letterSize / 2;
  int blankSize = (length - diameter) / 2;
  QSize betweenLetterSpace = QSize(blankSize - letterSize.width(), blankSize - letterSize.height()) / 2;
  QList<QRect>  labelRects = d->labelRects(positions, betweenLetterSpace);
  
  for (int i = 0; i < 6; ++i)
    {
    //QRect rect(positions[i] + QPoint(cos(d->AxesAngles[i]) * (betweenLetterSpace.width()+halfLetterSize.width()),
    //                                 -sin(d->AxesAngles[i]) * (betweenLetterSpace.height()+halfLetterSize.height()))
    //                                - QPoint(halfLetterSize.width(), halfLetterSize.height()), letterSize);
    QRect rect = labelRects[i];
    if (d->HighlightCurrentAxis)
      {
      QFont font = painter.font();
      font.setBold(d->CurrentAxis == (i + 1));
      painter.setFont(font);
      }
    painter.drawText(rect, Qt::AlignCenter, d->AxesLabels[i]);
    } 
  
  // Drawing the lines
  for (int i = 0; i < 6; ++i)
    {
    if (d->HighlightCurrentAxis)
      {
      QPen pen;
      if (d->CurrentAxis == (i + 1)) // axes start at 1
        {
        pen.setWidth(3);
        //pen.setColor(QColor(64, 64, 72)); // Payne's grey
        pen.setColor(this->palette().color(QPalette::Highlight));
        }
      painter.setPen(pen);
      }
    painter.drawLine(center, positions[i]);
    }
  
  QSize sphereRadius((blankSize / 2) / 1.6180339887,
                     (blankSize / 2) / 1.6180339887);
  // Draw the center sphere
  QRadialGradient rg(QPointF(0.3333, 0.3333),0.7);
  rg.setCoordinateMode(QGradient::ObjectBoundingMode);
  if (d->HighlightCurrentAxis &&
      d->CurrentAxis == ctkAxesWidget::None)
    {
    rg.setColorAt(0., this->palette().color(QPalette::Highlight));
    }
  else
    {
    rg.setColorAt(0., this->palette().color(QPalette::Light));
    }
  rg.setColorAt(1., QColor(64, 64, 72));
  painter.setBrush(QBrush(rg));
  painter.setPen(QPen(Qt::NoPen));
  painter.drawEllipse(QPointF(center), sphereRadius.width(), sphereRadius.height());
}

// ----------------------------------------------------------------------------------
void ctkAxesWidget::mousePressEvent(QMouseEvent *mouseEvent)
{
  Q_D(ctkAxesWidget);

  QPoint center = QPoint(this->width(), this->height())  / 2;
  int length = qMin(this->width(), this->height());
  int diameter = length / goldenRatio;
  int blankSize = (length - diameter) / 2;
  QSize sphereRadius((blankSize / 2) / 1.6180339887,
                     (blankSize / 2) / 1.6180339887);

  QPointF mousePos = mouseEvent->pos() - center;
  double distance2 = 
    mousePos.x() * mousePos.x() + mousePos.y() * mousePos.y();
  if (distance2 < sphereRadius.width()*sphereRadius.width())
    {
    this->setCurrentAxis(None);
    return;
    }
  
  double mouseAngle = atan2(-mousePos.y(), mousePos.x());
  // mouseAngle is in the interval [-pi,+pi] radians
  // change it to be in [-pi/8,  7/8 * pi]
  double PI_8 = 0.392699082;
  if (mouseAngle < -PI_8)
    {  
    mouseAngle += 2. * PI;
    }
  
  for (int i = 0; i < 6; ++i)
    {
    if (mouseAngle >= (d->AxesAngles[i] - PI_8) &&
        mouseAngle <= (d->AxesAngles[i] + PI_8))
      {
      // the user has clicked close on the axe
      this->setCurrentAxis(static_cast<Axis>(i+1));
      return;
      }
    }
  this->setCurrentAxis(None);  
}
