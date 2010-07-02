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
#include <QTimer>
#include <QDebug>

// CTK includes
#include "ctkCommandLineParser.h"
#include "ctkVTKSliceView.h"

// VTK includes
#include <vtkImageReader2Factory.h>
#include <vtkImageReader2.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkInteractorStyleImage.h>
#include <vtkRenderWindowInteractor.h>

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
int ctkVTKSliceViewTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  // Test arguments
  bool interactive = false;
  QString data_directory;
  QString filename = "HeadMRVolume.mhd";

  // Command line parser
  ctkCommandLineParser parser;
  parser.addBooleanArgument(0, "-I", &interactive);
  parser.addStringArgument(0, "-D", &data_directory);
  if (!parser.parseArguments(app.arguments()))
    {
    std::cerr << qPrintable(parser.errorString()) << std::endl;
    return EXIT_FAILURE;
    }

  QString imageFilename = data_directory + "/" + filename;

  // Instanciate the reader factory
  vtkSmartPointer<vtkImageReader2Factory> imageFactory =
      vtkSmartPointer<vtkImageReader2Factory>::New();

  // Instanciate an image reader
  vtkSmartPointer<vtkImageReader2> imageReader;
  imageReader.TakeReference(imageFactory->CreateImageReader2(imageFilename.toLatin1()));
  if (!imageReader)
    {
    std::cerr << "Failed to instanciate image reader using: " 
              << qPrintable(imageFilename) << std::endl;
    return EXIT_FAILURE;
    }

  // Read image
  imageReader->SetFileName(imageFilename.toLatin1());
  imageReader->Update();
  vtkSmartPointer<vtkImageData> image = imageReader->GetOutput();

  ctkVTKSliceView sliceView;
  sliceView.resize(300, 300);
  sliceView.setImageData(image);
  sliceView.setRenderWindowLayout(4, 4);
  sliceView.setHighlighted(0, 0, true);
  sliceView.setCornerAnnotationText("CTK");
  sliceView.scheduleRender();
  sliceView.show();

  if (!interactive)
    {
    QTimer::singleShot(1000, &app, SLOT(quit()));
    }
  return app.exec();
}
