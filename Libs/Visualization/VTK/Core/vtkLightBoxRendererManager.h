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

#ifndef __vtkLightBoxRendererManager_h
#define __vtkLightBoxRendererManager_h

#include <vtkObject.h>
#include <vtkVersion.h>

#include "ctkVisualizationVTKCoreExport.h"

class vtkRenderWindow;
class vtkRenderer;
class vtkImageData;
class vtkCamera;
class vtkCornerAnnotation;
class vtkAlgorithmOutput;

/// \ingroup Visualization_VTK_Core
class CTK_VISUALIZATION_VTK_CORE_EXPORT vtkLightBoxRendererManager : public vtkObject
{
  vtkTypeMacro(vtkLightBoxRendererManager,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;
  static vtkLightBoxRendererManager *New();

  void Initialize(vtkRenderWindow* renderWindow);

  bool IsInitialized();

  /// Set the layer associated with the renderers
  /// \note By default, the value is 0
  /// \sa vtkRenderer::SetLayer
  void SetRendererLayer(int newLayer);

  /// Get associated RenderWindow
  vtkRenderWindow* GetRenderWindow();

  /// Set image data
  void SetImageDataConnection(vtkAlgorithmOutput* newImageDataConnection);

  /// Get active camera
  /// Note that the same camera is used with all the renderWindowItem
  vtkCamera* GetActiveCamera();

  /// Set actice camera
  void SetActiveCamera(vtkCamera* newActiveCamera);

  /// Reset cameras associated with all renderWindowItem
  void ResetCamera();
  
  /// Return number of underlying render window Item
  /// \note In the current implementation
  int GetRenderWindowItemCount();
  
  /// Get a reference to the associated vtkRenderer(s) identified by its \a id
  vtkRenderer* GetRenderer(int id);
  
  /// Get a reference to the associated vtkRenderer(s) given its position in the grid
  /// \sa GetRenderer(int)
  vtkRenderer* GetRenderer(int rowId, int columnId);
  
  /// The layout type determines how the image slices should be displayed
  /// within the different render view items.
  /// \sa SetRenderWindowLayout() GetRenderWindowLayoutType()
  enum RenderWindowLayoutType{LeftRightTopBottom = 0, LeftRightBottomTop};
  
  /// Get current layout type
  int GetRenderWindowLayoutType() const;
  
  /// Set current \a layoutType
  void SetRenderWindowLayoutType(int layoutType);

  /// Split the current vtkRenderWindow in \a rowCount per \a columnCount grid
  void SetRenderWindowLayout(int rowCount, int columnCount);

  /// Set the \a rowCount
  /// \sa SetRenderWindowLayout
  void SetRenderWindowRowCount(int newRowCount);

  /// Get number of rows
  /// \sa SetRenderWindowLayout
  int GetRenderWindowRowCount();

  /// Set the \a columnCount
  /// \sa SetRenderWindowLayout
  void SetRenderWindowColumnCount(int newColumnCount);

  /// Get number of columns
  /// \sa SetRenderWindowLayout
  int GetRenderWindowColumnCount();

  /// Return True if the render view item identified by \a id is highlighted
  bool GetHighlightedById(int id);

  /// \brief Return True if the render view item identified by its position in the grid
  /// is highlighted
  /// \sa GetHighlightedById(int)
  bool GetHighlighted(int rowId, int columnId);

  /// Highlight / Unhighlight a render view item given its \a id
  void SetHighlightedById(int id, bool highlighted);

  ///  Highlight / Unhighlight a render view item given its position in the grid
  /// \sa setHighlighted(int, bool)
  void SetHighlighted(int rowId, int columnId, bool highlighted);

  /// \sa SetHighlighted SetHighlightedBoxColorById
  void SetHighlightedBoxColor(double highlightedBoxColor[3]);

  /// \brief Set the color of the box displayed around the highlighted item
  /// identified by \a id
  /// The highlightedBox is set with a width of 3 screen units.
  /// \sa SetHighlightedById SetHighlighted vtkProperty2D::SetLineWidth
  double* GetHighlightedBoxColor()const;
  
  /// Convenient function allowing to compute the renderWindowItemId
  /// given \a rowId and \a columnId.
  /// The following formula is used: ColumnCount * rowId + columnId
  /// \note The \a rowCount and \a columnCount correspond to the one set
  /// using setRenderWindowLayout.
  inline int ComputeRenderWindowItemId(int rowId, int columnId);
  
  /// Set corner annotation \a text
  void SetCornerAnnotationText(const std::string& text);

  /// Get current corner annotation
  const std::string GetCornerAnnotationText()const;

  /// Get corner annotation actor
  /// The same annotation is associated with all renderers managed by the light box
  vtkCornerAnnotation * GetCornerAnnotation()const;
  void SetCornerAnnotation(vtkCornerAnnotation* annotation);

  /// Set background color
  void SetBackgroundColor(const double newBackgroundColor[3]);

  /// Get background color
  double* GetBackgroundColor()const;

  /// Set color level
  void SetColorLevel(double colorLevel);

  /// Get color level
  double GetColorLevel()const;

  /// Set color window
  void SetColorWindow(double colorWindow);

  /// Get color window
  double GetColorWindow()const;

  /// Set color Window and color level
  void SetColorWindowAndLevel(double colorWindow, double colorLevel);
  
protected:

  vtkLightBoxRendererManager();
  ~vtkLightBoxRendererManager();
  
private:
  vtkLightBoxRendererManager(const vtkLightBoxRendererManager&); // Not implemented.
  void operator=(const vtkLightBoxRendererManager&);             // Not implemented.
  
//BTX
  class vtkInternal;
  vtkInternal* Internal;
//ETX

};

#endif

