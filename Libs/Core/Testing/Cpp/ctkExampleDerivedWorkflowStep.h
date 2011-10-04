/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

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

#ifndef __ctkExampleDerivedWorkflowStep_h
#define __ctkExampleDerivedWorkflowStep_h

// CTK includes
#include "ctkPimpl.h"
#include "ctkWorkflowStep.h"
#include "ctkWorkflowTransitions.h"

class ctkExampleDerivedWorkflowStepPrivate;

///
/// ctkExampleDerivedWorkflowStep is an example custom step created by
/// deriving ctkWorkflowStep and re-implementing validate(const QString&),
/// onEntry() and onExit().

class ctkExampleDerivedWorkflowStep : public ctkWorkflowStep
{

public:

  typedef ctkWorkflowStep Superclass;
  explicit ctkExampleDerivedWorkflowStep(const QString& newId);
  virtual ~ctkExampleDerivedWorkflowStep();

  /// Get the values for the counters of the number of times we have
  /// run the onEntry() and onExit() functions
  virtual int numberOfTimesRanOnEntry()const;
  virtual int numberOfTimesRanOnExit()const;

  virtual void validate(const QString& desiredBranchId = QString());

  /// Increments the counter numberOfTimesRanOnEntry
  virtual void onEntry(const ctkWorkflowStep* comingFrom,
                       const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType);

  /// Increments the counter numberOfTimesRanOnExit
  virtual void onExit(const ctkWorkflowStep* goingTo,
                      const ctkWorkflowInterstepTransition::InterstepTransitionType transitionType);

protected:
  QScopedPointer<ctkExampleDerivedWorkflowStepPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkExampleDerivedWorkflowStep);
  Q_DISABLE_COPY(ctkExampleDerivedWorkflowStep);

};

#endif
