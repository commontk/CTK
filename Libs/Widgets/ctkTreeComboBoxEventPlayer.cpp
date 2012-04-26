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
#include <QAbstractItemView>
#include <QApplication>
#include <QDebug>
#include <QKeyEvent>

// CTK includes
#include "ctkTreeComboBox.h"
#include "ctkTreeComboBoxEventPlayer.h"

// ----------------------------------------------------------------------------
ctkTreeComboBoxEventPlayer::ctkTreeComboBoxEventPlayer(QObject *parent)
  : pqWidgetEventPlayer(parent)
{
}

// ----------------------------------------------------------------------------
bool ctkTreeComboBoxEventPlayer::playEvent(QObject *Object,
                                           const QString &Command,
                                           const QString &Arguments,
                                           bool &Error)
{

  if (Command != "showpopup" &&
      Command != "hidepopup" &&
      Command != "indexChanged")
    {
    return false;
    }

  if(ctkTreeComboBox* const object =
       qobject_cast<ctkTreeComboBox*>(Object))
    {
//    if (Command == "showpopup")
//      {
//      object->showPopup();
//      object->update();
//      return true;
//      }
//    if (Command == "hidepopup")
//      {
//      object->hidePopup();
//      object->update();
//      return true;
//      }
//    if (Command == "set_string")
//      {
////      qDebug() << "setstring";
//      // to implemented
////      qDebug() << object->currentText() << object->view()->currentIndex().data().toString();
////      object->setItemText(0, object->view()->currentIndex().data().toString());
////      return true;
//      object->view()->setCurrentIndex(object->view()->currentIndex());
//      QKeyEvent event(QEvent::ShortcutOverride, Qt::Key_Enter, Qt::NoModifier);
//      // here we conditionally send the event, otherwise, nodeActivated would be
//      // fired even if the user didn't manually select the node.
//      // Warning: please note that sending a KeyEvent will close the popup menu
//      // of the combobox if it is open.
//      QApplication::sendEvent(object->view(), &event);
//      return true;
//      }
      if(Command == "indexChanged")
        {
        QModelIndexList indexes = object->model()->match(
            object->model()->index(0, 0), 0, QVariant(Arguments), 1,
            Qt::MatchRecursive | Qt::MatchExactly | Qt::MatchWrap);
        if(indexes.count() > 0)
          {
          object->view()->setCurrentIndex(indexes[0]);
          QKeyEvent event(QEvent::ShortcutOverride, Qt::Key_Enter, Qt::NoModifier);
          // here we conditionally send the event, otherwise, nodeActivated would be
          // fired even if the user didn't manually select the node.
          // Warning: please note that sending a KeyEvent will close the popup menu
          // of the combobox if it is open.
          QApplication::sendEvent(object->view(), &event);
          }
        return true;
        }
    }

  qCritical() << "calling showpopup/hidepopup on unhandled type " << Object;
  Error = true;
  return true;
}

