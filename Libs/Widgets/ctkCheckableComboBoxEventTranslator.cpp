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
#include "ctkCheckableComboBox.h"
#include "ctkCheckableComboBoxEventTranslator.h"
#include "ctkCheckableModelHelper.h"

// ----------------------------------------------------------------------------
ctkCheckableComboBoxEventTranslator::ctkCheckableComboBoxEventTranslator(QObject *parent)
  : pqWidgetEventTranslator(parent)
{
  this->CurrentObject = 0;
}

// ----------------------------------------------------------------------------
bool ctkCheckableComboBoxEventTranslator::translateEvent(QObject *Object,
                                                         QEvent *Event,
                                                         bool &Error)
{
  Q_UNUSED(Error);
  ctkCheckableComboBox* checkableCombo = NULL;
  for(QObject* test = Object; checkableCombo == NULL && test != NULL; test = test->parent())
    {
    checkableCombo = qobject_cast<ctkCheckableComboBox*>(test);
    }

  if(!checkableCombo)
    {
    // not for me
    return false;
    }

  if(Event->type() == QEvent::Enter && Object == checkableCombo)
    {
    if(this->CurrentObject != Object)
      {
      if(this->CurrentObject)
        {
        disconnect(this->CurrentObject, 0, this, 0);
        }
      this->CurrentObject = Object;
      connect(checkableCombo, SIGNAL(destroyed(QObject*)), this, SLOT(onDestroyed(QObject*)));
      connect(checkableCombo, SIGNAL(activated(const QString&)), this, SLOT(onStateChanged(const QString&)));
      connect(checkableCombo, SIGNAL(editTextChanged(const QString&)), this, SLOT(onStateChanged(const QString&)));
      connect(checkableCombo->model(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(onDataChanged(const QModelIndex&, const QModelIndex&)));
      }
    }

  return true;
}

// ----------------------------------------------------------------------------
void ctkCheckableComboBoxEventTranslator::onDestroyed(QObject* /*Object*/)
{
  this->CurrentObject = 0;
}

// ----------------------------------------------------------------------------
void ctkCheckableComboBoxEventTranslator::onStateChanged(const QString& State)
{
  emit recordEvent(this->CurrentObject, "set_string", State);
}

// ----------------------------------------------------------------------------
void ctkCheckableComboBoxEventTranslator::onDataChanged(const QModelIndex&,const QModelIndex&)
{
  ctkCheckableComboBox* checkableCombo = qobject_cast<ctkCheckableComboBox*>(this->CurrentObject);

  if (checkableCombo->checkedIndexes().count() > this->OldIndexList.count())
    {
    QModelIndexList checkList = checkableCombo->checkedIndexes();
    QStringList checkStringList;
    foreach(QModelIndex index, checkList)
      {
      checkStringList << QString::number(index.row());
      }

    emit recordEvent(this->CurrentObject,"check_indexes", checkStringList.join(" "));
    }
  else
    {
    QModelIndex startIndex = checkableCombo->model()->index(0,0, checkableCombo->rootModelIndex());
    QModelIndexList uncheckedList = checkableCombo->model()->match(
      startIndex, Qt::CheckStateRole, Qt::Unchecked, -1, Qt::MatchRecursive);
    QStringList uncheckedStringList;
    foreach (QModelIndex index, uncheckedList)
      {
      uncheckedStringList << QString::number(index.row());
      }

    emit recordEvent(this->CurrentObject,"uncheck_indexes", uncheckedStringList.join(" "));
    }
  this->OldIndexList = checkableCombo->checkedIndexes();
}

