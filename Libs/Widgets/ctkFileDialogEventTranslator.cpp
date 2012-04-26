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
#include <QVariant>

// CTK includes
#include "ctkFileDialog.h"
#include "ctkFileDialogEventTranslator.h"

// ----------------------------------------------------------------------------
ctkFileDialogEventTranslator::ctkFileDialogEventTranslator(pqTestUtility* util, QObject *parent)
  : pqNativeFileDialogEventTranslator(util, parent),
  CurrentObject(0)
{
}

// ----------------------------------------------------------------------------
bool ctkFileDialogEventTranslator::translateEvent(QObject *Object,
                                                   QEvent *Event,
                                                   bool &Error)
{
  Q_UNUSED(Error);
  ctkFileDialog* object = NULL;
  for(QObject* test = Object; object == NULL && test != NULL; test = test->parent())
    {
    object = qobject_cast<ctkFileDialog*>(test);
    }

  if(!object)
    {
    return this->Superclass::translateEvent(Object, Event, Error);
    }

  // If we find an action on the bottom widget, we return false.
  QWidget* bottomWidget = NULL;
  for(QObject* testWidget = Object; bottomWidget == NULL && testWidget!= NULL; testWidget = testWidget->parent())
    {
    if(object->bottomWidget() == qobject_cast<QWidget*>(testWidget))
      {
      return false;
      }
    }

  if((Event->type() == QEvent::Enter || Event->type() == QEvent::ShowToParent) &&
     Object == object)
    {
    if(this->CurrentObject != Object)
      {
      if(this->CurrentObject)
        {
        disconnect(this->CurrentObject, 0, this, 0);
        }
      this->CurrentObject = Object;

      connect(object, SIGNAL(filesSelected(const QStringList&)),
              this, SLOT(onFileSelectionChanged(const QStringList&)));
      connect(object, SIGNAL(rejected()), this, SLOT(onRejected()));
      connect(object, SIGNAL(destroyed(QObject*)),
              this, SLOT(onDestroyed(QObject*)));
      }
    }

  return true;
}

// ----------------------------------------------------------------------------
void ctkFileDialogEventTranslator::onDestroyed(QObject*)
{
  this->CurrentObject = 0;
}

// ----------------------------------------------------------------------------
void ctkFileDialogEventTranslator::onFileSelectionChanged(const QStringList& files)
{
  foreach(QString file, files)
    {
    QFileInfo info(file);
    emit recordEvent(this->CurrentObject, "comment", QString("Loading %1 ... ").arg(info.fileName()));
    }

  QString file = files.join("#");
  emit recordEvent(this->CurrentObject, "newFile", file);
}

// ----------------------------------------------------------------------------
void ctkFileDialogEventTranslator::onRejected()
{
  emit recordEvent(this->CurrentObject, "rejected", "");
}


