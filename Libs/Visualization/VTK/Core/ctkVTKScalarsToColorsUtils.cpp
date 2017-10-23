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
// CTK includes
#include "ctkLogger.h"
#include "ctkVTKScalarsToColorsUtils.h"

// VTK includes
#include <vtkDiscretizableColorTransferFunction.h>
#include <vtkMath.h>
#include <vtkPiecewiseFunction.h>
#include <vtkScalarsToColors.h>

// ----------------------------------------------------------------------------
static ctkLogger logger(
  "org.commontk.visualization.vtk.core.ctkVTKScalarsToColorsUtils");

// ----------------------------------------------------------------------------
void ctk::remapColorScale(
  vtkDiscretizableColorTransferFunction* colorTransferFunction,
  vtkDiscretizableColorTransferFunction* rescaledColorTransferFunction,
  double minRescale, double maxRescale)
{
  /// Color
  double rescaleWidth = maxRescale - minRescale;
  double* range = colorTransferFunction->GetRange();

  rescaledColorTransferFunction->RemoveAllPoints();

  for (int i = 0; i < colorTransferFunction->GetSize(); i++)
  {
    double val[6];
    colorTransferFunction->GetNodeValue(i, val);
    double normalized = (val[0] - range[0]) / (range[1] - range[0]);

    double newX = minRescale + normalized * rescaleWidth;
    rescaledColorTransferFunction->AddRGBPoint(newX, val[1], val[2], val[3],
      val[4], val[5]);
  }

  /// Opacity
  if (rescaledColorTransferFunction->GetScalarOpacityFunction() != CTK_NULLPTR)
  {
    rescaledColorTransferFunction->GetScalarOpacityFunction()->
      RemoveAllPoints();
  }
  else
  {
    vtkSmartPointer<vtkPiecewiseFunction> opacityFunction =
      vtkSmartPointer<vtkPiecewiseFunction>::New();
    rescaledColorTransferFunction->SetScalarOpacityFunction(opacityFunction);
  }

  if (colorTransferFunction->GetScalarOpacityFunction() == CTK_NULLPTR)
  {
    rescaledColorTransferFunction->Build();
    return;
  }

  for (int i = 0;
    i < colorTransferFunction->GetScalarOpacityFunction()->GetSize(); i++)
  {
    double val[4];
    colorTransferFunction->GetScalarOpacityFunction()->GetNodeValue(i, val);
    double normalized = (val[0] - range[0]) / (range[1] - range[0]);

    double newX = minRescale + normalized * rescaleWidth;
    rescaledColorTransferFunction->GetScalarOpacityFunction()->AddPoint(newX,
      val[1], val[2], val[3]);
  }
  rescaledColorTransferFunction->Build();
}

// ----------------------------------------------------------------------------
void ctk::remapColorScale(
  vtkDiscretizableColorTransferFunction* colorTransferFunction,
  double minRescale, double maxRescale)
{
  vtkSmartPointer<vtkDiscretizableColorTransferFunction> rescaled =
    vtkSmartPointer<vtkDiscretizableColorTransferFunction>::New();
  ctk::remapColorScale(colorTransferFunction, rescaled, minRescale, maxRescale);
  ctk::remapColorScale(rescaled, colorTransferFunction, minRescale, maxRescale);
}

// ----------------------------------------------------------------------------
void ctk::reverseColorMap(vtkDiscretizableColorTransferFunction* ctf)
{
  if (ctf == CTK_NULLPTR)
  {
    return;
  }

  int size = ctf->GetSize();
  for (int i = 0; i < size / 2; i++)
  {
    double val[6];
    ctf->GetNodeValue(i, val);

    double valRev[6];
    ctf->GetNodeValue(size - 1 - i, valRev);

    std::swap(val[0], valRev[0]);

    ctf->SetNodeValue(i, valRev);
    ctf->SetNodeValue(size - 1 - i, val);
  }
  ctf->Modified();
}

// ----------------------------------------------------------------------------
void ctk::setTransparency(vtkDiscretizableColorTransferFunction* ctf,
  double transparency)
{
  if (ctf == CTK_NULLPTR)
  {
    return;
  }

  for (int i = 0; i < ctf->GetScalarOpacityFunction()->GetSize(); ++i)
  {
    double val[4];
    ctf->GetScalarOpacityFunction()->GetNodeValue(i, val);

    val[1] = vtkMath::ClampValue(val[1] * transparency, 1e-6, 1.);

    ctf->GetScalarOpacityFunction()->SetNodeValue(i, val);
  }
  ctf->Modified();
}
