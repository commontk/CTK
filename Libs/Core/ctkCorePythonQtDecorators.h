/*=auto=========================================================================

 Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) 
 All Rights Reserved.

 See Doc/copyright/copyright.txt
 or http://www.slicer.org/copyright/copyright.txt for details.

 Program:   3D Slicer

=========================================================================auto=*/

#ifndef __ctkCorePythonQtDecorators_h
#define __ctkCorePythonQtDecorators_h

// Qt includes
#include <QObject>

// PythonQt includes
#include <PythonQt.h>

// CTK includes
#include <ctkWorkflowStep.h>
#include <ctkWorkflowTransitions.h>

#include "ctkCoreExport.h"

// NOTE:
//
// For decorators it is assumed that the methods will never be called
// with the self argument as NULL.  The self argument is the first argument
// for non-static methods.
//

/// \ingroup Core
class CTK_CORE_EXPORT ctkCorePythonQtDecorators : public QObject
{
  Q_OBJECT
public:

  ctkCorePythonQtDecorators()
    {
    PythonQt::self()->registerCPPClass("ctkWorkflowStep", 0, "CTKCore");
    PythonQt::self()->registerClass(&ctkWorkflowInterstepTransition::staticMetaObject, "CTKCore");
    }

public slots:

  //
  // ctkWorkflowStep
  //

  ctkWorkflowStep* new_ctkWorkflowStep()
    {
    return new ctkWorkflowStep();
    }

  ctkWorkflowStep* new_ctkWorkflowStep(ctkWorkflow* newWorkflow, const QString& newId = QString())
    {
    return new ctkWorkflowStep(newWorkflow, newId);
    }

  void delete_ctkWorkflowStep(ctkWorkflowStep * step)
    {
    delete step;
    }
    
  ctkWorkflow* workflow(ctkWorkflowStep* step)const
    {
    return step->workflow();
    }

  QString id(ctkWorkflowStep* step)const
    {
    return step->id();
    }

  void setId(ctkWorkflowStep* step, const QString& newId)const
    {
    step->setId(newId);
    }

  QString name(ctkWorkflowStep* step)const
    {
    return step->name();
    }
    
  void setName(ctkWorkflowStep* step, const QString& newName)
    {
    step->setName(newName);
    }

  QString description(ctkWorkflowStep* step)const
    {
    return step->description();
    }
    
  void setDescription(ctkWorkflowStep* step, const QString& newDescription)
    {
    step->setDescription(newDescription);
    }

  QString statusText(ctkWorkflowStep* step)const
    {
    return step->statusText();
    }

  bool hasValidateCommand(ctkWorkflowStep* step)const
    {
    return step->hasValidateCommand();
    }
    
  void setHasValidateCommand(ctkWorkflowStep* step, bool newHasValidateCommand)
    {
    step->setHasValidateCommand(newHasValidateCommand);
    }

  bool hasOnEntryCommand(ctkWorkflowStep* step)const
    {
    return step->hasOnEntryCommand();
    }
    
  void setHasOnEntryCommand(ctkWorkflowStep* step, bool newHasOnEntryCommand)
    {
    step->setHasOnEntryCommand(newHasOnEntryCommand);
    }

  bool hasOnExitCommand(ctkWorkflowStep* step)const
    {
    return step->hasOnExitCommand();
    }
    
  void setHasOnExitCommand(ctkWorkflowStep* step, bool newHasOnExitCommand)
    {
    step->setHasOnExitCommand(newHasOnExitCommand);
    }
  
  QObject* ctkWorkflowStepQObject(ctkWorkflowStep* step)
    {
    return step->ctkWorkflowStepQObject();
    }

  //
  // ctkWorkflowInterstepTransition
  //
  ctkWorkflowInterstepTransition* new_ctkWorkflowInterstepTransition(ctkWorkflowInterstepTransition::InterstepTransitionType newTransitionType)
    {
    return new ctkWorkflowInterstepTransition(newTransitionType);
    }
  
  ctkWorkflowInterstepTransition* new_ctkWorkflowInterstepTransition(ctkWorkflowInterstepTransition::InterstepTransitionType newTransitionType, const QString& newId)
    {
    return new ctkWorkflowInterstepTransition(newTransitionType, newId);
    }
    
  void delete_ctkWorkflowInterstepTransition(ctkWorkflowInterstepTransition * transition)
    {
    delete transition;
    }
};

#endif
