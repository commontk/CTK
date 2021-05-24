/*==============================================================================

  Library:   CTK

  Copyright (c) Pixel Medical Inc. 2021

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  ==============================================================================*/

#include "ctkDICOMDisplayedFieldGeneratorRuleFactory.h"

#include "ctkDICOMDatabase.h"
#include "ctkDICOMDisplayedFieldGeneratorDefaultRule.h"
#include "ctkDICOMDisplayedFieldGeneratorRadiotherapySeriesDescriptionRule.h"
#include "ctkDICOMDisplayedFieldGeneratorLastStudyDateRule.h"
#include "ctkDICOMDisplayedFieldGeneratorSeriesImageCountRule.h"
#include "ctkDICOMDisplayedFieldGeneratorStudyNumberOfSeriesRule.h"
#include "ctkDICOMDisplayedFieldGeneratorPatientNumberOfStudiesRule.h"

// Qt includes
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

//----------------------------------------------------------------------------
ctkDICOMDisplayedFieldGeneratorRuleFactory *ctkDICOMDisplayedFieldGeneratorRuleFactory::Instance = nullptr;

//----------------------------------------------------------------------------
class ctkDICOMDisplayedFieldGeneratorRuleFactoryCleanup
{
public:
  inline void use() { }

  ~ctkDICOMDisplayedFieldGeneratorRuleFactoryCleanup()
  {
    if (ctkDICOMDisplayedFieldGeneratorRuleFactory::Instance)
    {
      ctkDICOMDisplayedFieldGeneratorRuleFactory::cleanup();
    }
  }
};

//-----------------------------------------------------------------------------
static ctkDICOMDisplayedFieldGeneratorRuleFactoryCleanup ctkDICOMDisplayedFieldGeneratorRuleFactoryCleanupGlobal;

//-----------------------------------------------------------------------------
ctkDICOMDisplayedFieldGeneratorRuleFactory* ctkDICOMDisplayedFieldGeneratorRuleFactory::instance()
{
  if (!ctkDICOMDisplayedFieldGeneratorRuleFactory::Instance)
  {
    ctkDICOMDisplayedFieldGeneratorRuleFactoryCleanupGlobal.use();
    ctkDICOMDisplayedFieldGeneratorRuleFactory::Instance = new ctkDICOMDisplayedFieldGeneratorRuleFactory();

    // Pre-register commonly used rules
    ctkDICOMDisplayedFieldGeneratorRuleFactory::Instance->registerDisplayedFieldGeneratorRule(
      new ctkDICOMDisplayedFieldGeneratorDefaultRule);
    ctkDICOMDisplayedFieldGeneratorRuleFactory::Instance->registerDisplayedFieldGeneratorRule(
      new ctkDICOMDisplayedFieldGeneratorRadiotherapySeriesDescriptionRule);
    ctkDICOMDisplayedFieldGeneratorRuleFactory::Instance->registerDisplayedFieldGeneratorRule(
      new ctkDICOMDisplayedFieldGeneratorLastStudyDateRule);
    ctkDICOMDisplayedFieldGeneratorRuleFactory::Instance->registerDisplayedFieldGeneratorRule(
      new ctkDICOMDisplayedFieldGeneratorSeriesImageCountRule);
    ctkDICOMDisplayedFieldGeneratorRuleFactory::Instance->registerDisplayedFieldGeneratorRule(
      new ctkDICOMDisplayedFieldGeneratorStudyNumberOfSeriesRule);
    ctkDICOMDisplayedFieldGeneratorRuleFactory::Instance->registerDisplayedFieldGeneratorRule(
      new ctkDICOMDisplayedFieldGeneratorPatientNumberOfStudiesRule);

  }

  // Return the instance
  return ctkDICOMDisplayedFieldGeneratorRuleFactory::Instance;
}

//-----------------------------------------------------------------------------
void ctkDICOMDisplayedFieldGeneratorRuleFactory::cleanup()
{
  if (ctkDICOMDisplayedFieldGeneratorRuleFactory::Instance)
  {
    delete ctkDICOMDisplayedFieldGeneratorRuleFactory::Instance;
    ctkDICOMDisplayedFieldGeneratorRuleFactory::Instance = nullptr;
  }
}

//-----------------------------------------------------------------------------
ctkDICOMDisplayedFieldGeneratorRuleFactory::ctkDICOMDisplayedFieldGeneratorRuleFactory(QObject* parent)
  : QObject(parent)
{
  this->DisplayedFieldGeneratorRules.clear();
}

//-----------------------------------------------------------------------------
ctkDICOMDisplayedFieldGeneratorRuleFactory::~ctkDICOMDisplayedFieldGeneratorRuleFactory()
{
  foreach(auto rule, this->DisplayedFieldGeneratorRules)
  {
    delete rule;
  }

  this->DisplayedFieldGeneratorRules.clear();
}

//-----------------------------------------------------------------------------
bool ctkDICOMDisplayedFieldGeneratorRuleFactory::registerDisplayedFieldGeneratorRule(ctkDICOMDisplayedFieldGeneratorAbstractRule* rule)
{
  // Check for null
  if (!rule)
  {
    qCritical() << Q_FUNC_INFO << ": Invalid displayed field generator rule";
    return false;
  }
  if (rule->name().isEmpty())
  {
    qCritical() << Q_FUNC_INFO << ": Displayed field generator rule cannot be registered with empty name";
    return false;
  }

  // Check for already registered rule
  foreach(auto &registeredRule, this->DisplayedFieldGeneratorRules)
  {
    if (registeredRule->name() == rule->name())
    {
      qWarning() << Q_FUNC_INFO << ": displayed field generator rule type " << rule->name() << " is already registered.";
      return false;
    }
  }

  // Register the displayed field generator rule
  this->DisplayedFieldGeneratorRules.append(rule);

  // Emit registered signal
  emit displayedFieldGeneratorRuleRegistered();

  return true;
}

//---------------------------------------------------------------------------
QList<ctkDICOMDisplayedFieldGeneratorAbstractRule*>
ctkDICOMDisplayedFieldGeneratorRuleFactory::copyEnabledDisplayedFieldGeneratorRules(ctkDICOMDatabase* database)
{
  QList<ctkDICOMDisplayedFieldGeneratorAbstractRule*> copiedRules;

  if (!database)
  {
    qCritical() << Q_FUNC_INFO << " failed: DICOM database needs to be given";
    return copiedRules;
  }

  // Get names of disabled displayed field generator rules from database.
  // We get the disabled rules instead of the enabled ones so that if a rule does not have an entry in the table or
  // the table is missing, then the rules are treated as enabled by default.
  QList<QString> disabledRuleNames;
  QSqlQuery disabledDisplayedFieldGeneratorRulesQuery(database->database());
  disabledDisplayedFieldGeneratorRulesQuery.prepare("SELECT Name FROM DisplayedFieldGeneratorRules WHERE Enabled = 0;");
  if (disabledDisplayedFieldGeneratorRulesQuery.exec())
  {
    while (disabledDisplayedFieldGeneratorRulesQuery.next())
    {
      disabledRuleNames << disabledDisplayedFieldGeneratorRulesQuery.value(0).toString();
    }
    disabledDisplayedFieldGeneratorRulesQuery.finish();
  }

  foreach(ctkDICOMDisplayedFieldGeneratorAbstractRule* registeredRule, this->DisplayedFieldGeneratorRules)
  {
    // If rule is explicitly enabled or there is no such information then accept it
    if (!disabledRuleNames.contains(registeredRule->name()))
    {
      ctkDICOMDisplayedFieldGeneratorAbstractRule* clonedRule = registeredRule->clone();
      if (!clonedRule)
      {
        // Make sure we don't put a null pointer in the rule list
        qCritical() << Q_FUNC_INFO << " failed to clone displayed field generator rule: " << registeredRule->name();
        continue;
      }
      copiedRules << clonedRule;
    }
  }

  return copiedRules;
}
