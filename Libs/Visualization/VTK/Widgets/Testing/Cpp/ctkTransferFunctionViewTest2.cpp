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
#include "ctkVTKLookupTable.h"

// VTK includes
#include <vtkLookupTable.h>
#include <vtkSmartPointer.h>

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
int ctkTransferFunctionViewTest2(int argc, char * argv [] )
{
  QApplication app(argc, argv);
  
  vtkSmartPointer<vtkLookupTable> ctf = 
    vtkSmartPointer<vtkLookupTable>::New();

  ctf->SetNumberOfTableValues(32);
  ctf->SetHueRange(0.,1.);
  ctf->SetSaturationRange(1.,1.);
  ctf->SetValueRange(1.,1.);
  ctf->SetAlphaRange(1.,1.);
  ctf->Build();

  QSharedPointer<ctkTransferFunction> transferFunction = 
    QSharedPointer<ctkTransferFunction>(new ctkVTKLookupTable(ctf));
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
