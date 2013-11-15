
// Qt includes
#include <QCoreApplication>
#include <QDebug>

// CTK includes
#include "ctkTransferFunctionRepresentation.h"

// CTKVTK includes
#include "ctkVTKColorTransferFunction.h"
#include "ctkVTKLookupTable.h"

// VTK includes
#include <vtkColorTransferFunction.h>
#include <vtkLookupTable.h>
#include <vtkSmartPointer.h>

// STD includes
#include <cstdlib>
#include <iostream>

int ctkVTKTransferFunctionRepresentationTest1( int argc, char * argv [])
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);

//--------------------------------------------
//Test 1 : with default TransfertFunction
//----------------------------------------------------------------------

  /// Function not discrete
  ctkVTKColorTransferFunction defaultCTF;

  vtkColorTransferFunction*  colorTransferFunction = vtkColorTransferFunction::New();
  defaultCTF.setColorTransferFunction(colorTransferFunction);

  ctkTransferFunctionRepresentation representation;
  representation.setTransferFunction(&defaultCTF);

  qreal defaultMinRange = 0.;
  qreal defaultMaxRange = 1.;
  defaultCTF.range(defaultMinRange, defaultMaxRange);

  qreal firstPos = 10.;
  qreal secondPos = 20.;
  ctkControlPoint* defaultControlPoint;
  int firstIndex = defaultCTF.insertControlPoint(firstPos);
  int secondIndex = defaultCTF.insertControlPoint(secondPos);
  std::cout << "Index :" << firstIndex << "  " << secondIndex << std::endl;
  defaultControlPoint = defaultCTF.controlPoint(0);
  defaultControlPoint = defaultCTF.controlPoint(firstIndex);
  defaultControlPoint = defaultCTF.controlPoint(secondIndex);
  Q_UNUSED(defaultControlPoint)

  representation.computeCurve();
  representation.computeGradient();

  colorTransferFunction->Delete();

  /// Function discrete
  ctkVTKLookupTable defaultLuT;
  vtkLookupTable* lookupTable = vtkLookupTable::New();
  defaultLuT.setLookupTable(lookupTable);

  representation.setTransferFunction(&defaultLuT);

  representation.computeCurve();
  representation.computeGradient();

  lookupTable->Delete();
  return EXIT_SUCCESS;
}
