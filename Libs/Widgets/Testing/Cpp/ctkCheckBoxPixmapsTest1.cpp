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

// Qt includes
#include <QApplication>

// CTK includes
#include "ctkCheckBoxPixmaps.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkCheckBoxPixmapsTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkCheckBoxPixmaps checkBoxPixmaps;
  // make sure all the pixmaps are valid
  if (checkBoxPixmaps.pixmap(Qt::Checked, true).isNull())
    {
    std::cerr << "Failed to create active Qt::Checked pixmap" << std::endl;
    return EXIT_FAILURE;
    }
  if (checkBoxPixmaps.pixmap(Qt::PartiallyChecked, true).isNull())
    {
    std::cerr << "Failed to create active Qt::PartiallyChecked pixmap" << std::endl;
    return EXIT_FAILURE;
    }
  if (checkBoxPixmaps.pixmap(Qt::Unchecked, true).isNull())
    {
    std::cerr << "Failed to create active Qt::Unchecked pixmap" << std::endl;
    return EXIT_FAILURE;
    }
  if (checkBoxPixmaps.pixmap(Qt::Checked, false).isNull())
    {
    std::cerr << "Failed to create unactive Qt::Checked pixmap" << std::endl;
    return EXIT_FAILURE;
    }
  if (checkBoxPixmaps.pixmap(Qt::PartiallyChecked, false).isNull())
    {
    std::cerr << "Failed to create unactive Qt::PartiallyChecked pixmap" << std::endl;
    return EXIT_FAILURE;
    }
  if (checkBoxPixmaps.pixmap(Qt::Unchecked, false).isNull())
    {
    std::cerr << "Failed to create unactive Qt::Unchecked pixmap" << std::endl;
    return EXIT_FAILURE;
    }
  // check the int version of the ctkCheckBoxPixmaps::pixmap()
  if (checkBoxPixmaps.pixmap(0, false).isNull())
    {
    std::cerr << "Failed to create unactive Qt::Checked pixmap" << std::endl;
    return EXIT_FAILURE;
    }
  // Same pixmap ?
  const QPixmap& p1 = checkBoxPixmaps.pixmap(Qt::PartiallyChecked, false);
  const QPixmap& p2 = checkBoxPixmaps.pixmap(Qt::PartiallyChecked, false);
  if ( &p1 != &p2)
    {
    std::cerr << "The returned pixmap should be the same" << std::endl;
    return EXIT_FAILURE;
    }
  // Same pixmap ?
  if (&checkBoxPixmaps.pixmap(Qt::Unchecked, false) == 
      &checkBoxPixmaps.pixmap(Qt::Unchecked, true))
    {
    std::cerr << "The returned pixmaps should not be the same" << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
