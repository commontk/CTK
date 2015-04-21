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

#ifndef CTKLOCATIONMANAGER_H
#define CTKLOCATIONMANAGER_H

#include <QString>

class ctkBasicLocation;

/**
 * This class is used to manage the various Locations for BlueBerry.
 * <p>
 * Clients may not extend this class.
 * </p>
 * @noextend This class is not intended to be subclassed by clients.
 */
class ctkLocationManager
{

public:

  static const QString READ_ONLY_AREA_SUFFIX; // = ".readOnly";

  // configuration area file/dir names
  static const QString CONFIG_FILE; // = "config.ini";
  static const QString CTK_PROPERTIES; // = "ctk.properties";

  /**
   * Initializes the QFileInfo objects for the ctkLocationManager.
   */
  static void initializeLocations();

  /**
   * Returns the user ctkLocation object
   * @return the user ctkLocation object
   */
  static ctkBasicLocation* getUserLocation();

  /**
   * Returns the configuration ctkLocation object
   * @return the configuration ctkLocation object
   */
  static ctkBasicLocation* getConfigurationLocation();

  /**
   * Returns the install ctkLocation object
   * @return the install ctkLocation object
   */
  static ctkBasicLocation* getInstallLocation();

  /**
   * Returns the instance ctkLocation object
   * @return the instance ctkLocation object
   */
  static ctkBasicLocation* getInstanceLocation();

  static ctkBasicLocation* getCTKHomeLocation();

};

#endif // CTKLOCATIONMANAGER_H
