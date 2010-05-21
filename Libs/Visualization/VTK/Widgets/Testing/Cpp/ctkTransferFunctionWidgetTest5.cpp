/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

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
#include "ctkTransferFunctionWidget.h"
#include "ctkVTKHistogram.h"

// VTK includes
#include <vtkIntArray.h>
#include <vtkSmartPointer.h>

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
int ctkTransferFunctionWidgetTest5(int argc, char * argv [] )
{
  QApplication app(argc, argv);
  
  vtkSmartPointer<vtkIntArray> intArray = 
    vtkSmartPointer<vtkIntArray>::New();
  intArray->SetNumberOfComponents(1);
  intArray->SetNumberOfTuples(2000);
  for (int i = 0; i < 2000; ++i)
    {
    intArray->SetValue(i, qrand() % 30);
    }
  QSharedPointer<ctkVTKHistogram> histogram = 
    QSharedPointer<ctkVTKHistogram>(new ctkVTKHistogram(intArray));
  histogram->build();
  ctkTransferFunctionWidget transferFunctionWidget(histogram.data(), 0);
  // the widget is not really shown here, only when app.exec() is called
  transferFunctionWidget.show();

  QTimer autoExit;
  QObject::connect(&autoExit, SIGNAL(timeout()), &app, SLOT(quit()));
  autoExit.start(1000);

  return app.exec();
}
