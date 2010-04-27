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
#include "ctkVTKRenderViewPlugin.h"
#include "ctkVTKRenderView.h"

//-----------------------------------------------------------------------------
ctkVTKRenderViewPlugin::ctkVTKRenderViewPlugin(QObject *_parent):QObject(_parent)
{
}

//-----------------------------------------------------------------------------
QWidget *ctkVTKRenderViewPlugin::createWidget(QWidget *_parent)
{
  ctkVTKRenderView* _widget = new ctkVTKRenderView(_parent);
  return _widget;
}

//-----------------------------------------------------------------------------
QString ctkVTKRenderViewPlugin::domXml() const
{
  return "<widget class=\"ctkVTKRenderView\" \
          name=\"VTKRenderView\">\n"
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

//-----------------------------------------------------------------------------
QString ctkVTKRenderViewPlugin::includeFile() const
{
  return "ctkVTKRenderView.h";
}

//-----------------------------------------------------------------------------
bool ctkVTKRenderViewPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkVTKRenderViewPlugin::name() const
{
  return "ctkVTKRenderView";
}
