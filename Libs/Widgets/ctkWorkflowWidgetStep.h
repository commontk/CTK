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

#ifndef __ctkWorkflowWidgetStep_h
#define __ctkWorkflowWidgetStep_h 

// QT includes
#include <QWidget>
#include <QIcon>
#include <QBoxLayout>
#include <QFlags>

// CTK includes
#include "ctkPimpl.h"
#include "ctkWorkflowStep.h"
#include "ctkWorkflowTransitions.h"
#include "ctkWorkflowAbstractWidgetStep.h"
#include "ctkWidgetsExport.h"

class ctkWorkflowWidgetStepPrivate;
class ctkWorkflowButtonBoxWidget;
class ctkWorkflowGroupBox;
class ctkWorkflow;

///
/// \brief A concrete implementation of ctkWorkflowAbstractWidgetStep that derives from QWidget.

class ctkWorkflowWidgetStepPlugin;

class CTK_WIDGETS_EXPORT ctkWorkflowWidgetStep : public QWidget,
                                                 public ctkWorkflowAbstractWidgetStep
{ 
  Q_OBJECT
  Q_PROPERTY(QString id READ id WRITE setId)
  Q_PROPERTY(QIcon icon READ icon WRITE setIcon)
  Q_PROPERTY(QString backButtonText READ backButtonText WRITE setBackButtonText)
  Q_PROPERTY(QString nextButtonText READ nextButtonText WRITE setNextButtonText)
  Q_FLAGS(ButtonBoxHintForPlugin)
  Q_PROPERTY(ButtonBoxHintForPlugin buttonBoxHints READ buttonBoxHintsForPlugin WRITE setButtonBoxHintsForPlugin)

public:

  typedef ctkWorkflowAbstractWidgetStep Superclass;
  explicit ctkWorkflowWidgetStep(ctkWorkflow* newWorkflow, const QString& newId, QWidget* newParent = 0);
  explicit ctkWorkflowWidgetStep(QWidget* newParent = 0);
  virtual ~ctkWorkflowWidgetStep();

  virtual QWidget* stepArea();

  //-----------------------------------------------------------------------------
  // To have ButtonBoxHints displayed as a propery in QtDesigner ... 
  // Since ctkWorkflowAbstractWidgetStep is not a QObject, there is no way to 
  // add ctkWorkflowAbstractWidgetStep::ButtonBoxHints to the meta object system using 
  // the QFLAGS(ctkWorkflowAbstractWidgetStep::ButtonBoxHints)
  // The following enums, setter and getter serve as proxy for qtdesigner plugin
  // TODO Ideally, the following code should be private ..
  enum ButtonBoxHintForPlugin {
    qNoHints = 0x0,
    qBackButtonHidden = 0x1,
    qBackButtonDisabled = 0x2,
    qNextButtonHidden = 0x4,
    qNextButtonDisabled = 0x8,
    qButtonBoxHidden = 0x10
  };
  Q_DECLARE_FLAGS(ButtonBoxHintsForPlugin, ButtonBoxHintForPlugin)
  void setButtonBoxHintsForPlugin(ButtonBoxHintsForPlugin buttonBoxHints);
  ButtonBoxHintsForPlugin buttonBoxHintsForPlugin()const;
  //-----------------------------------------------------------------------------

protected:

  virtual void createUserInterface(){}

protected:
  QScopedPointer<ctkWorkflowWidgetStepPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkWorkflowWidgetStep);
  Q_DISABLE_COPY(ctkWorkflowWidgetStep);

};

Q_DECLARE_OPERATORS_FOR_FLAGS(ctkWorkflowWidgetStep::ButtonBoxHintsForPlugin)

#endif

