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

// Qt includes
#include <QMap>
#include <QDebug>

// CTK includes
#include "ctkWorkflowAbstractPagedWidget.h"
#include "ctkWorkflowStep.h"
#include "ctkWorkflowWidgetStep.h"
#include "ctkWorkflowWidget.h"
#include "ctkWorkflowGroupBox.h"
#include "ctkWorkflowButtonBoxWidget.h"

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
class ctkWorkflowAbstractPagedWidgetPrivate
{
  Q_DECLARE_PUBLIC(ctkWorkflowAbstractPagedWidget);
protected:
  ctkWorkflowAbstractPagedWidget* const q_ptr;
public:
  ctkWorkflowAbstractPagedWidgetPrivate(ctkWorkflowAbstractPagedWidget& object);
  ~ctkWorkflowAbstractPagedWidgetPrivate();

  // Maintain maps to associate:
  // - ctkWorkflowSteps with their page indices
  // - ctkWorkflowSteps with their labels
  // - page indices with their ctkWorkflowGroupBoxes
  QMap<ctkWorkflowStep*, int> StepToIndexMap;
  QMap<ctkWorkflowStep*, QString> StepToLabelMap;
  QMap<int, ctkWorkflowGroupBox*> IndexToGroupBoxMap;

  // Number of created pages
  int NumPages;

  ctkWorkflowGroupBox* GroupBoxShownPreviously;
  ctkWorkflowGroupBox* GroupBoxShownCurrently;

  // Create a new ctkWorkflowGroupBox based on the "recipe" provided by the superclass's
  // WorkflowGroupBox, and put it on a new page
  void createNewWorkflowGroupBox(int index);
};

// --------------------------------------------------------------------------
// ctkWorkflowAbstractPagedWidgetPrivate methods

//---------------------------------------------------------------------------
ctkWorkflowAbstractPagedWidgetPrivate::ctkWorkflowAbstractPagedWidgetPrivate(ctkWorkflowAbstractPagedWidget& object)
  :q_ptr(&object)
{
  this->GroupBoxShownPreviously = 0;
  this->GroupBoxShownCurrently = 0;
}

//---------------------------------------------------------------------------
ctkWorkflowAbstractPagedWidgetPrivate::~ctkWorkflowAbstractPagedWidgetPrivate()
{
}

//---------------------------------------------------------------------------
void ctkWorkflowAbstractPagedWidgetPrivate::createNewWorkflowGroupBox(int index)
{
  Q_Q(ctkWorkflowAbstractPagedWidget);

  ctkWorkflowGroupBox* recipe = q->workflowGroupBox();
  Q_ASSERT(recipe);

  ctkWorkflowGroupBox* newGroupBox = new ctkWorkflowGroupBox(q);
  newGroupBox->setPreText(recipe->preText());
  newGroupBox->setPostText(recipe->postText());
  newGroupBox->setHideWidgetsOfNonCurrentSteps(recipe->hideWidgetsOfNonCurrentSteps());

  q->createNewPage(newGroupBox);
  this->IndexToGroupBoxMap[index] = newGroupBox;
}

// --------------------------------------------------------------------------
// ctkWorkflowAbstractPagedWidgetMethods

// --------------------------------------------------------------------------
ctkWorkflowAbstractPagedWidget::ctkWorkflowAbstractPagedWidget(QWidget* _parent) : Superclass(_parent)
  , d_ptr(new ctkWorkflowAbstractPagedWidgetPrivate(*this))
{
  Q_D(ctkWorkflowAbstractPagedWidget);
  d->NumPages = 0;
}

// --------------------------------------------------------------------------
ctkWorkflowAbstractPagedWidget::~ctkWorkflowAbstractPagedWidget()
{
}

// --------------------------------------------------------------------------
void ctkWorkflowAbstractPagedWidget::associateStepWithPage(ctkWorkflowStep* step, int index)
{
  Q_D(ctkWorkflowAbstractPagedWidget);

  if (index < 0)
    {
    qWarning() << "Cannot associate step with a page of index -1";
    return;
    }

  if (step)
    {
    d->StepToIndexMap[step] = index;

    if (!d->IndexToGroupBoxMap.contains(index))
      {
      d->IndexToGroupBoxMap[index] = 0;
      }
    }
}

// --------------------------------------------------------------------------
void ctkWorkflowAbstractPagedWidget::associateStepWithLabel(ctkWorkflowStep* step, QString label)
{
  Q_D(ctkWorkflowAbstractPagedWidget);

  if (step)
    {
    d->StepToLabelMap[step] = label;
    }
}

// --------------------------------------------------------------------------
void ctkWorkflowAbstractPagedWidget::associateStepWithPage(ctkWorkflowStep* step, int index, QString label)
{
  this->associateStepWithPage(step, index);
  this->associateStepWithLabel(step, label);
}

// --------------------------------------------------------------------------
ctkWorkflowGroupBox* ctkWorkflowAbstractPagedWidget::workflowGroupBox(ctkWorkflowStep* step)const
{
  Q_D(const ctkWorkflowAbstractPagedWidget);

  if (d->StepToIndexMap.contains(step))
    {
    int index = d->StepToIndexMap[step];
    Q_ASSERT(d->IndexToGroupBoxMap.contains(index));
    return d->IndexToGroupBoxMap[index];
    }
  else
    {
    return 0;
    }
}

// --------------------------------------------------------------------------
void ctkWorkflowAbstractPagedWidget::updateStepUI(ctkWorkflowStep* currentStep)
{
  Q_D(ctkWorkflowAbstractPagedWidget);
  Q_ASSERT(currentStep);
  Q_ASSERT(this->workflowGroupBox());

  // Create layout and client area if this is our first time here
  if (!this->clientArea())
    {
    QVBoxLayout* layout = new QVBoxLayout();
    this->setLayout(layout);
    this->initClientArea();
    layout->addWidget(this->clientArea());
      
    if (this->showButtonBoxWidget())
      {
      layout->addWidget(this->buttonBoxWidget());
      }
      
    layout->setContentsMargins(0,0,0,0);
    
    // Use the superclass's groupBox as the first page, since we already have it
    d->IndexToGroupBoxMap[0] = this->workflowGroupBox();
    this->createNewPage(d->IndexToGroupBoxMap[0]);
    }

  // If the user hasn't previously associated the step with a page, then add a new page
  if (!d->StepToIndexMap.contains(currentStep))
    {
    this->associateStepWithPage(currentStep, d->NumPages++);
    }

  int index = d->StepToIndexMap[currentStep];
  
  // Create new page(s) if necessary
  // (i.e. if user specifies steps only for indices 3 and 4 with associateStepWithPage, we need to
  // create pages 0,1,2)
  if (!d->IndexToGroupBoxMap[index])
    {
    for (int i = 0; i <= index; i++)
      {
      if (!d->IndexToGroupBoxMap[i])
        {
        d->createNewWorkflowGroupBox(i);
        }
      }
    }

  // If the user hasn't previously associated the step with a label, then add a blank label
  if (!d->StepToLabelMap.contains(currentStep))
    {
    this->associateStepWithLabel(currentStep, "");
    }
  QString label = d->StepToLabelMap[currentStep];

  ctkWorkflowGroupBox* groupBox = d->IndexToGroupBoxMap[index];
  Q_ASSERT(groupBox);

  d->GroupBoxShownPreviously = d->GroupBoxShownCurrently;
  d->GroupBoxShownCurrently = groupBox;

  if (d->GroupBoxShownPreviously)
    {
    d->GroupBoxShownPreviously->updateGroupBox(0);
    }

  groupBox->updateGroupBox(currentStep);

  this->showPage(groupBox, label);
}
