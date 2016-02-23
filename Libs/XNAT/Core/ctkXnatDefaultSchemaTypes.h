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
  static QString XSI_IMAGE_SESSION_DATA; // = "xnat:imageSessionData"
  static QString XSI_SUBJECT_VARIABLE_DATA; // = "xnat:subjectVariablesData"
  static QString XSI_SCAN; // = "xnat:imageScanData"
  static QString XSI_ASSESSOR; // = "xnat:imageAssessorData"
  static QString XSI_FILE; // = "xnat:abstractResource"
  static QString XSI_RECONSTRUCTION; // = "xnat:reconstructedImageData"
  static QString XSI_RESOURCE; // = "xnat:resource"

  // Specific schema types for an experiment session
  static QString XSI_CR_SESSION; // = "xnat:crSessionData"
  static QString XSI_CT_SESSION; // = "xnat:ctSessionData"
  static QString XSI_DX_SESSION; // = "xnat:dxSessionData"
  static QString XSI_DX3DCRANIOFACIAL_SESSION; // = "xnat:dx3DCraniofacialSessionData"
  static QString XSI_ECG_SESSION; // = "xnat:ecgSessionData"
  static QString XSI_EEG_SESSION; // = "xnat:eegessionData"
  static QString XSI_EPS_SESSION; // = "xnat:epsSessionData"
  static QString XSI_ES_SESSION; // = "xnat:esSessionData"
  static QString XSI_ESV_SESSION; // = "xnat:esvSessionData"
  static QString XSI_GM_SESSION; // = "xnat:gmSessionData"
  static QString XSI_GMV_SESSION; // = "xnat:gmvSessionData"
  static QString XSI_HD_SESSION; // = "xnat:hdSessionData"
  static QString XSI_IO_SESSION; // = "xnat:ioSessionData"
  static QString XSI_MEG_SESSION; // = "xnat:megSessionData"
  static QString XSI_MG_SESSION; // = "xnat:mgSessionData"
  static QString XSI_MR_SESSION; // = "xnat:mrSessionData"
  static QString XSI_OP_SESSION; // = "xnat:opSessionData"
  static QString XSI_OPT_SESSION; // = "xnat:optSessionData"
  static QString XSI_OTHER_DICOM_SESSION; // = "xnat:otherDicomSessionData"
  static QString XSI_PET_SESSION; // = "xnat:petSessionData"
  static QString XSI_RF_SESSION; // = "xnat:rfSessionData"
  static QString XSI_RT_SESSION; // = "xnat:rtSessionData"
  static QString XSI_SM_SESSION; // = "xnat:smSessionData"
  static QString XSI_US_SESSION; // = "xnat:usSessionData"
  static QString XSI_XA_SESSION; // = "xnat:xaSessionData"
  static QString XSI_XA3D_SESSION; // = "xnat:xa3DSessionData"
  static QString XSI_XC_SESSION; // = "xnat:xcSessionData"
  static QString XSI_XCV_SESSION; // = "xnat:xcvSessionData"
};

#endif // CTKXNATDEFAULTSCHEMATYPES_H
