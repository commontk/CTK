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
#include "ctkTreeComboBox.h"
#include "ctkTreeComboBoxEventTranslator.h"

// ----------------------------------------------------------------------------
ctkTreeComboBoxEventTranslator::ctkTreeComboBoxEventTranslator(QObject *parent)
  : pqWidgetEventTranslator(parent)
{
  this->CurrentObject = 0;
}

// ----------------------------------------------------------------------------
bool ctkTreeComboBoxEventTranslator::translateEvent(QObject *Object,
                                                    QEvent *Event,
                                                    bool &Error)
{
  Q_UNUSED(Error);
  ctkTreeComboBox* treeCombo = NULL;
  for(QObject* test = Object; treeCombo == NULL && test != NULL; test = test->parent())
    {
    treeCombo = qobject_cast<ctkTreeComboBox*>(test);
    }

  if(!treeCombo)
    {
    return false;
    }

  if(Event->type() == QEvent::Enter && Object == treeCombo)
    {
    if(this->CurrentObject != Object)
      {
      if(this->CurrentObject)
        {
        disconnect(this->CurrentObject, 0, this, 0);
        }
      this->CurrentObject = Object;
//      connect(treeCombo, SIGNAL(popupShow()), this, SLOT(onPopupShow()));
//      connect(treeCombo, SIGNAL(popupHide()), this, SLOT(onPopupHide()));
      connect(treeCombo, SIGNAL(destroyed(QObject*)),
              this, SLOT(onDestroyed(QObject*)));
      connect(treeCombo, SIGNAL(currentIndexChanged(const QString&)),
              this, SLOT(onCurrentIndexChanged(const QString&)));
      return true;
      }
    }

  return true;
}

// ----------------------------------------------------------------------------
void ctkTreeComboBoxEventTranslator::onDestroyed(QObject*)
{
  this->CurrentObject = 0;
}

// ----------------------------------------------------------------------------
void ctkTreeComboBoxEventTranslator::onCurrentIndexChanged(const QString& text)
{
  emit recordEvent(this->CurrentObject, "indexChanged", text);
}


//// ----------------------------------------------------------------------------
//void ctkTreeComboBoxEventTranslator::onPopupShow()
//{
//  emit recordEvent(this->CurrentObject, "showpopup", "");
//}

//// ----------------------------------------------------------------------------
//void ctkTreeComboBoxEventTranslator::onPopupHide()
//{
//  emit recordEvent(this->CurrentObject, "hidepopup", "");
//}

