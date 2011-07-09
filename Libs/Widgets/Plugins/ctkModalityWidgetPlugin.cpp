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
#include "ctkModalityWidgetPlugin.h"
#include "ctkModalityWidget.h"

//-----------------------------------------------------------------------------
ctkModalityWidgetPlugin::ctkModalityWidgetPlugin(QObject* parentObject)
  : QObject(parentObject)
{
}

//-----------------------------------------------------------------------------
QWidget* ctkModalityWidgetPlugin::createWidget(QWidget* parentWidget)
{
  ctkModalityWidget* newWidget = new ctkModalityWidget(parentWidget);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkModalityWidgetPlugin::domXml() const
{
  return "<widget class=\"ctkModalityWidget\" \
          name=\"ModalityWidget\">\n"
          "</widget>\n";
}

//-----------------------------------------------------------------------------
QIcon ctkModalityWidgetPlugin::icon() const
{
  return QIcon(":/Icons/pushbutton.png");
}

//-----------------------------------------------------------------------------
QString ctkModalityWidgetPlugin::includeFile() const
{
  return "ctkModalityWidget.h";
}

//-----------------------------------------------------------------------------
bool ctkModalityWidgetPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkModalityWidgetPlugin::name() const
{
  return "ctkModalityWidget";
}
