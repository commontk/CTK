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

#ifndef __ctkVTKSliceView_p_h
#define __ctkVTKSliceView_p_h

// Qt includes
#include <QObject>
#include <QColor>
#include <QList>
#include <QSharedPointer>

// CTK includes
#include "ctkVTKSliceView.h"
#include "ctkVTKAbstractView_p.h"
#include <vtkLightBoxRendererManager.h>

// VTK includes
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkCornerAnnotation.h>

class vtkRenderWindowInteractor;

//-----------------------------------------------------------------------------
/// \ingroup Visualization_VTK_Widgets
class ctkVTKSliceViewPrivate : public ctkVTKAbstractViewPrivate
{
  Q_OBJECT
public:
  ctkVTKSliceViewPrivate(ctkVTKSliceView&);

  /// Convenient setup methods
  void setupCornerAnnotation();
  void setupRendering();

  vtkSmartPointer<vtkLightBoxRendererManager>   LightBoxRendererManager;
  bool                                          RenderPending;
  bool                                          RenderEnabled;
  vtkSmartPointer<vtkRenderer>                  OverlayRenderer;
  vtkSmartPointer<vtkCornerAnnotation>          OverlayCornerAnnotation;

};

#endif
