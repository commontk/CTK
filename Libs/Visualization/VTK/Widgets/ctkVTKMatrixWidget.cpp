/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#include "ctkVTKMatrixWidget.h"

// --------------------------------------------------------------------------
void ctkVTKMatrixWidget::setMatrix(vtkMatrix4x4* _matrix)
{
  this->setMatrixInternal(_matrix);
}

// --------------------------------------------------------------------------
ctkVTKMatrixWidget::ctkVTKMatrixWidget(QWidget* _parent) : Superclass(_parent)
{
}
