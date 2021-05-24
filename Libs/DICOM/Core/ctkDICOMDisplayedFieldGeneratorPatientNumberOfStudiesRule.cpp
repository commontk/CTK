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

#include "ctkDICOMDisplayedFieldGeneratorPatientNumberOfStudiesRule.h"

// ctkDICOM includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMItem.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

//------------------------------------------------------------------------------
ctkDICOMDisplayedFieldGeneratorPatientNumberOfStudiesRule::ctkDICOMDisplayedFieldGeneratorPatientNumberOfStudiesRule()
{
}

//------------------------------------------------------------------------------
QString ctkDICOMDisplayedFieldGeneratorPatientNumberOfStudiesRule::name()const
{
  return "PatientNumberOfStudies";
}

//------------------------------------------------------------------------------
ctkDICOMDisplayedFieldGeneratorAbstractRule* ctkDICOMDisplayedFieldGeneratorPatientNumberOfStudiesRule::clone()
{
  return new ctkDICOMDisplayedFieldGeneratorPatientNumberOfStudiesRule();
}

//------------------------------------------------------------------------------
QStringList ctkDICOMDisplayedFieldGeneratorPatientNumberOfStudiesRule::getRequiredDICOMTags()
{
  QStringList requiredTags;

  requiredTags << dicomTagToString(DCM_SOPInstanceUID);

  requiredTags << dicomTagToString(DCM_PatientID);
  requiredTags << dicomTagToString(DCM_PatientName);
  requiredTags << dicomTagToString(DCM_PatientBirthDate);

  requiredTags << dicomTagToString(DCM_StudyDate);

  return requiredTags;
}

//------------------------------------------------------------------------------
void ctkDICOMDisplayedFieldGeneratorPatientNumberOfStudiesRule::getDisplayedFieldsForInstance(
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
void ctkDICOMDisplayedFieldGeneratorPatientNumberOfStudiesRule::startUpdate()
{
  this->UpdatedPatientCompositeIDs.clear();
}

//------------------------------------------------------------------------------
void ctkDICOMDisplayedFieldGeneratorPatientNumberOfStudiesRule::endUpdate(
  QMap<QString, QMap<QString, QString> > &displayedFieldsMapSeries,
  QMap<QString, QMap<QString, QString> > &displayedFieldsMapStudy,
  QMap<QString, QMap<QString, QString> > &displayedFieldsMapPatient)
{
  // Update number of studies date for each updated patient
  foreach (QString compositeID, this->UpdatedPatientCompositeIDs)
  {
    QMap<QString, QString> displayedFieldsForCurrentPatient = displayedFieldsMapPatient[compositeID];
    int patientUID = displayedFieldsForCurrentPatient["UID"].toInt();
    QSqlQuery numberOfStudiesQuery(this->DICOMDatabase->database());
    numberOfStudiesQuery.prepare("SELECT COUNT(*) FROM Studies WHERE PatientsUID = ? ;");
    numberOfStudiesQuery.addBindValue(patientUID);
    if (!numberOfStudiesQuery.exec())
    {
      qCritical() << Q_FUNC_INFO << "SQLITE ERROR: " << numberOfStudiesQuery.lastError().driverText();
      continue;
    }

    numberOfStudiesQuery.first();
    int currentNumberOfStudies = numberOfStudiesQuery.value(0).toInt();

    displayedFieldsForCurrentPatient["DisplayedNumberOfStudies"] = QString::number(currentNumberOfStudies);
    displayedFieldsMapPatient[compositeID] = displayedFieldsForCurrentPatient;
  }
}
