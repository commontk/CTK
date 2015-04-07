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

#ifndef __ctkWidgetsPythonQtDecorators_h
#define __ctkWidgetsPythonQtDecorators_h

// PythonQt includes
#include <PythonQt.h>

// CTK includes
#include <ctkErrorLogModel.h>
#include <ctkTransferFunctionBarsItem.h>
#include <ctkTransferFunctionControlPointsItem.h>
#include <ctkTransferFunctionGradientItem.h>
#include <ctkWorkflowWidgetStep.h>

// NOTE:
//
// For decorators it is assumed that the methods will never be called
// with the self argument as NULL.  The self argument is the first argument
// for non-static methods.
//

/// \ingroup Widgets
class ctkWidgetsPythonQtDecorators : public QObject
{
  Q_OBJECT
public:

  ctkWidgetsPythonQtDecorators()
    {
    PythonQt::self()->addParentClass("ctkWorkflowWidgetStep", "ctkWorkflowStep",
                                     PythonQtUpcastingOffset<ctkWorkflowWidgetStep,ctkWorkflowStep>());
    }

public Q_SLOTS:

  // ctkWorkflowWidgetStep

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

  // ctkErrorLogLevel

  QString static_ctkErrorLogLevel_logLevelAsString(ctkErrorLogLevel::LogLevel logLevel)
    {
    return ctkErrorLogLevel::logLevelAsString(logLevel);
    }

  // ctkTransferFunctionBarsItem

  ctkTransferFunctionBarsItem* new_ctkTransferFunctionBarsItem(QGraphicsItem* parent = 0)
    {
    return new ctkTransferFunctionBarsItem(parent);
    }

  ctkTransferFunctionBarsItem* new_ctkTransferFunctionBarsItem(
      ctkTransferFunction* transferFunc,
      QGraphicsItem* parent = 0)
    {
    return new ctkTransferFunctionBarsItem(transferFunc, parent);
    }

  void delete_ctkTransferFunctionBarsItem(ctkTransferFunctionBarsItem* obj)
    {
    delete obj;
    }

  // ctkTransferFunctionControlPointsItem

  ctkTransferFunctionControlPointsItem* new_ctkTransferFunctionControlPointsItem(
      QGraphicsItem* parent = 0)
    {
    return new ctkTransferFunctionControlPointsItem(parent);
    }

  ctkTransferFunctionControlPointsItem* new_ctkTransferFunctionControlPointsItem(
      ctkTransferFunction* transferFunc,
      QGraphicsItem* parent = 0)
    {
    return new ctkTransferFunctionControlPointsItem(transferFunc, parent);
    }

  void delete_ctkTransferFunctionControlPointsItem(ctkTransferFunctionControlPointsItem* obj)
    {
    delete obj;
    }

  // ctkTransferFunctionGradientItem

  ctkTransferFunctionGradientItem* new_ctkTransferFunctionGradientItem(
      QGraphicsItem* parent = 0)
    {
    return new ctkTransferFunctionGradientItem(parent);
    }

  ctkTransferFunctionGradientItem* new_ctkTransferFunctionGradientItem(
      ctkTransferFunction* transferFunc,
      QGraphicsItem* parent = 0)
    {
    return new ctkTransferFunctionGradientItem(transferFunc, parent);
    }

  void delete_ctkTransferFunctionGradientItem(ctkTransferFunctionGradientItem* obj)
    {
    delete obj;
    }

};

//-----------------------------------------------------------------------------
/// \ingroup Widgets
void initCTKWidgetsPythonQtDecorators()
{
  // HACK: Since the CMake based light wrapping only consider class name matching the
  //       filename where the class is defined, let's explicitly register ctkErrorLogLevel
  //       so that the log level QFlags are exposed to python.
  PythonQt::self()->registerClass(&ctkErrorLogLevel::staticMetaObject, "CTKCore");

  PythonQt::self()->registerClass(&ctkTransferFunctionBarsItem::staticMetaObject, "CTKWidgets");
  PythonQt::self()->registerClass(&ctkTransferFunctionControlPointsItem::staticMetaObject, "CTKWidgets");
  PythonQt::self()->registerClass(&ctkTransferFunctionGradientItem::staticMetaObject, "CTKWidgets");

  PythonQt::self()->addDecorators(new ctkWidgetsPythonQtDecorators);
}

#endif
