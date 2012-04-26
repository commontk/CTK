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
#include <QCompleter>
#include <QDebug>
#include <QLineEdit>
#include <QToolButton>
#include <QTreeView>

// CTK includes
#include "ctkMenuComboBox.h"
#include "ctkMenuComboBoxEventPlayer.h"

QAction* actionByTitle(const QString& text, const QMenu* parentMenu);

// ----------------------------------------------------------------------------
ctkMenuComboBoxEventPlayer::ctkMenuComboBoxEventPlayer(QObject *parent)
  : pqWidgetEventPlayer(parent)
{

}

// ----------------------------------------------------------------------------
bool ctkMenuComboBoxEventPlayer::playEvent(QObject *Object,
                                           const QString &Command,
                                           const QString &Arguments,
                                           bool &Error)
{
  if (Command != "set_edit_string_menu" &&
      Command != "set_edit" &&
      Command != "show_popup" &&
      Command != "triggered" &&
      Command != "edit_finish")
    {
    return false;
    }

  if(ctkMenuComboBox* const object =
       qobject_cast<ctkMenuComboBox*>(Object))
    {
    if (Command == "show_popup")
      {
      if (Arguments == "true")
        {
        object->menuComboBoxInternal()->showPopup();
        }
      else
        {
        object->menu()->hide();
        }
      return true;
      }
    if (Command == "set_edit_string_menu")
      {
      object->menuComboBoxInternal()->setEditText(Arguments);
//      object->menuComboBoxInternal()->completer()->popup()->show();
//      qDebug() << "count" <<  qApp->topLevelWidgets().count();
//      foreach (QWidget * o, qApp->topLevelWidgets())
//        {
//        qDebug() << "o" << o;
//        }

      object->menuComboBoxInternal()->completer()->popup()->show();
//      object->menuComboBoxInternal()->completer()->popup()->hide();
//      qDebug() << "count" <<  qApp->topLevelWidgets().count();
//      foreach (QWidget * o, qApp->topLevelWidgets())
//        {
//        qDebug() << "o" << o;
//        }
//      object->menuComboBoxInternal()->lineEdit()->completer()->popup()->hide();
//      qDebug() << "count" <<  qApp->topLevelWidgets().count();
//      foreach (QWidget * o, qApp->topLevelWidgets())
//        {
//        qDebug() << "o" << o;
//        }
      return true;
      }
    if (Command == "set_edit")
      {
      const bool state = Arguments == "true" ? true : false;
      object->toolButtonInternal()->setChecked(state);
      return true;
      }
    if (Command == "triggered")
      {
      QAction* action = actionByTitle(Arguments, object->menu());
      action->trigger();
      return true;
      }
    }

  qCritical() << "calling set_edit_string_menu/set_edit on unhandled type " << Object;
  Error = true;
  return true;
}

//  ------------------------------------------------------------------------
QAction* actionByTitle(const QString& text, const QMenu* parentMenu)
{
  if (parentMenu->title() == text)
    {
    return 0;
    }
  foreach(QAction* action, parentMenu->actions())
    {
    if (!action->menu() && action->text().toLower() == text.toLower())
      {
      return action;
      }
    if (action->menu())
      {
      QAction* subAction = actionByTitle(text, action->menu());
      if(subAction)
        {
        return subAction;
        }
      }
    }
  return 0;
}

