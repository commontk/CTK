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

#include "ctkDICOMDisplayedFieldGeneratorSeriesImageCountRule.h"

// ctkDICOM includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMItem.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

//------------------------------------------------------------------------------
ctkDICOMDisplayedFieldGeneratorSeriesImageCountRule::ctkDICOMDisplayedFieldGeneratorSeriesImageCountRule()
{
}

//------------------------------------------------------------------------------
QString ctkDICOMDisplayedFieldGeneratorSeriesImageCountRule::name()const
{
  return "SeriesImageCount";
}

//------------------------------------------------------------------------------
ctkDICOMDisplayedFieldGeneratorAbstractRule* ctkDICOMDisplayedFieldGeneratorSeriesImageCountRule::clone()
{
  return new ctkDICOMDisplayedFieldGeneratorSeriesImageCountRule();
}

//------------------------------------------------------------------------------
QStringList ctkDICOMDisplayedFieldGeneratorSeriesImageCountRule::getRequiredDICOMTags()
{
  QStringList requiredTags;

  requiredTags << dicomTagToString(DCM_SeriesInstanceUID);

  return requiredTags;
}

//------------------------------------------------------------------------------
void ctkDICOMDisplayedFieldGeneratorSeriesImageCountRule::getDisplayedFieldsForInstance(
  const QMap<QString, QString> &cachedTagsForInstance, QMap<QString, QString> &displayedFieldsForCurrentSeries,
  QMap<QString, QString> &displayedFieldsForCurrentStudy, QMap<QString, QString> &displayedFieldsForCurrentPatient )
{
  // Store series identifiers for the instances that of which displayed fields are updated in this run.
  this->UpdatedSeriesInstanceUIDs.insert(cachedTagsForInstance[dicomTagToString(DCM_SeriesInstanceUID)]);
}

//------------------------------------------------------------------------------
void ctkDICOMDisplayedFieldGeneratorSeriesImageCountRule::startUpdate()
{
  this->UpdatedSeriesInstanceUIDs.clear();
}

//------------------------------------------------------------------------------
void ctkDICOMDisplayedFieldGeneratorSeriesImageCountRule::endUpdate(
  QMap<QString, QMap<QString, QString> > &displayedFieldsMapSeries,
  QMap<QString, QMap<QString, QString> > &displayedFieldsMapStudy,
  QMap<QString, QMap<QString, QString> > &displayedFieldsMapPatient)
{
  // Update image count for each updated series
  foreach (QString currentSeriesInstanceUid, this->UpdatedSeriesInstanceUIDs)
  {
    QSqlQuery countQuery(this->DICOMDatabase->database());
    countQuery.prepare("SELECT COUNT(*) FROM Images WHERE SeriesInstanceUID = ? ;");
    countQuery.addBindValue(currentSeriesInstanceUid);
    if (!countQuery.exec())
    {
      qCritical() << Q_FUNC_INFO << "SQLITE ERROR: " << countQuery.lastError().driverText();
      continue;
    }

    countQuery.first();
    int currentCount = countQuery.value(0).toInt();

    QMap<QString, QString> displayedFieldsForCurrentSeries = displayedFieldsMapSeries[currentSeriesInstanceUid];
    displayedFieldsForCurrentSeries["DisplayedCount"] = QString::number(currentCount);
    displayedFieldsMapSeries[currentSeriesInstanceUid] = displayedFieldsForCurrentSeries;
  }
}
