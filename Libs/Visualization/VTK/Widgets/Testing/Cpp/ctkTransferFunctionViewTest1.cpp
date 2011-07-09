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
#include "ctkVTKColorTransferFunction.h"

// VTK includes
#include <vtkColorTransferFunction.h>
#include <vtkSmartPointer.h>

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
int ctkTransferFunctionViewTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);
  
  vtkSmartPointer<vtkColorTransferFunction> ctf = 
    vtkSmartPointer<vtkColorTransferFunction>::New();
  //
  ctf->AddRGBPoint(0.2, 1.,0.,0., 0.5, 0.);
  //ctf->AddRGBPoint(0.5, 0.,0.,1.);
  ctf->AddRGBPoint(0.8, 0.,1.,0.);
  //ctf->AddHSVPoint(0., 0.,1.,1.);
  //ctf->AddHSVPoint(1., 0.66666,1.,1.);

  QSharedPointer<ctkTransferFunction> transferFunction = 
    QSharedPointer<ctkTransferFunction>(new ctkVTKColorTransferFunction(ctf));
  ctkTransferFunctionView transferFunctionView(0);
  ctkTransferFunctionGradientItem* gradient = 
    new ctkTransferFunctionGradientItem(transferFunction.data());
  ctkTransferFunctionControlPointsItem* controlPoints = 
    new ctkTransferFunctionControlPointsItem(transferFunction.data());
  
  transferFunctionView.scene()->addItem(gradient);
  transferFunctionView.scene()->addItem(controlPoints);
  // the widget is not really shown here, only when app.exec() is called
  transferFunctionView.show();

  //ctf->AddRGBPoint(0.7, 0.0,0.0,0.0);
  //ctkTransferFunctionView* toto = new ctkTransferFunctionView();
  QTimer autoExit;
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QObject::connect(&autoExit, SIGNAL(timeout()), &app, SLOT(quit()));
    autoExit.start(1000);
    }
  return app.exec();
}
