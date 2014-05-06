
// Qt includes
#include <QCoreApplication>
#include <QVariant>
#include <QColor>

// CTKVTK includes
#include "ctkVTKColorTransferFunction.h"

// VTK includes
#include <vtkColorTransferFunction.h>
#include <vtkSmartPointer.h>

// STD includes
#include <cstdlib>
#include <iostream>

int ctkVTKColorTransferFunctionTest1( int argc, char * argv [])
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);

//--------------------------------------------
//Test 1 : with default TransfertFunction
//----------------------------------------------------------------------

  ctkVTKColorTransferFunction defaultTF;

  //------Test Function Count-wihtout colorTransferFunction-------------
  if (defaultTF.count() != -1
        || defaultTF.minValue() != -1
        || defaultTF.maxValue() != -1)
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkVTKColorTransferFunction::count"
              <<std::endl;
    return EXIT_FAILURE;
    }

  //------Test Function range-------------------
  qreal defaultMinRange = 0.;
  qreal defaultMaxRange = 1.;
  defaultTF.range(defaultMinRange, defaultMaxRange);

  //------Test Function colorTransferFunction---
  vtkColorTransferFunction* defaultVTKColorTransferFunction = defaultTF.colorTransferFunction();
  if (defaultVTKColorTransferFunction != 0)
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkVTKColorTransferFunction::colorTransfertFunction  "
              //<< defaultVTKColorTransferFunction->GetSize()
              <<std::endl;
    return EXIT_FAILURE;
    }

  //-----Test Function vtkColorTransferFunction::New()---------
  vtkColorTransferFunction*  colorTransferFunction = vtkColorTransferFunction::New();
  if (colorTransferFunction->GetSize() > 0)
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkVTKColorTransferFunction::vtkColorTransferFunction::New() "
              //<< defaultVTKColorTransferFunction->GetSize()
              <<std::endl;
    return EXIT_FAILURE;
    }

  //------Test Function setColorTransfertFunction------
  defaultTF.setColorTransferFunction(colorTransferFunction);

  //------Test Function Value-------------------
  qreal defaultPos = 1.;
  QVariant defaultVariant = defaultTF.value(defaultPos);
  if (defaultTF.value(defaultPos).type() != QVariant::Color)
  {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkVTKColorTransferFunction::value> "
              <<std::endl;
    //return EXIT_FAILURE;
  }

  //------Test Function Count wiht colorTransferFunction-------------
  if (defaultTF.count() < 0)
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkVTKColorTransferFunction::count"
              <<std::endl;
    return EXIT_FAILURE;
    }

  //------Test Function isDiscrete--------------
  if(defaultTF.isDiscrete())
  {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkVTKColorTransferFunction::isDiscrete"
              <<std::endl;
    return EXIT_FAILURE;
  }

  //------Test Function isEnable----------------
  if(!defaultTF.isEditable())
  {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkVTKColorTransferFunction::isEditable"
              <<std::endl;
    return EXIT_FAILURE;
  }

  //------Test Function range-------------------
  defaultMinRange = 0.;
  defaultMaxRange = 1.;

  defaultTF.range(defaultMinRange,defaultMaxRange);

  //------Test Function minValue----------------
  QVariant defaultMinValue;
  defaultMinValue = defaultTF.minValue();
  if (defaultMinValue == -1)
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkVTKColorTransferFunction::minValue"
              <<std::endl;
    return EXIT_FAILURE;
    }

  //-----Test Function maxValue----------------
  QVariant defaultMaxValue;
  defaultMaxValue = defaultTF.maxValue();
  if (defaultMaxValue == -1)
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkVTKColorTransferFunction::minValue"
              <<std::endl;
    return EXIT_FAILURE;
    }
  //-----Test Function insertControlPoint with qreal------
  defaultPos = 1.;
  defaultTF.insertControlPoint(defaultPos);
  if (defaultTF.count() != 1)
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkVTKColorTransferFunction"
              << "::insertControlPoint(qreal) "
              << defaultTF.count()
              <<std::endl;
    return EXIT_FAILURE;
    }

  //------Test Function removeControlpoint--------
  qreal fakeDefaultPos = 2.;
  defaultTF.removeControlPoint(fakeDefaultPos);
  if (defaultTF.count() != 1)
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkVTKColorTransferFunction"
              << "::insertControlPoint(qreal) "
              << defaultTF.count()
              <<std::endl;
    return EXIT_FAILURE;
    }
  defaultTF.removeControlPoint(defaultPos);
  if (defaultTF.count() != 0)
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkVTKColorTransferFunction"
              << "::insertControlPoint(qreal) "
              << defaultTF.count()
              <<std::endl;
    return EXIT_FAILURE;
    }

  //------Test Function insertControlPoint with ctkControlPoint-------
  ctkControlPoint cp;
  int index1 = defaultTF.insertControlPoint(cp);
  if (defaultTF.count() != 1)
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkVTKColorTransferFunction"
              << "::insertControlPoint(qreal) "
              << defaultTF.count()
              <<std::endl;
    return EXIT_FAILURE;
    }

  //------Test Function setControlPointPos---------
  qreal newPos = 5.;
  defaultTF.setControlPointPos(index1,newPos);

  //------Test Function setControlPointValue-------
  QVariant newValue = QColor::fromRgbF(0., 1., 1.);
  defaultTF.setControlPointValue(index1,newValue);

  //------Test Function minValue----------------
  defaultMinValue = defaultTF.minValue();

  //-----Test Function maxValue----------------
  defaultMaxValue = defaultTF.maxValue();

  //-----Test ControlPoint---------------------
  qreal firstPos = 10.;
  qreal secondPos = 20.;
  ctkControlPoint* defaultControlPoint;
  int firstIndex = defaultTF.insertControlPoint(firstPos);
  int secondIndex = defaultTF.insertControlPoint(secondPos);
  std::cout << "Index :" << firstIndex << "  " << secondIndex << std::endl;
  defaultControlPoint = defaultTF.controlPoint(0);
  defaultControlPoint = defaultTF.controlPoint(firstIndex);
  defaultControlPoint = defaultTF.controlPoint(secondIndex);
  Q_UNUSED(defaultControlPoint)

  colorTransferFunction->Delete();
  return EXIT_SUCCESS;
}
