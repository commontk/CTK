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
#include "ctkExtensionFactory.h"

// #include "ctkCollapsibleWidgetContainerExtension.h"
// #include "ctkCollapsibleWidget.h"

// --------------------------------------------------------------------------
ctkExtensionFactory::ctkExtensionFactory(QExtensionManager *_parent)
    : QExtensionFactory(_parent)
{

}

// --------------------------------------------------------------------------
QObject *ctkExtensionFactory::createExtension(QObject *object,
                                               const QString &iid,
                                               QObject *_parent) const
{
  Q_UNUSED(object);
  Q_UNUSED(iid);
  Q_UNUSED(_parent);
  // ctkCollapsibleWidget *widget = qobject_cast<ctkCollapsibleWidget*>(object);
  // 
  // if (widget && (iid == Q_TYPEID(QDesignerContainerExtension))) {
  //     return new ctkCollapsibleWidgetContainerExtension(widget, parent);
  // } else {
  //     return 0;
  // }
  return 0; 
}
