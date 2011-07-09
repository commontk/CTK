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
