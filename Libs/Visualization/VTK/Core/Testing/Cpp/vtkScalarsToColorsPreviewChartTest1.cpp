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

// Qt includes
#include <QApplication>
#include <QDebug>
#include <QTimer>

// CTK includes
#include "vtkScalarsToColorsPreviewChart.h"

// VTK includes
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkDiscretizableColorTransferFunction.h>
#include <vtkNew.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

// STD includes

//-----------------------------------------------------------------------------
int vtkScalarsToColorsPreviewChartTest1(int argc, char * argv [] )
{
  QCoreApplication app(argc, argv);

  vtkNew<vtkScalarsToColorsPreviewChart> previewChart;

  vtkNew<vtkContextView> contextView;
  contextView->GetScene()->AddItem(previewChart.Get());

  //Dummy presets
  vtkNew<vtkDiscretizableColorTransferFunction> discretizableCTF;
  discretizableCTF->AddRGBPoint(0.0, 0, 0, 1.0);
  discretizableCTF->AddRGBPoint(255.0, 1.0, 0, 0);
  vtkNew<vtkPiecewiseFunction> piecewiseFunction;
  piecewiseFunction->AddPoint(0.0, 0);
  piecewiseFunction->AddPoint(255.0, 1.0);
  discretizableCTF->SetScalarOpacityFunction(piecewiseFunction.Get());
  discretizableCTF->EnableOpacityMappingOn();

  previewChart->SetColorTransferFunction(discretizableCTF.Get());
  previewChart->SetLayoutStrategy(vtkChart::FILL_SCENE);

  contextView->GetRenderWindow()->Render();
  contextView->GetRenderWindow()->GetInteractor()->Initialize();
  contextView->GetRenderWindow()->GetInteractor()->CreateOneShotTimer(10);
  contextView->GetRenderWindow()->GetInteractor()->Start();

  if (argc < 2 || QString(argv[1]) != "-I")
  {
    QTimer::singleShot(1000, &app, SLOT(quit()));
  }

  return app.exec();
}
