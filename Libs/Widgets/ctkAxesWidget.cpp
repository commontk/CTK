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

// Qt includes
#include <QDebug>
#include <QBrush>
#include <QGridLayout>
#include <QLine>
#include <QMouseEvent>
#include <QPainter>

// CTK includes
#include "ctkAxesWidget.h"

// STD includes
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
  ctkAxesWidget::Axis axisAtPos(QPoint pos)const;

  ctkAxesWidget::Axis CurrentAxis;
  ctkAxesWidget::Axis HighlightAxis;
  bool AutoReset;
  
  QStringList AxesLabels;
  QVector<double> AxesAngles;
  
};

//-----------------------------------------------------------------------------
ctkAxesWidgetPrivate::ctkAxesWidgetPrivate(ctkAxesWidget& object)
  :q_ptr(&object)
{
  qRegisterMetaType<ctkAxesWidget::Axis>("ctkAxesWidget::Axis");
  this->CurrentAxis = ctkAxesWidget::None;
  this->HighlightAxis = ctkAxesWidget::None;
  this->AutoReset = false;

  this->AxesLabels << "R" << "L" << "S" << "I" << "A" << "P";
  this->AxesAngles << 3.14159265 << 0 << 1.57079633 <<  4.71238898 << 5.49778714 << 2.35619449;
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

//-----------------------------------------------------------------------------
ctkAxesWidget::Axis ctkAxesWidgetPrivate::axisAtPos(QPoint pos)const
{
  Q_Q(const ctkAxesWidget);

  QPoint center = QPoint(q->width(), q->height())  / 2;
  int length = qMin(q->width(), q->height());
  int diameter = length / goldenRatio;
  int blankSize = (length - diameter) / 2;
  QSize sphereRadius((blankSize / 2) / 1.6180339887,
                     (blankSize / 2) / 1.6180339887);

  QPointF mousePos = pos - center;
  double distance2 = 
    mousePos.x() * mousePos.x() + mousePos.y() * mousePos.y();
  if (distance2 < sphereRadius.width()*sphereRadius.width())
    {
    return ctkAxesWidget::None;
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
    if (mouseAngle >= (this->AxesAngles[i] - PI_8) &&
        mouseAngle <= (this->AxesAngles[i] + PI_8))
      {
      // the mouse is over the axis
      return static_cast<ctkAxesWidget::Axis>(i+1);
      }
    }
  return ctkAxesWidget::None;
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
  d->HighlightAxis = newAxis;
  if (newAxis == d->CurrentAxis)
    {
    return;
    }
  d->CurrentAxis = newAxis;
  this->repaint();
  emit currentAxisChanged(d->CurrentAxis);
}

//-----------------------------------------------------------------------------
void ctkAxesWidget::setCurrentAxisToNone()
{
  this->setCurrentAxis(ctkAxesWidget::None);
}

// ----------------------------------------------------------------------------
bool ctkAxesWidget::autoReset() const
{
  Q_D(const ctkAxesWidget);
  return d->AutoReset;
}

// ----------------------------------------------------------------------------
void ctkAxesWidget::setAutoReset(bool newAutoReset)
{
  Q_D(ctkAxesWidget);
  if (d->AutoReset == newAutoReset)
    {
    return;
    }
  d->AutoReset = newAutoReset;
  if (d->AutoReset)
    {
    connect(this, SIGNAL(currentAxisChanged(ctkAxesWidget::Axis)),
            this, SLOT(setCurrentAxisToNone()));
    setCurrentAxisToNone();
    }
  else
    {
    disconnect(this, SIGNAL(currentAxisChanged(ctkAxesWidget::Axis)),
               this, SLOT(setCurrentAxisToNone()));
    }
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
  
  QList<QPoint> positions = d->extremities(center, radius);
  
  
  QFontMetrics fm = this->fontMetrics();
  QSize letterSize = fm.size(Qt::TextShowMnemonic, "X") + QSize(1,1);
  //QSize halfLetterSize = letterSize / 2;
  int blankSize = (length - diameter) / 2;
  QSize betweenLetterSpace = QSize(blankSize - letterSize.width(), blankSize - letterSize.height()) / 2;
  QList<QRect>  labelRects = d->labelRects(positions, betweenLetterSpace);
  
  for (int i = 0; i < 6; ++i)
    {
    //QRect rect(positions[i] + QPoint(cos(d->AxesAngles[i]) * (betweenLetterSpace.width()+halfLetterSize.width()),
    //                                 -sin(d->AxesAngles[i]) * (betweenLetterSpace.height()+halfLetterSize.height()))
    //                                - QPoint(halfLetterSize.width(), halfLetterSize.height()), letterSize);
    QRect rect = labelRects[i];
    //if (d->HighlightAxes)
      {
      QFont font = painter.font();
      font.setBold(d->HighlightAxis == (i + 1));
      painter.setFont(font);
      }
    painter.drawText(rect, Qt::AlignCenter, d->AxesLabels[i]);
    } 
  
  // Drawing the lines
  for (int i = 0; i < 6; ++i)
    {
    //if (d->HighlightAxes)
      {
      QPen pen;
      if (d->HighlightAxis == (i + 1)) // axes start at 1
        {
        pen.setWidth(3);
        //pen.setColor(QColor(64, 64, 72)); // Payne's grey
        pen.setColor(this->palette().color(QPalette::Active, QPalette::Highlight));
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
  if (//d->HighlightAxes &&
      d->HighlightAxis == ctkAxesWidget::None)
    {
    rg.setColorAt(0., this->palette().color(QPalette::Active, QPalette::Highlight));
    }
  else
    {
    rg.setColorAt(0., this->palette().color(QPalette::Active, QPalette::Light));
    }
  rg.setColorAt(1., QColor(64, 64, 72));
  painter.setBrush(QBrush(rg));
  painter.setPen(QPen(Qt::NoPen));
  painter.drawEllipse(QPointF(center), sphereRadius.width(), sphereRadius.height());
}

// ----------------------------------------------------------------------------------
bool ctkAxesWidget::setAxesLabels(const QStringList& labels)
{
  Q_D(ctkAxesWidget);
  if (labels.size() < 6)
    {
    qWarning("ctkAxesWidget::setAxesLabels() failed: At least 6 labels are expected.");
    return false;
    }

  if (labels == d->AxesLabels)
    {
    return true;
    }

  d->AxesLabels = labels.mid(0, 6);
  this->repaint();
  return true;
}

// ----------------------------------------------------------------------------------
QStringList ctkAxesWidget::axesLabels() const
{
  Q_D(const ctkAxesWidget);
  return d->AxesLabels;
}

// ----------------------------------------------------------------------------------
void ctkAxesWidget::mousePressEvent(QMouseEvent *mouseEvent)
{
  Q_D(ctkAxesWidget);
  d->HighlightAxis = d->axisAtPos(mouseEvent->pos());
  this->update();
}

// ----------------------------------------------------------------------------------
void ctkAxesWidget::mouseMoveEvent(QMouseEvent *mouseEvent)
{
  Q_D(ctkAxesWidget);
  d->HighlightAxis = d->axisAtPos(mouseEvent->pos());
  this->update();
}

// ----------------------------------------------------------------------------------
void ctkAxesWidget::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
  Q_D(ctkAxesWidget);
  this->setCurrentAxis(d->axisAtPos(mouseEvent->pos()));
}

// --------------------------------------------------------------------------
QSize ctkAxesWidget::minimumSizeHint()const
{
  // Pretty arbitrary size.
  return QSize(100, 100);
}

// --------------------------------------------------------------------------
QSize ctkAxesWidget::sizeHint()const
{
  // Pretty arbitrary size
  return QSize(100, 100);
}

//----------------------------------------------------------------------------
bool ctkAxesWidget::hasHeightForWidth()const
{
  return true;
}

//----------------------------------------------------------------------------
int ctkAxesWidget::heightForWidth(int width)const
{
  // Tends to be square
  return width;
}

