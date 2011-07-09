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
#include "ctkFittedTextBrowserPlugin.h"
#include "ctkFittedTextBrowser.h"

// --------------------------------------------------------------------------
ctkFittedTextBrowserPlugin::ctkFittedTextBrowserPlugin(QObject *_parent)
        : QObject(_parent)
{

}

// --------------------------------------------------------------------------
QWidget *ctkFittedTextBrowserPlugin::createWidget(QWidget *_parent)
{
  ctkFittedTextBrowser* _widget = new ctkFittedTextBrowser(_parent);
  return _widget;
}

// --------------------------------------------------------------------------
QString ctkFittedTextBrowserPlugin::domXml() const
{
  return "<widget class=\"ctkFittedTextBrowser\" \
          name=\"FittedTextBrowser\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkFittedTextBrowserPlugin::icon() const
{
  return QIcon(":/Icons/textedit.png");
}

// --------------------------------------------------------------------------
QString ctkFittedTextBrowserPlugin::includeFile() const
{
  return "ctkFittedTextBrowser.h";
}

// --------------------------------------------------------------------------
bool ctkFittedTextBrowserPlugin::isContainer() const
{
  return false;
}

// --------------------------------------------------------------------------
QString ctkFittedTextBrowserPlugin::name() const
{
  return "ctkFittedTextBrowser";
}
