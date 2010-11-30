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


#ifndef CTKQTMOBILITYSERVICECONSTANTS_P_H
#define CTKQTMOBILITYSERVICECONSTANTS_P_H

#include <QString>

/**
 * Defines standard names for QtMobility Service constants.
 */
struct ctkQtMobilityServiceConstants
{

  /**
     * Manifest header specifying the XML document within a plugin that contains
     * the plugin's QtMobility service descriptor.
     * <p>
     * The attribute value may be retrieved from the <code>ctkDictionary</code>
     * object returned by the <code>ctkPlugin::getHeaders</code> method.
     */
  static const QString SERVICE_DESCRIPTOR; // = "Service-Descriptor";
};

#endif // CTKQTMOBILITYSERVICECONSTANTS_P_H
