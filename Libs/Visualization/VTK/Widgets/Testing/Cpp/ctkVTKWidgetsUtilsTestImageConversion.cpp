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

// QT includes
#include <QApplication>
#include <QDialog>
#include <QFrame>
#include <QIcon>
#include <QImage>
#include <QLabel>
#include <QPixmap>
#include <QStyle>
#include <QTimer>
#include <QVBoxLayout>

// CTK includes
#include "ctkVTKOpenGLNativeWidget.h"
#include "ctkVTKRenderView.h"
#include "ctkVTKWidgetsUtils.h"
#include "ctkWidgetsUtils.h"

// VTK includes
#include "vtkImageData.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkVTKWidgetsUtilsTestImageConversion(int argc, char * argv [] )
{
#if CTK_USE_QVTKOPENGLWIDGET
    QSurfaceFormat format = ctkVTKOpenGLNativeWidget::defaultFormat();
    format.setSamples(0);
    QSurfaceFormat::setDefaultFormat(format);
#endif

  QApplication app(argc, argv);

  QFrame parentWidget;
  parentWidget.setFrameStyle(QFrame::Panel | QFrame::Raised);
  parentWidget.setLineWidth(2);

  ctkVTKRenderView vtkWidget(&parentWidget);
  QVBoxLayout* layout = new QVBoxLayout(&parentWidget);
  layout->addWidget(&vtkWidget);
  parentWidget.setLayout(layout);

  QIcon someQIcon = parentWidget.style()->standardIcon(QStyle::SP_DirIcon);
  QImage someQImage = someQIcon.pixmap(32,32).toImage();

  vtkNew<vtkImageData> someVtkImage;

  // Test qImageToVTKImageData
  //////////////////////////////////////

  // force alpha channel (default) - RGBA input
  ctk::qImageToVTKImageData(someQImage, someVtkImage);
  if (someVtkImage->GetDimensions()[0] != someQImage.size().width()
    || someVtkImage->GetDimensions()[1] != someQImage.size().height())
  {
    std::cout << "qImageToVTKImageData size mismatch" << std::endl;
    return EXIT_FAILURE;
  }
  if (someVtkImage->GetNumberOfScalarComponents()!=4)
  {
    std::cout << "qImageToVTKImageData expected 4 scalar components for RGBA input + alpha forced, got: "
      << someVtkImage->GetNumberOfScalarComponents() << std::endl;
    return EXIT_FAILURE;
  }

  // Test pixel color
  int testPosition[2] = { 8, 12 };
  QColor someQImageColor = QColor(someQImage.pixel(testPosition[0], testPosition[1]));
  unsigned char* someVtkImagePixel = static_cast<unsigned char*>(someVtkImage->GetScalarPointer(
    testPosition[0], someVtkImage->GetDimensions()[1] - testPosition[1] - 1, 0));
  QColor someVtkImageColor = QColor(someVtkImagePixel[0], someVtkImagePixel[1], someVtkImagePixel[2]);
  if (someQImageColor != someVtkImageColor)
  {
    std::cout << "qImageToVTKImageData Pixel color error at " << testPosition[0] << " " << testPosition[1] << ": "
      << " QImage " << someQImageColor.red() << " " << someQImageColor.green() << " " << someQImageColor.blue()
      << ", vtkImageData " << someVtkImageColor.red() << " " << someVtkImageColor.green() << " " << someVtkImageColor.blue()
      << std::endl;
    return EXIT_FAILURE;
  }

  // do not force alpha channel - RGBA input
  ctk::qImageToVTKImageData(someQImage, someVtkImage, false);
  if (someVtkImage->GetNumberOfScalarComponents() != 4)
  {
    std::cout << "qImageToVTKImageData expected 4 scalar components for RGBA input + alpha not forced, got: "
      << someVtkImage->GetNumberOfScalarComponents() << std::endl;
    return EXIT_FAILURE;
  }

  // do not force alpha channel - RGB input
  ctk::qImageToVTKImageData(someQImage.convertToFormat(QImage::Format_RGB888), someVtkImage, false);
  if (someVtkImage->GetNumberOfScalarComponents() != 3)
  {
    std::cout << "qImageToVTKImageData expected 3 scalar components for RGBA input + alpha not forced, got: "
      << someVtkImage->GetNumberOfScalarComponents() << std::endl;
    return EXIT_FAILURE;
  }

  // force alpha channel - RGB input
  ctk::qImageToVTKImageData(someQImage.convertToFormat(QImage::Format_RGB888), someVtkImage, true);
  if (someVtkImage->GetNumberOfScalarComponents() != 4)
  {
    std::cout << "qImageToVTKImageData expected 4 scalar components for RGBA input + alpha forced, got: "
      << someVtkImage->GetNumberOfScalarComponents() << std::endl;
    return EXIT_FAILURE;
  }

  // Test qImageToVTKImageData
  //////////////////////////////////////

  // RGBA input
  QImage convertedQImage = ctk::vtkImageDataToQImage(someVtkImage);
  if (someVtkImage->GetDimensions()[0] != convertedQImage.size().width()
    || someVtkImage->GetDimensions()[1] != convertedQImage.size().height())
  {
    std::cout << "vtkImageDataToQImage size mismatch" << std::endl;
    return EXIT_FAILURE;
  }
  if (!convertedQImage.hasAlphaChannel())
  {
    std::cout << "vtkImageDataToQImage expected alpha channel" << std::endl;
    return EXIT_FAILURE;
  }

  // Test pixel color
  QColor convertedQImageColor = QColor(convertedQImage.pixel(testPosition[0], testPosition[1]));
  if (someQImageColor != someVtkImageColor)
  {
    std::cout << "vtkImageDataToQImage Pixel color error at " << testPosition[0] << " " << testPosition[1] << ": "
      << " QImage " << convertedQImageColor.red() << " " << convertedQImageColor.green() << " " << convertedQImageColor.blue()
      << ", vtkImageData " << someVtkImageColor.red() << " " << someVtkImageColor.green() << " " << someVtkImageColor.blue()
      << std::endl;
    return EXIT_FAILURE;
  }

  // RGB input
  ctk::qImageToVTKImageData(someQImage.convertToFormat(QImage::Format_RGB888), someVtkImage, false);
  convertedQImage = ctk::vtkImageDataToQImage(someVtkImage);
  if (convertedQImage.hasAlphaChannel())
  {
    std::cout << "vtkImageDataToQImage expected no alpha channel" << std::endl;
    return EXIT_FAILURE;
  }

  // Display results
  QLabel screenshotLabel;
  screenshotLabel.setPixmap(QPixmap::fromImage(someQImage));
  screenshotLabel.show();

  if (argc < 2 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(100, &app, SLOT(quit()));
    }
  return app.exec();
}
