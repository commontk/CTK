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
#include "ctkTransferFunctionViewPlugin.h"
#include "ctkTransferFunctionView.h"

//-----------------------------------------------------------------------------
ctkTransferFunctionViewPlugin::ctkTransferFunctionViewPlugin(QObject *_parent)
        : QObject(_parent)
{

}

//-----------------------------------------------------------------------------
QWidget *ctkTransferFunctionViewPlugin::createWidget(QWidget *_parent)
{
  ctkTransferFunctionView* _widget = new ctkTransferFunctionView(_parent);
  return _widget;
}

//-----------------------------------------------------------------------------
QString ctkTransferFunctionViewPlugin::domXml() const
{
  return "<widget class=\"ctkTransferFunctionView\" \
          name=\"TransferFunctionView\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkTransferFunctionViewPlugin::icon() const
{
  return QIcon();
}

//-----------------------------------------------------------------------------
QString ctkTransferFunctionViewPlugin::includeFile() const
{
  return "ctkTransferFunctionView.h";
}

//-----------------------------------------------------------------------------
bool ctkTransferFunctionViewPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkTransferFunctionViewPlugin::name() const
{
  return "ctkTransferFunctionView";
}
