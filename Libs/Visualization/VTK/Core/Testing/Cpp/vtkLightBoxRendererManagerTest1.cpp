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

// CTKVTK includes
#include "vtkLightBoxRendererManager.h"

// CTKCore includes
#include "ctkCommandLineParser.h"

// VTK includes
#include <vtkImageData.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageReader2.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNew.h>
#include <vtkRegressionTestImage.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkTestUtilities.h>

// STD includes
#include <cstdlib>
  
//----------------------------------------------------------------------------
int vtkLightBoxRendererManagerTest1(int argc, char* argv[])
{
  const char * imageFilename = vtkTestUtilities::ExpandDataFileName(argc, argv, "HeadMRVolume.mhd");

  //----------------------------------------------------------------------------
  // Read Image
  //----------------------------------------------------------------------------
  // Instanciate the reader factory
  vtkNew<vtkImageReader2Factory> imageFactory;

  // Instanciate an image reader
  vtkSmartPointer<vtkImageReader2> imageReader;
  imageReader.TakeReference(imageFactory->CreateImageReader2(imageFilename));
  if (!imageReader)
    {
    std::cerr << "Failed to instanciate image reader using: " << imageFilename << std::endl;
    return EXIT_FAILURE;
    }

  // Read image
  imageReader->SetFileName(imageFilename);
  vtkAlgorithmOutput* imagePort = imageReader->GetOutputPort();

  //----------------------------------------------------------------------------
  // Renderer, RenderWindow and Interactor
  //----------------------------------------------------------------------------
  vtkNew<vtkRenderer> rr;
  vtkNew<vtkRenderWindow> rw;
  vtkNew<vtkRenderWindowInteractor> ri;
  rw->SetSize(600, 600);
  rw->SetMultiSamples(0); // Ensure to have the same test image everywhere
  rw->AddRenderer(rr.GetPointer());
  rw->SetInteractor(ri.GetPointer());
  
  // Set Interactor Style
  vtkNew<vtkInteractorStyleImage> iStyle;
  ri->SetInteractorStyle(iStyle.GetPointer());

  vtkNew<vtkLightBoxRendererManager> lightBoxRendererManager;

  //----------------------------------------------------------------------------
  // Check if non initialized case is handled properly / Check default value
  //----------------------------------------------------------------------------
  if (lightBoxRendererManager->IsInitialized() != 0)
    {
    std::cerr << "line " << __LINE__ << " - Problem with IsInitialized()" << std::endl;
    std::cerr << "  expected: 0" << std::endl;
    std::cerr << "  current:" << lightBoxRendererManager->IsInitialized() << std::endl;
    return EXIT_FAILURE;
    }

  if (lightBoxRendererManager->GetRenderWindow() != 0)
    {
    std::cerr << "line " << __LINE__ << " - Problem with GetRenderWindow()" << std::endl;
    std::cerr << "  expected: 0" << std::endl;
    std::cerr << "  current:" << lightBoxRendererManager->GetRenderWindow() << std::endl;
    return EXIT_FAILURE;
    }

  if (lightBoxRendererManager->GetActiveCamera() != 0)
    {
    std::cerr << "line " << __LINE__ << " - Problem with GetActiveCamera()" << std::endl;
    std::cerr << "  expected: 0" << std::endl;
    std::cerr << "  current:" << lightBoxRendererManager->GetActiveCamera() << std::endl;
    return EXIT_FAILURE;
    }

  if (lightBoxRendererManager->GetRenderWindowItemCount() != 0)
    {
    std::cerr << "line " << __LINE__ << " - Problem with GetRenderWindowItemCount()" << std::endl;
    std::cerr << "  expected: 0" << std::endl;
    std::cerr << "  current:" << lightBoxRendererManager->GetRenderWindowItemCount() << std::endl;
    return EXIT_FAILURE;
    }

  if (lightBoxRendererManager->GetRenderer(4) != 0)
    {
    std::cerr << "line " << __LINE__ << " - Problem with GetRenderer()" << std::endl;
    std::cerr << "  expected: 0" << std::endl;
    std::cerr << "  current:" << lightBoxRendererManager->GetRenderer(4) << std::endl;
    return EXIT_FAILURE;
    }

  if (lightBoxRendererManager->GetRenderer(1,1) != 0)
    {
    std::cerr << "line " << __LINE__ << " - Problem with GetRenderer()" << std::endl;
    std::cerr << "  expected: 0" << std::endl;
    std::cerr << "  current:" << lightBoxRendererManager->GetRenderer(1,1) << std::endl;
    return EXIT_FAILURE;
    }

  if (lightBoxRendererManager->GetCornerAnnotationText().compare("") != 0)
    {
    std::cerr << "line " << __LINE__ << " - Problem with GetCornerAnnotationText()" << std::endl;
    std::cerr << "  expected: Empty" << std::endl;
    std::cerr << "  current:" << lightBoxRendererManager->GetCornerAnnotationText() << std::endl;
    return EXIT_FAILURE;
    }

  if (lightBoxRendererManager->GetRenderWindowLayoutType() !=
      vtkLightBoxRendererManager::LeftRightTopBottom)
    {
    std::cerr << "line " << __LINE__ << " - Problem with GetRenderWindowLayoutType()" << std::endl;
    std::cerr << "  expected: " <<
        static_cast<int>(vtkLightBoxRendererManager::LeftRightTopBottom) << std::endl;
    std::cerr << "  current:" <<
        static_cast<int>(lightBoxRendererManager->GetRenderWindowLayoutType()) << std::endl;
    return EXIT_FAILURE;
    }

  unsigned long mtime = lightBoxRendererManager->GetMTime();

  lightBoxRendererManager->ResetCamera();
  if (mtime != lightBoxRendererManager->GetMTime())
    {
    std::cerr << "line " << __LINE__ << " - Problem with ResetCamera()" << std::endl;
    return EXIT_FAILURE;
    }

  lightBoxRendererManager->SetActiveCamera(0);
  if (mtime != lightBoxRendererManager->GetMTime())
    {
    std::cerr << "line " << __LINE__ << " - Problem with SetActiveCamera()" << std::endl;
    return EXIT_FAILURE;
    }

  lightBoxRendererManager->SetImageDataConnection(imagePort);

  if (mtime != lightBoxRendererManager->GetMTime())
    {
    std::cerr << "line " << __LINE__ << " - Problem with SetImageDataConnection()" << std::endl;
    return EXIT_FAILURE;
    }
  
  double highlightedboxColor[3] = {0.0, 1.0, 0.0};
  lightBoxRendererManager->SetHighlightedBoxColor(highlightedboxColor);
  if (mtime != lightBoxRendererManager->GetMTime())
    {
    std::cerr << "line " << __LINE__ << " - Problem with SetHighlightedBoxColor()" << std::endl;
    return EXIT_FAILURE;
    }

  //----------------------------------------------------------------------------
  // Initialize
  //----------------------------------------------------------------------------

  lightBoxRendererManager->Initialize(rw.GetPointer());

  if (lightBoxRendererManager->IsInitialized() != 1)
    {
    std::cerr << "line " << __LINE__ << " - Problem with IsInitialized()" << std::endl;
    std::cerr << "  expected: 1" << std::endl;
    std::cerr << "  current:" << lightBoxRendererManager->IsInitialized() << std::endl;
    return EXIT_FAILURE;
    }

  if (mtime == lightBoxRendererManager->GetMTime())
    {
    std::cerr << "line " << __LINE__ << " - Problem with IsInitialized()" << std::endl;
    return EXIT_FAILURE;
    }

  lightBoxRendererManager->SetImageDataConnection(imagePort);
  lightBoxRendererManager->SetRenderWindowLayout(4, 5);
  lightBoxRendererManager->SetHighlighted(2,2,true);
  lightBoxRendererManager->SetColorWindowAndLevel(100, 100);
  double backgroundColor[3] = {0.5, 0.5, 0.5};
  lightBoxRendererManager->SetBackgroundColor(backgroundColor);
  double highlightedBoxColor[3] = {1.0, 1.0, 0.0};
  lightBoxRendererManager->SetHighlightedBoxColor(highlightedBoxColor);

  int retval = vtkRegressionTestImage(rw.GetPointer());
  if (retval == vtkRegressionTester::DO_INTERACTOR)
    {
    rw->GetInteractor()->Initialize();
    rw->GetInteractor()->Start();
    }
  
  return !retval;
}
