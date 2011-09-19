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
#include "ctkThumbnailWidgetPlugin.h"
#include "ctkThumbnailWidget.h"

//-----------------------------------------------------------------------------
ctkThumbnailWidgetPlugin::ctkThumbnailWidgetPlugin(QObject* parentObject)
  : QObject(parentObject)
{
}

//-----------------------------------------------------------------------------
QWidget *ctkThumbnailWidgetPlugin::createWidget(QWidget* parentWidget)
{
  ctkThumbnailWidget* newWidget = new ctkThumbnailWidget(parentWidget);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkThumbnailWidgetPlugin::domXml() const
{
  return "<widget class=\"ctkThumbnailWidget\" name=\"ThumbnailWidget\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkThumbnailWidgetPlugin::icon() const
{
  return QIcon(":/Icons/widget.png");
}

//-----------------------------------------------------------------------------
QString ctkThumbnailWidgetPlugin::includeFile() const
{
  return "ctkThumbnailWidget.h";
}

//-----------------------------------------------------------------------------
bool ctkThumbnailWidgetPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkThumbnailWidgetPlugin::name() const
{
  return "ctkThumbnailWidget";
}
