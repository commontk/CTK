/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

// CTK includes
#include "ctkRangeWidgetPlugin.h"
#include "ctkRangeWidget.h"

//-----------------------------------------------------------------------------
ctkRangeWidgetPlugin::ctkRangeWidgetPlugin(QObject *_parent)
        : QObject(_parent)
{

}

//-----------------------------------------------------------------------------
QWidget *ctkRangeWidgetPlugin::createWidget(QWidget *_parent)
{
  ctkRangeWidget* _widget = new ctkRangeWidget(_parent);
  return _widget;
}

//-----------------------------------------------------------------------------
QString ctkRangeWidgetPlugin::domXml() const
{
  return "<widget class=\"ctkRangeWidget\" \
          name=\"ctkRangeWidget\">\n"
          "</widget>\n";
}

// --------------------------------------------------------------------------
QIcon ctkRangeWidgetPlugin::icon() const
{
  return QIcon(":/Icons/hrangeslider.png");
}

//-----------------------------------------------------------------------------
QString ctkRangeWidgetPlugin::includeFile() const
{
  return "ctkRangeWidget.h";
}

//-----------------------------------------------------------------------------
bool ctkRangeWidgetPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkRangeWidgetPlugin::name() const
{
  return "ctkRangeWidget";
}
