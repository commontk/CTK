/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __ctkVTKWidgetsAbstractPlugin_h
#define __ctkVTKWidgetsAbstractPlugin_h

// Qt includes
#include <QDesignerCustomWidgetInterface>

// CTK includes
#include "CTKVisualizationVTKWidgetsPluginsExport.h"

class CTK_VISUALIZATION_VTK_WIDGETS_PLUGINS_EXPORT ctkVTKWidgetsAbstractPlugin :
  public QDesignerCustomWidgetInterface
{
  Q_INTERFACES(QDesignerCustomWidgetInterface);
public:

  ctkVTKWidgetsAbstractPlugin();
  
  // Do *NOT* reimplement this method.
  QString group() const;
  
  // You can reimplement these methods
  virtual QIcon icon() const;
  virtual QString toolTip() const;
  virtual QString whatsThis() const;
  virtual void initialize(QDesignerFormEditorInterface *formEditor);
  
protected:
  bool Initialized;
};

#endif
