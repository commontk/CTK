/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

// Qt includes
#include <QUuid>

// CTK includes
#include <ctkDicomObjectLocatorCache.h>

// STD includes
#include <cstdlib>
#include <iostream>

//----------------------------------------------------------------------------
int ctkDicomObjectLocatorCacheTest1(int argc, char* argv[])
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);

  ctkDicomObjectLocatorCache cache;

  //----------------------------------------------------------------------------
  if (cache.remove(""))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with remove() method" << std::endl;
    return EXIT_FAILURE;
    }

  //----------------------------------------------------------------------------
  QString objectUuid = QUuid::createUuid().toString();
  ctkDicomAppHosting::ObjectLocator objectLocator;
  objectLocator.length = 64;
  objectLocator.source = "/path/to/source";

  ctkDicomAppHosting::ObjectLocator objectLocatorFound;
  if (cache.find(objectUuid, objectLocatorFound))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with find() method" << std::endl;
    return EXIT_FAILURE;
    }

  cache.insert(objectUuid, objectLocator);

  //----------------------------------------------------------------------------
  if (!cache.find(objectUuid, objectLocatorFound))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with find() method" << std::endl;
    return EXIT_FAILURE;
    }

  if (objectLocator != objectLocatorFound)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with find() method"
              << " - objectLocator != objectLocatorFound" << std::endl;
    return EXIT_FAILURE;
    }

  //----------------------------------------------------------------------------
  if (!cache.remove(objectUuid))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with remove() method" << std::endl;
    return EXIT_FAILURE;
    }

  //----------------------------------------------------------------------------
  cache.insert(objectUuid, objectLocator);
  cache.insert(objectUuid, objectLocator);
  cache.insert(objectUuid, objectLocator);

  // Since the Reference count associated with the objectLocator is equal to three,
  // after two successive call of "remove()", the objectLocator should still be in the cache.

  // First call to "remove()"
  if (!cache.remove(objectUuid))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with remove() method" << std::endl;
    return EXIT_FAILURE;
    }

  ctkDicomAppHosting::ObjectLocator objectLocatorFound2;
  if (!cache.find(objectUuid, objectLocatorFound2))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with find() method" << std::endl;
    return EXIT_FAILURE;
    }

  if (objectLocator != objectLocatorFound2)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with find() method"
              << " - objectLocator != objectLocatorFound" << std::endl;
    return EXIT_FAILURE;
    }

  // Second call to "remove()"
  if (!cache.remove(objectUuid))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with remove() method" << std::endl;
    return EXIT_FAILURE;
    }

  ctkDicomAppHosting::ObjectLocator objectLocatorFound3;
  if (!cache.find(objectUuid, objectLocatorFound3))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with find() method" << std::endl;
    return EXIT_FAILURE;
    }

  if (objectLocator != objectLocatorFound3)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with find() method"
              << " - objectLocator != objectLocatorFound" << std::endl;
    return EXIT_FAILURE;
    }

  // Third call to "remove()"
  if (!cache.remove(objectUuid))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with remove() method" << std::endl;
    return EXIT_FAILURE;
    }

  ctkDicomAppHosting::ObjectLocator objectLocatorFound4;
  if (cache.find(objectUuid, objectLocatorFound4))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with find() method" << std::endl;
    return EXIT_FAILURE;
    }

  // Fourth call to "remove()"
  if (cache.remove(objectUuid))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with remove() method" << std::endl;
    return EXIT_FAILURE;
    }

  //----------------------------------------------------------------------------

  ctkDicomAppHosting::AvailableData availableData;

  if (cache.isCached(availableData))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with isCached() method" << std::endl;
    return EXIT_FAILURE;
    }

  ctkDicomAppHosting::Patient patient;
  availableData.patients << patient;

  if (cache.isCached(availableData))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with isCached() method" << std::endl;
    return EXIT_FAILURE;
    }

  ctkDicomAppHosting::ObjectDescriptor objectDescriptor;
  objectDescriptor.descriptorUUID = objectUuid;

  availableData.objectDescriptors << objectDescriptor;

  if (cache.isCached(availableData))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with isCached() method" << std::endl;
    return EXIT_FAILURE;
    }

  cache.insert(objectUuid, objectLocator);

  if (!cache.isCached(availableData))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with isCached() method" << std::endl;
    return EXIT_FAILURE;
    }

  QString objectUuid2 = QUuid::createUuid().toString();
  ctkDicomAppHosting::ObjectDescriptor objectDescriptor2;
  objectDescriptor2.descriptorUUID = objectUuid2;
  ctkDicomAppHosting::ObjectLocator objectLocator2;
  objectLocator2.length = 50;
  objectLocator2.source = "/path/to/source2";

  availableData.objectDescriptors << objectDescriptor2;

  if (cache.isCached(availableData))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with isCached() method" << std::endl;
    return EXIT_FAILURE;
    }

  cache.insert(objectUuid2, objectLocator2);

  if (!cache.isCached(availableData))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with isCached() method" << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
