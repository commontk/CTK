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

#ifndef __ctkVTKRenderView_h
#define __ctkVTKRenderView_h

// Qt includes
#include <QWidget>

// CTK includes
#include <ctkPimpl.h>

#include "CTKVisualizationVTKWidgetsExport.h"

class ctkVTKRenderViewPrivate;
class vtkRenderWindowInteractor;

class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKRenderView : public QWidget
{
  Q_OBJECT
public:
  /// Constructors
  typedef QWidget   Superclass;
  explicit ctkVTKRenderView(QWidget* parent = 0);
  virtual ~ctkVTKRenderView();

  /// If a render has already been scheduled, this called is a no-op
  void scheduleRender();

  /// Force a render even if a render is already ocurring
  void forceRender();

  /// Set/Get window interactor
  vtkRenderWindowInteractor* interactor()const;
  void setInteractor(vtkRenderWindowInteractor* newInteractor);

  /// Set corner annotation text
  void setCornerAnnotationText(const QString& text);

  /// Set background color
  void setBackgroundColor(double r, double g, double b);

  void resetCamera();

  //virtual void setCornerText(const QString& text);
  
private:
  CTK_DECLARE_PRIVATE(ctkVTKRenderView);
}; 

#endif
