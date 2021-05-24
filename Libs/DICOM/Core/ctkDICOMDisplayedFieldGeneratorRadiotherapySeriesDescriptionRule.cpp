/*=========================================================================

  Library:   CTK

  Copyright (c) PerkLab 2018

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

#include "ctkDICOMDisplayedFieldGeneratorRadiotherapySeriesDescriptionRule.h"

//------------------------------------------------------------------------------
ctkDICOMDisplayedFieldGeneratorRadiotherapySeriesDescriptionRule::ctkDICOMDisplayedFieldGeneratorRadiotherapySeriesDescriptionRule()
  : EmptySeriesDescriptionRtPlan("Unnamed RT Plan")
  , EmptySeriesDescriptionRtStruct("Unnamed RT Structure Set")
  , EmptySeriesDescriptionRtImage("Unnamed RT Image")
{
}

//------------------------------------------------------------------------------
QString ctkDICOMDisplayedFieldGeneratorRadiotherapySeriesDescriptionRule::name()const
{
  return "RadiotherapySeriesDescription";
}

//------------------------------------------------------------------------------
ctkDICOMDisplayedFieldGeneratorAbstractRule* ctkDICOMDisplayedFieldGeneratorRadiotherapySeriesDescriptionRule::clone()
{
  return new ctkDICOMDisplayedFieldGeneratorRadiotherapySeriesDescriptionRule();
}

//------------------------------------------------------------------------------
QStringList ctkDICOMDisplayedFieldGeneratorRadiotherapySeriesDescriptionRule::getRequiredDICOMTags()
{
  QStringList requiredTags;

  requiredTags << dicomTagToString(DCM_Modality);
    
  requiredTags << dicomTagToString(DCM_RTPlanName);
  requiredTags << dicomTagToString(DCM_RTPlanLabel);    
  //requiredTags << dicomTagToString(DCM_RTPlanDate);
  //requiredTags << dicomTagToString(DCM_RTPlanTime);
  //requiredTags << dicomTagToString(DCM_RTPlanDescription);    
    
  requiredTags << dicomTagToString(DCM_StructureSetName);
  requiredTags << dicomTagToString(DCM_StructureSetLabel);
  //requiredTags << dicomTagToString(DCM_StructureSetDescription);
  //requiredTags << dicomTagToString(DCM_StructureSetDate);
  //requiredTags << dicomTagToString(DCM_StructureSetTime);
          
  requiredTags << dicomTagToString(DCM_RTImageName);
  requiredTags << dicomTagToString(DCM_RTImageLabel);
  requiredTags << dicomTagToString(DCM_RTImageDescription);
          
  return requiredTags;
}

//------------------------------------------------------------------------------
void ctkDICOMDisplayedFieldGeneratorRadiotherapySeriesDescriptionRule::registerEmptyFieldNames(
  QMap<QString, QString> emptyFieldsDisplaySeries,
  QMap<QString, QString> emptyFieldsDisplayStudies,
  QMap<QString, QString> emptyFieldsDisplayPatients )
{
  Q_UNUSED(emptyFieldsDisplayStudies);
  Q_UNUSED(emptyFieldsDisplayPatients);

  emptyFieldsDisplaySeries.insertMulti("SeriesDescription", this->EmptySeriesDescriptionRtPlan);
  emptyFieldsDisplaySeries.insertMulti("SeriesDescription", this->EmptySeriesDescriptionRtStruct);
  emptyFieldsDisplaySeries.insertMulti("SeriesDescription", this->EmptySeriesDescriptionRtImage);
}

//------------------------------------------------------------------------------
void ctkDICOMDisplayedFieldGeneratorRadiotherapySeriesDescriptionRule::getDisplayedFieldsForInstance(
  const QMap<QString, QString> &cachedTagsForInstance, QMap<QString, QString> &displayedFieldsForCurrentSeries,
  QMap<QString, QString> &displayedFieldsForCurrentStudy, QMap<QString, QString> &displayedFieldsForCurrentPatient )
{
  QString modality = cachedTagsForInstance[dicomTagToString(DCM_Modality)];
  if (!modality.compare("RTPLAN"))
  {
    if (!cachedTagsForInstance[dicomTagToString(DCM_RTPlanName)].isEmpty())
    {
      displayedFieldsForCurrentSeries["SeriesDescription"] = cachedTagsForInstance[dicomTagToString(DCM_RTPlanName)];
    }
    else if (!cachedTagsForInstance[dicomTagToString(DCM_RTPlanLabel)].isEmpty())
    {
      displayedFieldsForCurrentSeries["SeriesDescription"] = cachedTagsForInstance[dicomTagToString(DCM_RTPlanLabel)];
    }
    else
    {
      displayedFieldsForCurrentSeries["SeriesDescription"] = QString(this->EmptySeriesDescriptionRtPlan);
    }
  }
  else if (!modality.compare("RTSTRUCT"))
  {
    if (!cachedTagsForInstance[dicomTagToString(DCM_StructureSetName)].isEmpty())
    {
      displayedFieldsForCurrentSeries["SeriesDescription"] = cachedTagsForInstance[dicomTagToString(DCM_StructureSetName)];
    }
    else if (!cachedTagsForInstance[dicomTagToString(DCM_StructureSetLabel)].isEmpty())
    {
      displayedFieldsForCurrentSeries["SeriesDescription"] = cachedTagsForInstance[dicomTagToString(DCM_StructureSetLabel)];
    }
    else
    {
      displayedFieldsForCurrentSeries["SeriesDescription"] = QString(this->EmptySeriesDescriptionRtStruct);
    }
  }
  else if (!modality.compare("RTIMAGE"))
  {
    if (!cachedTagsForInstance[dicomTagToString(DCM_RTImageName)].isEmpty())
    {
      displayedFieldsForCurrentSeries["SeriesDescription"] = cachedTagsForInstance[dicomTagToString(DCM_RTImageName)];
    }
    else if (!cachedTagsForInstance[dicomTagToString(DCM_RTImageLabel)].isEmpty())
    {
      displayedFieldsForCurrentSeries["SeriesDescription"] = cachedTagsForInstance[dicomTagToString(DCM_RTImageLabel)];
    }
    else
    {
      displayedFieldsForCurrentSeries["SeriesDescription"] = QString(this->EmptySeriesDescriptionRtImage);
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMDisplayedFieldGeneratorRadiotherapySeriesDescriptionRule::mergeDisplayedFieldsForInstance(
  const QMap<QString, QString> &initialFieldsSeries, const QMap<QString, QString> &initialFieldsStudy, const QMap<QString, QString> &initialFieldsPatient,
  const QMap<QString, QString> &newFieldsSeries, const QMap<QString, QString> &newFieldsStudy, const QMap<QString, QString> &newFieldsPatient,
  QMap<QString, QString> &mergedFieldsSeries, QMap<QString, QString> &mergedFieldsStudy, QMap<QString, QString> &mergedFieldsPatient,
  const QMap<QString, QString> &emptyFieldsSeries, const QMap<QString, QString> &emptyFieldsStudy, const QMap<QString, QString> &emptyFieldsPatient
  )
{
  Q_UNUSED(initialFieldsStudy);
  Q_UNUSED(initialFieldsPatient);
  Q_UNUSED(newFieldsStudy);
  Q_UNUSED(newFieldsPatient);
  Q_UNUSED(mergedFieldsStudy);
  Q_UNUSED(mergedFieldsPatient);
  Q_UNUSED(emptyFieldsStudy);
  Q_UNUSED(emptyFieldsPatient);
  mergeConcatenate("SeriesDescription", initialFieldsSeries, newFieldsSeries, mergedFieldsSeries, emptyFieldsSeries);
}
