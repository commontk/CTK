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
#include <QComboBox>
#include <QDebug>
#include <QEvent>
#include <QLineEdit>
#include <QModelIndex>
#include <QToolButton>

// CTK includes
#include "ctkCompleter.h"
#include "ctkMenuComboBox.h"
#include "ctkMenuComboBoxEventTranslator.h"

// ----------------------------------------------------------------------------
ctkMenuComboBoxEventTranslator::ctkMenuComboBoxEventTranslator(QObject *parent)
  : pqWidgetEventTranslator(parent)
{
  this->CurrentObject = 0;
  this->CurrentView = 0;
}

// ----------------------------------------------------------------------------
bool ctkMenuComboBoxEventTranslator::translateEvent(QObject *Object,
                                                    QEvent *Event,
                                                    bool &Error)
{
  Q_UNUSED(Error);
  ctkMenuComboBox* menuCombo = NULL;
  if (this->CurrentView &&
      qobject_cast<QAbstractItemView*>(Object) == this->CurrentView)
    {
    return true;
    }

  for(QObject* test = Object; menuCombo == NULL && test != NULL; test = test->parent())
    {
    menuCombo = qobject_cast<ctkMenuComboBox*>(test);
    }

  if(!menuCombo)
    {
    return false;
    }

  if(Event->type() == QEvent::Enter && Object == menuCombo)
    {
    if(this->CurrentObject != Object)
      {
      if(this->CurrentObject)
        {
        disconnect(this->CurrentObject, 0, this, 0);
        }
      this->CurrentObject = Object;
      this->CurrentView = menuCombo->searchCompleter()->popup();

      connect(menuCombo, SIGNAL(destroyed(QObject*)),
              this, SLOT(onDestroyed()));
      // to show hide the Menu
//      connect(menuCombo->menu(), SIGNAL(triggered(QAction*)),
//              this, SLOT(onActionTriggered(QAction*)));
      connect(menuCombo->menu(), SIGNAL(aboutToHide()),
              this, SLOT(onAboutToHide()));
      connect(menuCombo->menu(), SIGNAL(aboutToShow()),
              this, SLOT(onAboutToShow()));
      // connect for the completer
      connect(menuCombo->menuComboBoxInternal(), SIGNAL(editTextChanged(QString)),
              this, SLOT(onEditTextChanged(const QString&)));
      connect(menuCombo->toolButtonInternal(), SIGNAL(clicked(bool)),
              this, SLOT(onToolButtonClicked(bool)));
      connect(menuCombo, SIGNAL(actionChanged(QAction*)),
              this, SLOT(onActionTriggered(QAction*)));

      //connect for the QMenu

      }
    }

  return true;
}

// ----------------------------------------------------------------------------
void ctkMenuComboBoxEventTranslator::onDestroyed()
{
  this->CurrentObject = 0;
}

// ----------------------------------------------------------------------------
void ctkMenuComboBoxEventTranslator::onAboutToShow()
{
  emit recordEvent(this->CurrentObject, "show_popup", "true");
}

// ----------------------------------------------------------------------------
void ctkMenuComboBoxEventTranslator::onAboutToHide()
{
  emit recordEvent(this->CurrentObject, "show_popup", "false");
}

// ----------------------------------------------------------------------------
void ctkMenuComboBoxEventTranslator::onEditTextChanged(const QString& text)
{
  emit recordEvent(this->CurrentObject, "set_edit_string_menu", text);
}

// ----------------------------------------------------------------------------
void ctkMenuComboBoxEventTranslator::onToolButtonClicked(bool state)
{
  QString arg = state ? "true" : "false";
  emit recordEvent(this->CurrentObject, "set_edit", arg);
}

// ----------------------------------------------------------------------------
void ctkMenuComboBoxEventTranslator::onActionTriggered(QAction* action)
{
  emit recordEvent(this->CurrentObject, "triggered", action->text());
}

// ----------------------------------------------------------------------------
void ctkMenuComboBoxEventTranslator::connectAllMenu(QMenu* menu)
{
  connect(menu, SIGNAL(triggered(QAction*)),
          this, SLOT(onActionTriggered(QAction*)));

  foreach (QAction* action, menu->actions())
    {
    if (action->menu())
      {
      this->connectAllMenu(action->menu());
      }
    }
}
