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

#ifndef __ctkVTKThumbnailView_h
#define __ctkVTKThumbnailView_h

// CTK includes
#include "ctkVisualizationVTKWidgetsExport.h"
#include "ctkVTKObject.h"
#include "ctkVTKRenderView.h"

class ctkVTKThumbnailViewPrivate;

/// \ingroup Visualization_VTK_Widgets
class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKThumbnailView : public ctkVTKRenderView
{
  Q_OBJECT
  QVTK_OBJECT

public:
  /// Superclass typedef
  typedef ctkVTKRenderView Superclass;
  
  /// Constructors
  explicit ctkVTKThumbnailView(QWidget* parent = 0);
  virtual ~ctkVTKThumbnailView();

  void setRendererToListen(vtkRenderer* renderer);

protected Q_SLOTS:
  void checkAbort();
  void updateBounds();
  void updateCamera();

protected:
  QScopedPointer<ctkVTKThumbnailViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkVTKThumbnailView);
  Q_DISABLE_COPY(ctkVTKThumbnailView);
};

#endif
