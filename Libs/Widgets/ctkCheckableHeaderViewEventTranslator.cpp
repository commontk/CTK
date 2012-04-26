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
#include <QEvent>

// CTK includes
#include "ctkCheckableHeaderView.h"
#include "ctkCheckableHeaderViewEventTranslator.h"
#include "ctkCheckableModelHelper.h"

// ----------------------------------------------------------------------------
ctkCheckableHeaderViewEventTranslator::ctkCheckableHeaderViewEventTranslator(QObject *parent)
  : pqWidgetEventTranslator(parent)
{
  this->CurrentObject = 0;
}

// ----------------------------------------------------------------------------
bool ctkCheckableHeaderViewEventTranslator::translateEvent(QObject *Object,
                                                           QEvent *Event,
                                                           bool &Error)
{
  Q_UNUSED(Error);
  ctkCheckableHeaderView* object = qobject_cast<ctkCheckableHeaderView*>(Object);
  if(!object)
    {
    return false;
    }

  switch(Event->type())
    {
    case QEvent::Enter :
      this->CurrentObject = object;
      connect(object->model(), SIGNAL(headerDataChanged(Qt::Orientation,int,int)),
              this, SLOT(onHeaderDataChanged(Qt::Orientation,int,int)));
      break;
    case QEvent::Leave :
      this->CurrentObject = 0;
      disconnect(object->model(), 0, this, 0);
      break;
    default:
      return false;
      break;
    }

  return true;
}

// ----------------------------------------------------------------------------
void ctkCheckableHeaderViewEventTranslator::onHeaderDataChanged(Qt::Orientation, int first, int)
{
  ctkCheckableHeaderView* checkableHeader = qobject_cast<ctkCheckableHeaderView*>(this->CurrentObject);

  if(checkableHeader->checkState(first) == Qt::Checked)
    {
    emit recordEvent(this->CurrentObject,"set_section_checked", QString::number(first));
    }
  else
    {
    emit recordEvent(this->CurrentObject,"set_section_unchecked", QString::number(first));
    }
}


