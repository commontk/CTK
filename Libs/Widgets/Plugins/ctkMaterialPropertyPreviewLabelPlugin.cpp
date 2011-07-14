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
#include "ctkMaterialPropertyPreviewLabelPlugin.h"
#include "ctkMaterialPropertyPreviewLabel.h"

//-----------------------------------------------------------------------------
ctkMaterialPropertyPreviewLabelPlugin::ctkMaterialPropertyPreviewLabelPlugin(QObject *_parent) : QObject(_parent)
{
}

//-----------------------------------------------------------------------------
QWidget *ctkMaterialPropertyPreviewLabelPlugin::createWidget(QWidget *_parent)
{
  ctkMaterialPropertyPreviewLabel* _widget = new ctkMaterialPropertyPreviewLabel(_parent);
  return _widget;
}

//-----------------------------------------------------------------------------
QString ctkMaterialPropertyPreviewLabelPlugin::domXml() const
{
  return "<widget class=\"ctkMaterialPropertyPreviewLabel\" \
          name=\"MaterialPropertyPreviewLabel\">\n"
          "</widget>\n";
}

//-----------------------------------------------------------------------------
QIcon ctkMaterialPropertyPreviewLabelPlugin::icon() const
{
  return QIcon(":/Icons/pushbutton.png");
}

//-----------------------------------------------------------------------------
QString ctkMaterialPropertyPreviewLabelPlugin::includeFile() const
{
  return "ctkMaterialPropertyPreviewLabel.h";
}

//-----------------------------------------------------------------------------
bool ctkMaterialPropertyPreviewLabelPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkMaterialPropertyPreviewLabelPlugin::name() const
{
  return "ctkMaterialPropertyPreviewLabel";
}
