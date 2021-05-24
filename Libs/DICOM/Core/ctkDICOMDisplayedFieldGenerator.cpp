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
#include "ctkDICOMDatabase.h"
#include "ctkDICOMDisplayedFieldGenerator.h"
#include "ctkDICOMDisplayedFieldGenerator_p.h"
#include "ctkDICOMDisplayedFieldGeneratorAbstractRule.h"
#include "ctkDICOMDisplayedFieldGeneratorRuleFactory.h"

//------------------------------------------------------------------------------
static ctkLogger logger("org.commontk.dicom.DICOMDisplayedFieldGenerator" );
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// ctkDICOMDisplayedFieldGeneratorPrivate methods

//------------------------------------------------------------------------------
ctkDICOMDisplayedFieldGeneratorPrivate::ctkDICOMDisplayedFieldGeneratorPrivate(ctkDICOMDisplayedFieldGenerator& o)
  : q_ptr(&o)
  , Database(nullptr)
{
}

//------------------------------------------------------------------------------
ctkDICOMDisplayedFieldGeneratorPrivate::~ctkDICOMDisplayedFieldGeneratorPrivate() = default;

//------------------------------------------------------------------------------
void ctkDICOMDisplayedFieldGeneratorPrivate::setupEnabledDisplayedFieldGeneratorRules()
{
  if (!this->Database)
  {
    qCritical() << Q_FUNC_INFO << " failed: DICOM database needs to be set";
    return;
  }
  if (!this->GeneratorRules.isEmpty())
  {
    qWarning() << Q_FUNC_INFO << " : Generator rules have not been cleared before new update session";

    this->clearDisplayedFieldGeneratorRules();
  }

  // Instantiate enabled rules registered in factory
  this->GeneratorRules =
    ctkDICOMDisplayedFieldGeneratorRuleFactory::instance()->copyEnabledDisplayedFieldGeneratorRules(this->Database);

  // Setup generator rules used in this update session
  foreach(ctkDICOMDisplayedFieldGeneratorAbstractRule* rule, this->GeneratorRules)
  {
    // Set database
    rule->setDatabase(this->Database);
    // Register empty field names
    rule->registerEmptyFieldNames(
      this->EmptyFieldNamesSeries, this->EmptyFieldNamesStudies, this->EmptyFieldNamesPatients );
  }

  //TODO: Process Options field in DisplayedFieldGeneratorRules table when has a concrete use.
}

//------------------------------------------------------------------------------
void ctkDICOMDisplayedFieldGeneratorPrivate::clearDisplayedFieldGeneratorRules()
{
  // Delete generator rules and clear container
  foreach(ctkDICOMDisplayedFieldGeneratorAbstractRule* rule, this->GeneratorRules)
  {
    delete rule;
  }
  this->GeneratorRules.clear();

  // Clear empty field names registered previously by the rules
  this->EmptyFieldNamesSeries.clear();
  this->EmptyFieldNamesStudies.clear();
  this->EmptyFieldNamesPatients.clear();
}


//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ctkDICOMDisplayedFieldGenerator methods

//------------------------------------------------------------------------------
ctkDICOMDisplayedFieldGenerator::ctkDICOMDisplayedFieldGenerator(QObject *parent)
  : d_ptr(new ctkDICOMDisplayedFieldGeneratorPrivate(*this))
{
  Q_UNUSED(parent);
}

//------------------------------------------------------------------------------
ctkDICOMDisplayedFieldGenerator::~ctkDICOMDisplayedFieldGenerator()
{
  Q_D(ctkDICOMDisplayedFieldGenerator);
  d->clearDisplayedFieldGeneratorRules();
}

//------------------------------------------------------------------------------
QStringList ctkDICOMDisplayedFieldGenerator::getRequiredTags()
{
  Q_D(ctkDICOMDisplayedFieldGenerator);

  QStringList requiredTags;
  foreach(ctkDICOMDisplayedFieldGeneratorAbstractRule* rule, d->GeneratorRules)
  {
    requiredTags << rule->getRequiredDICOMTags();
  }
  requiredTags.removeDuplicates();

  return requiredTags;
}

//------------------------------------------------------------------------------
void ctkDICOMDisplayedFieldGenerator::updateDisplayedFieldsForInstance(
  const QString& sopInstanceUID, const QMap<QString, QString> &cachedTagsForInstance,
  QMap<QString, QString> &displayedFieldsForCurrentSeries,
  QMap<QString, QString> &displayedFieldsForCurrentStudy,
  QMap<QString, QString> &displayedFieldsForCurrentPatient )
{
  Q_D(ctkDICOMDisplayedFieldGenerator);
  Q_UNUSED(sopInstanceUID);

  QMap<QString, QString> newFieldsSeries;
  QMap<QString, QString> newFieldsStudy;
  QMap<QString, QString> newFieldsPatient;
  foreach(ctkDICOMDisplayedFieldGeneratorAbstractRule* rule, d->GeneratorRules)
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
void ctkDICOMDisplayedFieldGenerator::startUpdate()
{
  Q_D(ctkDICOMDisplayedFieldGenerator);

  // Re-initialize generator rules in case new ones have been registered or the rule options changed in the database
  d->clearDisplayedFieldGeneratorRules();
  d->setupEnabledDisplayedFieldGeneratorRules();

  foreach(ctkDICOMDisplayedFieldGeneratorAbstractRule* rule, d->GeneratorRules)
  {
    rule->startUpdate();
  }
}

//------------------------------------------------------------------------------
void ctkDICOMDisplayedFieldGenerator::endUpdate(QMap<QString, QMap<QString, QString> > &displayedFieldsMapSeries,
                                                QMap<QString, QMap<QString, QString> > &displayedFieldsMapStudy,
                                                QMap<QString, QMap<QString, QString> > &displayedFieldsMapPatient)
{
  Q_D(ctkDICOMDisplayedFieldGenerator);

  foreach(ctkDICOMDisplayedFieldGeneratorAbstractRule* rule, d->GeneratorRules)
  {
    rule->endUpdate(displayedFieldsMapSeries, displayedFieldsMapStudy, displayedFieldsMapPatient);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMDisplayedFieldGenerator::setDatabase(ctkDICOMDatabase* database)
{
  Q_D(ctkDICOMDisplayedFieldGenerator);

  if (d->Database == database)
  {
    return;
  }

  d->Database = database;

  d->setupEnabledDisplayedFieldGeneratorRules();
}
