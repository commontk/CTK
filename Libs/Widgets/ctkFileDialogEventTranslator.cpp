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
  ctkFileDialog* fileDialog = NULL;
  for(QObject* test = Object; fileDialog == NULL && test != NULL; test = test->parent())
    {
    fileDialog = qobject_cast<ctkFileDialog*>(test);
    }

  if(!fileDialog)
    {
    return this->Superclass::translateEvent(Object, Event, Error);
    }

  if((Event->type() == QEvent::Enter || Event->type() == QEvent::ShowToParent) &&
     Object == fileDialog)
    {
    if(this->CurrentObject != Object)
      {
      if(this->CurrentObject)
        {
        disconnect(this->CurrentObject, 0, this, 0);
        }
      this->CurrentObject = Object;

      connect(fileDialog, SIGNAL(filesSelected(const QStringList&)),
              this, SLOT(onFileSelectionChanged(const QStringList&)));
      connect(fileDialog, SIGNAL(rejected()), this, SLOT(onRejected()));
      connect(fileDialog, SIGNAL(destroyed(QObject*)),
              this, SLOT(onDestroyed(QObject*)));
      }
    }

  // All events performed in the dialog widgets are swallowed (e.g. scrollbar
  // events)...
  bool acceptEvent = true;
  // ...except for the events on the bottom widget (if any).
  QWidget* bottomWidget = fileDialog->bottomWidget();
  if (bottomWidget)
    {
    for(QObject* eventAncestor = Object; eventAncestor != NULL; eventAncestor = eventAncestor->parent())
      {
      if (bottomWidget == eventAncestor)
        {
        // The event originated from a widget of the bottom widget, let the
        // other translators record it.
        acceptEvent = false;
        break;
        }
      }
    }

  return acceptEvent;
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


