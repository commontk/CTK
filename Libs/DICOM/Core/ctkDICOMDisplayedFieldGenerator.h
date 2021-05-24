/*=========================================================================

  Library:   CTK

  Copyright (c) PerkLab 2013

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

#ifndef __ctkDICOMDisplayedFieldGenerator_h
#define __ctkDICOMDisplayedFieldGenerator_h

// Qt includes
#include <QObject>
#include <QStringList>

#include "ctkDICOMCoreExport.h"

class ctkDICOMDisplayedFieldGeneratorPrivate;
class ctkDICOMDisplayedFieldGeneratorAbstractRule;
class ctkDICOMDatabase;

/// \ingroup DICOM_Core
///
/// \brief Generates displayable data fields from DICOM tags
/// 
/// The \sa updateDisplayedFieldsForInstance function is called from the DICOM database when update of the
/// displayed fields is needed.
/// 
/// Displayed fields are determined by the rules, subclasses of ctkDICOMDisplayedFieldGeneratorAbstractRule.
/// The rules need to be registered to take part of the generation. When updating the displayed fields,
/// every rule defines the fields it is responsible for using the cached DICOM tags in the database.
/// Tags can be requested to be cached in the rules from the getRequiredDICOMTags function. After the fields
/// are defined in each rule, the results are merged together. The merging rules are also defined in the
/// rule classes. Each field can requested to be merged with "expect same value", which uses the only
/// non-empty value and throws a warning if conflicting values are encountered, or with "concatenate",
/// which simply concatenates the displayed field values together.
///
class CTK_DICOM_CORE_EXPORT ctkDICOMDisplayedFieldGenerator : public QObject
{
  Q_OBJECT
public:
  explicit ctkDICOMDisplayedFieldGenerator(QObject* parent);
  virtual ~ctkDICOMDisplayedFieldGenerator();

  /// Set DICOM database
  Q_INVOKABLE void setDatabase(ctkDICOMDatabase* database);

  /// Collect the DICOM tags required by all the registered rules
  Q_INVOKABLE QStringList getRequiredTags();

  /// Update displayed fields for an instance, invoking all registered rules
  Q_INVOKABLE void updateDisplayedFieldsForInstance(const QString& sopInstanceUID,
                                                    const QMap<QString, QString> &cachedTags,
                                                    QMap<QString, QString> &displayedFieldsForCurrentSeries,
                                                    QMap<QString, QString> &displayedFieldsForCurrentStudy,
                                                    QMap<QString, QString> &displayedFieldsForCurrentPatient);

  /// Start updating displayed fields (reset counters, etc.).
  /// Calls function with same name of all registered rules.
  Q_INVOKABLE void startUpdate();

  /// End updating displayed fields (accumulate stored variables, compute final result, etc.).
  /// Has a chance to update any field in the series, study, or patient field maps, based on
  /// the maps themselves or the database.
  /// Calls function with same name of all registered rules.
  Q_INVOKABLE void endUpdate(QMap<QString, QMap<QString, QString> > &displayedFieldsMapSeries,
                             QMap<QString, QMap<QString, QString> > &displayedFieldsMapStudy,
                             QMap<QString, QMap<QString, QString> > &displayedFieldsMapPatient);

protected:
  QScopedPointer<ctkDICOMDisplayedFieldGeneratorPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMDisplayedFieldGenerator);
  Q_DISABLE_COPY(ctkDICOMDisplayedFieldGenerator);
};

#endif
