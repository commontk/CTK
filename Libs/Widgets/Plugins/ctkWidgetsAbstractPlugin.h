/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __ctkWidgetsAbstractPlugin_h
#define __ctkWidgetsAbstractPlugin_h

// Qt includes
#include <QDesignerCustomWidgetInterface>

// CTK includes
#include "CTKWidgetsPluginsExport.h"

class CTK_WIDGETS_PLUGINS_EXPORT ctkWidgetsAbstractPlugin :
  public QDesignerCustomWidgetInterface
{
  Q_INTERFACES(QDesignerCustomWidgetInterface);
public:

  ctkWidgetsAbstractPlugin();
  
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
