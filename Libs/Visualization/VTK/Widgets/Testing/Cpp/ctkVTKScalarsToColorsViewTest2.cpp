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
#include "ctkVTKScalarsToColorsView.h"

// VTK includes
#include <vtkChartXY.h>
#include <vtkColorTransferFunction.h>
#include <vtkImageAccumulate.h>
#include <vtkImageData.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkPen.h>
#include <vtkPlotBar.h>
#include <vtkSmartPointer.h>
#include <vtkTable.h>
#include <vtkVersion.h>
#if CTK_USE_QVTKOPENGLWIDGET
#include <QVTKOpenGLWidget.h>
#endif

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
int ctkVTKScalarsToColorsViewTest2(int argc, char * argv [] )
{
#if CTK_USE_QVTKOPENGLWIDGET
    QSurfaceFormat format = QVTKOpenGLWidget::defaultFormat();
    format.setSamples(0);
    QSurfaceFormat::setDefaultFormat(format);
#endif

  QApplication app(argc, argv);

  // Transfer Function
  vtkSmartPointer<vtkColorTransferFunction> ctf =
    vtkSmartPointer<vtkColorTransferFunction>::New();
  //
  ctf->AddRGBPoint(0., 0.6941,0.76,0., 0.5, 0.);
  ctf->AddRGBPoint(85., 0.,0.6941,0.96);
  ctf->AddRGBPoint(170., 0.9686,0.0,0.6941);
  ctf->AddRGBPoint(255., 0.9686,0.76,0.);

  // Image
  vtkSmartPointer<vtkImageData> image =
    vtkSmartPointer<vtkImageData>::New();
  image->SetDimensions(256, 256, 1);
  image->SetSpacing(20000., 30., 900000000.);
  image->SetOrigin(-251234568., 0.00001, 40.2);
  image->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
  unsigned char * ptr = static_cast<unsigned char *>(image->GetScalarPointer());
  for (int i=0; i<256*256*1; ++i)
    {
    *(ptr++)= rand() % 256;
    }

  // Histogram
  vtkSmartPointer<vtkImageAccumulate> histogram =
    vtkSmartPointer<vtkImageAccumulate>::New();
  histogram->SetInputData(image);
  histogram->SetComponentExtent(0, 255, 0 , 0 , 0, 0);
  histogram->Update();
  vtkAbstractArray* bins =
    histogram->GetOutput()->GetPointData()->GetScalars();

  // Histogram table
  vtkSmartPointer<vtkTable> table =
    vtkSmartPointer<vtkTable>::New();
  vtkSmartPointer<vtkIntArray> intArray =
    vtkSmartPointer<vtkIntArray>::New();
  intArray->SetNumberOfComponents(1);
  intArray->SetNumberOfTuples(256);
  intArray->SetName("intensities");
  for (int i = 0; i < 256; ++i)
    {
    intArray->SetValue(i, i);
    }
  bins->SetName("Histogram");
  table->AddColumn(intArray);
  table->AddColumn(histogram->GetOutput()->GetPointData()->GetScalars());

  //Histogram Plot
  vtkSmartPointer<vtkPlotBar> histogramPlot =
    vtkSmartPointer<vtkPlotBar>::New();
  histogramPlot->SetInputData(table, 0, 1);
  histogramPlot->SetColor(0.078, 0.008, 0);//not exactly black...just because.
  histogramPlot->GetPen()->SetLineType(vtkPen::NO_PEN);

  ctkVTKScalarsToColorsView view(0);
  // add transfer function item
  vtkPlot* ctfPlot = view.addColorTransferFunction(ctf);
  view.chart()->SetPlotCorner(ctfPlot, 1);
  ctfPlot->SetOpacity(0.7);
  // add histogram item
  view.addPlot(histogramPlot);
  view.chart()->SetBarWidthFraction(1.);
  view.setAxesToChartBounds();
  view.show();

  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }
  return app.exec();
}
