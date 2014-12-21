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

#include "vtkLightBoxRendererManager.h"

// VTK includes
#include <vtkAlgorithmOutput.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkConfigure.h>
#include <vtkCornerAnnotation.h>
#include <vtkImageData.h>
#include <vtkImageMapper.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty2D.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>
#include <vtkVersion.h>
#include <vtkWeakPointer.h>

// STD includes
#include <vector>
#include <cassert>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkLightBoxRendererManager);

namespace
{
//-----------------------------------------------------------------------------
// RenderWindowItem
//-----------------------------------------------------------------------------
/// A RenderWindow can be split in 1x1, 2x3, ... grid view, each element of that grid
/// will be identified as RenderWindowItem
class RenderWindowItem
{
public:
  RenderWindowItem(const double rendererBackgroundColor[3], const double highlightedBoxColor[3],
                   double colorWindow, double colorLevel);
  ~RenderWindowItem();
  void SetViewport(double xMin, double yMin, double viewportWidth, double viewportHeight);

  /// Create the actor supporing the image mapper
  void SetupImageMapperActor(double colorWindow, double colorLevel);

  /// Create a box around the renderer.
  void SetupHighlightedBoxActor(const double highlightedBoxColor[3], bool visible = false);

  /// Set HighlightedBox color
  void SetHighlightedBoxColor(double* newHighlightedBoxColor);

  vtkSmartPointer<vtkRenderer>                Renderer;
  vtkSmartPointer<vtkImageMapper>             ImageMapper;
  vtkSmartPointer<vtkActor2D>                 HighlightedBoxActor;
  vtkSmartPointer<vtkActor2D>                 ImageActor;
};
}

// --------------------------------------------------------------------------
// RenderWindowItem methods

//-----------------------------------------------------------------------------
RenderWindowItem::RenderWindowItem(const double rendererBackgroundColor[3],
                                   const double highlightedBoxColor[3],
                                   double colorWindow, double colorLevel)
{
  // Instantiate a renderer
  this->Renderer = vtkSmartPointer<vtkRenderer>::New();
  this->Renderer->SetBackground(rendererBackgroundColor[0],
                                rendererBackgroundColor[1],
                                rendererBackgroundColor[2]);

  // Ensure a vtkCamera is created and associated with the renderer
  // This is particularly important to ensure WorldToView/ViewToWorld
  // work as expected.
  this->Renderer->GetActiveCamera();

  this->SetupImageMapperActor(colorWindow, colorLevel);
  this->SetupHighlightedBoxActor(highlightedBoxColor);
}

//-----------------------------------------------------------------------------
RenderWindowItem::~RenderWindowItem()
{
#if (VTK_MAJOR_VERSION <= 5)
  this->ImageMapper->SetInput(0);
#else
  this->ImageMapper->SetInputConnection(0);
#endif
}

//-----------------------------------------------------------------------------
void RenderWindowItem::SetViewport(double xMin, double yMin,
                                   double viewportWidth, double viewportHeight)
{
  assert(this->Renderer);
  this->Renderer->SetViewport( xMin, yMin, (xMin + viewportWidth), (yMin + viewportHeight));
}

//---------------------------------------------------------------------------
void RenderWindowItem::SetupImageMapperActor(double colorWindow, double colorLevel)
{
  assert(this->Renderer);
  assert(!this->ImageMapper);

  // Instantiate an image mapper
  this->ImageMapper = vtkSmartPointer<vtkImageMapper>::New();
  this->ImageMapper->SetColorWindow(colorWindow);
  this->ImageMapper->SetColorLevel(colorLevel);

  // .. and its corresponding 2D actor
  this->ImageActor = vtkSmartPointer<vtkActor2D>::New();
  this->ImageActor->SetMapper(this->ImageMapper);
  this->ImageActor->GetProperty()->SetDisplayLocationToBackground();

  // .. and add it to the renderer
#if VTK_MAJOR_VERSION <= 5
  this->Renderer->AddActor2D(this->ImageActor.GetPointer());
#endif
}

//---------------------------------------------------------------------------
void RenderWindowItem::SetupHighlightedBoxActor(const double highlightedBoxColor[3], bool visible)
{
  assert(this->Renderer);
  assert(!this->HighlightedBoxActor);
  
  // Create a highlight actor (2D box around viewport)
  vtkNew<vtkPolyData> poly;
  vtkNew<vtkPoints> points;
  // Normalized Viewport means :
  // 0. -> 0;
  // 1. -> width - 1 ;
  // For a line of a width of 1, from (0.f,0.f) to (10.f,0.f), the line is on
  // 2 pixels. What pixel to draw the line on ?
  //
  //     |       |       |       |       |       |       |
  //  1  |       |       |       |       |       |       |
  //     |       |       |       |       |       |       |
  //     +-------+-------+-------+-------+-------+-------+
  //     |       |       |       |       |       |       |
  //  0  | What pixel    |================================
  //     | line shall    |
  //     +--be drawn---(0,0)
  //     |  above or     |
  // -1  |   below?      |================================
  //     |       |       |       |       |       |       |
  //  ^  +-------+-------+-------+-------+-------+-------+
  //     |       |       |       |       |       |       |
  // 1px |       |       |       |       |       |       |
  //     |       |       |       |       |       |       |
  //  V  +-------+-------+-------+-------+-------+-------+
  //     <  1px  >  -1       0       1       2       3
  // It depends of the graphic card, this is why we need to add an offset.
  // 0.0002 seems to work for most of the window sizes.
  double shift = 0.0002;
  points->InsertNextPoint(0. + shift, 0. + shift, 0); // bottom-left
  points->InsertNextPoint(1. + shift, 0. + shift, 0); // bottom-right
  points->InsertNextPoint(1. + shift, 1. + shift + 0.1, 0); // top-right to fill the 1,1 pixel
  points->InsertNextPoint(1. + shift, 1. + shift, 0); // top-right
  points->InsertNextPoint(0. + shift, 1. + shift, 0); // top-left
  points->InsertNextPoint(0. + shift, 0. + shift - 0.1, 0); // bottom-left to fill the 0,0 pixel.
  
  vtkNew<vtkCellArray> cells;
  cells->InsertNextCell(6);
  cells->InsertCellPoint(0);
  cells->InsertCellPoint(1);
  cells->InsertCellPoint(2);
  cells->InsertCellPoint(3);
  cells->InsertCellPoint(4);
  cells->InsertCellPoint(5);
  poly->SetPoints(points.GetPointer());
  poly->SetLines(cells.GetPointer());

  vtkNew<vtkCoordinate> coordinate;
  coordinate->SetCoordinateSystemToNormalizedViewport();
  coordinate->SetViewport(this->Renderer);

  vtkNew<vtkPolyDataMapper2D> polyDataMapper;
#if VTK_MAJOR_VERSION <= 5
  polyDataMapper->SetInput(poly.GetPointer());
#else
  polyDataMapper->SetInputData(poly.GetPointer());
#endif
  polyDataMapper->SetTransformCoordinate(coordinate.GetPointer());
  #if ! (VTK_MAJOR_VERSION == 5 && VTK_MINOR_VERSION == 8)
    polyDataMapper->SetTransformCoordinateUseDouble(true);
  #endif

  this->HighlightedBoxActor = vtkSmartPointer<vtkActor2D>::New();
  this->HighlightedBoxActor->SetMapper(polyDataMapper.GetPointer());
  this->HighlightedBoxActor->GetProperty()->SetColor(highlightedBoxColor[0],
                                                     highlightedBoxColor[1],
                                                     highlightedBoxColor[2]);
  this->HighlightedBoxActor->GetProperty()->SetDisplayLocationToForeground();
  this->HighlightedBoxActor->GetProperty()->SetLineWidth(1.0f);
  this->HighlightedBoxActor->SetVisibility(visible);

#if VTK_MAJOR_VERSION <= 5
  this->Renderer->AddActor2D(this->HighlightedBoxActor);
#endif
}

//-----------------------------------------------------------------------------
void RenderWindowItem::SetHighlightedBoxColor(double* newHighlightedBoxColor)
{
  this->HighlightedBoxActor->GetProperty()->SetColor(newHighlightedBoxColor);
}

//-----------------------------------------------------------------------------
// vtkInternal
//-----------------------------------------------------------------------------
class vtkLightBoxRendererManager::vtkInternal
{
public:
  vtkInternal(vtkLightBoxRendererManager* external);
  ~vtkInternal();

  /// Convenient setup methods
  void SetupCornerAnnotation();
  void setupRendering();

  /// Update render window ImageMapper Z slice according to \a layoutType
  void updateRenderWindowItemsZIndex(int layoutType);
  void SetItemInput(RenderWindowItem* item);

  vtkSmartPointer<vtkRenderWindow>              RenderWindow;
  int                                           RenderWindowRowCount;
  int                                           RenderWindowColumnCount;
  int                                           RenderWindowLayoutType;
  double                                        HighlightedBoxColor[3];
  int                                           RendererLayer;
  vtkWeakPointer<vtkRenderWindowInteractor>     CurrentInteractor;
  vtkSmartPointer<vtkCornerAnnotation>          CornerAnnotation;
  std::string                                   CornerAnnotationText;

#if (VTK_MAJOR_VERSION <= 5)
  vtkWeakPointer<vtkImageData>                  ImageData;
#else
  vtkWeakPointer<vtkAlgorithmOutput>            ImageDataConnection;
#endif
  double                                        ColorWindow;
  double                                        ColorLevel;
  double                                        RendererBackgroundColor[3];

  /// Collection of RenderWindowItem
  std::vector<RenderWindowItem* >                  RenderWindowItemList;
  
  /// .. and its associated convenient typedef
  typedef std::vector<RenderWindowItem*>::iterator RenderWindowItemListIt;
  
  /// Reference to the public interface
  vtkLightBoxRendererManager*                         External;
};

// --------------------------------------------------------------------------
// vtkInternal methods

// --------------------------------------------------------------------------
vtkLightBoxRendererManager::vtkInternal::vtkInternal(vtkLightBoxRendererManager* external):
  External(external)
{
  this->CornerAnnotation = vtkSmartPointer<vtkCornerAnnotation>::New();
  this->RenderWindowRowCount = 0;
  this->RenderWindowColumnCount = 0;
  this->RenderWindowLayoutType = vtkLightBoxRendererManager::LeftRightTopBottom;
  this->ColorWindow = 255;
  this->ColorLevel = 127.5;
  this->RendererLayer = 0;
  // Default background color: black
  this->RendererBackgroundColor[0] = 0.0;
  this->RendererBackgroundColor[1] = 0.0;
  this->RendererBackgroundColor[2] = 0.0;
  // Default highlightedBox color: green
  this->HighlightedBoxColor[0] = 0.0;
  this->HighlightedBoxColor[1] = 1.0;
  this->HighlightedBoxColor[2] = 0.0;

  this->CornerAnnotation->SetMaximumLineHeight(0.07);
  vtkTextProperty *tprop = this->CornerAnnotation->GetTextProperty();
  tprop->ShadowOn();
}

// --------------------------------------------------------------------------
vtkLightBoxRendererManager::vtkInternal::~vtkInternal()
{
  for(RenderWindowItemListIt it = this->RenderWindowItemList.begin();
      it != this->RenderWindowItemList.end();
      ++it)
    {
    delete *it;
    }
  this->RenderWindowItemList.clear();
}

// --------------------------------------------------------------------------
void vtkLightBoxRendererManager::vtkInternal::SetupCornerAnnotation()
{
  for(RenderWindowItemListIt it = this->RenderWindowItemList.begin();
      it != this->RenderWindowItemList.end();
      ++it)
    {
    if (!(*it)->Renderer->HasViewProp(this->CornerAnnotation))
      {
      (*it)->Renderer->AddViewProp(this->CornerAnnotation);
      }
    }

  this->CornerAnnotation->ClearAllTexts();
  this->CornerAnnotation->SetText(2, this->CornerAnnotationText.c_str());
}

//---------------------------------------------------------------------------
void vtkLightBoxRendererManager::vtkInternal::setupRendering()
{
  assert(this->RenderWindow);
  
  // Remove only renderers managed by this light box
  for(RenderWindowItemListIt it = this->RenderWindowItemList.begin();
      it != this->RenderWindowItemList.end();
      ++it)
    {
    this->RenderWindow->GetRenderers()->RemoveItem((*it)->Renderer);
    }

  // Compute the width and height of each RenderWindowItem
  double viewportWidth  = 1.0 / static_cast<double>(this->RenderWindowColumnCount);
  double viewportHeight = 1.0 / static_cast<double>(this->RenderWindowRowCount);

  // Postion of the Top-Left corner of the RenderWindowItem
  float xMin, yMin;

  // Loop through RenderWindowItem
  for ( int rowId = 0; rowId < this->RenderWindowRowCount; ++rowId )
    {
    yMin = (this->RenderWindowRowCount - 1 - rowId) * viewportHeight;
    xMin = 0.0;

    for ( int columnId = 0; columnId < this->RenderWindowColumnCount; ++columnId )
      {
      // Get reference to the renderWindowItem
      RenderWindowItem * item  =
          this->RenderWindowItemList.at(
              this->External->ComputeRenderWindowItemId(rowId, columnId));

      // Set viewport
      item->SetViewport(xMin, yMin, viewportWidth, viewportHeight);

      // Add to RenderWindow
      this->RenderWindow->AddRenderer(item->Renderer);

      xMin += viewportWidth;
      }
    }
}

// --------------------------------------------------------------------------
void vtkLightBoxRendererManager::vtkInternal::updateRenderWindowItemsZIndex(int layoutType)
{
  for (int rowId = 0; rowId < this->RenderWindowRowCount; ++rowId)
    {
    for (int columnId = 0; columnId < this->RenderWindowColumnCount; ++columnId)
      {
      int itemId = this->External->ComputeRenderWindowItemId(rowId, columnId);
      assert(itemId <= static_cast<int>(this->RenderWindowItemList.size()));

      RenderWindowItem * item = this->RenderWindowItemList.at(itemId);
      assert(item->ImageMapper->GetInput());

      // Default to ctkVTKSliceView::LeftRightTopBottom
      int zSliceIndex = rowId * this->RenderWindowColumnCount + columnId;

      if (layoutType == vtkLightBoxRendererManager::LeftRightBottomTop)
        {
        zSliceIndex = (this->RenderWindowRowCount - rowId - 1) *
                      this->RenderWindowColumnCount + columnId;
        }

      item->ImageMapper->SetZSlice(zSliceIndex);
      }
    }
}


// --------------------------------------------------------------------------
void vtkLightBoxRendererManager::vtkInternal
::SetItemInput(RenderWindowItem* item)
{
#if (VTK_MAJOR_VERSION <= 5)
  item->ImageMapper->SetInput(this->ImageData);
#else
  item->ImageMapper->SetInputConnection(this->ImageDataConnection);
  bool hasViewProp = item->Renderer->HasViewProp(item->ImageActor);
  if (!this->ImageDataConnection && hasViewProp)
    {
    item->Renderer->RemoveViewProp(item->ImageActor);
    item->Renderer->RemoveViewProp(item->HighlightedBoxActor);
    }
  else if (this->ImageDataConnection && !hasViewProp)
    {
    item->Renderer->AddActor2D(item->ImageActor);
    item->Renderer->AddActor2D(item->HighlightedBoxActor);
    }
#endif
}

//---------------------------------------------------------------------------
// vtkLightBoxRendererManager methods

// --------------------------------------------------------------------------
vtkLightBoxRendererManager::vtkLightBoxRendererManager() : Superclass()
{
  this->Internal = new vtkInternal(this);
}

// --------------------------------------------------------------------------
vtkLightBoxRendererManager::~vtkLightBoxRendererManager()
{
  delete this->Internal;
}

//----------------------------------------------------------------------------
void vtkLightBoxRendererManager::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkLightBoxRendererManager::SetRendererLayer(int newLayer)
{
  if (this->IsInitialized())
    {
    vtkErrorMacro(<< "SetRendererLayer failed - vtkLightBoxRendererManager is initialized");
    return;
    }

  if (newLayer == this->Internal->RendererLayer)
    {
    return;
    }

  this->Internal->RendererLayer = newLayer;

  this->Modified();
}

//----------------------------------------------------------------------------
vtkRenderWindow* vtkLightBoxRendererManager::GetRenderWindow()
{
  return this->Internal->RenderWindow;
}

//----------------------------------------------------------------------------
void vtkLightBoxRendererManager::Initialize(vtkRenderWindow* renderWindow)
{
  if (this->Internal->RenderWindow)
    {
    vtkWarningMacro( << "vtkLightBoxRendererManager has already been initialized");
    return;
    }
  if (!renderWindow)
    {
    vtkErrorMacro("Failed to initialize vtkLightBoxRendererManager with a NULL renderWindow");
    return;
    }
  this->Internal->RenderWindow = renderWindow;

  // Set default Layout
  this->SetRenderWindowLayout(1, 1); // Modified() is invoked by SetRenderWindowLayout
}

//----------------------------------------------------------------------------
bool vtkLightBoxRendererManager::IsInitialized()
{
  return this->Internal->RenderWindow;
}

//----------------------------------------------------------------------------
#if (VTK_MAJOR_VERSION <= 5)
void vtkLightBoxRendererManager::SetImageData(vtkImageData* newImageData)
#else
void vtkLightBoxRendererManager::SetImageDataConnection(vtkAlgorithmOutput* newImageDataConnection)
#endif
{
  if (!this->IsInitialized())
    {
#if (VTK_MAJOR_VERSION <= 5)
    vtkErrorMacro(<< "SetImageData failed - vtkLightBoxRendererManager is NOT initialized");
#else
    vtkErrorMacro(<< "SetImageDataConnection failed - vtkLightBoxRendererManager is NOT initialized");
#endif
    return;
    }

#if (VTK_MAJOR_VERSION <= 5)
  this->Internal->ImageData = newImageData;
#else
  this->Internal->ImageDataConnection = newImageDataConnection;
#endif

  vtkInternal::RenderWindowItemListIt it;
  for(it = this->Internal->RenderWindowItemList.begin();
      it != this->Internal->RenderWindowItemList.end();
      ++it)
    {
    this->Internal->SetItemInput(*it);
    }

#if (VTK_MAJOR_VERSION <= 5)
  if (newImageData)
#else
  if (newImageDataConnection)
#endif
    {
    this->Internal->updateRenderWindowItemsZIndex(this->Internal->RenderWindowLayoutType);
    }

  this->Modified();
}

//----------------------------------------------------------------------------
vtkCamera* vtkLightBoxRendererManager::GetActiveCamera()
{
  if (this->Internal->RenderWindowItemList.size() == 0)
    {
    return 0;
    }

  // Obtain reference of the first renderer
  vtkRenderer * firstRenderer = this->Internal->RenderWindowItemList.at(0)->Renderer;
  if (firstRenderer->IsActiveCameraCreated())
    {
    return firstRenderer->GetActiveCamera();
    }
  else
    {
    return 0;
    }
  return 0;
}

//----------------------------------------------------------------------------
void vtkLightBoxRendererManager::SetActiveCamera(vtkCamera* newActiveCamera)
{
  if (!this->IsInitialized())
    {
    vtkErrorMacro(<< "SetActiveCamera failed - vtkLightBoxRendererManager is NOT initialized");
    return;
    }
  if (newActiveCamera == this->GetActiveCamera())
    {
    return;
    }

  newActiveCamera->ParallelProjectionOn();

  vtkInternal::RenderWindowItemListIt it;
  for(it = this->Internal->RenderWindowItemList.begin();
      it != this->Internal->RenderWindowItemList.end();
      ++it)
    {
    (*it)->Renderer->SetActiveCamera(newActiveCamera);
    }

  this->Modified();
}

//----------------------------------------------------------------------------
void vtkLightBoxRendererManager::ResetCamera()
{
  if (!this->IsInitialized())
    {
    vtkErrorMacro(<< "ResetCamera failed - vtkLightBoxRendererManager is NOT initialized");
    return;
    }
  vtkInternal::RenderWindowItemListIt it;
  for(it = this->Internal->RenderWindowItemList.begin();
      it != this->Internal->RenderWindowItemList.end();
      ++it)
    {
    (*it)->Renderer->ResetCamera();
    }
  this->Modified();
}

//----------------------------------------------------------------------------
int vtkLightBoxRendererManager::GetRenderWindowItemCount()
{
  return static_cast<int>(this->Internal->RenderWindowItemList.size());
}

//----------------------------------------------------------------------------
vtkRenderer* vtkLightBoxRendererManager::GetRenderer(int id)
{
  if (id < 0 || id >= static_cast<int>(this->Internal->RenderWindowItemList.size()))
    {
    return 0;
    }
  return this->Internal->RenderWindowItemList.at(id)->Renderer;
}

//----------------------------------------------------------------------------
vtkRenderer* vtkLightBoxRendererManager::GetRenderer(int rowId, int columnId)
{
  return this->GetRenderer(this->ComputeRenderWindowItemId(rowId, columnId));
}

//----------------------------------------------------------------------------
void vtkLightBoxRendererManager::SetRenderWindowLayoutType(int layoutType)
{
  if (this->Internal->RenderWindowLayoutType == layoutType)
    {
    return;
    }

#if (VTK_MAJOR_VERSION <= 5)
  if (this->Internal->ImageData)
#else
  if (this->Internal->ImageDataConnection)
#endif
    {
    this->Internal->updateRenderWindowItemsZIndex(layoutType);
    }

  this->Internal->RenderWindowLayoutType = layoutType;

  this->Modified();
}

//----------------------------------------------------------------------------
int vtkLightBoxRendererManager::GetRenderWindowLayoutType() const
{
  return this->Internal->RenderWindowLayoutType;
}

//----------------------------------------------------------------------------
void vtkLightBoxRendererManager::SetRenderWindowLayout(int rowCount, int columnCount)
{
  if (!this->IsInitialized())
    {
    vtkErrorMacro(<< "SetRenderWindowLayout failed - "
                  "vtkLightBoxRendererManager is NOT initialized");
    return;
    }

  // Sanity checks
  assert(rowCount >= 0 && columnCount >= 0);
  if(!(rowCount >= 0 && columnCount >= 0))
    {
    return;
    }


  if (this->Internal->RenderWindowRowCount == rowCount && 
      this->Internal->RenderWindowColumnCount == columnCount)
    {
    return;
    }

  int extraItem = (rowCount * columnCount)
    - static_cast<int>(this->Internal->RenderWindowItemList.size());
  if (extraItem > 0)
    {
    // Create extra RenderWindowItem(s)
    while(extraItem > 0)
      {
      RenderWindowItem * item =
          new RenderWindowItem(this->Internal->RendererBackgroundColor,
                               this->Internal->HighlightedBoxColor,
                               this->Internal->ColorWindow, this->Internal->ColorLevel);
      item->Renderer->SetLayer(this->Internal->RendererLayer);
      this->Internal->SetItemInput(item);
      this->Internal->RenderWindowItemList.push_back(item);
      --extraItem;
      }
    }
  else
    {
    // Remove extra RenderWindowItem(s)
    extraItem = extraItem >= 0 ? extraItem : -extraItem; // Compute Abs
    while(extraItem > 0)
      {
      delete this->Internal->RenderWindowItemList.back();
      this->Internal->RenderWindowItemList.pop_back();
      --extraItem;
      }
    }

  this->Internal->RenderWindowRowCount = rowCount;
  this->Internal->RenderWindowColumnCount = columnCount;

  this->Internal->setupRendering();
  this->Internal->SetupCornerAnnotation();

#if (VTK_MAJOR_VERSION <= 5)
  if (this->Internal->ImageData)
#else
  if (this->Internal->ImageDataConnection)
#endif
    {
    this->Internal->updateRenderWindowItemsZIndex(this->Internal->RenderWindowLayoutType);
    }
  this->Modified();
}

//----------------------------------------------------------------------------
int vtkLightBoxRendererManager::GetRenderWindowRowCount()
{
  return this->Internal->RenderWindowRowCount;
}

//----------------------------------------------------------------------------
void vtkLightBoxRendererManager::SetRenderWindowRowCount(int newRowCount)
{
  this->SetRenderWindowLayout(newRowCount, this->GetRenderWindowColumnCount());
}

//----------------------------------------------------------------------------
int vtkLightBoxRendererManager::GetRenderWindowColumnCount()
{
  return this->Internal->RenderWindowColumnCount;
}

//----------------------------------------------------------------------------
void vtkLightBoxRendererManager::SetRenderWindowColumnCount(int newColumnCount)
{
  this->SetRenderWindowLayout(this->GetRenderWindowRowCount(), newColumnCount);
}

//----------------------------------------------------------------------------
bool vtkLightBoxRendererManager::GetHighlightedById(int id)
{
  if (!this->IsInitialized())
    {
    vtkErrorMacro(<< "SetHighlightedById failed - vtkLightBoxRendererManager is NOT initialized");
    return false;
    }
  if (id < 0 || id >= static_cast<int>(this->Internal->RenderWindowItemList.size()))
    {
    return false;
    }
  return this->Internal->RenderWindowItemList.at(id)->HighlightedBoxActor->GetVisibility();
}

//----------------------------------------------------------------------------
bool vtkLightBoxRendererManager::GetHighlighted(int rowId, int columnId)
{
  return this->GetHighlightedById(this->ComputeRenderWindowItemId(rowId, columnId));
}

//----------------------------------------------------------------------------
void vtkLightBoxRendererManager::SetHighlightedById(int id, bool highlighted)
{
  if (!this->IsInitialized())
    {
    vtkErrorMacro(<< "SetHighlightedById failed - vtkLightBoxRendererManager is NOT initialized");
    return;
    }
  if (id < 0 || id >= static_cast<int>(this->Internal->RenderWindowItemList.size()))
    {
    return;
    }
  this->Internal->RenderWindowItemList.at(id)->HighlightedBoxActor->SetVisibility(highlighted);

  this->Modified();
}

//----------------------------------------------------------------------------
void vtkLightBoxRendererManager::SetHighlighted(int rowId, int columnId, bool highlighted)
{
  this->SetHighlightedById(this->ComputeRenderWindowItemId(rowId, columnId), highlighted);
}

//----------------------------------------------------------------------------
void vtkLightBoxRendererManager::SetHighlightedBoxColor(double newHighlightedBoxColor[3])
{
  if (!this->IsInitialized())
    {
    vtkErrorMacro(<< "SetHighlightedById failed - vtkLightBoxRendererManager is NOT initialized");
    return;
    }

  if (this->Internal->HighlightedBoxColor[0] == newHighlightedBoxColor[0] &&
      this->Internal->HighlightedBoxColor[1] == newHighlightedBoxColor[1] &&
      this->Internal->HighlightedBoxColor[2] == newHighlightedBoxColor[2])
    {
    return;
    }

  this->Internal->HighlightedBoxColor[0] = newHighlightedBoxColor[0];
  this->Internal->HighlightedBoxColor[1] = newHighlightedBoxColor[1];
  this->Internal->HighlightedBoxColor[2] = newHighlightedBoxColor[2];

  vtkInternal::RenderWindowItemListIt it;
  for(it = this->Internal->RenderWindowItemList.begin();
      it != this->Internal->RenderWindowItemList.end();
      ++it)
    {
    (*it)->SetHighlightedBoxColor(newHighlightedBoxColor);
    }

  this->Modified();
}

//----------------------------------------------------------------------------
double* vtkLightBoxRendererManager::GetHighlightedBoxColor() const
{
  return this->Internal->HighlightedBoxColor;
}

//----------------------------------------------------------------------------
int vtkLightBoxRendererManager::ComputeRenderWindowItemId(int rowId, int columnId)
{
  return this->Internal->RenderWindowColumnCount * rowId + columnId;
}

//----------------------------------------------------------------------------
void vtkLightBoxRendererManager::SetCornerAnnotationText(const std::string& text)
{
  if (!this->IsInitialized())
    {
    vtkErrorMacro(<< "SetCornerAnnotationText failed - "
                  "vtkLightBoxRendererManager is NOT initialized");
    return;
    }
  if (text.compare(this->Internal->CornerAnnotationText) == 0)
    {
    return;
    }

  this->Internal->CornerAnnotation->ClearAllTexts();
  this->Internal->CornerAnnotation->SetText(2, text.c_str());

  this->Internal->CornerAnnotationText = text;

  this->Modified();
}

//----------------------------------------------------------------------------
const std::string vtkLightBoxRendererManager::GetCornerAnnotationText() const
{
  const char * text = this->Internal->CornerAnnotation->GetText(2);
  return text ? text : "";
}

// --------------------------------------------------------------------------
vtkCornerAnnotation * vtkLightBoxRendererManager::GetCornerAnnotation() const
{
  return this->Internal->CornerAnnotation;
}

// --------------------------------------------------------------------------
void vtkLightBoxRendererManager::SetCornerAnnotation(vtkCornerAnnotation* annotation)
{
  // Remove current corner annotation
  vtkInternal::RenderWindowItemListIt it;
  for(it = this->Internal->RenderWindowItemList.begin();
      it != this->Internal->RenderWindowItemList.end();
      ++it)
    {
    if (!(*it)->Renderer->HasViewProp(this->Internal->CornerAnnotation))
      {
      (*it)->Renderer->RemoveViewProp(this->Internal->CornerAnnotation);
      }
    }
  this->Internal->CornerAnnotation = annotation;
}

// --------------------------------------------------------------------------
void vtkLightBoxRendererManager::SetBackgroundColor(const double newBackgroundColor[3])
{
  if (!this->IsInitialized())
    {
    vtkErrorMacro(<< "SetBackgroundColor failed - vtkLightBoxRendererManager is NOT initialized");
    return;
    }
  vtkInternal::RenderWindowItemListIt it;
  for(it = this->Internal->RenderWindowItemList.begin();
      it != this->Internal->RenderWindowItemList.end();
      ++it)
    {
    (*it)->Renderer->SetBackground(newBackgroundColor[0],
                                   newBackgroundColor[1],
                                   newBackgroundColor[2]);
    }

  this->Internal->RendererBackgroundColor[0] = newBackgroundColor[0];
  this->Internal->RendererBackgroundColor[1] = newBackgroundColor[1];
  this->Internal->RendererBackgroundColor[2] = newBackgroundColor[2];

  this->Modified();
}

//----------------------------------------------------------------------------
double* vtkLightBoxRendererManager::GetBackgroundColor()const
{
  return this->Internal->RendererBackgroundColor;
}

//----------------------------------------------------------------------------
void vtkLightBoxRendererManager::SetColorLevel(double colorLevel)
{
  this->SetColorWindowAndLevel(this->Internal->ColorWindow, colorLevel);
}

//----------------------------------------------------------------------------
double vtkLightBoxRendererManager::GetColorLevel()const
{
  return this->Internal->ColorLevel;
}

//----------------------------------------------------------------------------
void vtkLightBoxRendererManager::SetColorWindow(double colorWindow)
{
  this->SetColorWindowAndLevel(colorWindow, this->Internal->ColorLevel);
}

//----------------------------------------------------------------------------
double vtkLightBoxRendererManager::GetColorWindow()const
{
  return this->Internal->ColorWindow;
}

//----------------------------------------------------------------------------
void vtkLightBoxRendererManager::SetColorWindowAndLevel(double colorWindow, double colorLevel)
{
  if (this->Internal->ColorWindow == colorWindow &&
      this->Internal->ColorLevel == colorLevel)
    {
    return;
    }

  vtkInternal::RenderWindowItemListIt it;
  for(it = this->Internal->RenderWindowItemList.begin();
      it != this->Internal->RenderWindowItemList.end();
      ++it)
    {
    (*it)->ImageMapper->SetColorWindow(colorWindow);
    (*it)->ImageMapper->SetColorLevel(colorLevel);
    }

  this->Internal->ColorWindow = colorWindow;
  this->Internal->ColorLevel = colorLevel;

  this->Modified();
}

