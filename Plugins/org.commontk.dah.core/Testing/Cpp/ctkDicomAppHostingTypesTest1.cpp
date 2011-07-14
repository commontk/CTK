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

// CTK includes
#include <ctkDicomAppHostingTypes.h>

// STD includes
#include <cstdlib>
#include <iostream>

namespace
{
//----------------------------------------------------------------------------
template<typename STRUCT>
bool verifyOperator(const STRUCT& left, const STRUCT& right, bool equalExpected)
{
  bool equal = (left == right);
  bool different = (left != right);

  if (equalExpected)
    {
    if (!equal)
      {
      return false;
      }
    if (different)
      {
      return false;
      }
    }
  else
    {
    if (equal)
      {
      return false;
      }
    if (!different)
      {
      return false;
      }
    }
  return true;
}
}

//----------------------------------------------------------------------------
#define CHECK_OP(TYPE, LEFT, RIGHT, EQUAL_EXPECTED)        \
  if (!verifyOperator<ctkDicomAppHosting::TYPE>(           \
        LEFT, RIGHT, /*equalExpected=*/ EQUAL_EXPECTED))   \
    {                                                      \
    std::cerr << "Line "                                   \
              << __LINE__ << " - Problem with " << #TYPE   \
              << " comparison operator" << std::endl;      \
    return EXIT_FAILURE;                                   \
    }

//----------------------------------------------------------------------------
int ctkDicomAppHostingTypesTest1(int argc, char* argv[])
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);

  //----------------------------------------------------------------------------
  ctkDicomAppHosting::Status status1;
  ctkDicomAppHosting::Status status2;
  CHECK_OP(Status, status1, status2, /*EQUAL_EXPECTED=*/ true);

  status2.codeMeaning = "codeMeaning";
  CHECK_OP(Status, status1, status2, /*EQUAL_EXPECTED=*/ false);

  status2.codeMeaning = "";
  CHECK_OP(Status, status1, status2, /*EQUAL_EXPECTED=*/ true);

  status2.codeValue = "codeValue";
  CHECK_OP(Status, status1, status2, /*EQUAL_EXPECTED=*/ false);

  status2.codeValue = "";
  CHECK_OP(Status, status1, status2, /*EQUAL_EXPECTED=*/ true);

  status2.codingSchemeDesignator = "codingSchemeDesignator";
  CHECK_OP(Status, status1, status2, /*EQUAL_EXPECTED=*/ false);

  status2.codingSchemeDesignator = "";
  CHECK_OP(Status, status1, status2, /*EQUAL_EXPECTED=*/ true);

  status2.statusType = ctkDicomAppHosting::FATALERROR;
  CHECK_OP(Status, status1, status2, /*EQUAL_EXPECTED=*/ false);

  status2.statusType = ctkDicomAppHosting::INFORMATION;
  CHECK_OP(Status, status1, status2, /*EQUAL_EXPECTED=*/ true);

  //----------------------------------------------------------------------------
  ctkDicomAppHosting::ObjectLocator objectLocator1;
  ctkDicomAppHosting::ObjectLocator objectLocator2;
  CHECK_OP(ObjectLocator, objectLocator1, objectLocator2, /*EQUAL_EXPECTED=*/ true);

  objectLocator2.length = 64;
  CHECK_OP(ObjectLocator, objectLocator1, objectLocator2, /*EQUAL_EXPECTED=*/ false);

  objectLocator2.length = 0;
  CHECK_OP(ObjectLocator, objectLocator1, objectLocator2, /*EQUAL_EXPECTED=*/ true);

  objectLocator2.locator = "locator";
  CHECK_OP(ObjectLocator, objectLocator1, objectLocator2, /*EQUAL_EXPECTED=*/ false);

  objectLocator2.locator = "";
  CHECK_OP(ObjectLocator, objectLocator1, objectLocator2, /*EQUAL_EXPECTED=*/ true);

  objectLocator2.offset = 100;
  CHECK_OP(ObjectLocator, objectLocator1, objectLocator2, /*EQUAL_EXPECTED=*/ false);

  objectLocator2.offset = 0;
  CHECK_OP(ObjectLocator, objectLocator1, objectLocator2, /*EQUAL_EXPECTED=*/ true);

  objectLocator2.source = "source";
  CHECK_OP(ObjectLocator, objectLocator1, objectLocator2, /*EQUAL_EXPECTED=*/ false);

  objectLocator2.source = "";
  CHECK_OP(ObjectLocator, objectLocator1, objectLocator2, /*EQUAL_EXPECTED=*/ true);

  objectLocator2.transferSyntax = "transferSyntax";
  CHECK_OP(ObjectLocator, objectLocator1, objectLocator2, /*EQUAL_EXPECTED=*/ false);

  objectLocator2.transferSyntax = "";
  CHECK_OP(ObjectLocator, objectLocator1, objectLocator2, /*EQUAL_EXPECTED=*/ true);

  objectLocator2.URI = "URI";
  CHECK_OP(ObjectLocator, objectLocator1, objectLocator2, /*EQUAL_EXPECTED=*/ false);

  objectLocator2.URI = "";
  CHECK_OP(ObjectLocator, objectLocator1, objectLocator2, /*EQUAL_EXPECTED=*/ true);
  
  //----------------------------------------------------------------------------
  ctkDicomAppHosting::ObjectDescriptor objectDescriptor1;
  ctkDicomAppHosting::ObjectDescriptor objectDescriptor2;
  CHECK_OP(ObjectDescriptor, objectDescriptor1, objectDescriptor2, /*EQUAL_EXPECTED=*/ true);
  
  objectDescriptor2.classUID = "classUID";
  CHECK_OP(ObjectDescriptor, objectDescriptor1, objectDescriptor2, /*EQUAL_EXPECTED=*/ false);

  objectDescriptor2.classUID = "";
  CHECK_OP(ObjectDescriptor, objectDescriptor1, objectDescriptor2, /*EQUAL_EXPECTED=*/ true);

  objectDescriptor2.descriptorUUID = "descriptorUUID";
  CHECK_OP(ObjectDescriptor, objectDescriptor1, objectDescriptor2, /*EQUAL_EXPECTED=*/ false);

  objectDescriptor2.descriptorUUID = "";
  CHECK_OP(ObjectDescriptor, objectDescriptor1, objectDescriptor2, /*EQUAL_EXPECTED=*/ true);

  objectDescriptor2.mimeType = "mimeType";
  CHECK_OP(ObjectDescriptor, objectDescriptor1, objectDescriptor2, /*EQUAL_EXPECTED=*/ false);

  objectDescriptor2.mimeType = "";
  CHECK_OP(ObjectDescriptor, objectDescriptor1, objectDescriptor2, /*EQUAL_EXPECTED=*/ true);

  objectDescriptor2.modality = "modality";
  CHECK_OP(ObjectDescriptor, objectDescriptor1, objectDescriptor2, /*EQUAL_EXPECTED=*/ false);

  objectDescriptor2.modality = "";
  CHECK_OP(ObjectDescriptor, objectDescriptor1, objectDescriptor2, /*EQUAL_EXPECTED=*/ true);

  objectDescriptor2.transferSyntaxUID = "transferSyntaxUID";
  CHECK_OP(ObjectDescriptor, objectDescriptor1, objectDescriptor2, /*EQUAL_EXPECTED=*/ false);

  objectDescriptor2.transferSyntaxUID = "";
  CHECK_OP(ObjectDescriptor, objectDescriptor1, objectDescriptor2, /*EQUAL_EXPECTED=*/ true);

  //----------------------------------------------------------------------------
  ctkDicomAppHosting::Series series1;
  ctkDicomAppHosting::Series series2;
  CHECK_OP(Series, series1, series2, /*EQUAL_EXPECTED=*/ true);

  series2.seriesUID = "studyUID";
  CHECK_OP(Series, series1, series2, /*EQUAL_EXPECTED=*/ false);

  series2.seriesUID = "";
  CHECK_OP(Series, series1, series2, /*EQUAL_EXPECTED=*/ true);

  series2.objectDescriptors << objectDescriptor1 << objectDescriptor2;
  CHECK_OP(Series, series1, series2, /*EQUAL_EXPECTED=*/ false);

  series1.objectDescriptors << objectDescriptor1 << objectDescriptor2;
  CHECK_OP(Series, series1, series2, /*EQUAL_EXPECTED=*/ true);

  series1.objectDescriptors.clear();
  CHECK_OP(Series, series1, series2, /*EQUAL_EXPECTED=*/ false);

  series2.objectDescriptors.clear();
  CHECK_OP(Series, series1, series2, /*EQUAL_EXPECTED=*/ true);

  //----------------------------------------------------------------------------
  ctkDicomAppHosting::Study study1;
  ctkDicomAppHosting::Study study2;
  CHECK_OP(Study, study1, study2, /*EQUAL_EXPECTED=*/ true);

  study1.series << series1 << series2;
  CHECK_OP(Study, study1, study2, /*EQUAL_EXPECTED=*/ false);

  study2.series << series1 << series2;
  CHECK_OP(Study, study1, study2, /*EQUAL_EXPECTED=*/ true);

  study2.studyUID = "studyUID";
  CHECK_OP(Study, study1, study2, /*EQUAL_EXPECTED=*/ false);

  study2.studyUID = "";
  CHECK_OP(Study, study1, study2, /*EQUAL_EXPECTED=*/ true);

  study2.objectDescriptors << objectDescriptor1 << objectDescriptor2;
  CHECK_OP(Study, study1, study2, /*EQUAL_EXPECTED=*/ false);

  study1.objectDescriptors << objectDescriptor1 << objectDescriptor2;
  CHECK_OP(Study, study1, study2, /*EQUAL_EXPECTED=*/ true);

  study1.objectDescriptors.clear();
  CHECK_OP(Study, study1, study2, /*EQUAL_EXPECTED=*/ false);

  study2.objectDescriptors.clear();
  CHECK_OP(Study, study1, study2, /*EQUAL_EXPECTED=*/ true);

  //----------------------------------------------------------------------------
  ctkDicomAppHosting::Patient patient1;
  ctkDicomAppHosting::Patient patient2;
  CHECK_OP(Patient, patient1, patient2, /*EQUAL_EXPECTED=*/ true);

  patient2.assigningAuthority = "assigningAuthority";
  CHECK_OP(Patient, patient1, patient2, /*EQUAL_EXPECTED=*/ false);

  patient2.assigningAuthority = "";
  CHECK_OP(Patient, patient1, patient2, /*EQUAL_EXPECTED=*/ true);

  patient2.birthDate = "birthDate";
  CHECK_OP(Patient, patient1, patient2, /*EQUAL_EXPECTED=*/ false);

  patient2.birthDate = "";
  CHECK_OP(Patient, patient1, patient2, /*EQUAL_EXPECTED=*/ true);

  patient2.id = "id";
  CHECK_OP(Patient, patient1, patient2, /*EQUAL_EXPECTED=*/ false);

  patient2.id = "";
  CHECK_OP(Patient, patient1, patient2, /*EQUAL_EXPECTED=*/ true);

  patient2.name = "name";
  CHECK_OP(Patient, patient1, patient2, /*EQUAL_EXPECTED=*/ false);

  patient2.name = "";
  CHECK_OP(Patient, patient1, patient2, /*EQUAL_EXPECTED=*/ true);

  patient2.sex = "sex";
  CHECK_OP(Patient, patient1, patient2, /*EQUAL_EXPECTED=*/ false);

  patient2.sex = "";
  CHECK_OP(Patient, patient1, patient2, /*EQUAL_EXPECTED=*/ true);

  //----------------------------------------------------------------------------
  ctkDicomAppHosting::AvailableData availableData1;
  ctkDicomAppHosting::AvailableData availableData2;
  CHECK_OP(AvailableData, availableData1, availableData2, /*EQUAL_EXPECTED=*/ true);

  ctkDicomAppHosting::Patient p1;
  p1.name = "name";
  p1.studies << study1 << study2;

  ctkDicomAppHosting::Patient p2;
  p2.name = "name";
  p2.studies << study1 << study2;
  CHECK_OP(Patient, p1, p2, /*EQUAL_EXPECTED=*/ true);

  availableData1.patients << p1 << p2;
  CHECK_OP(AvailableData, availableData1, availableData2, /*EQUAL_EXPECTED=*/ false);

  availableData2.patients << p1 << p2;
  CHECK_OP(AvailableData, availableData1, availableData2, /*EQUAL_EXPECTED=*/ true);
  
  return EXIT_SUCCESS;
}
