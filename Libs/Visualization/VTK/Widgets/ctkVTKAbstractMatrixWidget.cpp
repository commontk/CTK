/*=========================================================================

  Library:   CTK
 
  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 
=========================================================================*/

// Qt includes
#include <QDebug>
#include <QVector>

// CTK includes
#include "ctkVTKAbstractMatrixWidget_p.h"
#include "ctkVTKAbstractMatrixWidget.h"

// VTK includes
#include <vtkMatrix4x4.h>

// --------------------------------------------------------------------------
ctkVTKAbstractMatrixWidgetPrivate::ctkVTKAbstractMatrixWidgetPrivate()
  :QObject(0) // will be reparented in init()
{
}

void ctkVTKAbstractMatrixWidgetPrivate::init()
{
  CTK_P(ctkVTKAbstractMatrixWidget);
  this->setParent(p);
  this->updateMatrix();
}

// --------------------------------------------------------------------------
void ctkVTKAbstractMatrixWidgetPrivate::setMatrix(vtkMatrix4x4* matrixVariable)
{
  qvtkReconnect(this->Matrix.GetPointer(), matrixVariable, 
                vtkCommand::ModifiedEvent, this, SLOT(updateMatrix()));

  this->Matrix = matrixVariable;
  this->updateMatrix();
}

// --------------------------------------------------------------------------
vtkMatrix4x4* ctkVTKAbstractMatrixWidgetPrivate::matrix() const
{
  return this->Matrix;
}

// --------------------------------------------------------------------------
void ctkVTKAbstractMatrixWidgetPrivate::updateMatrix()
{
  CTK_P(ctkVTKAbstractMatrixWidget);
  // if there is no transform to show/edit, disable the widget
  p->setEnabled(this->Matrix != 0);

  if (this->Matrix == 0)
    {
    p->reset();
    return;
    }
  QVector<double> vector;
  //todo: fasten the loop
  for (int i=0; i < 4; i++)
    {
    for (int j=0; j < 4; j++)
      {
      vector.append(this->Matrix->GetElement(i,j)); 
      }
    }
  p->setVector( vector );
}

// --------------------------------------------------------------------------
ctkVTKAbstractMatrixWidget::ctkVTKAbstractMatrixWidget(QWidget* parentVariable) : Superclass(parentVariable)
{
  CTK_INIT_PRIVATE(ctkVTKAbstractMatrixWidget);
  ctk_d()->init();
}

// --------------------------------------------------------------------------
vtkMatrix4x4* ctkVTKAbstractMatrixWidget::matrix()const
{
  CTK_D(const ctkVTKAbstractMatrixWidget);
  return d->matrix();
}

// --------------------------------------------------------------------------
void ctkVTKAbstractMatrixWidget::setMatrixInternal(vtkMatrix4x4* matrixVariable)
{
  CTK_D(ctkVTKAbstractMatrixWidget);
  d->setMatrix(matrixVariable);
}
