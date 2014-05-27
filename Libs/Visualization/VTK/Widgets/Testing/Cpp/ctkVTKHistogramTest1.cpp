
// Qt includes
#include <QCoreApplication>

// CTKVTK includes
#include "ctkVTKHistogram.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkDataArray.h>

// STD includes
#include <cstdlib>
#include <iostream>

int ctkVTKHistogramTest1( int argc, char * argv [])
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);

//---------------------------------------------------
// test 1 : With default Histogram
//---------------------------------------------------

  ctkVTKHistogram defaultHistogram;

  //------Test constructor---------------------------

  if (defaultHistogram.count() != 0
      || defaultHistogram.minValue() != 0
      || defaultHistogram.maxValue() != 0
      || defaultHistogram.component() != 0)
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkVTKHistogram::ctkVTKHistogram "
              << defaultHistogram.count()
              << std::endl;
    return EXIT_FAILURE;
    }

  //-----Test setComponent---------------------------
  int newComponent = 1;
  defaultHistogram.setComponent(newComponent);
  if (defaultHistogram.component() != newComponent)
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkVTKHistogram::setComponent "
              << defaultHistogram.component()
              << std::endl;
    return EXIT_FAILURE;
    }
  // All the following data array have only 1 component.
  defaultHistogram.setComponent(0);
  //------Test build--------------------------------
  defaultHistogram.build();

  //-----Test Range----------------------------------
  qreal minRange = 1;
  qreal maxRange = 2;
  defaultHistogram.range(minRange,maxRange);

  //------Test dataArray-----------------------------
  vtkSmartPointer<vtkDataArray> defaultDataArray;
  defaultDataArray = defaultHistogram.dataArray();

  //------Test setDataArray--------------------------
  int dataType = VTK_CHAR;
  vtkSmartPointer<vtkDataArray> newDataArray;
  newDataArray.TakeReference(vtkDataArray::CreateDataArray(dataType));
  defaultHistogram.setDataArray(newDataArray);
  if (defaultHistogram.dataArray() != newDataArray)
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkVTKHistogram::setDataArray "
              << std::endl;
    return EXIT_FAILURE;
    }

  //------Test build---------------------------------
  defaultHistogram.build();

  dataType = VTK_INT;
  newDataArray.TakeReference(vtkDataArray::CreateDataArray(dataType));
  newDataArray->SetNumberOfComponents(1);
  newDataArray->InsertNextTuple1(50);
  newDataArray->InsertNextTuple1(143);
  newDataArray->InsertNextTuple1(210);
  newDataArray->InsertNextTuple1(210);
  defaultHistogram.setDataArray(newDataArray);
  if (defaultHistogram.dataArray() != newDataArray)
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkVTKHistogram::setDataArray "
              << std::endl;
    return EXIT_FAILURE;
    }

  //------Test build---------------------------------
  defaultHistogram.build();

  dataType = VTK_FLOAT;
  newDataArray.TakeReference(vtkDataArray::CreateDataArray(dataType));
  defaultHistogram.setDataArray(newDataArray);
  if (defaultHistogram.dataArray() != newDataArray)
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkVTKHistogram::setDataArray "
              << std::endl;
    return EXIT_FAILURE;
    }

  //------Test setNumberOfBins-----------------------
  defaultHistogram.setNumberOfBins(5);

  //-----Test Range----------------------------------
  defaultHistogram.range(minRange,maxRange);

  //------Test build---------------------------------
  defaultHistogram.build();

  //------Test value---------------------------------
  qreal defaultPos = 0.;
  defaultHistogram.value(defaultPos);

  //------Test ControlPoint--------------------------
  defaultHistogram.controlPoint(0);

  //------Test removeControlPoint--------------------
  /// Function NOT implemented
  defaultHistogram.removeControlPoint(0);

  return EXIT_SUCCESS;
}

