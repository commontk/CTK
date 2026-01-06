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

  This file was originally developed by Davide Punzo, punzodavide@hotmail.it,
  and development was supported by the Program for Intelligent Image-Guided Interventions (PI3).

=========================================================================*/

// CTK includes
#include "ctkDICOMStudyListViewPlugin.h"
#include "ctkDICOMStudyListView.h"

//-----------------------------------------------------------------------------
ctkDICOMStudyListViewPlugin::ctkDICOMStudyListViewPlugin(QObject* pluginParent)
  : QObject(pluginParent)
{
}

//-----------------------------------------------------------------------------
QWidget *ctkDICOMStudyListViewPlugin::createWidget(QWidget *parentForWidget)
{
  ctkDICOMStudyListView* newWidget = new ctkDICOMStudyListView(parentForWidget);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkDICOMStudyListViewPlugin::domXml() const
{
  return "<widget class=\"ctkDICOMStudyListView\" \
          name=\"StudyListView\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkDICOMStudyListViewPlugin::icon() const
{
  return QIcon(":/Icons/listview.png");
}

//-----------------------------------------------------------------------------
QString ctkDICOMStudyListViewPlugin::includeFile() const
{
  return "ctkDICOMStudyListView.h";
}

//-----------------------------------------------------------------------------
bool ctkDICOMStudyListViewPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkDICOMStudyListViewPlugin::name() const
{
  return "ctkDICOMStudyListView";
}
