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
  and development was supported by the Center for Intelligent Image-guided Interventions (CI3).

=========================================================================*/

// CTK includes
#include "ctkDICOMServerNodeWidget2Plugin.h"
#include "ctkDICOMServerNodeWidget2.h"

//-----------------------------------------------------------------------------
ctkDICOMServerNodeWidget2Plugin::ctkDICOMServerNodeWidget2Plugin(QObject* pluginParent)
  : QObject(pluginParent)
{
}

//-----------------------------------------------------------------------------
QWidget *ctkDICOMServerNodeWidget2Plugin::createWidget(QWidget *parentForWidget)
{
  ctkDICOMServerNodeWidget2* newWidget = new ctkDICOMServerNodeWidget2(parentForWidget);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkDICOMServerNodeWidget2Plugin::domXml() const
{
  return "<widget class=\"ctkDICOMServerNodeWidget2\" \
          name=\"ServerNode\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkDICOMServerNodeWidget2Plugin::icon() const
{
  return QIcon(":/Icons/widget.png");
}

//-----------------------------------------------------------------------------
QString ctkDICOMServerNodeWidget2Plugin::includeFile() const
{
  return "ctkDICOMServerNodeWidget2.h";
}

//-----------------------------------------------------------------------------
bool ctkDICOMServerNodeWidget2Plugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkDICOMServerNodeWidget2Plugin::name() const
{
  return "ctkDICOMServerNodeWidget2";
}
