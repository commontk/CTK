/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __ctkVTKRenderViewPlugin_h
#define __ctkVTKRenderViewPlugin_h

// CTK includes
#include "ctkVTKWidgetsAbstractPlugin.h"

class CTK_VISUALIZATION_VTK_WIDGETS_PLUGINS_EXPORT ctkVTKRenderViewPlugin :
  public QObject,
  public ctkVTKWidgetsAbstractPlugin
{
  Q_OBJECT

public:
  ctkVTKRenderViewPlugin(QObject *_parent = 0);
  
  QWidget *createWidget(QWidget *_parent);
  QString domXml() const;
  QString includeFile() const;
  bool isContainer() const;
  QString name() const;
  
};

#endif
