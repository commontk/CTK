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

#ifndef __ctkDICOMDisplayedFieldGeneratorLastStudyDateRule_h
#define __ctkDICOMDisplayedFieldGeneratorLastStudyDateRule_h

// Qt includes
#include <QSet>
#include <QStringList>

#include "ctkDICOMDisplayedFieldGeneratorAbstractRule.h"

/// \ingroup DICOM_Core
///
/// Rule for generating last study date for patients that belong to the newly added instances
class CTK_DICOM_CORE_EXPORT ctkDICOMDisplayedFieldGeneratorLastStudyDateRule : public ctkDICOMDisplayedFieldGeneratorAbstractRule
{
public:
  /// Constructor
  explicit ctkDICOMDisplayedFieldGeneratorLastStudyDateRule();

  /// Get name of rule
  QString name()const override;

  /// Clone displayed field generator rule. Override to return a new instance of the rule sub-class
  ctkDICOMDisplayedFieldGeneratorAbstractRule* clone() override;

  /// Specify list of DICOM tags required by the rule. These tags will be included in the tag cache
  QStringList getRequiredDICOMTags() override;

  /// Generate displayed fields for a certain instance based on its cached tags
  /// The way these generated fields will be used is defined by \sa mergeDisplayedFieldsForInstance
  void getDisplayedFieldsForInstance(
    const QMap<QString, QString> &cachedTagsForInstance, QMap<QString, QString> &displayedFieldsForCurrentSeries,
    QMap<QString, QString> &displayedFieldsForCurrentStudy, QMap<QString, QString> &displayedFieldsForCurrentPatient ) override;

  /// Start updating displayed fields (reset counters, etc.). No-op by default.
  void startUpdate() override;

  /// End updating displayed fields (accumulate stored variables, compute final result, etc.). No-op by default.
  /// Has a chance to update any field in the series, study, or patient field maps, based on
  /// the maps themselves or the database.
  void endUpdate(QMap<QString, QMap<QString, QString> > &displayedFieldsMapSeries,
                 QMap<QString, QMap<QString, QString> > &displayedFieldsMapStudy,
                 QMap<QString, QMap<QString, QString> > &displayedFieldsMapPatient) override;

protected:
  /// Composite IDs (containing PatientID, PatientName, PatientBirthDate) of patients that contain instances of which displayed fields are updated in this run.
  QSet<QString> UpdatedPatientCompositeIDs;
};

#endif
