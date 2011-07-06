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

// Qt includes
#include <QDebug>
#include <QMouseEvent>

// CTK includes
#include "ctkLogger.h"
#include "ctkVTKChartView.h"

// VTK includes
#include <vtkAxis.h>
#include <vtkChartXY.h>
#include <vtkContext2D.h>
#include <vtkContextMouseEvent.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkOpenGLContextDevice2D.h>
#include <vtkPlot.h>
#include <vtkRenderWindow.h>

//----------------------------------------------------------------------------
static ctkLogger logger("org.commontk.visualization.vtk.widgets.ctkVTKChartView");
//----------------------------------------------------------------------------

class ctkVTKChartViewPrivate
{
  Q_DECLARE_PUBLIC(ctkVTKChartView);
protected:
  ctkVTKChartView* const q_ptr;
public:
  ctkVTKChartViewPrivate(ctkVTKChartView& object);
  void init();
  void chartBounds(double* bounds)const;

  vtkSmartPointer<vtkContextView> ContextView;
  vtkSmartPointer<vtkChartXY> Chart;
  double UserBounds[8];
};

// ----------------------------------------------------------------------------
// ctkVTKChartViewPrivate methods

// ----------------------------------------------------------------------------
ctkVTKChartViewPrivate::ctkVTKChartViewPrivate(ctkVTKChartView& object)
  :q_ptr(&object)
{
  this->ContextView = vtkSmartPointer<vtkContextView>::New();
  this->Chart = vtkSmartPointer<vtkChartXY>::New();
  this->ContextView->GetScene()->AddItem(this->Chart);
  this->UserBounds[0] = this->UserBounds[2] = this->UserBounds[4] = this->UserBounds[6] = 0.;
  this->UserBounds[1] = this->UserBounds[3] = this->UserBounds[5] = this->UserBounds[7] = -1.;
}

// ----------------------------------------------------------------------------
void ctkVTKChartViewPrivate::init()
{
  Q_Q(ctkVTKChartView);
  this->ContextView->SetInteractor(q->GetInteractor());
  q->SetRenderWindow(this->ContextView->GetRenderWindow());
  // low def for now (faster)
  //q->GetRenderWindow()->SetMultiSamples(0);
  //vtkOpenGLContextDevice2D::SafeDownCast(this->ContextView->GetContext()->GetDevice())
  //                                       ->SetStringRendererToQt();
  this->Chart->SetActionToButton(vtkChart::PAN, vtkContextMouseEvent::MIDDLE_BUTTON);
  this->Chart->SetActionToButton(vtkChart::SELECT, vtkContextMouseEvent::RIGHT_BUTTON);
}

// ----------------------------------------------------------------------------
void ctkVTKChartViewPrivate::chartBounds(double* bounds)const
{
  Q_Q(const ctkVTKChartView);
  bounds[0] = bounds[2] = bounds[4] = bounds[6] = VTK_DOUBLE_MAX;
  bounds[1] = bounds[3] = bounds[5] = bounds[7] = VTK_DOUBLE_MIN;
  vtkChartXY* chart = q->chart();
  const vtkIdType plotCount = chart->GetNumberOfPlots();
  for (vtkIdType i = 0; i < plotCount; ++i)
    {
    vtkPlot* plot = chart->GetPlot(i);

    int corner = chart->GetPlotCorner(plot);
    double plotBounds[4];
    plot->GetBounds(plotBounds);
    switch (corner)
      {
      // bottom left
      case 0:
        // x
        bounds[2] = bounds[2] > plotBounds[0] ?
          plotBounds[0] : bounds[2];
        bounds[3] = bounds[3] < plotBounds[1] ?
          plotBounds[1] : bounds[3];
        // y
        bounds[0] = bounds[0] > plotBounds[2] ?
          plotBounds[2] : bounds[0];
        bounds[1] = bounds[1] < plotBounds[3] ?
          plotBounds[3] : bounds[1];
        break;
      // bottom right
      case 1:
        // x
        bounds[2] = bounds[2] > plotBounds[0] ?
          plotBounds[0] : bounds[2];
        bounds[3] = bounds[3] < plotBounds[1] ?
          plotBounds[1] : bounds[3];
        // y
        bounds[4] = bounds[4] > plotBounds[2] ?
          plotBounds[2] : bounds[4];
        bounds[5] = bounds[5] < plotBounds[3] ?
          plotBounds[3] : bounds[5];
        break;
      // top right
      case 2:
        // x
        bounds[6] = bounds[6] > plotBounds[0] ?
          plotBounds[0] : bounds[6];
        bounds[7] = bounds[7] < plotBounds[1] ?
          plotBounds[1] : bounds[7];
        // y
        bounds[4] = bounds[4] > plotBounds[2] ?
          plotBounds[2] : bounds[4];
        bounds[5] = bounds[5] < plotBounds[3] ?
          plotBounds[3] : bounds[5];
        break;
      // top left
      case 3:
        // x
        bounds[6] = bounds[6] > plotBounds[0] ?
          plotBounds[0] : bounds[6];
        bounds[7] = bounds[7] < plotBounds[1] ?
          plotBounds[1] : bounds[7];
        // y
        bounds[0] = bounds[0] > plotBounds[2] ?
          plotBounds[2] : bounds[1];
        bounds[1] = bounds[0] < plotBounds[3] ?
          plotBounds[3] : bounds[1];
        break;
      }
    }
}

// ----------------------------------------------------------------------------
// ctkVTKChartView methods

// ----------------------------------------------------------------------------
ctkVTKChartView::ctkVTKChartView(QWidget* parentWidget)
  :QVTKWidget(parentWidget)
  , d_ptr(new ctkVTKChartViewPrivate(*this))
{
  Q_D(ctkVTKChartView);
  d->init();
  this->setAutomaticImageCacheEnabled(true);
}

// ----------------------------------------------------------------------------
ctkVTKChartView::~ctkVTKChartView()
{
}

// ----------------------------------------------------------------------------
void ctkVTKChartView::setTitle(const QString& newTitle)
{
  Q_D(ctkVTKChartView);
  d->Chart->SetTitle(newTitle.toLatin1().data());
}

// ----------------------------------------------------------------------------
QString ctkVTKChartView::title()const
{
  Q_D(const ctkVTKChartView);
  return QString(d->Chart->GetTitle());
}

// ----------------------------------------------------------------------------
vtkChartXY* ctkVTKChartView::chart()const
{
  Q_D(const ctkVTKChartView);
  return d->Chart;
}

// ----------------------------------------------------------------------------
vtkContextScene* ctkVTKChartView::scene()const
{
  Q_D(const ctkVTKChartView);
  return d->ContextView->GetScene();
}

// ----------------------------------------------------------------------------
void ctkVTKChartView::addPlot(vtkPlot* plot)
{
  Q_D(ctkVTKChartView);
  d->Chart->AddPlot(plot);
  emit this->plotAdded(plot);
  this->onChartUpdated();
}

// ----------------------------------------------------------------------------
void ctkVTKChartView::onChartUpdated()
{
  emit boundsChanged();
}

// ----------------------------------------------------------------------------
void ctkVTKChartView::chartBounds(double* bounds)const
{
  Q_D(const ctkVTKChartView);
  if (d->UserBounds[1] < d->UserBounds[0])
    {
    // Invalid user bounds, return the real chart bounds
    d->chartBounds(bounds);
    return;
    }
  this->chartUserBounds(bounds);
}

// ----------------------------------------------------------------------------
void ctkVTKChartView::setChartUserBounds(double* userBounds)
{
  Q_D(ctkVTKChartView);
  for (int i= 0; i < 8; ++i)
    {
    d->UserBounds[i] = userBounds[i];
    }
  this->onChartUpdated();
}

// ----------------------------------------------------------------------------
void ctkVTKChartView::chartUserBounds(double* bounds)const
{
  Q_D(const ctkVTKChartView);
  for (int i= 0; i < 8; ++i)
    {
    bounds[i] = d->UserBounds[i];
    }
}

// ----------------------------------------------------------------------------
void ctkVTKChartView::setAxesToChartBounds()
{
  vtkChartXY* chart = this->chart();
  double bounds[8];
  this->chartBounds(bounds);
  for (int i = 0; i < chart->GetNumberOfAxes(); ++i)
    {
    if (bounds[2*i] != VTK_DOUBLE_MAX)
      {
      chart->GetAxis(i)->SetRange(bounds[2*i], bounds[2*i+1]);
      chart->GetAxis(i)->SetBehavior(1);
      }
    }
}

// ----------------------------------------------------------------------------
void ctkVTKChartView::boundAxesToChartBounds()
{
  vtkChartXY* chart = this->chart();
  double bounds[8];
  this->chartBounds(bounds);
  for (int i = 0; i < chart->GetNumberOfAxes(); ++i)
    {
    if (bounds[2*i] != VTK_DOUBLE_MAX)
      {
      chart->GetAxis(i)->SetMinimumLimit(bounds[2*i]);
      chart->GetAxis(i)->SetMaximumLimit(bounds[2*i + 1]);
      }
    }
}

// ----------------------------------------------------------------------------
void ctkVTKChartView::chartBoundsToPlotBounds(double bounds[8], double plotBounds[4])const
{
  plotBounds[0] = bounds[vtkAxis::BOTTOM*2];
  plotBounds[1] = bounds[vtkAxis::BOTTOM*2 + 1];
  plotBounds[2] = bounds[vtkAxis::LEFT*2];
  plotBounds[3] = bounds[vtkAxis::LEFT*2+1];
}

// ----------------------------------------------------------------------------
void ctkVTKChartView::mouseDoubleClickEvent(QMouseEvent* event)
{
  if (event->button() == Qt::MidButton)
    {
    this->setAxesToChartBounds();
    }
  this->QVTKWidget::mouseDoubleClickEvent(event);
}
