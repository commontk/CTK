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
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>

// CTK includes
#include "ctkCommandLineParser.h"
#include "ctkVTKObjectEventsObserver.h"
#include "ctkVTKSliceView.h"

// VTK includes
#include <vtkImageReader2Factory.h>
#include <vtkImageReader2.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkInteractorStyleImage.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkLightBoxRendererManager.h>
#if CTK_USE_QVTKOPENGLWIDGET
#include <QVTKOpenGLWidget.h>
#endif

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
int ctkVTKSliceViewTest2(int argc, char * argv [] )
{
#if CTK_USE_QVTKOPENGLWIDGET
    QSurfaceFormat format = QVTKOpenGLWidget::defaultFormat();
    format.setSamples(0);
    QSurfaceFormat::setDefaultFormat(format);
#endif

  QApplication app(argc, argv);

  // Test arguments
  QString filename = "HeadMRVolume.mhd";

  // Command line parser
  ctkCommandLineParser parser;
  parser.addArgument("", "-I", QVariant::Bool);
  parser.addArgument("", "-D", QVariant::String);
  bool ok = false;
  QHash<QString, QVariant> parsedArgs = parser.parseArguments(app.arguments(), &ok);
  if (!ok)
    {
    std::cerr << qPrintable(parser.errorString()) << std::endl;
    return EXIT_FAILURE;
    }

  bool interactive = parsedArgs["-I"].toBool();
  QString data_directory = parsedArgs["-D"].toString();

  QString imageFilename = data_directory + "/" + filename;

  // Instanciate the reader factory
  vtkSmartPointer<vtkImageReader2Factory> imageFactory =
      vtkSmartPointer<vtkImageReader2Factory>::New();

  // Instanciate an image reader
  vtkSmartPointer<vtkImageReader2> imageReader;
  imageReader.TakeReference(imageFactory->CreateImageReader2(imageFilename.toUtf8()));
  if (!imageReader)
    {
    std::cerr << "Failed to instanciate image reader using: " 
              << qPrintable(imageFilename) << std::endl;
    return EXIT_FAILURE;
    }

  // Read image
  imageReader->SetFileName(imageFilename.toUtf8());
  imageReader->Update(); // XXX This shouldn't be needed. See issue #467
  vtkAlgorithmOutput* imagePort = imageReader->GetOutputPort();

  // Top level widget
  QWidget widget;

  // .. and its associated layout
  QVBoxLayout * topLevelLayout = new QVBoxLayout(&widget);
  topLevelLayout->setContentsMargins(0, 0, 0, 0);

  // Horizontal layout to contain the spinboxes
  QHBoxLayout * spinBoxLayout = new QHBoxLayout;
  topLevelLayout->addLayout(spinBoxLayout);

  int defaultRowCount = 4;
  int defaultColumnCount = 3;

  // SpinBox to change number of row in lightBox
  QSpinBox * rowSpinBox = new QSpinBox;
  rowSpinBox->setRange(1, 10);
  rowSpinBox->setSingleStep(1);
  rowSpinBox->setValue(defaultRowCount);
  spinBoxLayout->addWidget(rowSpinBox);

  // SpinBox to change number of column in lightBox
  QSpinBox * columnSpinBox = new QSpinBox;
  columnSpinBox->setRange(1, 10);
  columnSpinBox->setSingleStep(1);
  columnSpinBox->setValue(defaultColumnCount);
  spinBoxLayout->addWidget(columnSpinBox);

  ctkVTKSliceView * sliceView = new ctkVTKSliceView;
  sliceView->setRenderEnabled(true);
  sliceView->setMinimumSize(600, 600);
  sliceView->setImageDataConnection(imagePort);
  sliceView->setHighlightedBoxColor(QColor(Qt::yellow));
  sliceView->lightBoxRendererManager()->SetRenderWindowLayout(defaultRowCount, defaultColumnCount);
  sliceView->lightBoxRendererManager()->SetHighlighted(0, 0, true);
  sliceView->setCornerAnnotationText("CTK");
  sliceView->scheduleRender();
  topLevelLayout->addWidget(sliceView);

  // Set connection
  QObject::connect(rowSpinBox, SIGNAL(valueChanged(int)),
                   sliceView, SLOT(setLightBoxRendererManagerRowCount(int)));
  QObject::connect(columnSpinBox, SIGNAL(valueChanged(int)),
                   sliceView, SLOT(setLightBoxRendererManagerColumnCount(int)));

  ctkVTKObjectEventsObserver vtkObserver;
  vtkObserver.addConnection(sliceView->lightBoxRendererManager(), vtkCommand::ModifiedEvent,
                            sliceView, SLOT(scheduleRender()));

  widget.show();

  // TODO Add image regression test

  if (!interactive)
    {
    QTimer::singleShot(1000, &app, SLOT(quit()));
    }
  return app.exec();
}
