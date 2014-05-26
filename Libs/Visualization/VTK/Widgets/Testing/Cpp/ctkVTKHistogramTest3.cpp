
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

int ctkVTKHistogramTest3( int argc, char * argv [])
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);

//---------------------------------------------------
// test 3 :
//---------------------------------------------------

  //------Test build--------------------------------
  ctkVTKHistogram histogram;

  vtkSmartPointer<vtkDataArray> dataArray;
  dataArray.TakeReference(vtkDataArray::CreateDataArray(VTK_CHAR));
  dataArray->InsertNextTuple1(0);
  dataArray->InsertNextTuple1(0);
  dataArray->InsertNextTuple1(0);
  dataArray->InsertNextTuple1(0);
  histogram.setDataArray(dataArray);
  if (histogram.dataArray() != dataArray)
    {
    std::cerr << "Line : " << __LINE__
              << " - Problem with ctkVTKHistogram::setDataArray "
              << std::endl;
    return EXIT_FAILURE;
    }

  //------Test build---------------------------------
  histogram.build();

  if (histogram.count() != 256)
    {
    std::cerr << "Failed to build histogram" << histogram.count()
              << std::endl;
    return EXIT_FAILURE;
    }

  if (histogram.value(0).toInt() != 4)
    {
    std::cerr << "Failed to build histogram" << histogram.value(0).toInt()
              << std::endl;
    return EXIT_FAILURE;
    }

  if (histogram.value(1).toInt() != 0)
    {
    std::cerr << "Failed to build histogram" << histogram.value(1).toInt()
              << std::endl;
    return EXIT_FAILURE;
    }

  if (histogram.value(255).toInt() != 0)
    {
    std::cerr << "Failed to build histogram" << histogram.value(255).toInt()
              << std::endl;
    return EXIT_FAILURE;
    }

  if (histogram.value(1024).toInt() != 0)
    {
    std::cerr << "Failed to build histogram" << histogram.value(1024).toInt()
              << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

