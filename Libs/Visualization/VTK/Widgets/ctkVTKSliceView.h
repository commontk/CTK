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

#ifndef __ctkVTKSliceView_h
#define __ctkVTKSliceView_h

// Qt includes
#include <QWidget>

// CTK includes
#include <ctkPimpl.h>

#include "CTKVisualizationVTKWidgetsExport.h"

class ctkVTKSliceViewPrivate;
class vtkInteractorObserver;
class vtkRenderWindowInteractor;
class vtkRenderWindow;
class vtkRenderer;
class vtkCamera; 
class vtkImageData;

class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKSliceView : public QWidget
{
  Q_OBJECT
  Q_ENUMS(RenderWindowLayoutType)
  Q_PROPERTY(RenderWindowLayoutType renderWindowLayoutType READ renderWindowLayoutType
             WRITE setRenderWindowLayoutType)
  Q_PROPERTY(QString cornerAnnotationText READ cornerAnnotationText WRITE setCornerAnnotationText)
  Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
  Q_PROPERTY(double colorWindow READ colorWindow)
  Q_PROPERTY(double colorLevel READ colorLevel)
  Q_PROPERTY(bool renderEnabled READ renderEnabled WRITE setRenderEnabled)

public:
  /// Constructors
  typedef QWidget   Superclass;
  explicit ctkVTKSliceView(QWidget* parent = 0);
  virtual ~ctkVTKSliceView();

  /// If a render has already been scheduled, this called is a no-op
  void scheduleRender();

public slots:

  /// Force a render even if a render is already ocurring
  void forceRender();

  /// Reset cameras associated with all renderWindowItem
  void resetCamera();

  /// Set image data
  void setImageData(vtkImageData* newImageData);

  /// Set corner annotation \a text
  void setCornerAnnotationText(const QString& text);

  /// Set background color
  void setBackgroundColor(const QColor& newBackgroundColor);

  /// Enable/Disable rendering
  void setRenderEnabled(bool value);

public:

  /// Get underlying RenderWindow
  vtkRenderWindow* renderWindow() const;
  
  /// Set/Get window interactor
  vtkRenderWindowInteractor* interactor() const;
  void setInteractor(vtkRenderWindowInteractor* newInteractor);

  /// Get current interactor style
  vtkInteractorObserver* interactorStyle();
  
  /// Get corner annotation text
  QString cornerAnnotationText() const;

  /// Get background color
  QColor backgroundColor() const;

  /// Get active camera
  /// Note that the same camera is used with all the renderWindowItem
  vtkCamera* activeCamera();

  /// Set actice camera
  void setActiveCamera(vtkCamera* newActiveCamera);
  
  /// Return number of underlying renderer
  int rendererCount();
  
  /// Get a reference to the associated vtkRenderer(s) identified by its \a id
  vtkRenderer* renderer(int id);

  /// Get a reference to the associated vtkRenderer(s) given its posiion in the grid
  /// \sa renderer(int)
  vtkRenderer* renderer(int rowId, int columnId);

  /// The layout type determines how the image slices should be displayed
  /// within the different render view items.
  /// \sa setRenderWindowLayout() renderWindowLayoutType()
  enum RenderWindowLayoutType{ LeftRightTopBottom, LeftRightBottomTop};

  /// Get current layout type
  RenderWindowLayoutType renderWindowLayoutType() const;

  /// Set current \a layoutType
  void setRenderWindowLayoutType(RenderWindowLayoutType layoutType);

  /// Split the current vtkRenderWindow in \a rowCount per \a columnCount grid
  void setRenderWindowLayout(int rowCount, int columnCount);

  /// Highlight / Unhighlight a render view item given its \a id
  void setHighlightedById(int id, bool value);

  ///  Highlight / Unhighlight a render view item given its position in the grid
  /// \sa setHighlighted(int, bool)
  void setHighlighted(int rowId, int columnId, bool value);

  /// Convenient function allowing to compute the renderWindowItemId
  /// given \a rowId and \a columnId.
  /// The following formula is used: ColumnCount * rowId + columnId
  /// Note that the \a rowCount and \a columnCount correspond to the one set
  /// using setRenderWindowLayout.
  inline int renderWindowItemId(int rowId, int columnId);

  /// Return if rendering is enabled
  bool renderEnabled() const;

  /// Get current color window
  double colorWindow() const;

  /// Get current color level
  double colorLevel() const;
  

signals:
  void resized(QResizeEvent*);

protected:
  virtual void resizeEvent(QResizeEvent * event);

private:
  CTK_DECLARE_PRIVATE(ctkVTKSliceView);
}; 

#endif
