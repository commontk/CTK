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

#ifndef __ctkBranchingWorkflowStep_h
#define __ctkBranchingWorkflowStep_h

// CTK includes
#include "ctkWorkflowStep.h"
#include "ctkWorkflow.h"

//-----------------------------------------------------------------------------
class ctkBranchingWorkflowStep : public ctkWorkflowStep
{

public:

  typedef ctkWorkflowStep Superclass;
  explicit ctkBranchingWorkflowStep(const QString& newId) : Superclass(newId){};

  void setBranchId(const QString& newId)
  {
    this->branchId = newId;
  }

  virtual void validate(const QString& desiredBranchId = QString())
  {
    Q_UNUSED(desiredBranchId);
    this->validationComplete(true, this->branchId);
  }

private:
  QString branchId;

};

#endif
