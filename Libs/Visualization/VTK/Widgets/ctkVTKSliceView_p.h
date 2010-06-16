/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

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

#ifndef __ctkVTKSliceView_p_h
#define __ctkVTKSliceView_p_h

// Qt includes
#include <QObject>
#include <QColor>
#include <QList>
#include <QSharedPointer>

// CTK includes
#include <ctkPimpl.h>
#include <ctkVTKObject.h>
#include "ctkVTKSliceView.h"

// VTK includes
#include <QVTKWidget.h>
#include <vtkAxesActor.h>
#include <vtkCornerAnnotation.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkWeakPointer.h>
#include <vtkImageMapper.h>

#include <vtkOrientationMarkerWidget.h>

class vtkRenderWindowInteractor;


//-----------------------------------------------------------------------------
/// A RenderWindow can be split in 1x1, 2x3, ... grid view, each element of that grid
/// will be identified as RenderWindowItem
class RenderWindowItem
{
public:
  RenderWindowItem(const QColor& rendererBackgroundColor, double colorWindow, double colorLevel);
  void setViewport(double xMin, double yMin, double viewportWidth, double viewportHeight);

  /// Create the actor supporing the image mapper
  void setupImageMapperActor(double colorWindow, double colorLevel);

  /// Create a box around the renderer.
  void setupHighlightBoxActor(bool visible = false);

  vtkSmartPointer<vtkRenderer>     Renderer;
  vtkSmartPointer<vtkImageMapper>  ImageMapper;
  vtkSmartPointer<vtkActor2D>      HighlightBoxActor;
};

//-----------------------------------------------------------------------------
class ctkVTKSliceViewPrivate : public QObject,
                               public ctkPrivate<ctkVTKSliceView>
{
  Q_OBJECT
  CTK_DECLARE_PUBLIC(ctkVTKSliceView);
public:
  ctkVTKSliceViewPrivate();

  /// Convenient setup methods
  void setupCornerAnnotation();
  void setupRendering();
  void setupDefaultInteractor();

  /// Update render window ImageMapper Z slice according to \a layoutType
  void updateRenderWindowItemsZIndex(ctkVTKSliceView::RenderWindowLayoutType layoutType);

  QVTKWidget*                                   VTKWidget;
  vtkSmartPointer<vtkRenderWindow>              RenderWindow;
  bool                                          RenderPending;
  bool                                          RenderEnabled;

  int                                           RenderWindowRowCount;
  int                                           RenderWindowColumnCount;
  ctkVTKSliceView::RenderWindowLayoutType       RenderWindowLayoutType;
  
  vtkSmartPointer<vtkAxesActor>                 Axes;
  vtkSmartPointer<vtkOrientationMarkerWidget>   Orientation;
  vtkSmartPointer<vtkCornerAnnotation>          CornerAnnotation;

  vtkWeakPointer<vtkRenderWindowInteractor>     CurrentInteractor;

  QList<QSharedPointer<RenderWindowItem> >      RenderWindowItemList;
  vtkWeakPointer<vtkImageData>                  ImageData;

  double                                        ColorWindow;
  double                                        ColorLevel;
  QColor                                        RendererBackgroundColor;

};

#endif
