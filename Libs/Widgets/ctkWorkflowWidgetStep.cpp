/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  =========================================================================*/

// Qt includes
#include <QObject>
#include <QWidget>
#include <QList>
#include <QDebug>
#include <QIcon>

// CTK includes
#include "ctkWorkflowWidgetStep.h"
#include "ctkWorkflowWidget.h"
#include "ctkWorkflow.h"
#include "ctkWorkflowButtonBoxWidget.h"
#include "ctkLogger.h"

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
static ctkLogger logger("org.commontk.libs.widgets.ctkWorkflowWidgetStep");
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class ctkWorkflowWidgetStepPrivate
{
public:
  ctkWorkflowWidgetStepPrivate();
  ~ctkWorkflowWidgetStepPrivate(){};
  ctkWorkflowWidgetStep::ButtonBoxHintForPlugin ButtonBoxHintsForPlugin;
};

//-----------------------------------------------------------------------------
// ctkWorkflowWidgetStepPrivate methods

//-----------------------------------------------------------------------------
ctkWorkflowWidgetStepPrivate::ctkWorkflowWidgetStepPrivate()
{
}

//-----------------------------------------------------------------------------
// ctkWorkflowWidgetStep methods

//-----------------------------------------------------------------------------
ctkWorkflowWidgetStep::ctkWorkflowWidgetStep(ctkWorkflow* newWorkflow, 
  const QString& newId, QWidget* newParent) : QWidget(newParent), ctkWorkflowAbstractWidgetStep(newWorkflow, newId)
  , d_ptr(new ctkWorkflowWidgetStepPrivate)
{
  Q_D(ctkWorkflowWidgetStep);
  d->ButtonBoxHintsForPlugin = ctkWorkflowWidgetStep::qNoHints;
}

//-----------------------------------------------------------------------------
ctkWorkflowWidgetStep::ctkWorkflowWidgetStep(QWidget* newParent) : QWidget(newParent), ctkWorkflowAbstractWidgetStep(0, QString())
  , d_ptr(new ctkWorkflowWidgetStepPrivate)
{
  Q_D(ctkWorkflowWidgetStep);
  d->ButtonBoxHintsForPlugin = ctkWorkflowWidgetStep::qNoHints;
}

//-----------------------------------------------------------------------------
ctkWorkflowWidgetStep::~ctkWorkflowWidgetStep()
{
}

//-----------------------------------------------------------------------------
QWidget* ctkWorkflowWidgetStep::stepArea()
{
  return this;
}

//-----------------------------------------------------------------------------
ctkWorkflowWidgetStep::ButtonBoxHintsForPlugin ctkWorkflowWidgetStep::buttonBoxHintsForPlugin() const
{
  return ctkWorkflowWidgetStep::ButtonBoxHintsForPlugin(QFlag(this->Superclass::buttonBoxHints()));
}

//-----------------------------------------------------------------------------
void ctkWorkflowWidgetStep::setButtonBoxHintsForPlugin(
  ctkWorkflowWidgetStep::ButtonBoxHintsForPlugin newButtonBoxHints)
{
  this->Superclass::setButtonBoxHints(QFlag(newButtonBoxHints));
}
