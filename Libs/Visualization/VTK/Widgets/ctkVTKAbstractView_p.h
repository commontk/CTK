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

#ifndef __ctkVTKAbstractView_p_h
#define __ctkVTKAbstractView_p_h

// Qt includes
#include <QObject>
#include <QTime>
class QTimer;

// CTK includes
#include "ctkVTKAbstractView.h"

// VTK includes
#include <vtkCornerAnnotation.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkWeakPointer.h>

//-----------------------------------------------------------------------------
/// \ingroup Visualization_VTK_Widgets
class ctkVTKAbstractViewPrivate : public QObject
{
  Q_OBJECT
  Q_DECLARE_PUBLIC(ctkVTKAbstractView);

protected:
  ctkVTKAbstractView* const q_ptr;

public:
  ctkVTKAbstractViewPrivate(ctkVTKAbstractView& object);

  /// Convenient setup methods
  virtual void init();
  virtual void setupCornerAnnotation();
  virtual void setupRendering();

  QList<vtkRenderer*> renderers()const;
  vtkRenderer* firstRenderer()const;

  ctkVTKOpenGLNativeWidget*                     VTKWidget;
#ifdef CTK_USE_QVTKOPENGLWIDGET
  vtkSmartPointer<vtkGenericOpenGLRenderWindow> RenderWindow;
#else
  vtkSmartPointer<vtkRenderWindow>              RenderWindow;
#endif
  QTimer*                                       RequestTimer;
  QTime                                         RequestTime;
  bool                                          RenderEnabled;
  double                                        MaximumUpdateRate;
  bool                                          FPSVisible;
  QTimer*                                       FPSTimer;
  int                                           FPS;
  static int                                    MultiSamples;
  int                                           PauseRenderCount;

  vtkSmartPointer<vtkCornerAnnotation>          CornerAnnotation;
};

#endif
