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

// Qt includes
#include <QDebug>
#include <QEvent>

// CTK includes
#include "ctkPopupWidget.h"
#include "ctkPopupWidgetEventTranslator.h"

// ----------------------------------------------------------------------------
ctkPopupWidgetEventTranslator::ctkPopupWidgetEventTranslator(QObject *parent)
  : pqWidgetEventTranslator(parent),
  CurrentObject(0)
{
}

// ----------------------------------------------------------------------------
bool ctkPopupWidgetEventTranslator::translateEvent(QObject *Object,
                                                   QEvent *Event,
                                                   bool &Error)
{
  Q_UNUSED(Error);
  ctkPopupWidget* const object = qobject_cast<ctkPopupWidget*>(Object);

  if(!object)
    {
    return false;
    }


  if(Event->type() == QEvent::ShowToParent )
    {
    if(this->CurrentObject != Object)
      {
      if(this->CurrentObject)
        {
        disconnect(this->CurrentObject, 0, this, 0);
        }
      this->CurrentObject = Object;

      // connect
      connect(object, SIGNAL(destroyed(QObject*)),
              this, SLOT(onDestroyed()));
      connect(object, SIGNAL(popupOpened(const bool&)),
              this, SLOT(onPopupOpened(const bool&)));
      }
    }
  return false;
}

// ----------------------------------------------------------------------------
void ctkPopupWidgetEventTranslator::onDestroyed()
{
  this->CurrentObject = 0;
}

// ----------------------------------------------------------------------------
void ctkPopupWidgetEventTranslator::onPopupOpened(const bool& open)
{
  QString state = open ? "true" : "false";
  emit recordEvent(this->CurrentObject, "popupOpen", state);
}


