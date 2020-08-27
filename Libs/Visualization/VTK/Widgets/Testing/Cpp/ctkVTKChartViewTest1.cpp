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
#include "ctkVTKChartView.h"

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkPlotBar.h>
#include <vtkTable.h>
#include <vtkVersion.h>
#if CTK_USE_QVTKOPENGLWIDGET
#include <QVTKOpenGLWidget.h>
#endif

// STD includes
#include <iostream>


// Monthly circulation data
static int data_2008[] = {10822, 10941, 9979, 10370, 9460, 11228, 15093, 12231, 10160, 9816, 9384, 7892};
static int data_2009[] = {9058, 9474, 9979, 9408, 8900, 11569, 14688, 12231, 10294, 9585, 8957, 8590};
static int data_2010[] = {9058, 10941, 9979, 10270, 8900, 11228, 14688, 12231, 10160, 9585, 9384, 8590};

//-----------------------------------------------------------------------------
int ctkVTKChartViewTest1(int argc, char * argv [] )
{
#if CTK_USE_QVTKOPENGLWIDGET
    QSurfaceFormat format = QVTKOpenGLWidget::defaultFormat();
    format.setSamples(0);
    QSurfaceFormat::setDefaultFormat(format);
#endif

  QApplication app(argc, argv);

  ctkVTKChartView view(0);

  // Create a table with some points in it...
  vtkNew<vtkTable> table;

  vtkNew<vtkIntArray> arrMonth;
  arrMonth->SetName("Month");
  table->AddColumn(arrMonth.GetPointer());

  vtkNew<vtkIntArray> arr2008;
  arr2008->SetName("2008");
  table->AddColumn(arr2008.GetPointer());

  vtkNew<vtkIntArray> arr2009;
  arr2009->SetName("2009");
  table->AddColumn(arr2009.GetPointer());

  vtkNew<vtkIntArray> arr2010;
  arr2010->SetName("2010");
  table->AddColumn(arr2010.GetPointer());

  table->SetNumberOfRows(12);
  for (int i = 0; i < 12; i++)
    {
    table->SetValue(i,0,i+1);
    table->SetValue(i,1,data_2008[i]);
    table->SetValue(i,2,data_2009[i]);
    table->SetValue(i,3,data_2010[i]);
    }

  // Add multiple line plots, setting the colors etc
  vtkPlotBar* bar = vtkPlotBar::New();
  bar->SetInputData(table.GetPointer(), 0, 1);
  bar->SetColor(0, 255, 0, 255);
  view.addPlot(bar);
  bar->Delete();

  bar = vtkPlotBar::New();
  bar->SetInputData(table.GetPointer(), 0, 2);
  bar->SetColor(255, 0, 0, 255);
  view.addPlot(bar);
  bar->Delete();

  bar = vtkPlotBar::New();
  bar->SetInputData(table.GetPointer(), 0, 3);
  bar->SetColor(0, 0, 255, 255);
  view.addPlot(bar);
  bar->Delete();

  view.show();

  QTimer autoExit;
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QObject::connect(&autoExit, SIGNAL(timeout()), &app, SLOT(quit()));
    autoExit.start(1000);
    }
  return app.exec();
}
