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
#include <QTimer>
#include <QVBoxLayout>
#include <QDebug>

// CTK includes
#include "ctkVTKAbstractView.h"
#include "ctkVTKAbstractView_p.h"
#include "ctkLogger.h"

// VTK includes
#include <vtkRenderWindowInteractor.h>
#include <vtkTextProperty.h>

//--------------------------------------------------------------------------
static ctkLogger logger("org.commontk.visualization.vtk.widgets.ctkVTKAbstractView");
//--------------------------------------------------------------------------

// --------------------------------------------------------------------------
// ctkVTKAbstractViewPrivate methods

// --------------------------------------------------------------------------
ctkVTKAbstractViewPrivate::ctkVTKAbstractViewPrivate(ctkVTKAbstractView& object)
  : q_ptr(&object)
{
  this->RenderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  this->CornerAnnotation = vtkSmartPointer<vtkCornerAnnotation>::New();
  this->RenderPending = false;
  this->RenderEnabled = true;
}

// --------------------------------------------------------------------------
void ctkVTKAbstractViewPrivate::init()
{
  Q_Q(ctkVTKAbstractView);

  this->setParent(q);

  this->VTKWidget = new QVTKWidget;
  q->setLayout(new QVBoxLayout);
  q->layout()->setMargin(0);
  q->layout()->setSpacing(0);
  q->layout()->addWidget(this->VTKWidget);

  this->setupCornerAnnotation();
  this->setupRendering();
}

// --------------------------------------------------------------------------
void ctkVTKAbstractViewPrivate::setupCornerAnnotation()
{
  logger.trace("setupCornerAnnotation");
  this->CornerAnnotation->SetMaximumLineHeight(0.07);
  vtkTextProperty *tprop = this->CornerAnnotation->GetTextProperty();
  tprop->ShadowOn();
  this->CornerAnnotation->ClearAllTexts();
}

//---------------------------------------------------------------------------
void ctkVTKAbstractViewPrivate::setupRendering()
{
  logger.trace("setupRendering");
  Q_ASSERT(this->RenderWindow);
  this->RenderWindow->SetAlphaBitPlanes(1);
  this->RenderWindow->SetMultiSamples(0);
  this->RenderWindow->StereoCapableWindowOn();

  this->VTKWidget->SetRenderWindow(this->RenderWindow);
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

  logger.trace(QString("scheduleRender - RenderEnabled: %1 - RenderPending: %2").
               arg(d->RenderEnabled ? "true" : "false")
               .arg(d->RenderPending ? "true:" : "false"));

  if (!d->RenderEnabled)
    {
    return;
    }
  if (!d->RenderPending)
    {
    d->RenderPending = true;
    QTimer::singleShot(0, this, SLOT(forceRender()));
    }
}

//----------------------------------------------------------------------------
void ctkVTKAbstractView::forceRender()
{
  Q_D(ctkVTKAbstractView);

  d->RenderPending = false;
  logger.trace(QString("forceRender - RenderEnabled: %1")
               .arg(d->RenderEnabled ? "true" : "false"));

  if (!d->RenderEnabled || !this->isVisible())
    {
    return;
    }
  d->RenderWindow->Render();
}

//----------------------------------------------------------------------------
CTK_GET_CPP(ctkVTKAbstractView, vtkRenderWindow*, renderWindow, RenderWindow);

//----------------------------------------------------------------------------
void ctkVTKAbstractView::setInteractor(vtkRenderWindowInteractor* newInteractor)
{
  Q_D(ctkVTKAbstractView);

  logger.trace("setInteractor");

  d->RenderWindow->SetInteractor(newInteractor);
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
  logger.trace(QString("setCornerAnnotationText: %1").arg(text));
  d->CornerAnnotation->ClearAllTexts();
  d->CornerAnnotation->SetText(2, text.toLatin1());
}

//----------------------------------------------------------------------------
QString ctkVTKAbstractView::cornerAnnotationText() const
{
  Q_D(const ctkVTKAbstractView);
  return QLatin1String(d->CornerAnnotation->GetText(2));
}

//----------------------------------------------------------------------------
vtkCornerAnnotation* ctkVTKAbstractView::cornerAnnotation() const
{
  Q_D(const ctkVTKAbstractView);
  return d->CornerAnnotation;
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
