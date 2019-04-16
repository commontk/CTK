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

// Qt includes
#include <QStringList>

// ctkDICOM includes
#include "ctkLogger.h"
#include "ctkDICOMDisplayedFieldGenerator.h"
#include "ctkDICOMDisplayedFieldGenerator_p.h"

#include "ctkDICOMDatabase.h"
#include "ctkDICOMDisplayedFieldGeneratorDefaultRule.h"
#include "ctkDICOMDisplayedFieldGeneratorRadiotherapySeriesDescriptionRule.h"

//------------------------------------------------------------------------------
static ctkLogger logger("org.commontk.dicom.DICOMDisplayedFieldGenerator" );
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// ctkDICOMDisplayedFieldGeneratorPrivate methods

//------------------------------------------------------------------------------
ctkDICOMDisplayedFieldGeneratorPrivate::ctkDICOMDisplayedFieldGeneratorPrivate(ctkDICOMDisplayedFieldGenerator& o)
  : q_ptr(&o)
  , Database(NULL)
{
  // register commonly used rules
  this->AllRules.append(new ctkDICOMDisplayedFieldGeneratorDefaultRule);
  this->AllRules.append(new ctkDICOMDisplayedFieldGeneratorRadiotherapySeriesDescriptionRule);

  foreach(ctkDICOMDisplayedFieldGeneratorAbstractRule* rule, this->AllRules)
  {
    rule->registerEmptyFieldNames(
      this->EmptyFieldNamesSeries, this->EmptyFieldNamesStudies, this->EmptyFieldNamesPatients );
  }
}

//------------------------------------------------------------------------------
ctkDICOMDisplayedFieldGeneratorPrivate::~ctkDICOMDisplayedFieldGeneratorPrivate()
{
  foreach(ctkDICOMDisplayedFieldGeneratorAbstractRule* rule, this->AllRules)
  {
    delete rule;
  }
  this->AllRules.clear();
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ctkDICOMDisplayedFieldGenerator methods

//------------------------------------------------------------------------------
ctkDICOMDisplayedFieldGenerator::ctkDICOMDisplayedFieldGenerator(QObject *parent):d_ptr(new ctkDICOMDisplayedFieldGeneratorPrivate(*this))
{
  Q_UNUSED(parent);
}

//------------------------------------------------------------------------------
ctkDICOMDisplayedFieldGenerator::~ctkDICOMDisplayedFieldGenerator()
{
}

//------------------------------------------------------------------------------
QStringList ctkDICOMDisplayedFieldGenerator::getRequiredTags()
{
  Q_D(ctkDICOMDisplayedFieldGenerator);

  QStringList requiredTags;
  foreach(ctkDICOMDisplayedFieldGeneratorAbstractRule* rule, d->AllRules)
  {
    requiredTags << rule->getRequiredDICOMTags();
  }

  // TODO: remove duplicates from requiredTags (maybe also sort)
  return requiredTags;
}

//------------------------------------------------------------------------------
void ctkDICOMDisplayedFieldGenerator::updateDisplayedFieldsForInstance( QString sopInstanceUID,
  QMap<QString, QString> &displayedFieldsForCurrentSeries, QMap<QString, QString> &displayedFieldsForCurrentStudy, QMap<QString, QString> &displayedFieldsForCurrentPatient )
{
  Q_D(ctkDICOMDisplayedFieldGenerator);

  QMap<QString, QString> cachedTagsForInstance;
  d->Database->getCachedTags(sopInstanceUID, cachedTagsForInstance);

  QMap<QString, QString> newFieldsSeries;
  QMap<QString, QString> newFieldsStudy;
  QMap<QString, QString> newFieldsPatient;
  foreach(ctkDICOMDisplayedFieldGeneratorAbstractRule* rule, d->AllRules)
  {
    QMap<QString, QString> initialFieldsSeries = displayedFieldsForCurrentSeries;
    QMap<QString, QString> initialFieldsStudy = displayedFieldsForCurrentStudy;
    QMap<QString, QString> initialFieldsPatient = displayedFieldsForCurrentPatient;

    rule->getDisplayedFieldsForInstance(cachedTagsForInstance, newFieldsSeries, newFieldsStudy, newFieldsPatient);

    rule->mergeDisplayedFieldsForInstance(
      initialFieldsSeries, initialFieldsStudy, initialFieldsPatient, // original DB contents
      newFieldsSeries, newFieldsStudy, newFieldsPatient, // new value
      displayedFieldsForCurrentSeries, displayedFieldsForCurrentStudy, displayedFieldsForCurrentPatient, // new DB contents
      d->EmptyFieldNamesSeries, d->EmptyFieldNamesStudies, d->EmptyFieldNamesPatients // empty field names defined by all the rules
    );
  }
}

//------------------------------------------------------------------------------
void ctkDICOMDisplayedFieldGenerator::setDatabase(ctkDICOMDatabase* database)
{
  Q_D(ctkDICOMDisplayedFieldGenerator);
  d->Database=database;
}

//------------------------------------------------------------------------------
void ctkDICOMDisplayedFieldGenerator::registerDisplayedFieldGeneratorRule(ctkDICOMDisplayedFieldGeneratorAbstractRule* rule)
{
  Q_D(ctkDICOMDisplayedFieldGenerator);
  d->AllRules.append(rule);
}
