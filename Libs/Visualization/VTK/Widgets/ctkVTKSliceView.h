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
class vtkLightBoxRendererManager;
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
  Q_PROPERTY(RenderWindowLayoutType renderWindowLayoutType
             READ renderWindowLayoutType WRITE setRenderWindowLayoutType)
  Q_PROPERTY(QString cornerAnnotationText READ cornerAnnotationText WRITE setCornerAnnotationText)
  Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
  Q_PROPERTY(QColor highlightedBoxColor READ highlightedBoxColor WRITE setHighlightedBoxColor)
  Q_PROPERTY(bool renderEnabled READ renderEnabled WRITE setRenderEnabled)
  Q_PROPERTY(double ColorLevel READ colorLevel WRITE setColorLevel)
  Q_PROPERTY(double ColorWindow READ colorWindow WRITE setColorWindow)

public:
  /// Constructors
  typedef QWidget   Superclass;
  explicit ctkVTKSliceView(QWidget* parent = 0);
  virtual ~ctkVTKSliceView();

  /// The layout type determines how the image slices should be displayed
  /// within the different render view items.
  /// \sa setRenderWindowLayout() renderWindowLayoutType()
  enum RenderWindowLayoutType{LeftRightTopBottom = 0, LeftRightBottomTop};

  /// Return if rendering is enabled
  bool renderEnabled() const;

  /// Convenient method to get the underlying RenderWindow
  vtkRenderWindow* renderWindow() const;

  /// Get lightBoxRendererManager
  vtkLightBoxRendererManager* lightBoxRendererManager() const;

  /// Set/Get window interactor
  vtkRenderWindowInteractor* interactor() const;
  void setInteractor(vtkRenderWindowInteractor* newInteractor);

  /// Get current interactor style
  vtkInteractorObserver* interactorStyle()const;

  /// Get corner annotation text
  /// \sa setCornerAnnotationText();
  QString cornerAnnotationText()const;

  /// Get background color
  /// \sa setBackgroundColor();
  QColor backgroundColor()const;

  /// Get highlightedBox color
  /// \sa setHighlightedBoxColor();
  QColor highlightedBoxColor()const;

  /// Get renderWindow layout type
  /// \sa setRenderWindowLayoutType();
  RenderWindowLayoutType renderWindowLayoutType()const;

  /// Get color level
  double colorLevel()const;

  /// Get color window
  double colorWindow()const;

public slots:

  /// If a render has already been scheduled, this called is a no-op
  void scheduleRender();

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

  /// Set highlightedBox color
  void setHighlightedBoxColor(const QColor& newHighlightedBoxColor);

  /// Enable/Disable rendering
  void setRenderEnabled(bool value);

  /// Set RenderWindow layout type
  void setRenderWindowLayoutType(RenderWindowLayoutType layoutType);

  /// Set color level
  void setColorLevel(double newColorLevel);

  /// Set color window
  void setColorWindow(double newColorWindow);

  /// Change the number of row of the associated lightBox
  /// \sa lightBoxRendererManager()
  void setLightBoxRendererManagerRowCount(int newRowCount);

  /// Change the number of column of the associated lightBox
  /// \sa lightBoxRendererManager()
  void setLightBoxRendererManagerColumnCount(int newColumnCount);
  
signals:
  void resized(const QSize& size, const QSize& oldSize);

protected:
  virtual void resizeEvent(QResizeEvent * event);

private:
  CTK_DECLARE_PRIVATE(ctkVTKSliceView);
}; 

#endif
