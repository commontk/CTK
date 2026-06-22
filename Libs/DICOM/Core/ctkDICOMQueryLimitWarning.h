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

#ifndef __ctkDICOMQueryLimitWarning_h
#define __ctkDICOMQueryLimitWarning_h

// Qt includes
#include <QtGlobal>
#include <QString>

// ctkDICOMCore includes
#include "ctkDICOMCoreExport.h"

/// \ingroup DICOM_Core
enum class ctkDICOMQueryLimitReason
{
  ClientMaximumReached,
  ServerResponseLimit,
  ServerIncompleteWarning
};

/// \ingroup DICOM_Core
struct CTK_DICOM_CORE_EXPORT ctkDICOMQueryLimitWarning
{
  ctkDICOMQueryLimitReason Reason{ctkDICOMQueryLimitReason::ClientMaximumReached};
  QString Level;
  quint16 DimseStatus{0};
  int Limit{0};
};

/// Format a query limit warning for display in the GUI and job logs.
CTK_DICOM_CORE_EXPORT QString formatQueryLimitWarning(const ctkDICOMQueryLimitWarning& warning);

#endif
