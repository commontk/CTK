/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

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
