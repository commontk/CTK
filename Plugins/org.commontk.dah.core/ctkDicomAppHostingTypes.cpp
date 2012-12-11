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

// CTK includes
#include "ctkDicomAppHostingTypes.h"

namespace ctkDicomAppHosting {

//----------------------------------------------------------------------------
bool operator ==(const ctkDicomAppHosting::Status& left,
                 const ctkDicomAppHosting::Status& right)
{
  return (left.statusType == right.statusType &&
          left.codingSchemeDesignator == right.codingSchemeDesignator &&
          left.codeValue == right.codeValue &&
          left.codeMeaning == right.codeMeaning);
}

//----------------------------------------------------------------------------
bool operator !=(const ctkDicomAppHosting::Status& left,
                 const ctkDicomAppHosting::Status& right)
{
  return !(left == right);
}

//----------------------------------------------------------------------------
bool operator ==(const ctkDicomAppHosting::ObjectLocator& left,
                 const ctkDicomAppHosting::ObjectLocator& right)
{
  return (left.locator == right.locator &&
          left.source == right.source &&
          left.transferSyntax == right.transferSyntax &&
          left.length == right.length &&
          left.offset == right.offset &&
          left.URI == right.URI);
}

//----------------------------------------------------------------------------
bool operator !=(const ctkDicomAppHosting::ObjectLocator& left,
                 const ctkDicomAppHosting::ObjectLocator& right)
{
  return !(left == right);
}

//----------------------------------------------------------------------------
bool operator ==(const ctkDicomAppHosting::ObjectDescriptor& left,
                 const ctkDicomAppHosting::ObjectDescriptor& right)
{
  return (left.descriptorUUID == right.descriptorUUID &&
          left.mimeType == right.mimeType &&
          left.classUID == right.classUID &&
          left.transferSyntaxUID == right.transferSyntaxUID &&
          left.modality == right.modality);
}

//----------------------------------------------------------------------------
bool operator !=(const ctkDicomAppHosting::ObjectDescriptor& left,
                 const ctkDicomAppHosting::ObjectDescriptor& right)
{
  return !(left == right);
}

//----------------------------------------------------------------------------
bool operator ==(const ctkDicomAppHosting::Series& left,
                 const ctkDicomAppHosting::Series& right)
{
  return (left.seriesUID == right.seriesUID &&
          left.objectDescriptors == right.objectDescriptors);
}

//----------------------------------------------------------------------------
bool operator !=(const ctkDicomAppHosting::Series& left,
                 const ctkDicomAppHosting::Series& right)
{
  return !(left == right);
}

//----------------------------------------------------------------------------
bool operator ==(const ctkDicomAppHosting::Study& left,
                 const ctkDicomAppHosting::Study& right)
{
  return (left.studyUID == right.studyUID &&
          left.series == right.series &&
          left.objectDescriptors == right.objectDescriptors);
}

//----------------------------------------------------------------------------
bool operator !=(const ctkDicomAppHosting::Study& left,
                 const ctkDicomAppHosting::Study& right)
{
  return !(left == right);
}

//----------------------------------------------------------------------------
bool operator ==(const ctkDicomAppHosting::Patient& left,
                 const ctkDicomAppHosting::Patient& right)
{
  return (left.assigningAuthority == right.assigningAuthority &&
          left.birthDate == right.birthDate &&
          left.id == right.id &&
          left.name == right.name &&
          left.objectDescriptors == right.objectDescriptors &&
          left.sex == right.sex &&
          left.studies == right.studies);
}

//----------------------------------------------------------------------------
bool operator !=(const ctkDicomAppHosting::Patient& left,
                 const ctkDicomAppHosting::Patient& right)
{
  return !(left == right);
}

//----------------------------------------------------------------------------
bool operator ==(const ctkDicomAppHosting::AvailableData& left,
                 const ctkDicomAppHosting::AvailableData& right)
{
  return (left.patients == right.patients &&
          left.objectDescriptors == right.objectDescriptors);
}

//----------------------------------------------------------------------------
bool operator !=(const ctkDicomAppHosting::AvailableData& left,
                 const ctkDicomAppHosting::AvailableData& right)
{
  return !(left == right);
}

}

