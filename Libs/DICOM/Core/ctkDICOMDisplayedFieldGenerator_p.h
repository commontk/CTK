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

#ifndef CTKDICOMDisplayedFieldGeneratorPrivate_H
#define CTKDICOMDisplayedFieldGeneratorPrivate_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QString>

#include "ctkDICOMDisplayedFieldGenerator.h"

class ctkDICOMDatabase;
class ctkDICOMDisplayedFieldGeneratorAbstractRule;

//------------------------------------------------------------------------------
class ctkDICOMDisplayedFieldGeneratorPrivate : public QObject
{
  Q_OBJECT

  Q_DECLARE_PUBLIC(ctkDICOMDisplayedFieldGenerator);

protected:
  ctkDICOMDisplayedFieldGenerator* const q_ptr;

public:
  ctkDICOMDisplayedFieldGeneratorPrivate(ctkDICOMDisplayedFieldGenerator&);
  ~ctkDICOMDisplayedFieldGeneratorPrivate();

  /// Instantiate and setup displayed field generator rules that are enabled according to the database.
  /// Populates \sa GeneratorRules list when update starts (\sa ctkDICOMDisplayedFieldGenerator::startUpdate).
  void setupEnabledDisplayedFieldGeneratorRules();

  /// Clear list of displayed field generator rules when update ends (\sa ctkDICOMDisplayedFieldGenerator::endUpdate).
  void clearDisplayedFieldGeneratorRules();

public:
  /// List of enabled generator rules.
  /// Populated on \sa ctkDICOMDisplayedFieldGenerator::startUpdate and cleared on \sa ctkDICOMDisplayedFieldGenerator::endUpdate
  QList<ctkDICOMDisplayedFieldGeneratorAbstractRule*> GeneratorRules;

  ctkDICOMDatabase* Database;

  QMap<QString, QString> EmptyFieldNamesPatients;
  QMap<QString, QString> EmptyFieldNamesStudies;
  QMap<QString, QString> EmptyFieldNamesSeries;
};


#endif // CTKDICOMDisplayedFieldGeneratorPrivate_H
