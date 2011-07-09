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

// CTK includes
#include "ctkWorkflowWidgetStepPlugin.h"
#include "ctkWorkflowWidgetStep.h"

//-----------------------------------------------------------------------------
ctkWorkflowWidgetStepPlugin::ctkWorkflowWidgetStepPlugin(QObject *newParent)
        : QObject(newParent)
{

}

//-----------------------------------------------------------------------------
QWidget *ctkWorkflowWidgetStepPlugin::createWidget(QWidget *newParent)
{
  ctkWorkflowWidgetStep* _widget = new ctkWorkflowWidgetStep(newParent);
  return _widget;
}

//-----------------------------------------------------------------------------
QString ctkWorkflowWidgetStepPlugin::domXml() const
{
  return "<widget class=\"ctkWorkflowWidgetStep\" \
          name=\"WorkflowWidgetStep\">\n"
          " <property name=\"geometry\">\n"
          "  <rect>\n"
          "   <x>0</x>\n"
          "   <y>0</y>\n"
          "   <width>200</width>\n"
          "   <height>200</height>\n"
          "  </rect>\n"
          " </property>\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkWorkflowWidgetStepPlugin::icon() const
{
  return QIcon(":/Icons/widget.png");
}

//-----------------------------------------------------------------------------
QString ctkWorkflowWidgetStepPlugin::includeFile() const
{
  return "ctkWorkflowWidgetStep.h";
}

//-----------------------------------------------------------------------------
bool ctkWorkflowWidgetStepPlugin::isContainer() const
{
  return true;
}

//-----------------------------------------------------------------------------
QString ctkWorkflowWidgetStepPlugin::name() const
{
  return "ctkWorkflowWidgetStep";
}
