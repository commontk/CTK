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
// QT includes
#include <QApplication>
#include <QStyle>

// CTK includes
#include "ctkExpandButtonPlugin.h"
#include "ctkExpandButton.h"

//-----------------------------------------------------------------------------
ctkExpandButtonPlugin::ctkExpandButtonPlugin(QObject *_parent)
        : QObject(_parent)
{

}

//-----------------------------------------------------------------------------
QWidget *ctkExpandButtonPlugin::createWidget(QWidget *_parent)
{
  ctkExpandButton* _widget = new ctkExpandButton(_parent);
  return _widget;
}

//-----------------------------------------------------------------------------
QString ctkExpandButtonPlugin::domXml() const
{
  return "<widget class=\"ctkExpandButton\" \
          name=\"Button\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkExpandButtonPlugin::icon() const
{
  return QIcon(QApplication::style()->standardIcon(
                 QStyle::SP_ToolBarHorizontalExtensionButton));
}

//-----------------------------------------------------------------------------
QString ctkExpandButtonPlugin::includeFile() const
{
  return "ctkExpandButton.h";
}

//-----------------------------------------------------------------------------
bool ctkExpandButtonPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkExpandButtonPlugin::name() const
{
  return "ctkExpandButton";
}
