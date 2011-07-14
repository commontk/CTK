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
#include "ctkTransferFunctionBarsItem.h"
#include "ctkTransferFunctionScene.h"
#include "ctkTransferFunctionView.h"
#include "ctkVTKHistogram.h"

// VTK includes
#include <vtkIntArray.h>
#include <vtkSmartPointer.h>

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
int ctkTransferFunctionBarsItemTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);
  
  vtkSmartPointer<vtkIntArray> intArray = 
    vtkSmartPointer<vtkIntArray>::New();
  intArray->SetNumberOfComponents(1);
  intArray->SetNumberOfTuples(20000);
  for (int i = 0; i < 20000; ++i)
    {
    intArray->SetValue(i, rand() % 10);
    }
  QSharedPointer<ctkVTKHistogram> histogram = 
    QSharedPointer<ctkVTKHistogram>(new ctkVTKHistogram(intArray));
  histogram->build();
  
  ctkTransferFunctionView transferFunctionView;
  
  ctkTransferFunctionBarsItem * histogramItem = new ctkTransferFunctionBarsItem;
  histogramItem->setTransferFunction(histogram.data());
  histogramItem->setBarWidth(1.);
  transferFunctionView.scene()->addItem(histogramItem);
  
  // the widget is not really shown here, only when app.exec() is called
  transferFunctionView.show();
  
  QTimer autoExit;
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QObject::connect(&autoExit, SIGNAL(timeout()), &app, SLOT(quit()));
    autoExit.start(100);
    }

  return app.exec();
}
