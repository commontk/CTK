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

#ifndef __ctkVTKSliceView_h
#define __ctkVTKSliceView_h

// CTK includes
#include "ctkVTKAbstractView.h"
#include "vtkVersion.h"
class vtkLightBoxRendererManager;
class ctkVTKSliceViewPrivate;

class vtkCamera;
class vtkImageData;
class vtkRenderer;
class vtkAlgorithmOutput;

/// \ingroup Visualization_VTK_Widgets
/// Specific implementation for a 2D view that supports lightbox display.
/// \note There is no support for gradient background yet.
class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKSliceView : public ctkVTKAbstractView
{
  Q_OBJECT
  Q_ENUMS(RenderWindowLayoutType)
  Q_PROPERTY(RenderWindowLayoutType renderWindowLayoutType
             READ renderWindowLayoutType WRITE setRenderWindowLayoutType)
  Q_PROPERTY(QColor highlightedBoxColor READ highlightedBoxColor WRITE setHighlightedBoxColor)
  Q_PROPERTY(double colorLevel READ colorLevel WRITE setColorLevel)
  Q_PROPERTY(double colorWindow READ colorWindow WRITE setColorWindow)

public:
  /// Constructors
  typedef ctkVTKAbstractView Superclass;
  explicit ctkVTKSliceView(QWidget* parent = 0);
  virtual ~ctkVTKSliceView();

  /// The layout type determines how the image slices should be displayed
  /// within the different render view items.
  /// \sa setRenderWindowLayout() renderWindowLayoutType()
  enum RenderWindowLayoutType{LeftRightTopBottom = 0, LeftRightBottomTop};

  /// Set active camera
  void setActiveCamera(vtkCamera * newActiveCamera);

  /// Get lightBoxRendererManager
  Q_INVOKABLE vtkLightBoxRendererManager* lightBoxRendererManager() const;

  /// Get overlay renderer
  vtkRenderer* overlayRenderer() const;

  /// Get overlay corner annotation actor
  /// This corresponds to the cornerAnnotation associated added in the single overlay renderer
  vtkCornerAnnotation* overlayCornerAnnotation()const;

  /// Set background color
  /// \sa vtkLightBoxRendererManager::SetBackgroundColor
  virtual void setBackgroundColor(const QColor& newBackgroundColor);

  /// Get background color
  /// \sa setBackgroundColor();
  virtual QColor backgroundColor()const;

  /// Get highlightedBox color
  /// \sa setHighlightedBoxColor();
  QColor highlightedBoxColor()const;

  /// Get renderWindow layout type
  /// \sa setRenderWindowLayoutType();
  RenderWindowLayoutType renderWindowLayoutType()const;

  /// Get color level
  /// \sa setColorLevel();
  double colorLevel()const;

  /// Get color window
  /// \sa setColorWindow();
  double colorWindow()const;

public Q_SLOTS:

  /// Reset cameras associated with all renderWindowItem
  /// \sa vtkLightBoxRendererManager::ResetCamera
  void resetCamera();

  /// Set image data
  /// \sa vtkLightBoxRendererManager::SetImageDataConnection
  void setImageDataConnection(vtkAlgorithmOutput* newImageDataPort);

  /// Set highlightedBox color
  /// \sa vtkLightBoxRendererManager::SetHighlightedBoxColor
  void setHighlightedBoxColor(const QColor& newHighlightedBoxColor);

  /// Set RenderWindow layout type
  /// \sa vtkLightBoxRendererManager::SetRenderWindowLayoutType
  void setRenderWindowLayoutType(RenderWindowLayoutType layoutType);

  /// Set color level
  /// \sa vtkLightBoxRendererManager::SetColorLevel
  void setColorLevel(double newColorLevel);

  /// Set color window
  /// \sa vtkLightBoxRendererManager::SetColorWindow
  void setColorWindow(double newColorWindow);

  /// Change the number of row of the associated lightBox
  /// \sa lightBoxRendererManager()
  /// \sa vtkLightBoxRendererManager::SetRenderWindowRowCount
  void setLightBoxRendererManagerRowCount(int newRowCount);

  /// Change the number of column of the associated lightBox
  /// \sa lightBoxRendererManager()
  /// \sa vtkLightBoxRendererManager::SetRenderWindowColumnCount
  void setLightBoxRendererManagerColumnCount(int newColumnCount);
  
Q_SIGNALS:
  void resized(const QSize& size);

protected:
  virtual bool eventFilter(QObject *object, QEvent *event);

private:
  Q_DECLARE_PRIVATE(ctkVTKSliceView);
  Q_DISABLE_COPY(ctkVTKSliceView);
}; 

#endif
