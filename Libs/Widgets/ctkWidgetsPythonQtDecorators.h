/*=auto=========================================================================

 Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) 
 All Rights Reserved.

 See Doc/copyright/copyright.txt
 or http://www.slicer.org/copyright/copyright.txt for details.

 Program:   3D Slicer

=========================================================================auto=*/

#ifndef __ctkWidgetsPythonQtDecorators_h
#define __ctkWidgetsPythonQtDecorators_h

// Qt includes
#include <QObject>

// PythonQt includes
#include <PythonQt.h>

// CTK includes
#include <ctkWorkflowWidgetStep.h>

#include "ctkWidgetsExport.h"

// NOTE:
//
// For decorators it is assumed that the methods will never be called
// with the self argument as NULL.  The self argument is the first argument
// for non-static methods.
//

class CTK_WIDGETS_EXPORT ctkWidgetsPythonQtDecorators : public QObject
{
  Q_OBJECT
public:

  ctkWidgetsPythonQtDecorators()
    {
    PythonQt::self()->addParentClass("ctkWorkflowWidgetStep", "ctkWorkflowStep",
                                     PythonQtUpcastingOffset<ctkWorkflowWidgetStep,ctkWorkflowStep>());
    }

public slots:

  bool hasCreateUserInterfaceCommand(ctkWorkflowWidgetStep* step)const
    {
    return step->hasCreateUserInterfaceCommand();
    }

  void setHasCreateUserInterfaceCommand(
    ctkWorkflowWidgetStep* step, bool newHasCreateUserInterfaceCommand)
    {
    step->setHasCreateUserInterfaceCommand(newHasCreateUserInterfaceCommand);
    }

  bool hasShowUserInterfaceCommand(ctkWorkflowWidgetStep* step)const
    {
    return step->hasShowUserInterfaceCommand();
    }

  void setHasShowUserInterfaceCommand(
    ctkWorkflowWidgetStep* step, bool newHasShowUserInterfaceCommand)
    {
    step->setHasShowUserInterfaceCommand(newHasShowUserInterfaceCommand);
    }
};

#endif
