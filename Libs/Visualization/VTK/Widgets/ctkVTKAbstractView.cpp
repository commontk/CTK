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
#include <QTimer>
#include <QVBoxLayout>
#include <QDebug>

// CTK includes
#include "ctkVTKAbstractView.h"
#include "ctkVTKAbstractView_p.h"
#include "ctkLogger.h"

// VTK includes
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTextProperty.h>

//--------------------------------------------------------------------------
static ctkLogger logger("org.commontk.visualization.vtk.widgets.ctkVTKAbstractView");
//--------------------------------------------------------------------------
int ctkVTKAbstractViewPrivate::MultiSamples = 0;  // Default for static var
//--------------------------------------------------------------------------

// --------------------------------------------------------------------------
// ctkVTKAbstractViewPrivate methods

// --------------------------------------------------------------------------
ctkVTKAbstractViewPrivate::ctkVTKAbstractViewPrivate(ctkVTKAbstractView& object)
  : q_ptr(&object)
{
#ifdef CTK_USE_QVTKOPENGLWIDGET
  this->RenderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
#else
  this->RenderWindow = vtkSmartPointer<vtkRenderWindow>::New();
#endif
  this->CornerAnnotation = vtkSmartPointer<vtkCornerAnnotation>::New();
  this->RequestTimer = 0;
  this->RenderEnabled = true;
  this->MaximumUpdateRate = 60.0;
  this->FPSVisible = false;
  this->FPSTimer = 0;
  this->FPS = 0;
  this->PauseRenderCount = 0;
}

// --------------------------------------------------------------------------
void ctkVTKAbstractViewPrivate::init()
{
  Q_Q(ctkVTKAbstractView);

  this->setParent(q);
  this->VTKWidget = new ctkVTKOpenGLNativeWidget;
#ifdef CTK_USE_QVTKOPENGLWIDGET
  this->VTKWidget->setEnableHiDPI(true);
  QObject::connect(this->VTKWidget, SIGNAL(resized()),
                   q, SLOT(forceRender()));
#endif
  q->setLayout(new QVBoxLayout);
  q->layout()->setMargin(0);
  q->layout()->setSpacing(0);
  q->layout()->addWidget(this->VTKWidget);

  this->RequestTimer = new QTimer(q);
  this->RequestTimer->setSingleShot(true);
  QObject::connect(this->RequestTimer, SIGNAL(timeout()),
                   q, SLOT(requestRender()));

  this->FPSTimer = new QTimer(q);
  this->FPSTimer->setInterval(1000);
  QObject::connect(this->FPSTimer, SIGNAL(timeout()),
                   q, SLOT(updateFPS()));

  this->setupCornerAnnotation();
  this->setupRendering();

  // block renders and observe interactor to enforce framerate
  q->setInteractor(this->RenderWindow->GetInteractor());
}

// --------------------------------------------------------------------------
void ctkVTKAbstractViewPrivate::setupCornerAnnotation()
{
  this->CornerAnnotation->SetMaximumLineHeight(0.07);
  vtkTextProperty *tprop = this->CornerAnnotation->GetTextProperty();
  tprop->ShadowOn();
  this->CornerAnnotation->ClearAllTexts();
}

//---------------------------------------------------------------------------
void ctkVTKAbstractViewPrivate::setupRendering()
{
  Q_ASSERT(this->RenderWindow);
  this->RenderWindow->SetAlphaBitPlanes(1);
  int nSamples = ctkVTKAbstractView::multiSamples();
  if (nSamples < 0)
    {
    nSamples = vtkOpenGLRenderWindow::GetGlobalMaximumNumberOfMultiSamples();
    }
  this->RenderWindow->SetMultiSamples(nSamples);
  this->RenderWindow->StereoCapableWindowOn();
#if VTK_MAJOR_VERSION >= 9 || (VTK_MAJOR_VERSION >= 8 && VTK_MINOR_VERSION >= 90)
  this->VTKWidget->setRenderWindow(this->RenderWindow);
#else
  this->VTKWidget->SetRenderWindow(this->RenderWindow);
#endif
}

//---------------------------------------------------------------------------
QList<vtkRenderer*> ctkVTKAbstractViewPrivate::renderers()const
{
  QList<vtkRenderer*> rendererList;

  vtkRendererCollection* rendererCollection = this->RenderWindow->GetRenderers();
  vtkCollectionSimpleIterator rendererIterator;
  rendererCollection->InitTraversal(rendererIterator);
  vtkRenderer *renderer;
  while ( (renderer= rendererCollection->GetNextRenderer(rendererIterator)) )
    {
    rendererList << renderer;
    }
  return rendererList;
}

//---------------------------------------------------------------------------
vtkRenderer* ctkVTKAbstractViewPrivate::firstRenderer()const
{
  return static_cast<vtkRenderer*>(this->RenderWindow->GetRenderers()
    ->GetItemAsObject(0));
}

//---------------------------------------------------------------------------
// ctkVTKAbstractView methods

// --------------------------------------------------------------------------
ctkVTKAbstractView::ctkVTKAbstractView(QWidget* parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new ctkVTKAbstractViewPrivate(*this))
{
  Q_D(ctkVTKAbstractView);
  d->init();
}

// --------------------------------------------------------------------------
ctkVTKAbstractView::ctkVTKAbstractView(ctkVTKAbstractViewPrivate* pimpl, QWidget* parentWidget)
  : Superclass(parentWidget)
  , d_ptr(pimpl)
{
  // derived classes must call init manually. Calling init() here may results in
  // actions on a derived public class not yet finished to be created
}

//----------------------------------------------------------------------------
ctkVTKAbstractView::~ctkVTKAbstractView()
{
}

//----------------------------------------------------------------------------
void ctkVTKAbstractView::scheduleRender()
{
  Q_D(ctkVTKAbstractView);

  //logger.trace(QString("scheduleRender - RenderEnabled: %1 - Request render elapsed: %2ms").
  //             arg(d->RenderEnabled ? "true" : "false")
  //             .arg(d->RequestTime.elapsed()));

  if (!d->RenderEnabled)
    {
    return;
    }

  double msecsBeforeRender = 0;
  // If the MaximumUpdateRate is set to 0 then it indicates that rendering is done next time
  // the application is idle.
  if (d->MaximumUpdateRate > 0.0)
    {
    msecsBeforeRender = 1000. / d->MaximumUpdateRate;
    }
  if(d->VTKWidget->testAttribute(Qt::WA_WState_InPaintEvent))
    {
    // If the request comes from the system (widget exposed, resized...), the
    // render must be done immediately.
    this->requestRender();
    }
  else if (!d->RequestTime.isValid())
    {
    d->RequestTime.start();
    d->RequestTimer->start(static_cast<int>(msecsBeforeRender));
    }
  else if (d->RequestTime.elapsed() > msecsBeforeRender)
    {
    // The rendering hasn't still be done, but msecsBeforeRender milliseconds
    // have already been elapsed, it is likely that RequestTimer has already
    // timed out, but the event queue hasn't been processed yet, rendering is
    // done now to ensure the desired framerate is respected.
    this->requestRender();
    }
}

//----------------------------------------------------------------------------
void ctkVTKAbstractView::requestRender()
{
  Q_D(const ctkVTKAbstractView);

  if (this->isRenderPaused())
    {
    return;
    }
  this->forceRender();
}

//----------------------------------------------------------------------------
void ctkVTKAbstractView::forceRender()
{
  Q_D(ctkVTKAbstractView);

  if (this->sender() == d->RequestTimer  &&
      !d->RequestTime.isValid())
    {
    // The slot associated to the timeout signal is now called, however the
    // render has already been executed meanwhile. There is no need to do it
    // again.
    return;
    }

  // The timer can be stopped if it hasn't timed out yet.
  d->RequestTimer->stop();
  d->RequestTime = QTime();

  //logger.trace(QString("forceRender - RenderEnabled: %1")
  //             .arg(d->RenderEnabled ? "true" : "false"));

  if (!d->RenderEnabled || !this->isVisible())
    {
    return;
    }
  d->RenderWindow->Render();
}

//----------------------------------------------------------------------------
bool ctkVTKAbstractView::isRenderPaused()const
{
  Q_D(const ctkVTKAbstractView);
  return d->PauseRenderCount > 0;
}

//----------------------------------------------------------------------------
int ctkVTKAbstractView::pauseRender()
{
  Q_D(ctkVTKAbstractView);
  ++d->PauseRenderCount;
  return d->PauseRenderCount;
}

//----------------------------------------------------------------------------
int ctkVTKAbstractView::resumeRender()
{
  Q_D(ctkVTKAbstractView);
  if (d->PauseRenderCount > 0)
    {
    --d->PauseRenderCount;
    }
  else
    {
    qWarning() << Q_FUNC_INFO << "Cannot resume rendering, pause render count is already 0!";
    }

  // If the rendering is not paused and has been scheduled, call scheduleRender
  if (!this->isRenderPaused() && d->RequestTimer && d->RequestTime.isValid())
    {
    this->scheduleRender();
    }
  return d->PauseRenderCount;
}

//----------------------------------------------------------------------------
int ctkVTKAbstractView::setRenderPaused(bool pause)
{
  Q_D(const ctkVTKAbstractView);

  if (pause)
    {
    this->pauseRender();
    }
  else
    {
    this->resumeRender();
    }
  return d->PauseRenderCount;
}

//----------------------------------------------------------------------------
CTK_GET_CPP(ctkVTKAbstractView, vtkRenderWindow*, renderWindow, RenderWindow);

//----------------------------------------------------------------------------
void ctkVTKAbstractView::setInteractor(vtkRenderWindowInteractor* newInteractor)
{
  Q_D(ctkVTKAbstractView);

  d->RenderWindow->SetInteractor(newInteractor);
  // Prevent the interactor to call Render() on the render window; only
  // scheduleRender() and forceRender() can Render() the window.
  // This is done to ensure the desired framerate is respected.
  newInteractor->SetEnableRender(false);
  qvtkReconnect(d->RenderWindow->GetInteractor(), newInteractor,
                vtkCommand::RenderEvent, this, SLOT(scheduleRender()));
}

//----------------------------------------------------------------------------
vtkRenderWindowInteractor* ctkVTKAbstractView::interactor()const
{
  Q_D(const ctkVTKAbstractView);
  return d->RenderWindow->GetInteractor();
}

//----------------------------------------------------------------------------
vtkInteractorObserver* ctkVTKAbstractView::interactorStyle()const
{
  return this->interactor() ?
    this->interactor()->GetInteractorStyle() : 0;
}

//----------------------------------------------------------------------------
void ctkVTKAbstractView::setCornerAnnotationText(const QString& text)
{
  Q_D(ctkVTKAbstractView);
  d->CornerAnnotation->ClearAllTexts();
  d->CornerAnnotation->SetText(2, text.toUtf8());
}

//----------------------------------------------------------------------------
QString ctkVTKAbstractView::cornerAnnotationText() const
{
  Q_D(const ctkVTKAbstractView);
  return QString::fromUtf8(d->CornerAnnotation->GetText(2));
}

//----------------------------------------------------------------------------
vtkCornerAnnotation* ctkVTKAbstractView::cornerAnnotation() const
{
  Q_D(const ctkVTKAbstractView);
  return d->CornerAnnotation;
}

//----------------------------------------------------------------------------
ctkVTKOpenGLNativeWidget * ctkVTKAbstractView::VTKWidget() const
{
  Q_D(const ctkVTKAbstractView);
  return d->VTKWidget;
}

//----------------------------------------------------------------------------
CTK_SET_CPP(ctkVTKAbstractView, bool, setRenderEnabled, RenderEnabled);
CTK_GET_CPP(ctkVTKAbstractView, bool, renderEnabled, RenderEnabled);

//----------------------------------------------------------------------------
QSize ctkVTKAbstractView::minimumSizeHint()const
{
  // Arbitrary size. 50x50 because smaller seems too small.
  return QSize(50, 50);
}

//----------------------------------------------------------------------------
QSize ctkVTKAbstractView::sizeHint()const
{
  // Arbitrary size. 300x300 is the default vtkRenderWindow size.
  return QSize(300, 300);
}

//----------------------------------------------------------------------------
bool ctkVTKAbstractView::hasHeightForWidth()const
{
  return true;
}

//----------------------------------------------------------------------------
int ctkVTKAbstractView::heightForWidth(int width)const
{
  // typically VTK render window tend to be square...
  return width;
}

//----------------------------------------------------------------------------
void ctkVTKAbstractView::setBackgroundColor(const QColor& newBackgroundColor)
{
  Q_D(ctkVTKAbstractView);
  double color[3];
  color[0] = newBackgroundColor.redF();
  color[1] = newBackgroundColor.greenF();
  color[2] = newBackgroundColor.blueF();
  foreach(vtkRenderer* renderer, d->renderers())
    {
    renderer->SetBackground(color);
    }
}

//----------------------------------------------------------------------------
QColor ctkVTKAbstractView::backgroundColor()const
{
  Q_D(const ctkVTKAbstractView);
  vtkRenderer* firstRenderer = d->firstRenderer();
  return firstRenderer ? QColor::fromRgbF(firstRenderer->GetBackground()[0],
                                          firstRenderer->GetBackground()[1],
                                          firstRenderer->GetBackground()[2])
                       : QColor();
}

//----------------------------------------------------------------------------
void ctkVTKAbstractView::setBackgroundColor2(const QColor& newBackgroundColor)
{
  Q_D(ctkVTKAbstractView);
  double color[3];
  color[0] = newBackgroundColor.redF();
  color[1] = newBackgroundColor.greenF();
  color[2] = newBackgroundColor.blueF();
  foreach(vtkRenderer* renderer, d->renderers())
    {
    renderer->SetBackground2(color);
    }
}

//----------------------------------------------------------------------------
QColor ctkVTKAbstractView::backgroundColor2()const
{
  Q_D(const ctkVTKAbstractView);
  vtkRenderer* firstRenderer = d->firstRenderer();
  return firstRenderer ? QColor::fromRgbF(firstRenderer->GetBackground2()[0],
                                          firstRenderer->GetBackground2()[1],
                                          firstRenderer->GetBackground2()[2])
                       : QColor();
}

//----------------------------------------------------------------------------
void ctkVTKAbstractView::setGradientBackground(bool enable)
{
  Q_D(ctkVTKAbstractView);
  foreach(vtkRenderer* renderer, d->renderers())
    {
    renderer->SetGradientBackground(enable);
    }
}

//----------------------------------------------------------------------------
bool ctkVTKAbstractView::gradientBackground()const
{
  Q_D(const ctkVTKAbstractView);
  vtkRenderer* firstRenderer = d->firstRenderer();
  return firstRenderer ? firstRenderer->GetGradientBackground() : false;
}

//----------------------------------------------------------------------------
void ctkVTKAbstractView::setFPSVisible(bool show)
{
  Q_D(ctkVTKAbstractView);
  if (d->FPSVisible == show)
    {
    return;
    }
  d->FPSVisible = show;
  vtkRenderer* renderer = d->firstRenderer();
  if (d->FPSVisible)
    {
    d->FPSTimer->start();
    qvtkConnect(renderer,
                vtkCommand::EndEvent, this, SLOT(onRender()));
    }
  else
    {
    d->FPSTimer->stop();
    qvtkDisconnect(renderer,
                   vtkCommand::EndEvent, this, SLOT(onRender()));
    d->CornerAnnotation->SetText(1, "");
    }
}

//----------------------------------------------------------------------------
bool ctkVTKAbstractView::isFPSVisible()const
{
  Q_D(const ctkVTKAbstractView);
  return d->FPSVisible;
}

//----------------------------------------------------------------------------
void ctkVTKAbstractView::onRender()
{
  Q_D(ctkVTKAbstractView);
  ++d->FPS;
}

//----------------------------------------------------------------------------
void ctkVTKAbstractView::updateFPS()
{
  Q_D(ctkVTKAbstractView);
  vtkRenderer* renderer = d->firstRenderer();
  double lastRenderTime = renderer ? renderer->GetLastRenderTimeInSeconds() : 0.;
  QString fpsString = tr("FPS: %1(%2s)").arg(d->FPS).arg(lastRenderTime);
  d->FPS = 0;
  d->CornerAnnotation->SetText(1, fpsString.toUtf8());
}

//----------------------------------------------------------------------------
bool ctkVTKAbstractView::useDepthPeeling()const
{
  Q_D(const ctkVTKAbstractView);
  vtkRenderer* renderer = d->firstRenderer();
  return renderer ? static_cast<bool>(renderer->GetUseDepthPeeling()):0;
}

//----------------------------------------------------------------------------
void ctkVTKAbstractView::setUseDepthPeeling(bool useDepthPeeling)
{
  Q_D(ctkVTKAbstractView);
  vtkRenderer* renderer = d->firstRenderer();
  if (!renderer)
    {
    return;
    }
  this->renderWindow()->SetAlphaBitPlanes( useDepthPeeling ? 1 : 0);
  int nSamples = ctkVTKAbstractView::multiSamples();
  if (nSamples < 0)
    {
    nSamples = vtkOpenGLRenderWindow::GetGlobalMaximumNumberOfMultiSamples();
    }
  this->renderWindow()->SetMultiSamples(useDepthPeeling ? 0 : nSamples);
  renderer->SetUseDepthPeeling(useDepthPeeling ? 1 : 0);
#ifdef CTK_USE_QVTKOPENGLWIDGET
  renderer->SetUseDepthPeelingForVolumes(useDepthPeeling);
#endif
}

//----------------------------------------------------------------------------
int ctkVTKAbstractView::multiSamples()
{
  return ctkVTKAbstractViewPrivate::MultiSamples;
}

//----------------------------------------------------------------------------
void ctkVTKAbstractView::setMultiSamples(int number)
{
  ctkVTKAbstractViewPrivate::MultiSamples = number;
}

//----------------------------------------------------------------------------
double ctkVTKAbstractView::maximumUpdateRate()const
{
  Q_D(const ctkVTKAbstractView);
  return d->MaximumUpdateRate;
}

//----------------------------------------------------------------------------
void ctkVTKAbstractView::setMaximumUpdateRate(double fps)
{
  Q_D(ctkVTKAbstractView);
  d->MaximumUpdateRate = fps;
}
