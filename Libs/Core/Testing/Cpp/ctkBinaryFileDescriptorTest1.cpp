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
#include <QFile>

// CTK includes
#include "ctkBinaryFileDescriptor.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkBinaryFileDescriptorTest1(int argc, char * argv[])
{
  if (argc <= 1)
    {
    std::cerr << "Missing argument" << std::endl;
    return EXIT_FAILURE;
    }
  QString filePath(argv[1]);

  if (!QFile::exists(filePath))
    {
    std::cerr << "Line " << __LINE__ << " - "
              << "filePath [" << qPrintable(filePath)
              << "] do *NOT* exist" << std::endl;
    return EXIT_FAILURE;
    }

  ctkBinaryFileDescriptor bfd;
  if (bfd.load())
    {
    // Should fail to load without any valid filename
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with load() method" << std::endl;
    return EXIT_FAILURE;
    }
  if (!bfd.unload())
    {
    // Unload inconditionnally return True
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with unload() method" << std::endl;
    return EXIT_FAILURE;
    }
  if (bfd.isLoaded())
    {
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with isLoaded() method" << std::endl;
    return EXIT_FAILURE;
    }
  if (!bfd.fileName().isEmpty())
    {
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with fileName() method" << std::endl;
    return EXIT_FAILURE;
    }

  bfd.setFileName(filePath);

  if (bfd.fileName() != filePath)
    {
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with fileName() method" << std::endl;
    return EXIT_FAILURE;
    }

  if (!bfd.load())
    {
    // Should succeed since a valid filename is provided
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with load() method" << std::endl;
    return EXIT_FAILURE;
    }

  void * main_pointer = bfd.resolve("main");
  if (!main_pointer)
    {
    std::cerr << "Line " << __LINE__ << " - "
              << "Problem with resolve() method - "
              << "Failed to revolve 'main' symbol !" << std::endl;
    return EXIT_FAILURE;
    }

  void * mtBlancElevationInMeters_pointer = bfd.resolve("MtBlancElevationInMeters");
  int * mtBlancElevationInMeters = reinterpret_cast<int*>(mtBlancElevationInMeters_pointer);
  if (!mtBlancElevationInMeters)
    {
    std::cerr << "Line " << __LINE__ << " - "
              << "Failed to case 'mtBlancElevationInMeters' pointer !" << std::endl;
    return EXIT_FAILURE;
    }
  if (*mtBlancElevationInMeters != 4810)
    {
    std::cerr << "Line " << __LINE__ << " - "
              << "Failed to invoke function associated with symbol 'MtBlancElevationInMeters' !" << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

