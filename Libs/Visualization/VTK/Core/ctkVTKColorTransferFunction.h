/*=========================================================================

  Library:   ctk

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __ctkVTKColorTransferFunction_h
#define __ctkVTKColorTransferFunction_h

// CTK includes
#include "ctkTransferFunction.h"
#include "ctkPimpl.h"
#include "CTKVisualizationVTKCoreExport.h"
#include "ctkVTKObject.h"

class vtkColorTransferFunction;
class ctkVTKColorTransferFunctionPrivate;

///
/// Transfer function for a vtkColorTransferFunction. 
/// The value is an RGB QColor (no alpha supported)
class CTK_VISUALIZATION_VTK_CORE_EXPORT ctkVTKColorTransferFunction: public ctkTransferFunction
{
  Q_OBJECT;
  QVTK_OBJECT;
public:
  ctkVTKColorTransferFunction(vtkColorTransferFunction* colorTransferFunction, 
                              QObject* parent = 0);
  virtual ~ctkVTKColorTransferFunction();
  
  virtual ctkControlPoint* controlPoint(int index)const;
  virtual QVariant value(qreal pos)const;
  virtual int count()const;

  virtual void range(qreal& minRange, qreal& maxRange)const;
  virtual QVariant minValue()const;
  virtual QVariant maxValue()const;

  virtual int insertControlPoint(const ctkControlPoint& cp);
  virtual void setControlPointPos(int index, qreal pos);
  virtual void setControlPointValue(int index, const QVariant& value);
  
  void setColorTransferFunction(vtkColorTransferFunction* colorTransferFunction);
  vtkColorTransferFunction* colorTransferFunction()const;
private:
  CTK_DECLARE_PRIVATE(ctkVTKColorTransferFunction);
};

#endif
