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
          name=\"CTKFittedTextBrowser\">\n"
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
