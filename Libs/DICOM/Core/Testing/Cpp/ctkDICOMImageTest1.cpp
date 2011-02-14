
// Qt includes
#include <QApplication>
#include <QLabel>


// ctkDICOMCore includes
#include "ctkDICOMImage.h"

// DCMTK includes
#include <dcmimage.h>

// STD includes
#include <iostream>


int ctkDICOMImageTest1( int argc, char * argv [] )
{
  QApplication app(argc, argv);

  if (argc <= 1)
    {
    std::cerr << "Warning, no dicom file given. Test stops" << std::endl;
    std::cerr << "Usage: qctkDICOMImageTest1 <dicom file>" << std::endl;
    return EXIT_FAILURE;
  }

  DicomImage dcmtkImage(argv[1]);
  ctkDICOMImage ctkImage(&dcmtkImage);

  QLabel qtImage;
  QPixmap pixmap = QPixmap::fromImage(ctkImage.frame(0),Qt::AvoidDither);
  if (pixmap.isNull())
    {
    std::cerr << "Failed to convert QImage to QPixmap" ;
    return EXIT_FAILURE;
    }
  qtImage.setPixmap(pixmap);
  qtImage.show();

  if (argc > 2 && QString(argv[2]) == "-I")
    {
    return app.exec();
    }
  return EXIT_SUCCESS;
}
