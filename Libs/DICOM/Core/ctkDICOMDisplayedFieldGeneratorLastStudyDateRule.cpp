/*=========================================================================

  Library:   CTK

  Copyright (c) Pixel Medical 2020

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

#include "ctkDICOMDisplayedFieldGeneratorLastStudyDateRule.h"

// ctkDICOM includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMItem.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

//------------------------------------------------------------------------------
ctkDICOMDisplayedFieldGeneratorLastStudyDateRule::ctkDICOMDisplayedFieldGeneratorLastStudyDateRule()
{
}

//------------------------------------------------------------------------------
QString ctkDICOMDisplayedFieldGeneratorLastStudyDateRule::name()const
{
  return "LastStudyDate";
}

//------------------------------------------------------------------------------
ctkDICOMDisplayedFieldGeneratorAbstractRule* ctkDICOMDisplayedFieldGeneratorLastStudyDateRule::clone()
{
  return new ctkDICOMDisplayedFieldGeneratorLastStudyDateRule();
}

//------------------------------------------------------------------------------
QStringList ctkDICOMDisplayedFieldGeneratorLastStudyDateRule::getRequiredDICOMTags()
{
  QStringList requiredTags;

  requiredTags << dicomTagToString(DCM_PatientID);
  requiredTags << dicomTagToString(DCM_PatientName);
  requiredTags << dicomTagToString(DCM_PatientBirthDate);

  requiredTags << dicomTagToString(DCM_StudyDate);

  return requiredTags;
}

//------------------------------------------------------------------------------
void ctkDICOMDisplayedFieldGeneratorLastStudyDateRule::getDisplayedFieldsForInstance(
  const QMap<QString, QString> &cachedTagsForInstance, QMap<QString, QString> &displayedFieldsForCurrentSeries,
  QMap<QString, QString> &displayedFieldsForCurrentStudy, QMap<QString, QString> &displayedFieldsForCurrentPatient )
{
  // Store patient information for the instances that of which displayed fields are updated in this run.
  QString patientID = cachedTagsForInstance[ctkDICOMItem::TagKeyStripped(DCM_PatientID)];
  QString patientsBirthDate = cachedTagsForInstance[ctkDICOMItem::TagKeyStripped(DCM_PatientBirthDate)];
  QString patientsName = cachedTagsForInstance[ctkDICOMItem::TagKeyStripped(DCM_PatientName)];
  this->UpdatedPatientCompositeIDs.insert(ctkDICOMDatabase::compositePatientID(patientID, patientsName, patientsBirthDate));
}

//------------------------------------------------------------------------------
void ctkDICOMDisplayedFieldGeneratorLastStudyDateRule::startUpdate()
{
  this->UpdatedPatientCompositeIDs.clear();
}

//------------------------------------------------------------------------------
void ctkDICOMDisplayedFieldGeneratorLastStudyDateRule::endUpdate(
  QMap<QString, QMap<QString, QString> > &displayedFieldsMapSeries,
  QMap<QString, QMap<QString, QString> > &displayedFieldsMapStudy,
  QMap<QString, QMap<QString, QString> > &displayedFieldsMapPatient)
{
  // Update last study date for each updated patient
  foreach (QString compositeID, this->UpdatedPatientCompositeIDs)
  {
    // Calculate and set last study date for the patient
    QMap<QString, QString> displayedFieldsForCurrentPatient = displayedFieldsMapPatient[compositeID];
    int patientUID = displayedFieldsForCurrentPatient["UID"].toInt();
    QSqlQuery numberOfStudiesQuery(this->DICOMDatabase->database());
    numberOfStudiesQuery.prepare("SELECT MAX(StudyDate) FROM Studies WHERE PatientsUID = ? ;");
    numberOfStudiesQuery.addBindValue(patientUID);
    if (!numberOfStudiesQuery.exec())
    {
      qCritical() << Q_FUNC_INFO << "SQLITE ERROR: " << numberOfStudiesQuery.lastError().driverText();
      continue;
    }

    numberOfStudiesQuery.first();
    QDate lastStudyDate = QDate::fromString(numberOfStudiesQuery.value(0).toString(), "yyyyMMdd");
    if (!lastStudyDate.isValid())
    {
      // The "yyyyMMdd" format should be the only one occurring because the StudyDate SQL field is DATE type,
      // which is expressed in that format. Still, especially when adding a new series in a database with
      // existing display fields (as opposed to re-generating all of them when the DisplayedFieldsUpdatedTimestamp
      // is NULL overall), this other format with dashes appears. Handling it here is a workaround for this case.
      lastStudyDate = QDate::fromString(numberOfStudiesQuery.value(0).toString(), "yyyy-MM-dd");
    }

    displayedFieldsForCurrentPatient["DisplayedLastStudyDate"] = lastStudyDate.toString("yyyy-MM-dd");
    displayedFieldsMapPatient[compositeID] = displayedFieldsForCurrentPatient;
  }
}
