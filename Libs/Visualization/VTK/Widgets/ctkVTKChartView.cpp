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
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkOpenGLContextDevice2D.h>
#include <vtkPlot.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

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

#ifdef CTK_USE_QVTKOPENGLWIDGET
  vtkSmartPointer<vtkGenericOpenGLRenderWindow> RenderWindow;
#endif
  vtkSmartPointer<vtkContextView> ContextView;
  vtkSmartPointer<vtkChartXY> Chart;
  double UserBounds[8];
  mutable double OldBounds[8];
};

// ----------------------------------------------------------------------------
// ctkVTKChartViewPrivate methods

// ----------------------------------------------------------------------------
ctkVTKChartViewPrivate::ctkVTKChartViewPrivate(ctkVTKChartView& object)
  :q_ptr(&object)
{
  this->ContextView = vtkSmartPointer<vtkContextView>::New();
#ifdef CTK_USE_QVTKOPENGLWIDGET
  this->RenderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
#endif
  this->Chart = vtkSmartPointer<vtkChartXY>::New();
  this->ContextView->GetScene()->AddItem(this->Chart);
  this->UserBounds[0] = this->UserBounds[2] = this->UserBounds[4] = this->UserBounds[6] = 0.;
  this->UserBounds[1] = this->UserBounds[3] = this->UserBounds[5] = this->UserBounds[7] = -1.;
  this->OldBounds[0] = this->OldBounds[2] = this->OldBounds[4] = this->OldBounds[6] = 0.;
  this->OldBounds[1] = this->OldBounds[3] = this->OldBounds[5] = this->OldBounds[7] = -1.;
}

// ----------------------------------------------------------------------------
void ctkVTKChartViewPrivate::init()
{
  Q_Q(ctkVTKChartView);

#ifdef CTK_USE_QVTKOPENGLWIDGET
# if VTK_MAJOR_VERSION >= 9 || (VTK_MAJOR_VERSION >= 8 && VTK_MINOR_VERSION >= 90)
  q->setRenderWindow(this->RenderWindow);
# else
  q->SetRenderWindow(this->RenderWindow);
# endif
  this->ContextView->SetRenderWindow(this->RenderWindow);
#endif

#if VTK_MAJOR_VERSION >= 9 || (VTK_MAJOR_VERSION >= 8 && VTK_MINOR_VERSION >= 90)
  this->ContextView->SetInteractor(q->interactor());
  q->setRenderWindow(this->ContextView->GetRenderWindow());
#else
  this->ContextView->SetInteractor(q->GetInteractor());
  q->SetRenderWindow(this->ContextView->GetRenderWindow());
#endif

  // low def for now (faster)
  //q->GetRenderWindow()->SetMultiSamples(0);
  //vtkOpenGLContextDevice2D::SafeDownCast(this->ContextView->GetContext()->GetDevice())
  //                                       ->SetStringRendererToQt();
#ifndef Q_WS_X11
# if VTK_MAJOR_VERSION >= 9 || (VTK_MAJOR_VERSION >= 8 && VTK_MINOR_VERSION >= 90)
  q->renderWindow()->SetLineSmoothing(true);
# else
  q->GetRenderWindow()->SetLineSmoothing(true);
# endif
#endif
  this->Chart->SetActionToButton(vtkChart::PAN, vtkContextMouseEvent::MIDDLE_BUTTON);
  this->Chart->SetActionToButton(vtkChart::SELECT, vtkContextMouseEvent::RIGHT_BUTTON);

  q->qvtkConnect(q->chart()->GetAxis(vtkAxis::BOTTOM),vtkCommand::ModifiedEvent,
                    q, SIGNAL(extentChanged()));
  q->qvtkConnect(q->chart()->GetAxis(vtkAxis::LEFT),vtkCommand::ModifiedEvent,
                    q, SIGNAL(extentChanged()));

}

// ----------------------------------------------------------------------------
void ctkVTKChartViewPrivate::chartBounds(double* bounds)const
{
  if (!bounds)
    {
    return;
    }
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
        bounds[2] = bounds[2] > plotBounds[0] ? plotBounds[0] : bounds[2];
        bounds[3] = bounds[3] < plotBounds[1] ? plotBounds[1] : bounds[3];
        // y
        bounds[0] = bounds[0] > plotBounds[2] ? plotBounds[2] : bounds[0];
        bounds[1] = bounds[1] < plotBounds[3] ? plotBounds[3] : bounds[1];
        break;
      // bottom right
      case 1:
        // x
        bounds[2] = bounds[2] > plotBounds[0] ? plotBounds[0] : bounds[2];
        bounds[3] = bounds[3] < plotBounds[1] ? plotBounds[1] : bounds[3];
        // y
        bounds[4] = bounds[4] > plotBounds[2] ? plotBounds[2] : bounds[4];
        bounds[5] = bounds[5] < plotBounds[3] ? plotBounds[3] : bounds[5];
        break;
      // top right
      case 2:
        // x
        bounds[6] = bounds[6] > plotBounds[0] ? plotBounds[0] : bounds[6];
        bounds[7] = bounds[7] < plotBounds[1] ? plotBounds[1] : bounds[7];
        // y
        bounds[4] = bounds[4] > plotBounds[2] ? plotBounds[2] : bounds[4];
        bounds[5] = bounds[5] < plotBounds[3] ? plotBounds[3] : bounds[5];
        break;
      // top left
      case 3:
        // x
        bounds[6] = bounds[6] > plotBounds[0] ? plotBounds[0] : bounds[6];
        bounds[7] = bounds[7] < plotBounds[1] ? plotBounds[1] : bounds[7];
        // y
        bounds[0] = bounds[0] > plotBounds[2] ? plotBounds[2] : bounds[1];
        bounds[1] = bounds[0] < plotBounds[3] ? plotBounds[3] : bounds[1];
        break;
      }
    }
}

// ----------------------------------------------------------------------------
// ctkVTKChartView methods

CTK_GET_CPP(ctkVTKChartView, vtkRenderWindow*, renderWindow, RenderWindow);

// ----------------------------------------------------------------------------
ctkVTKChartView::ctkVTKChartView(QWidget* parentWidget)
  :Superclass(parentWidget)
  , d_ptr(new ctkVTKChartViewPrivate(*this))
{
  Q_D(ctkVTKChartView);
  d->init();
  //this->setAutomaticImageCacheEnabled(true);
}

// ----------------------------------------------------------------------------
ctkVTKChartView::~ctkVTKChartView()
{
}

// ----------------------------------------------------------------------------
void ctkVTKChartView::setTitle(const QString& newTitle)
{
  Q_D(ctkVTKChartView);
  d->Chart->SetTitle(newTitle.toUtf8().data());
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
void ctkVTKChartView::removePlot(vtkPlot* plot)
{
  Q_D(ctkVTKChartView);
  vtkIdType index = this->plotIndex(plot);
  if (index == vtkIdType(-1))
    {
    return;
    }
  d->Chart->RemovePlot(index);
  emit this->plotRemoved(plot);
  this->onChartUpdated();
}

// ----------------------------------------------------------------------------
void ctkVTKChartView::removeAllPlots()
{
  Q_D(ctkVTKChartView);
  while(d->Chart->GetNumberOfPlots() > 0)
    {
    this->removePlot(d->Chart->GetPlot(0));
    }
}

// ----------------------------------------------------------------------------
vtkIdType ctkVTKChartView::plotIndex(vtkPlot* plot)
{
  Q_D(ctkVTKChartView);
  // GetPlotIndex is missing from vtkChart API
  for (vtkIdType i = 0; i < d->Chart->GetNumberOfPlots(); ++i)
    {
    if (plot == d->Chart->GetPlot(i))
      {
      return i;
      }
    }
  return -1;
}

// ----------------------------------------------------------------------------
void ctkVTKChartView::onChartUpdated()
{
  Q_D(ctkVTKChartView);
  double oldBounds[8];
  memcpy(oldBounds, d->OldBounds, 8 * sizeof(double));
  double newBounds[8];
  this->chartBounds(newBounds);
  if (oldBounds[0] != newBounds[0] ||
      oldBounds[1] != newBounds[1] ||
      oldBounds[2] != newBounds[2] ||
      oldBounds[3] != newBounds[3] ||
      oldBounds[4] != newBounds[4] ||
      oldBounds[5] != newBounds[5] ||
      oldBounds[6] != newBounds[6] ||
      oldBounds[7] != newBounds[7])
    {
    emit boundsChanged();
    }
}

// ----------------------------------------------------------------------------
void ctkVTKChartView::chartExtent(double* extent)const
{
  if (!extent)
    {
    return;
    }
  extent[0] = extent[2] = extent[4] = extent[6] = VTK_DOUBLE_MAX;
  extent[1] = extent[3] = extent[5] = extent[7] = VTK_DOUBLE_MIN;
  vtkChartXY* chart = this->chart();
  vtkAxis* axis = chart->GetAxis(vtkAxis::BOTTOM);
  extent[0] = qMin(axis->GetMinimum(), extent[0]);
  extent[1] = qMax(axis->GetMaximum(), extent[1]);
  axis = chart->GetAxis(vtkAxis::LEFT);
  extent[2] = qMin(axis->GetMinimum(), extent[2]);
  extent[3] = qMax(axis->GetMaximum(), extent[3]);
  axis = chart->GetAxis(vtkAxis::TOP);
  extent[4] = qMin(axis->GetMinimum(), extent[4]);
  extent[5] = qMax(axis->GetMaximum(), extent[5]);
  axis = chart->GetAxis(vtkAxis::RIGHT);
  extent[6] = qMin(axis->GetMinimum(), extent[6]);
  extent[7] = qMax(axis->GetMaximum(), extent[7]);
}

// ----------------------------------------------------------------------------
void ctkVTKChartView::setChartUserExtent(double* userExtent)
{
  if (!userExtent)
    {
    qCritical() << Q_FUNC_INFO << ": Invalid user extent";
    return;
    }
  vtkChartXY* chart = this->chart();
  vtkAxis* axis = chart->GetAxis(vtkAxis::BOTTOM);
  axis->SetRange(userExtent[0], userExtent[1]);
  axis = chart->GetAxis(vtkAxis::LEFT);
  axis->SetRange(userExtent[2], userExtent[3]);
  axis = chart->GetAxis(vtkAxis::TOP);
  axis->SetRange(userExtent[4], userExtent[5]);
  axis = chart->GetAxis(vtkAxis::RIGHT);
  axis->SetRange(userExtent[6], userExtent[7]);
}

// ----------------------------------------------------------------------------
void ctkVTKChartView::chartBounds(double* bounds)const
{
  Q_D(const ctkVTKChartView);
  if (d->UserBounds[1] < d->UserBounds[0])
    {
    // Invalid user bounds, return the real chart bounds
    d->chartBounds(bounds);
    }
  else
    {
    this->chartUserBounds(bounds);
    }
  memcpy(d->OldBounds, bounds, 8 * sizeof(double));
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
  this->Superclass::mouseDoubleClickEvent(event);
}
