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
#include "ctkMatrixWidget.h"
#include "ctkMatrixWidgetEventTranslator.h"

// ----------------------------------------------------------------------------
ctkMatrixWidgetEventTranslator::ctkMatrixWidgetEventTranslator(QObject *parent)
  : pqWidgetEventTranslator(parent)
{
  this->CurrentObject = 0;
}

// ----------------------------------------------------------------------------
bool ctkMatrixWidgetEventTranslator::translateEvent(QObject *Object,
                                                    QEvent *Event,
                                                    bool &Error)
{
  Q_UNUSED(Error);
  ctkMatrixWidget* matrix = NULL;
  for(QObject* test = Object; matrix == NULL && test != NULL; test = test->parent())
    {
    matrix = qobject_cast<ctkMatrixWidget*>(test);
    }

  if(!matrix)
    {
    return false;
    }

  if(Event->type() == QEvent::Enter && Object == matrix)
    {
    if(this->CurrentObject != Object)
      {
      if(this->CurrentObject)
        {
        disconnect(this->CurrentObject, 0, this, 0);
        }
      this->CurrentObject = Object;

      connect(matrix, SIGNAL(destroyed(QObject*)), this, SLOT(onDestroyed()));
      connect(matrix, SIGNAL(matrixChanged()), this, SLOT(onMatrixChanged()));
      }
    }

  return true;
}

// ----------------------------------------------------------------------------
void ctkMatrixWidgetEventTranslator::onDestroyed()
{
  this->CurrentObject = 0;
}

// ----------------------------------------------------------------------------
void ctkMatrixWidgetEventTranslator::onMatrixChanged()
{
  ctkMatrixWidget* object = qobject_cast<ctkMatrixWidget*>(this->CurrentObject);
  QVector<double> value = object->values();
  QStringList list;
  foreach(double val, value)
    {
    list << QString::number(val);
    }
  emit recordEvent(this->CurrentObject, "set_values", list.join(" "));
}
