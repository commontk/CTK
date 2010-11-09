/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

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
/*==============================================================================

  Program: 3D Slicer

  Copyright (c) 2010 Kitware Inc.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// QT includes
#include <QApplication>
#include <QTimer>

// qMRML includes
#include "ctkVTKThumbnailView.h"

// VTK includes
#include <vtkSmartPointer.h>

#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

// STD includes
#include <cstdlib>
#include <iostream>

int ctkVTKThumbnailViewTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);
  
  ctkVTKThumbnailView thumbnailView;
  thumbnailView.setWindowTitle("Thumbnail view");
  
  ctkVTKRenderView renderView;
  renderView.setWindowTitle("Render view");

  vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
  vtkCubeSource *cube= vtkCubeSource::New();
  mapper->SetInput(cube->GetOutput());
  cube->Delete();
  vtkActor *actor = vtkActor::New();
  actor->SetMapper(mapper);
  mapper->Delete();

  renderView.renderer()->AddActor(actor);
  actor->Delete();

  thumbnailView.setRendererToListen(renderView.renderer());

  thumbnailView.show();
  renderView.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}
