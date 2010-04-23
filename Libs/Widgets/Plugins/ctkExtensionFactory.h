/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __ctkExtensionFactory_h
#define __ctkExtensionFactory_h

// Qt includes
#include <QtDesigner/QExtensionFactory>

// CTK includes
#include "CTKWidgetsPluginsExport.h"

class QExtensionManager;

class CTK_WIDGETS_PLUGINS_EXPORT ctkExtensionFactory: public QExtensionFactory
{
    Q_OBJECT

public:
    ctkExtensionFactory(QExtensionManager *parent = 0);

protected:
    QObject *createExtension(QObject *object, const QString &iid, QObject *parent) const;
};

#endif
