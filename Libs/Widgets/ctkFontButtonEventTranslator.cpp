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
#include <QFont>
#include <QEvent>

// CTK includes
#include "ctkFontButton.h"
#include "ctkFontButtonEventTranslator.h"

// ----------------------------------------------------------------------------
ctkFontButtonEventTranslator::ctkFontButtonEventTranslator(QObject *parent)
  : pqWidgetEventTranslator(parent),
  CurrentObject(0)
{
}

// ----------------------------------------------------------------------------
bool ctkFontButtonEventTranslator::translateEvent(QObject *Object,
                                                   QEvent *Event,
                                                   bool &Error)
{
  Q_UNUSED(Error);
  ctkFontButton* object = NULL;
  for(QObject* test = Object; object == NULL && test != NULL; test = test->parent())
    {
    object = qobject_cast<ctkFontButton*>(test);
    }

  if(!object)
    {
    return false;
    }


  if(Event->type() == QEvent::Enter && Object == object)
    {
    if(this->CurrentObject != Object)
      {
      if(this->CurrentObject)
        {
        disconnect(this->CurrentObject, 0, this, 0);
        }
      this->CurrentObject = Object;

      // connect
      connect(object, SIGNAL(currentFontChanged(const QFont&)),
              this, SLOT(onCurrentFontChanged(const QFont&)));
      connect(object, SIGNAL(destroyed(QObject*)),
              this, SLOT(onDestroyed(QObject*)));
      }
    }

  return true;
}

// ----------------------------------------------------------------------------
void ctkFontButtonEventTranslator::onDestroyed(const QObject& /*Object*/ )
{
  this->CurrentObject = 0;
}

// ----------------------------------------------------------------------------
void ctkFontButtonEventTranslator::onCurrentFontChanged(const QFont& newFont)
{
  emit recordEvent(this->CurrentObject, "newFont", newFont.toString());
}


