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
#include <QLatin1Char>

// ctkDICOMCore includes
#include "ctkDICOMQuery.h"
#include "ctkDICOMQueryLimitWarning.h"

// DCMTK includes
#include <dcmtk/dcmnet/diutil.h>

//----------------------------------------------------------------------------
QString formatQueryLimitWarning(const ctkDICOMQueryLimitWarning& warning)
{
  switch (warning.Reason)
  {
    case ctkDICOMQueryLimitReason::ClientMaximumReached:
      return ctkDICOMQuery::tr(
        "The number of responses of the query task at %1 level "
        "surpassed the maximum value of permitted results (i.e. %2).")
        .arg(warning.Level)
        .arg(warning.Limit);
    case ctkDICOMQueryLimitReason::ServerResponseLimit:
      return ctkDICOMQuery::tr(
        "Server reported incomplete %1 query results (DICOM status B001: "
        "response limit reached). Refine filters or use pagination if the "
        "server supports Repository Query.")
        .arg(warning.Level);
    case ctkDICOMQueryLimitReason::ServerIncompleteWarning:
      return ctkDICOMQuery::tr(
        "Server reported incomplete %1 query results (DICOM status 0x%2: %3). "
        "Refine filters if results look incomplete.")
        .arg(warning.Level)
        .arg(warning.DimseStatus, 4, 16, QLatin1Char('0'))
        .arg(QString(DU_cfindStatusString(warning.DimseStatus)));
    default:
      return QString();
  }
}
