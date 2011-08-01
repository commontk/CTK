
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

int ctkVTKHistogramTest4( int argc, char * argv [])
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);

//---------------------------------------------------
// test 4 :
//---------------------------------------------------

  //------Test build--------------------------------
  ctkVTKHistogram histogram;

  vtkSmartPointer<vtkDataArray> dataArray = vtkDataArray::CreateDataArray(VTK_FLOAT);
  dataArray->InsertNextTuple1( 10.001);
  dataArray->InsertNextTuple1( -0.231);
  dataArray->InsertNextTuple1( 220.0001);
  dataArray->InsertNextTuple1(1234.0);
  dataArray->InsertNextTuple1(220.0);
  histogram.setDataArray(dataArray);
  if (histogram.dataArray() != dataArray)
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkVTKHistogram::setDataArray "
              << std::endl;
    return EXIT_FAILURE;
    }

  histogram.setNumberOfBins(256);
  //------Test build---------------------------------
  histogram.build();

  if (histogram.count() != 256)
    {
    std::cerr << "Failed to build histogram" << histogram.count()
              << std::endl;
    return EXIT_FAILURE;
    }

  if (histogram.value(-0.231).toInt() != 1)
    {
    std::cerr << "Failed to build histogram" << histogram.value(-0.231).toInt()
              << std::endl;
    return EXIT_FAILURE;
    }

  if (histogram.value(10.001).toInt() != 1)
    {
    std::cerr << "Failed to build histogram" << histogram.value(10.001).toInt()
              << std::endl;
    return EXIT_FAILURE;
    }

  if (histogram.value(220).toInt() != 2)
    {
    std::cerr << "Failed to build histogram" << histogram.value(220).toInt()
              << std::endl;
    return EXIT_FAILURE;
    }

  if (histogram.value(500).toInt() != 0)
    {
    std::cerr << "Failed to build histogram" << histogram.value(500).toInt()
              << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

