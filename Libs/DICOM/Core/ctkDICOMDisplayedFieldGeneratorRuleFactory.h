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

#ifndef __ctkDICOMDisplayedFieldGeneratorRuleFactory_h_
#define __ctkDICOMDisplayedFieldGeneratorRuleFactory_h_

#include "ctkDICOMCoreExport.h" 

// Qt includes
#include <QObject>
#include <QList>

class ctkDICOMDatabase;
class ctkDICOMDisplayedFieldGeneratorAbstractRule;
class ctkDICOMDisplayedFieldGeneratorRuleFactoryCleanup;

/// \ingroup SlicerRt_QtModules_Segmentations
/// \class ctkDICOMDisplayedFieldGeneratorRuleFactory
/// \brief Singleton class managing displayed field generator rules
class CTK_DICOM_CORE_EXPORT ctkDICOMDisplayedFieldGeneratorRuleFactory : public QObject
{
  Q_OBJECT

public:
  /// Instance getter for the singleton class
  /// \return Instance object
  Q_INVOKABLE static ctkDICOMDisplayedFieldGeneratorRuleFactory* instance();

public:
  /// Register a displayed field generator rule. The factory takes ownership of the object (caller must not delete it).
  /// \return True if rule is registered successfully, false otherwise
  Q_INVOKABLE bool registerDisplayedFieldGeneratorRule(ctkDICOMDisplayedFieldGeneratorAbstractRule* rule);

  /// Return the list of displayed field generator rules registered.
  Q_INVOKABLE const QList<ctkDICOMDisplayedFieldGeneratorAbstractRule*>& displayedFieldGeneratorRules()
    { return this->DisplayedFieldGeneratorRules; }

  /// Return a list with the copies of displayed field generator rules that are enabled according to the database.
  /// \param database DICOM database in which the DisplayedFieldGeneratorRules table is used to decide if a rule is
  ///   enabled or not. If the table itself or an entry with a particular rule name is missing then the rule is treated as enabled.
  Q_INVOKABLE QList<ctkDICOMDisplayedFieldGeneratorAbstractRule*> copyEnabledDisplayedFieldGeneratorRules(ctkDICOMDatabase* database);

signals:
  void displayedFieldGeneratorRuleRegistered();
  void displayedFieldGeneratorRuleUnregistered();

protected:
  QList<ctkDICOMDisplayedFieldGeneratorAbstractRule*> DisplayedFieldGeneratorRules;

private:
  /// Allows cleanup of the singleton at application exit
  static void cleanup();

private:
  ctkDICOMDisplayedFieldGeneratorRuleFactory(QObject* parent=nullptr);
  ~ctkDICOMDisplayedFieldGeneratorRuleFactory() override;

  Q_DISABLE_COPY(ctkDICOMDisplayedFieldGeneratorRuleFactory);
  friend class ctkDICOMDisplayedFieldGeneratorRuleFactoryCleanup;
  friend class PythonQtWrapper_ctkDICOMDisplayedFieldGeneratorRuleFactory; // Allow Python wrapping without enabling direct instantiation

private:
  /// Instance of the singleton
  static ctkDICOMDisplayedFieldGeneratorRuleFactory* Instance;
};

#endif // __ctkDICOMDisplayedFieldGeneratorRuleFactory_h_
