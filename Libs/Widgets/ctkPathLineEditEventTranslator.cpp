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
#include <QComboBox>
#include <QDebug>
#include <QEvent>

// CTK includes
#include "ctkPathLineEdit.h"
#include "ctkPathLineEditEventTranslator.h"

// ----------------------------------------------------------------------------
ctkPathLineEditEventTranslator::ctkPathLineEditEventTranslator(QObject *parent)
  : pqWidgetEventTranslator(parent)
{
  this->CurrentObject = 0;
}

// ----------------------------------------------------------------------------
bool ctkPathLineEditEventTranslator::translateEvent(QObject *Object,
                                                   QEvent *Event,
                                                   bool &Error)
{
  Q_UNUSED(Error);
  ctkPathLineEdit* pathLineEdit = NULL;
  for(QObject* test = Object; pathLineEdit == NULL && test != NULL; test = test->parent())
    {
    pathLineEdit = qobject_cast<ctkPathLineEdit*>(test);
    }

  if(!pathLineEdit)
    {
    return false;
    }

  if(Event->type() == QEvent::Enter && Object == pathLineEdit)
    {
    if(this->CurrentObject != Object)
      {
      if(this->CurrentObject)
        {
        disconnect(this->CurrentObject, 0, this, 0);
        }
      this->CurrentObject = Object;

      connect(pathLineEdit, SIGNAL(destroyed(QObject*)),
              this, SLOT(onDestroyed()));
      connect(pathLineEdit->comboBox(), SIGNAL(editTextChanged(const QString&)),
              this, SLOT(onEditTextChanged(const QString&)));
      connect(pathLineEdit, SIGNAL(currentPathChanged(const QString&)),
              this, SLOT(onCurrentPathChanged(const QString&)));
      }
    }

  return true;
}

// ----------------------------------------------------------------------------
void ctkPathLineEditEventTranslator::onDestroyed()
{
  this->CurrentObject = 0;
}

// ----------------------------------------------------------------------------
void ctkPathLineEditEventTranslator::onCurrentPathChanged(const QString& text)
{
  emit recordEvent(this->CurrentObject, "set_path_string", text);
}

// ----------------------------------------------------------------------------
void ctkPathLineEditEventTranslator::onEditTextChanged(const QString& text)
{
  emit recordEvent(this->CurrentObject, "set_edit_string", text);
}

