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
#include <vtkLightBoxRendererManager.h>
#include "ctkVTKSliceView.h"

// VTK includes
#include <QVTKWidget.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkWeakPointer.h>
#include <vtkImageMapper.h>
#include <vtkCornerAnnotation.h>

class vtkRenderWindowInteractor;

//-----------------------------------------------------------------------------
class ctkVTKSliceViewPrivate : public QObject
{
  Q_OBJECT
public:
  ctkVTKSliceViewPrivate();

  /// Convenient setup methods
  void setupCornerAnnotation();
  void setupRendering();
  void setupDefaultInteractor();

  QVTKWidget*                                   VTKWidget;
  vtkSmartPointer<vtkRenderWindow>              RenderWindow;
  vtkSmartPointer<vtkLightBoxRendererManager>   LightBoxRendererManager;
  bool                                          RenderPending;
  bool                                          RenderEnabled;
  vtkSmartPointer<vtkRenderer>                  OverlayRenderer;
  vtkSmartPointer<vtkCornerAnnotation>          OverlayCornerAnnotation;

};

#endif
