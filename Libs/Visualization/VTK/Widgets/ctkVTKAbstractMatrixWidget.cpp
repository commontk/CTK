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
#include <QDebug>
#include <QVector>

// CTK includes
#include "ctkVTKAbstractMatrixWidget_p.h"
#include "ctkVTKAbstractMatrixWidget.h"

// VTK includes
#include <vtkMatrix4x4.h>

// --------------------------------------------------------------------------
ctkVTKAbstractMatrixWidgetPrivate
::ctkVTKAbstractMatrixWidgetPrivate(ctkVTKAbstractMatrixWidget& object)
  : QObject(0) // will be reparented in init()
  , q_ptr(&object)
{
}

// --------------------------------------------------------------------------
ctkVTKAbstractMatrixWidget::~ctkVTKAbstractMatrixWidget()
{
}

// --------------------------------------------------------------------------
void ctkVTKAbstractMatrixWidgetPrivate::init()
{
  Q_Q(ctkVTKAbstractMatrixWidget);
  this->setParent(q);
  connect(q, SIGNAL(matrixChanged()), this, SLOT(updateVTKMatrix()));
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
  Q_Q(ctkVTKAbstractMatrixWidget);
  // if there is no transform to show/edit, disable the widget
  q->setEnabled(this->Matrix.GetPointer() != 0);

  if (this->Matrix.GetPointer() == 0)
    {
    q->identity();
    return;
    }
  QVector<double> vector;
  for (int i=0; i < 4; i++)
    {
    for (int j=0; j < 4; j++)
      {
      vector.append(this->Matrix->GetElement(i,j));
      }
    }
  q->setValues( vector );
}

// --------------------------------------------------------------------------
void ctkVTKAbstractMatrixWidgetPrivate::updateVTKMatrix()
{
  Q_Q(ctkVTKAbstractMatrixWidget);
  if (this->Matrix.GetPointer() == 0)
    {
    return;
    }
  double elements[16];
  int n = 0;
  for (int i=0; i < 4; i++)
    {
    for (int j=0; j < 4; j++)
      {
      elements[n++] = q->value(i,j);
      }
    }
  bool blocked = this->qvtkBlockAll(true);
  this->Matrix->DeepCopy(elements);
  this->qvtkBlockAll(blocked);
}

// --------------------------------------------------------------------------
ctkVTKAbstractMatrixWidget::ctkVTKAbstractMatrixWidget(QWidget* parentVariable)
  : Superclass(4, 4, parentVariable)
  , d_ptr(new ctkVTKAbstractMatrixWidgetPrivate(*this))
{
  Q_D(ctkVTKAbstractMatrixWidget);
  d->init();
}

// --------------------------------------------------------------------------
vtkMatrix4x4* ctkVTKAbstractMatrixWidget::matrix()const
{
  Q_D(const ctkVTKAbstractMatrixWidget);
  return d->matrix();
}

// --------------------------------------------------------------------------
void ctkVTKAbstractMatrixWidget::setMatrixInternal(vtkMatrix4x4* matrixVariable)
{
  Q_D(ctkVTKAbstractMatrixWidget);
  d->setMatrix(matrixVariable);
}

// --------------------------------------------------------------------------
void ctkVTKAbstractMatrixWidget::setColumnCount(int newColumnCount)
{
  Q_UNUSED(newColumnCount);
  this->Superclass::setColumnCount(4);
}

// --------------------------------------------------------------------------
void ctkVTKAbstractMatrixWidget::setRowCount(int newRowCount)
{
  Q_UNUSED(newRowCount);
  this->Superclass::setRowCount(4);
}
