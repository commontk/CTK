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

// CTK includes
#include "ctkWidgetsTestingUtilities.h"

// Qt includes
#include <QColor>
#include <QImage>

// STD includes
#include <iostream>

using namespace ctkWidgetsTestingUtilities;

//----------------------------------------------------------------------------
bool TestCheckImagesEqual();

//----------------------------------------------------------------------------
int ctkWidgetsTestingUtilitiesTest(int , char * [])
{
  bool res = true;
  res = res && TestCheckImagesEqual();
  return res ? EXIT_SUCCESS : EXIT_FAILURE;
}

//----------------------------------------------------------------------------
bool TestCheckImagesEqual()
{
  {
    // Invalid format
    QImage a(1, 1, QImage::Format_Invalid);
    QImage b(1, 1, QImage::Format_Invalid);
    if (CheckImagesEqual(a, b))
      {
      std::cerr << "Line " << __LINE__ << " - CheckImagesEqual failed" << std::endl;
      return false;
      }
  }

  {
    // Unsupported format
    QImage a(1, 1, QImage::Format_ARGB32);
    QImage b(1, 1, QImage::Format_ARGB32);
    if (CheckImagesEqual(a, b))
      {
      std::cerr << "Line " << __LINE__ << " - CheckImagesEqual failed" << std::endl;
      return false;
      }
  }

  {
    // One image in unsupported format
    QImage a(1, 1, QImage::Format_RGB32);
    QImage b(1, 1, QImage::Format_ARGB32);
    if (CheckImagesEqual(a, b))
      {
      std::cerr << "Line " << __LINE__ << " - CheckImagesEqual failed" << std::endl;
      return false;
      }

    QImage c(1, 1, QImage::Format_ARGB32);
    QImage d(1, 1, QImage::Format_RGB32);
    if (CheckImagesEqual(c, d))
      {
      std::cerr << "Line " << __LINE__ << " - CheckImagesEqual failed" << std::endl;
      return false;
      }
  }

  {
    // Images of different size
    QImage a(1, 1, QImage::Format_RGB32);
    QImage b(2, 1, QImage::Format_RGB32);
    if (CheckImagesEqual(a, b))
      {
      std::cerr << "Line " << __LINE__ << " - CheckImagesEqual failed" << std::endl;
      return false;
      }

    QImage c(1, 2, QImage::Format_ARGB32);
    QImage d(1, 1, QImage::Format_RGB32);
    if (CheckImagesEqual(c, d))
      {
      std::cerr << "Line " << __LINE__ << " - CheckImagesEqual failed" << std::endl;
      return false;
      }
  }

  {
    // Identical images
    QImage a(10, 10, QImage::Format_RGB32);
    a.fill(Qt::green);
    QImage b(10, 10, QImage::Format_RGB32);
    b.fill(Qt::green);
    if (!CheckImagesEqual(a, b, 0.0f))
      {
      std::cerr << "Line " << __LINE__ << " - CheckImagesEqual failed" << std::endl;
      return false;
      }

    // Change one pixel in first image
    a.setPixel(2, 3, qRgb(255, 0, 0));
    if (CheckImagesEqual(a, b, 0.f))
      {
      std::cerr << "Line " << __LINE__ << " - CheckImagesEqual failed" << std::endl;
      return false;
      }

    // Percent threshold not met
    if (CheckImagesEqual(a, b, 0.5f))
      {
      std::cerr << "Line " << __LINE__ << " - CheckImagesEqual failed" << std::endl;
      return false;
      }

    // Percent threshold met
    if (!CheckImagesEqual(a, b, 1.f))
      {
      std::cerr << "Line " << __LINE__ << " - CheckImagesEqual failed" << std::endl;
      return false;
      }

    // Change one pixel in other image
    // Percent threshold not met
    b.setPixel(4, 5, qRgb(255, 255, 0));
    if (CheckImagesEqual(a, b, 1.f))
      {
      std::cerr << "Line " << __LINE__ << " - CheckImagesEqual failed" << std::endl;
      return false;
      }

    // Percent threshold met
    if (!CheckImagesEqual(a, b, 2.f))
      {
      std::cerr << "Line " << __LINE__ << " - CheckImagesEqual failed" << std::endl;
      return false;
      }

    // Change one pixel in first image to match second image
    a.setPixel(4, 5, qRgb(255, 255, 0));
    if (!CheckImagesEqual(a, b, 1.f))
      {
      std::cerr << "Line " << __LINE__ << " - CheckImagesEqual failed" << std::endl;
      return false;
      }

    // Identical images
    b.setPixel(2, 3, qRgb(255, 0, 0));
    if (!CheckImagesEqual(a, b, 0.f))
      {
      std::cerr << "Line " << __LINE__ << " - CheckImagesEqual failed" << std::endl;
      return false;
      }
  }

  return true;
}
