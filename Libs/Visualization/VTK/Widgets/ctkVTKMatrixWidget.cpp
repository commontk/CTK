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
