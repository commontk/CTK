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
#include "ctkConsole.h"
#include "ctkConsoleEventTranslator.h"

// ----------------------------------------------------------------------------
ctkConsoleEventTranslator::ctkConsoleEventTranslator(QObject* parent)
  : pqWidgetEventTranslator(parent)
{
  this->CurrentObject = 0;
  this->CurrentCompleter = 0;
  this->CurrentPopup = 0;
}

// ----------------------------------------------------------------------------
bool ctkConsoleEventTranslator::translateEvent(QObject *Object,
                                               QEvent *Event,
                                               bool &Error)
{
  Q_UNUSED(Error);
  // We don't want to save the actions in the completer.
  ctkConsole* console = NULL;
  for (QObject* test = Object ; console == NULL && test != NULL ; test = test->parent())
    {
    console = qobject_cast<ctkConsole*>(test);
    }
  // We don't want to save the actions in the completer.
  if (this->CurrentCompleter && Object == this->CurrentCompleter)
    {
    return true;
    }
  if (this->CurrentPopup &&
      this->CurrentPopup == qobject_cast<QAbstractItemView*>(Object))
    {
    return true;
    }
  if (!console)
    {
    return false;
    }


  if (Event->type() == QEvent::Enter && Object == console)
    {
    if (this->CurrentObject != Object)
      {
      if (this->CurrentObject)
        {
        qDebug() << "Disconnect ...";
        QObject::disconnect(this->CurrentObject, 0 , this, 0);
        }
      this->CurrentObject = Object;
      this->CurrentCompleter = qobject_cast<QObject*>(console->completer());
      this->CurrentPopup = console->completer()->popup();
      QObject::connect(console, SIGNAL(destroyed(QObject*)),
                       this, SLOT(onDestroyed(QObject*)));
      QObject::connect(console, SIGNAL(aboutToExecute(QString)),
                       this, SLOT(onAboutToExecute(QString)));
      }
    }

  return true;
}

// ----------------------------------------------------------------------------
void ctkConsoleEventTranslator::onDestroyed(QObject* /*object*/)
{
  this->CurrentObject = 0;
  this->CurrentCompleter = 0;
  this->CurrentPopup = 0;
}

// ----------------------------------------------------------------------------
void ctkConsoleEventTranslator::onAboutToExecute(const QString& command)
{
  this->recordEvent(this->CurrentObject, "command", command);
}
