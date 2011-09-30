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
#include "ctkVTKVolumePropertyWidget.h"

// VTK includes
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkSmartPointer.h>
#include <vtkVolumeProperty.h>

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
int ctkVTKVolumePropertyWidgetTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  vtkSmartPointer<vtkColorTransferFunction> ctf =
    vtkSmartPointer<vtkColorTransferFunction>::New();
  ctf->AddRGBPoint(0.2, 0.6941,0.76,0., 0.5, 0.);
  ctf->AddRGBPoint(0.4, 0.,0.6941,0.96);
  ctf->AddRGBPoint(0.8, 0.9686,0.76,0.);

  vtkSmartPointer<vtkPiecewiseFunction> otf =
    vtkSmartPointer<vtkPiecewiseFunction>::New();
  otf->AddPoint(0.2, 0.6941);
  otf->AddPoint(0.4, 0.641);
  otf->AddPoint(0.8, 0.9686);

  vtkSmartPointer<vtkPiecewiseFunction> otf2 =
    vtkSmartPointer<vtkPiecewiseFunction>::New();
  otf2->AddPoint(0.0, 0.6941);
  otf2->AddPoint(0.1, 0.641);
  otf2->AddPoint(0.7, 0.9686);

  vtkSmartPointer<vtkVolumeProperty> volumeProperty =
    vtkSmartPointer<vtkVolumeProperty>::New();
  volumeProperty->SetColor(ctf);
  volumeProperty->SetScalarOpacity(otf);
  double range[2] = {0., 1.};
  volumeProperty->GetGradientOpacity()->AdjustRange(range);

  ctkVTKVolumePropertyWidget widget;
  //widget.setUseThresholdSlider(true);
  widget.setVolumeProperty(volumeProperty);

  volumeProperty->SetScalarOpacity(otf2);

  widget.moveAllPoints(0.1, -0.1);
  widget.moveAllPoints(-0.1, 0.1);

  widget.show();

  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }
  return app.exec();
}
