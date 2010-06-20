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
class vtkInteractorObserver;
class vtkRenderWindowInteractor;
class vtkRenderWindow;
class vtkRenderer;
class vtkCamera; 

class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKRenderView : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(QString cornerAnnotationText READ cornerAnnotationText WRITE setCornerAnnotationText)
  Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
  Q_PROPERTY(bool renderEnabled READ renderEnabled WRITE setRenderEnabled)
public:
  /// Constructors
  typedef QWidget   Superclass;
  explicit ctkVTKRenderView(QWidget* parent = 0);
  virtual ~ctkVTKRenderView();

  /// If a render has already been scheduled, this called is a no-op
  void scheduleRender();

public slots:
  /// Force a render even if a render is already ocurring
  void forceRender();

public:
  /// Get underlying RenderWindow
  vtkRenderWindow* renderWindow()const;
  
  /// Set/Get window interactor
  vtkRenderWindowInteractor* interactor()const;
  void setInteractor(vtkRenderWindowInteractor* newInteractor);

  /// Get current interactor style
  vtkInteractorObserver* interactorStyle();

  /// Set/Get corner annotation \a text
  void setCornerAnnotationText(const QString& text);
  QString cornerAnnotationText() const;

  /// Set/Get background color
  void setBackgroundColor(const QColor& newBackgroundColor);
  QColor backgroundColor() const;

  /// Get active camera
  vtkCamera* activeCamera();

  /// Reset camera
  void resetCamera();

  /// Get a reference to the associated vtkRenderer
  vtkRenderer* renderer()const;

  /// Return if rendering is enabled
  bool renderEnabled() const;

  /// Enable/Disable rendering
  void setRenderEnabled(bool value);
  
private:
  CTK_DECLARE_PRIVATE(ctkVTKRenderView);
}; 

#endif
