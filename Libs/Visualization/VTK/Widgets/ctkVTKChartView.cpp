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

// CTK includes
#include "ctkLogger.h"
#include "ctkVTKChartView.h"

// VTK includes
#include <vtkAxis.h>
#include <vtkChartXY.h>
#include <vtkContext2D.h>
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

  vtkSmartPointer<vtkContextView> ContextView;
  vtkSmartPointer<vtkChartXY> Chart;
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
void ctkVTKChartView::addPlot(vtkPlot* plot)
{
  Q_D(ctkVTKChartView);
  d->Chart->AddPlot(plot);
}

// ----------------------------------------------------------------------------
void ctkVTKChartView::fitAxesToBounds()
{
  vtkChartXY* chart = this->chart();
  const vtkIdType plotCount = chart->GetNumberOfPlots();
  double extremaBounds[8] = {VTK_DOUBLE_MAX, VTK_DOUBLE_MIN,
                             VTK_DOUBLE_MAX, VTK_DOUBLE_MIN,
                             VTK_DOUBLE_MAX, VTK_DOUBLE_MIN};
  for (vtkIdType i = 0; i < plotCount; ++i)
    {
    vtkPlot* plot = chart->GetPlot(i);

    int corner = chart->GetPlotCorner(plot);
    double bounds[4];
    plot->GetBounds(bounds);
    switch (corner)
      {
      // bottom left
      case 0:
        // x
        extremaBounds[2] = extremaBounds[2] > bounds[0] ?
          bounds[0] : extremaBounds[2];
        extremaBounds[3] = extremaBounds[3] < bounds[1] ?
          bounds[1] : extremaBounds[3];
        // y
        extremaBounds[0] = extremaBounds[0] > bounds[2] ?
          bounds[2] : extremaBounds[0];
        extremaBounds[1] = extremaBounds[1] < bounds[3] ?
          bounds[3] : extremaBounds[1];
        break;
      // bottom right
      case 1:
        // x
        extremaBounds[2] = extremaBounds[2] > bounds[0] ?
          bounds[0] : extremaBounds[2];
        extremaBounds[3] = extremaBounds[3] < bounds[1] ?
          bounds[1] : extremaBounds[3];
        // y
        extremaBounds[4] = extremaBounds[4] > bounds[2] ?
          bounds[2] : extremaBounds[4];
        extremaBounds[5] = extremaBounds[5] < bounds[3] ?
          bounds[3] : extremaBounds[5];
        break;
      // top right
      case 2:
        // x
        extremaBounds[6] = extremaBounds[6] > bounds[0] ?
          bounds[0] : extremaBounds[6];
        extremaBounds[7] = extremaBounds[7] < bounds[1] ?
          bounds[1] : extremaBounds[7];
        // y
        extremaBounds[4] = extremaBounds[4] > bounds[2] ?
          bounds[2] : extremaBounds[4];
        extremaBounds[5] = extremaBounds[5] < bounds[3] ?
          bounds[3] : extremaBounds[5];
        break;
      // top left
      case 3:
        // x
        extremaBounds[6] = extremaBounds[6] > bounds[0] ?
          bounds[0] : extremaBounds[6];
        extremaBounds[7] = extremaBounds[7] < bounds[1] ?
          bounds[1] : extremaBounds[7];
        // y
        extremaBounds[0] = extremaBounds[0] > bounds[2] ?
          bounds[2] : extremaBounds[1];
        extremaBounds[1] = extremaBounds[0] < bounds[3] ?
          bounds[3] : extremaBounds[1];
        break;
      }
    }
  for (int i = 0; i < chart->GetNumberOfAxes(); ++i)
    {
    if (extremaBounds[2*i] != VTK_DOUBLE_MAX)
      {
      chart->GetAxis(i)->SetRange(extremaBounds[2*i], extremaBounds[2*i+1]);
      chart->GetAxis(i)->SetBehavior(1);
      }
    }
}
