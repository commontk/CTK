/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __ctkVTKRenderView_p_h
#define __ctkVTKRenderView_p_h

// Qt includes
#include <QObject>

// CTK includes
#include <ctkPimpl.h>
#include <ctkVTKObject.h>
#include "ctkVTKRenderView.h"

// VTK includes
#include <QVTKWidget.h>
#include <vtkAxesActor.h>
#include <vtkCornerAnnotation.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkWeakPointer.h>

class vtkRenderWindowInteractor;

//-----------------------------------------------------------------------------
class ctkVTKRenderViewPrivate : public QObject,
                                public ctkPrivate<ctkVTKRenderView>
{
  Q_OBJECT
  CTK_DECLARE_PUBLIC(ctkVTKRenderView);
public:
  ctkVTKRenderViewPrivate();

  /// Convenient setup methods
  void setupCornerAnnotation();
  void setupRendering();
  void setupDefaultInteractor();

  QVTKWidget*                                   VTKWidget;
  vtkSmartPointer<vtkRenderer>                  Renderer;
  vtkSmartPointer<vtkRenderWindow>              RenderWindow;
  bool                                          RenderPending;
  
  vtkSmartPointer<vtkAxesActor>                 Axes;
  vtkSmartPointer<vtkOrientationMarkerWidget>   Orientation;
  vtkSmartPointer<vtkCornerAnnotation>          CornerAnnotation;

  vtkWeakPointer<vtkRenderWindowInteractor>     CurrentInteractor;

};

#endif
