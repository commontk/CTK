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

#include "ctkDICOMDisplayedFieldGeneratorStudyNumberOfSeriesRule.h"

// ctkDICOM includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMItem.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

//------------------------------------------------------------------------------
ctkDICOMDisplayedFieldGeneratorStudyNumberOfSeriesRule::ctkDICOMDisplayedFieldGeneratorStudyNumberOfSeriesRule()
{
}

//------------------------------------------------------------------------------
QString ctkDICOMDisplayedFieldGeneratorStudyNumberOfSeriesRule::name()const
{
  return "StudyNumberOfSeries";
}

//------------------------------------------------------------------------------
ctkDICOMDisplayedFieldGeneratorAbstractRule* ctkDICOMDisplayedFieldGeneratorStudyNumberOfSeriesRule::clone()
{
  return new ctkDICOMDisplayedFieldGeneratorStudyNumberOfSeriesRule();
}

//------------------------------------------------------------------------------
QStringList ctkDICOMDisplayedFieldGeneratorStudyNumberOfSeriesRule::getRequiredDICOMTags()
{
  QStringList requiredTags;

  requiredTags << dicomTagToString(DCM_StudyInstanceUID);

  return requiredTags;
}

//------------------------------------------------------------------------------
void ctkDICOMDisplayedFieldGeneratorStudyNumberOfSeriesRule::getDisplayedFieldsForInstance(
  const QMap<QString, QString> &cachedTagsForInstance, QMap<QString, QString> &displayedFieldsForCurrentSeries,
  QMap<QString, QString> &displayedFieldsForCurrentStudy, QMap<QString, QString> &displayedFieldsForCurrentPatient )
{
  // Store study identifiers for the instances that of which displayed fields are updated in this run.
  this->UpdatedStudyInstanceUIDs.insert(cachedTagsForInstance[dicomTagToString(DCM_StudyInstanceUID)]);
}

//------------------------------------------------------------------------------
void ctkDICOMDisplayedFieldGeneratorStudyNumberOfSeriesRule::startUpdate()
{
  this->UpdatedStudyInstanceUIDs.clear();
}

//------------------------------------------------------------------------------
void ctkDICOMDisplayedFieldGeneratorStudyNumberOfSeriesRule::endUpdate(
  QMap<QString, QMap<QString, QString> > &displayedFieldsMapSeries,
  QMap<QString, QMap<QString, QString> > &displayedFieldsMapStudy,
  QMap<QString, QMap<QString, QString> > &displayedFieldsMapPatient)
{
  // Update number of series for each updated study
  foreach (QString currentStudyInstanceUid, this->UpdatedStudyInstanceUIDs)
  {
    QSqlQuery numberOfSeriesQuery(this->DICOMDatabase->database());
    numberOfSeriesQuery.prepare("SELECT COUNT(*) FROM Series WHERE StudyInstanceUID = ? ;");
    numberOfSeriesQuery.addBindValue(currentStudyInstanceUid);
    if (!numberOfSeriesQuery.exec())
    {
      qCritical() << Q_FUNC_INFO << "SQLITE ERROR: " << numberOfSeriesQuery.lastError().driverText();
      continue;
    }

    numberOfSeriesQuery.first();
    int currentNumberOfSeries = numberOfSeriesQuery.value(0).toInt();

    QMap<QString, QString> displayedFieldsForCurrentStudy = displayedFieldsMapStudy[currentStudyInstanceUid];
    displayedFieldsForCurrentStudy["DisplayedNumberOfSeries"] = QString::number(currentNumberOfSeries);
    displayedFieldsMapStudy[currentStudyInstanceUid] = displayedFieldsForCurrentStudy;
  }
}
