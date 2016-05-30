
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

int ctkVTKHistogramTest2( int argc, char * argv [])
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);

//---------------------------------------------------
// test 2 :
//---------------------------------------------------

  //------Test build--------------------------------
  ctkVTKHistogram rgbHistogram;

  vtkSmartPointer<vtkDataArray> rgbDataArray;
  rgbDataArray.TakeReference(vtkDataArray::CreateDataArray(VTK_INT));
  rgbDataArray->SetNumberOfComponents(3);
  rgbDataArray->InsertNextTuple3(   0,  50,     0);
  rgbDataArray->InsertNextTuple3(1000, 143, -1412);
  rgbDataArray->InsertNextTuple3(-543, 210,   151);
  rgbDataArray->InsertNextTuple3(  -1, 210,    10);

  // Generate histogram on the Green values
  rgbHistogram.setComponent(1);
  rgbHistogram.setDataArray(rgbDataArray);
  if (rgbHistogram.dataArray() != rgbDataArray)
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkVTKHistogram::setDataArray "
              << std::endl;
    return EXIT_FAILURE;
    }

  double range[2], expectedRange[2] = {50.0, 211.0};
  rgbHistogram.range(range[0], range[1]);
  if (range[0] != expectedRange[0] && range[1] != expectedRange[1])
    {
    std::cerr << "Bad range: " << range[0] << " " << range[1] << std::endl
              << " expected: " << expectedRange[0] << " " << expectedRange[1]
              << std::endl;
    return EXIT_FAILURE;
    }

  //------Test build---------------------------------
  rgbHistogram.build();

  if (rgbHistogram.count() != (210 - 50 + 1))
    {
    std::cerr << "Failed to build histogram" << rgbHistogram.count()
              << std::endl;
    return EXIT_FAILURE;
    }

  if (rgbHistogram.value(210).toInt() != 2)
    {
    std::cerr << "Failed to build histogram" << rgbHistogram.value(210).toInt()
              << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}

