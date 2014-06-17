/*=============================================================================

  Library: XNAT/Core

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

#ifndef CTKXNATDEFAULTSCHEMATYPES_H
#define CTKXNATDEFAULTSCHEMATYPES_H

#include "ctkXNATCoreExport.h"

#include <QString>

/**
 * @ingroup XNAT_Core
 */
struct CTK_XNAT_CORE_EXPORT ctkXnatDefaultSchemaTypes
{
  static QString XSI_PROJECT; // = "xnat:projectData"
  static QString XSI_SUBJECT; // = "xnat:subjectData"
  static QString XSI_EXPERIMENT; // = "xnat:experimentData"
  static QString XSI_SCAN; // = "xnat:imageScanData"
  static QString XSI_SCAN_RESOURCE; // = "xnat:scanResource"
  static QString XSI_ASSESSOR; // = "xnat:imageAssessorData"
  static QString XSI_ASSESSOR_RESOURCE; // = "xnat:assessorResource"
  static QString XSI_FILE; // = "xnat:abstractResource"
  static QString XSI_RECONSTRUCTION; // = "xnat:reconstructedImageData"
  static QString XSI_RECONSTRUCTION_RESOURCE; // = "xnat:reconstructionResource"
  static QString XSI_RESOURCE; // = "xnat:resource"
};

#endif // CTKXNATDEFAULTSCHEMATYPES_H
