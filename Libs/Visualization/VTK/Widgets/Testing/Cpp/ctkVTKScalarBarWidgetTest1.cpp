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
#include <QTimer>

// CTK includes
#include "ctkVTKScalarBarWidget.h"

// VTK includes
#include <vtkScalarBarActor.h>
#include <vtkScalarBarWidget.h>
#include <vtkSmartPointer.h>

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
int ctkVTKScalarBarWidgetTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  vtkSmartPointer<vtkScalarBarWidget> scalarBar =
    vtkSmartPointer<vtkScalarBarWidget>::New();
  vtkScalarBarActor* actor = scalarBar->GetScalarBarActor();

  ctkVTKScalarBarWidget widget;
  widget.setScalarBarWidget(scalarBar);
  widget.setScalarBarWidget(0);
  widget.setScalarBarWidget(scalarBar);

  // display
  widget.setDisplay(false);
  if (scalarBar->GetEnabled() != false)
    {
    std::cerr << "ctkVTKScalarBarWidget::setDisplay() failed"
              << std::endl;
    return EXIT_FAILURE;
    }
  // it will fail as there is no interactor set to the widget
  scalarBar->SetEnabled(true);
  if (widget.display() != static_cast<bool>(scalarBar->GetEnabled()))
    {
    std::cerr << "ctkVTKScalarBarWidget::setDisplay() failed"
              << std::endl;
    return EXIT_FAILURE;
    }

  // max number of colors
  widget.setMaxNumberOfColors(20);
  if (actor->GetMaximumNumberOfColors() != 20)
    {
    std::cerr << "ctkVTKScalarBarWidget::setMaxNumberOfColors() failed"
              << std::endl;
    return EXIT_FAILURE;
    }

  actor->SetMaximumNumberOfColors(30);
  if (widget.maxNumberOfColors() != 30)
    {
    std::cerr << "ctkVTKScalarBarWidget::setMaxNumberOfColors() failed"
              << std::endl;
    return EXIT_FAILURE;
    }
  
  // number of labels
  widget.setNumberOfLabels(5);
  if (actor->GetNumberOfLabels() != 5)
    {
    std::cerr << "ctkVTKScalarBarWidget::setNumberOfLabels() failed"
              << std::endl;
    return EXIT_FAILURE;
    }

  actor->SetNumberOfLabels(10);
  if (widget.numberOfLabels() != 10)
    {
    std::cerr << "ctkVTKScalarBarWidget::setNumberOfLabels() failed"
              << std::endl;
    return EXIT_FAILURE;
    }

  // title
  widget.setTitle("title");
  if (QString(actor->GetTitle()) != "title")
    {
    std::cerr << "ctkVTKScalarBarWidget::setTitle() failed"
              << std::endl;
    return EXIT_FAILURE;
    }

  actor->SetTitle("title 2");
  if (widget.title() != "title 2")
    {
    std::cerr << "ctkVTKScalarBarWidget::setTitle() failed"
              << std::endl;
    return EXIT_FAILURE;
    }
    
  // labels format
  widget.setLabelsFormat("@#$%^&*");
  if (QString(actor->GetLabelFormat()) != "@#$%^&*")
    {
    std::cerr << "ctkVTKScalarBarWidget::setLabelsFormat() failed"
              << std::endl;
    return EXIT_FAILURE;
    }

  actor->SetLabelFormat("@#$%^&*@#$%^&*");
  if (widget.labelsFormat() != "@#$%^&*@#$%^&*")
    {
    std::cerr << "ctkVTKScalarBarWidget::setLabelsFormat() failed"
              << std::endl;
    return EXIT_FAILURE;
    }

  widget.show();
  
  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }
  return app.exec();
}
