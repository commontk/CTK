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
#include <QSharedPointer>
#include <QTimer>

// CTK includes
#include "ctkTransferFunction.h"
#include "ctkTransferFunctionControlPointsItem.h"
#include "ctkTransferFunctionGradientItem.h"
#include "ctkTransferFunctionView.h"
#include "ctkVTKCompositeFunction.h"

// VTK includes
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkSmartPointer.h>

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
int ctkTransferFunctionViewTest4(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  // Points have to be between 0 and 1

  vtkSmartPointer<vtkPiecewiseFunction> pwf =
    vtkSmartPointer<vtkPiecewiseFunction>::New();
  //
  pwf->AddPoint(0., 0.3, .5, .5);
  pwf->AddPoint(0.2,.2, .5, .5);
  pwf->AddPoint(0.3, .5, .5, .5);
  pwf->AddPoint(0.9, .5, .5, .5);

  vtkSmartPointer<vtkColorTransferFunction> ctf =
    vtkSmartPointer<vtkColorTransferFunction>::New();
  //
  ctf->AddRGBPoint(0., 1.,0.,0., 0., 0.);
  ctf->AddRGBPoint(0.2, 0.,1.,0., 0.5, 0.);
  ctf->AddRGBPoint(0.3, 1.,0.,0.6, 0.5, 0.);
  ctf->AddRGBPoint(0.9, 0.,0.,1., 0.5, 0.);

  QSharedPointer<ctkTransferFunction> transferFunction =
    QSharedPointer<ctkTransferFunction>(new ctkVTKCompositeFunction(pwf, ctf));
  ctkTransferFunctionView transferFunctionView(0);
  ctkTransferFunctionGradientItem* gradient = 
    new ctkTransferFunctionGradientItem(transferFunction.data());
  ctkTransferFunctionControlPointsItem* controlPoints = 
    new ctkTransferFunctionControlPointsItem(transferFunction.data());
  
  transferFunctionView.scene()->addItem(gradient);
  transferFunctionView.scene()->addItem(controlPoints);
  // the widget is not really shown here, only when app.exec() is called
  transferFunctionView.show();

  QTimer autoExit;
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QObject::connect(&autoExit, SIGNAL(timeout()), &app, SLOT(quit()));
    autoExit.start(1000);
    }
  return app.exec();
}
