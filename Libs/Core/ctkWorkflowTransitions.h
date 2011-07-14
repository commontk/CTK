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

#ifndef __ctkWorkflowTransition_h
#define __ctkWorkflowTransition_h

// Qt includes
#include <QEvent>
#include <QAbstractTransition>
#include <QString>

// CTK includes
#include "ctkCoreExport.h"

/// \ingroup Core
/// \brief Custom transitions for use with ctkWorkflow.
///
/// ctkWorkflowIntrastepTransition: for transition between states of the same step.  The transition
/// occurs only when the transition event's EventTransitionType matches the transition's
/// TransitionType).
///
/// ctkWorkflowInterstepTransition: for transition between states of different steps.  The
/// transition occurs only when the transition event's EventTransitionType matches the transition's
/// TransitionType and the transition event's EventId matches the transition's Id.

//-----------------------------------------------------------------------------
struct CTK_CORE_EXPORT ctkWorkflowIntrastepTransitionEvent : public QEvent
{

  /// EventTransitionType is the value of a transition event, used to conditionally trigger transitions
  ctkWorkflowIntrastepTransitionEvent(int newTransitionType)
    : QEvent(QEvent::Type(getWorkflowIntrastepTransitionEventType())),
      EventTransitionType(newTransitionType){}

  /// Reserve a custom event type, ensuring that we are not re-using an
  /// event type that was previously used
  static inline int getWorkflowIntrastepTransitionEventType()
  {
    static int workflowIntrastepTransitionEventType = QEvent::registerEventType(QEvent::User+1);
    return workflowIntrastepTransitionEventType;
  }

  int EventTransitionType;
};

//-----------------------------------------------------------------------------
/// \ingroup Core
class CTK_CORE_EXPORT ctkWorkflowIntrastepTransition : public QAbstractTransition
{
  Q_OBJECT

public:

  enum IntrastepTransitionType
  {
    ValidationTransition = 0,
    ValidationFailedTransition
  };

  ctkWorkflowIntrastepTransition(IntrastepTransitionType newTransitionType)
    : TransitionType(newTransitionType){}

  IntrastepTransitionType transitionType() {return this->TransitionType;}

protected:
  virtual bool eventTest(QEvent* e)
  {
    // check the event type
    if (e->type() != ctkWorkflowIntrastepTransitionEvent::getWorkflowIntrastepTransitionEventType())
      {
      return false;
      }

    // check the event value (i.e. the TransitionType)
    ctkWorkflowIntrastepTransitionEvent* workflowEvent = static_cast<ctkWorkflowIntrastepTransitionEvent*>(e);

    return (this->TransitionType == workflowEvent->EventTransitionType);
  }

  void onTransition(QEvent*){}

private:
  IntrastepTransitionType TransitionType;

};

//-----------------------------------------------------------------------------
/// \ingroup Core
struct CTK_CORE_EXPORT ctkWorkflowInterstepTransitionEvent : public QEvent
{

  /// EventTransitionType is the value of a transition event, used to conditionally trigger transitions
  ctkWorkflowInterstepTransitionEvent(int newTransitionType)
    : QEvent(QEvent::Type(getWorkflowInterstepTransitionEventType())),
      EventTransitionType(newTransitionType){}
  ctkWorkflowInterstepTransitionEvent(int newTransitionType, const QString& newId)
    : QEvent(QEvent::Type(getWorkflowInterstepTransitionEventType())),
    EventTransitionType(newTransitionType),
    EventId(newId){}

  /// Reserve a custom event type, ensuring that we are not re-using an
  /// event type that was previously used
  static inline int getWorkflowInterstepTransitionEventType()
  {
    static int workflowInterstepTransitionEventType = QEvent::registerEventType(QEvent::User+1);
    return workflowInterstepTransitionEventType;
  }

  int     EventTransitionType;
  QString EventId;
};

//-----------------------------------------------------------------------------
/// \ingroup Core
class CTK_CORE_EXPORT ctkWorkflowInterstepTransition : public QAbstractTransition
{
  Q_OBJECT
  Q_ENUMS(InterstepTransitionType)

public:

  enum InterstepTransitionType
  {
    TransitionToNextStep = 0,
    TransitionToPreviousStep,
    StartingWorkflow,
    StoppingWorkflow,
    TransitionToPreviousStartingStepAfterSuccessfulGoToFinishStep
  };

  ctkWorkflowInterstepTransition(InterstepTransitionType newTransitionType)
    : TransitionType(newTransitionType){}
  ctkWorkflowInterstepTransition(InterstepTransitionType newTransitionType, const QString& newId)
    : TransitionType(newTransitionType),
    Id(newId) {}

  InterstepTransitionType transitionType() {return this->TransitionType;}
  QString& id() {return this->Id;}

protected:
  virtual bool eventTest(QEvent* e)
  {
    // check the event type
    if (e->type() != ctkWorkflowInterstepTransitionEvent::getWorkflowInterstepTransitionEventType())
      {
      return false;
      }

    // check the event value (i.e. the TransitionType)
    ctkWorkflowInterstepTransitionEvent* workflowEvent = static_cast<ctkWorkflowInterstepTransitionEvent*>(e);

    return (this->TransitionType == workflowEvent->EventTransitionType
            && this->Id == workflowEvent->EventId); 
  }

  void onTransition(QEvent*){}

private:
  InterstepTransitionType TransitionType;
  QString                 Id;

};

#endif
