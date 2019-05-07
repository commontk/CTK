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
#include <QActionEvent>
#include <QCompleter>
#include <QDebug>
#include <QEvent>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QStringList>
#include <QStringListModel>
#include <QToolButton>

// CTK includes
#include "ctkCompleter.h"
#include "ctkSearchBox.h"
#include "ctkMenuComboBox_p.h"

// -------------------------------------------------------------------------
ctkMenuComboBoxInternal::ctkMenuComboBoxInternal()
{
}
// -------------------------------------------------------------------------
ctkMenuComboBoxInternal::~ctkMenuComboBoxInternal()
{
}

// -------------------------------------------------------------------------
void ctkMenuComboBoxInternal::showPopup()
{
  QMenu* menu = this->Menu;
  if (!menu)
    {
    return;
    }
  menu->popup(this->mapToGlobal(this->rect().bottomLeft()));
  static int minWidth = menu->sizeHint().width();
  menu->setFixedWidth(qMax(this->width(), minWidth));
  emit popupShown();
}

// -------------------------------------------------------------------------
QSize ctkMenuComboBoxInternal::minimumSizeHint()const
{
  // Cached QComboBox::minimumSizeHint is not recomputed when the current
  // index change, however QComboBox::sizeHint is. Use it instead.
  return this->sizeHint();
}

// -------------------------------------------------------------------------
ctkMenuComboBoxPrivate::ctkMenuComboBoxPrivate(ctkMenuComboBox& object)
  :q_ptr(&object)
{
  this->MenuComboBox = 0;
  this->SearchCompleter = 0;
  this->EditBehavior = ctkMenuComboBox::NotEditable;
  this->IsDefaultTextCurrent = true;
  this->IsDefaultIconCurrent = true;
}

// -------------------------------------------------------------------------
void ctkMenuComboBoxPrivate::init()
{
  Q_Q(ctkMenuComboBox);
  this->setParent(q);

  QHBoxLayout* layout = new QHBoxLayout(q);
  layout->setContentsMargins(0,0,0,0);
  layout->setSizeConstraint(QLayout::SetMinimumSize);
  layout->setSpacing(0);

  // SearchButton
  this->SearchButton = new QToolButton();
  this->SearchButton->setText(q->tr("Search"));
  this->SearchButton->setIcon(QIcon(":/Icons/search.svg"));
  this->SearchButton->setCheckable(true);
  this->SearchButton->setAutoRaise(true);
  layout->addWidget(this->SearchButton);
  q->connect(this->SearchButton, SIGNAL(toggled(bool)),
             this, SLOT(setComboBoxEditable(bool)));

  // MenuComboBox
  this->MenuComboBox = new ctkMenuComboBoxInternal();
  this->MenuComboBox->setMinimumContentsLength(12);
  layout->addWidget(this->MenuComboBox);
  this->MenuComboBox->installEventFilter(q);
  this->MenuComboBox->setInsertPolicy(QComboBox::NoInsert);
  this->MenuComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  this->MenuComboBox->addItem(this->DefaultIcon, this->DefaultText);
  q->connect(this->MenuComboBox, SIGNAL(popupShown()),
             q, SIGNAL(popupShown()));

  this->SearchCompleter = new ctkCompleter(QStringList(), this->MenuComboBox);
  this->SearchCompleter->popup()->setParent(q);
  this->SearchCompleter->setCaseSensitivity(Qt::CaseInsensitive);
  this->SearchCompleter->setModelFiltering(ctkCompleter::FilterWordStartsWith);
  q->connect(this->SearchCompleter, SIGNAL(activated(QString)),
             this, SLOT(onCompletion(QString)));

  // Automatically set the minimumSizeHint of the layout to the widget
  layout->setSizeConstraint(QLayout::SetMinimumSize);
  // Behave like a QComboBox
  q->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed,
                               QSizePolicy::ComboBox));

  q->setDefaultText(ctkMenuComboBox::tr("Search..."));
}

//  ------------------------------------------------------------------------
QAction* ctkMenuComboBoxPrivate::actionByTitle(const QString& text, const QMenu* parentMenu)
{
  if (!parentMenu || parentMenu->title() == text)
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
      QAction* subAction = this->actionByTitle(text, action->menu());
      if(subAction)
        {
        return subAction;
        }
      }
    }
  return 0;
}

//  ------------------------------------------------------------------------
void ctkMenuComboBoxPrivate::setCurrentText(const QString& newCurrentText)
{
  if (this->MenuComboBox->lineEdit())
    {
    static_cast<ctkSearchBox*>(this->MenuComboBox->lineEdit())
      ->setPlaceholderText(newCurrentText);
    }

  this->MenuComboBox->setItemText(this->MenuComboBox->currentIndex(),
                                  newCurrentText);
}

//  ------------------------------------------------------------------------
QString ctkMenuComboBoxPrivate::currentText()const
{
  return this->MenuComboBox->itemText(this->MenuComboBox->currentIndex());
}

//  ------------------------------------------------------------------------
QIcon ctkMenuComboBoxPrivate::currentIcon()const
{
  return this->MenuComboBox->itemIcon(this->MenuComboBox->currentIndex());
}

//  ------------------------------------------------------------------------
void ctkMenuComboBoxPrivate::setCurrentIcon(const QIcon& newCurrentIcon)
{
  this->MenuComboBox->setItemIcon(this->MenuComboBox->currentIndex(),
                                  newCurrentIcon);
}

// -------------------------------------------------------------------------
void ctkMenuComboBoxPrivate::setComboBoxEditable(bool edit)
{
  Q_Q(ctkMenuComboBox);
  if(edit)
    {
    if (!this->MenuComboBox->lineEdit())
      {
      ctkSearchBox* line = new ctkSearchBox();
      this->MenuComboBox->setLineEdit(line);
      if (q->isSearchIconVisible())
        {
        this->MenuComboBox->lineEdit()->selectAll();
        this->MenuComboBox->setFocus();
        }
      q->connect(line, SIGNAL(editingFinished()),
                 q,SLOT(onEditingFinished()));
      }
    this->MenuComboBox->setCompleter(this->SearchCompleter);
    }

  this->MenuComboBox->setEditable(edit);
}

// -------------------------------------------------------------------------
void ctkMenuComboBoxPrivate::onCompletion(const QString& text)
{
  Q_Q(ctkMenuComboBox);

  // In Qt5, when QCompleter sends its activated() signal, QComboBox sets
  // its current index to the activated item, if found. Work around that behavior
  // by re-selecting the original item.
  this->MenuComboBox->setCurrentIndex(0);

  // Set text to the completed string
  if (this->MenuComboBox->lineEdit())
    {
    this->MenuComboBox->lineEdit()->setText(text);
    }

  q->onEditingFinished();
}

// -------------------------------------------------------------------------
void ctkMenuComboBoxPrivate::addAction(QAction *action)
{
  if (action->menu())
    {
    this->addMenuToCompleter(action->menu());
    }
  else
    {
    this->addActionToCompleter(action);
    }
}

// -------------------------------------------------------------------------
void ctkMenuComboBoxPrivate::removeAction(QAction *action)
{
  if (action->menu())
    {
    this->removeMenuFromCompleter(action->menu());
    }
  else
    {
    this->removeActionFromCompleter(action);
    }
}

// -------------------------------------------------------------------------
void ctkMenuComboBoxPrivate::addMenuToCompleter(QMenu* menu)
{
  Q_Q(ctkMenuComboBox);

  menu->installEventFilter(q);

  // Bug QT : see this link for more details
  // https://bugreports.qt.nokia.com/browse/QTBUG-20929?focusedCommentId=161370#comment-161370
  // if the submenu doesn't have a parent, the submenu triggered(QAction*)
  // signal is not propagated. So we listened this submenu to fix the bug.
  QObject* emptyObject = 0;
  if(menu->parent() == emptyObject)
    {
    q->connect(menu, SIGNAL(triggered(QAction*)),
               q, SLOT(onActionSelected(QAction*)), Qt::UniqueConnection);
    }

  foreach (QAction* action, menu->actions())
    {
    this->addAction(action);
    }
}

// -------------------------------------------------------------------------
void ctkMenuComboBoxPrivate::removeMenuFromCompleter(QMenu* menu)
{
  Q_Q(ctkMenuComboBox);

  menu->removeEventFilter(q);

  foreach (QAction* action, menu->actions())
    {
    this->removeAction(action);
    }
}

// -------------------------------------------------------------------------
void ctkMenuComboBoxPrivate::addActionToCompleter(QAction *action)
{
  QStringListModel* model = qobject_cast<QStringListModel* >(
    this->SearchCompleter->sourceModel());
  Q_ASSERT(model);
  QModelIndex start = model->index(0,0);
  QModelIndexList indexList = model->match(start, 0, action->text(), 1, Qt::MatchFixedString|Qt::MatchWrap);
  if (indexList.count())
    {
    return;
    }

  int actionCount = model->rowCount();
  model->insertRow(actionCount);
  QModelIndex index = model->index(actionCount, 0);
  model->setData(index, action->text());
}

//  ------------------------------------------------------------------------
void ctkMenuComboBoxPrivate::removeActionFromCompleter(QAction *action)
{
  QStringListModel* model = qobject_cast<QStringListModel* >(
    this->SearchCompleter->sourceModel());
  Q_ASSERT(model);
  if (!model->stringList().contains(action->text()) )
    {
    return;
    }

  QModelIndex start = model->index(0,0);
  QModelIndexList indexList = model->match(start, 0, action->text());
  Q_ASSERT(indexList.count() == 1);
  foreach (QModelIndex index, indexList)
    {
    // Search completer model is a flat list
    model->removeRow(index.row());
    }
}

//  ------------------------------------------------------------------------
ctkMenuComboBox::ctkMenuComboBox(QWidget* _parent)
  :QWidget(_parent)
  , d_ptr(new ctkMenuComboBoxPrivate(*this))
{
  Q_D(ctkMenuComboBox);
  d->init();
}

//  ------------------------------------------------------------------------
ctkMenuComboBox::~ctkMenuComboBox()
{
}

//  ------------------------------------------------------------------------
void ctkMenuComboBox::setMenu(QMenu* menu)
{
  Q_D(ctkMenuComboBox);
  if (d->MenuComboBox->Menu == menu)
    {
    return;
    }
  d->MenuComboBox->Menu = menu;
  this->setCompleterMenu(menu);
}

// -------------------------------------------------------------------------
QMenu* ctkMenuComboBox::menu()const
{
  Q_D(const ctkMenuComboBox);
  return d->MenuComboBox->Menu;
}

// -------------------------------------------------------------------------
void ctkMenuComboBox::setCompleterMenu(QMenu* menu)
{
  Q_D(ctkMenuComboBox);

  if (d->CompleterMenu == menu)
    {
    return;
    }

  if (d->CompleterMenu)
    {
    QObject::disconnect(d->CompleterMenu,SIGNAL(triggered(QAction*)),
                        this,SLOT(onActionSelected(QAction*)));
    this->removeAction(d->CompleterMenu->menuAction());
    d->removeMenuFromCompleter(d->CompleterMenu);
    }

  d->CompleterMenu = menu;

  if (d->CompleterMenu)
    {
    d->addMenuToCompleter(d->CompleterMenu);
    this->addAction(d->CompleterMenu->menuAction());
    QObject::connect(d->CompleterMenu,SIGNAL(triggered(QAction*)),
                     this,SLOT(onActionSelected(QAction*)), Qt::UniqueConnection);
    }
}

// -------------------------------------------------------------------------
QMenu* ctkMenuComboBox::completerMenu()const
{
  Q_D(const ctkMenuComboBox);
  return d->CompleterMenu;
}

// -------------------------------------------------------------------------
void ctkMenuComboBox::setDefaultText(const QString& newDefaultText)
{
  Q_D(ctkMenuComboBox);
  d->DefaultText = newDefaultText;
  if (d->IsDefaultTextCurrent)
    {
    d->setCurrentText(d->DefaultText);
    }
}

// -------------------------------------------------------------------------
QString ctkMenuComboBox::defaultText()const
{
  Q_D(const ctkMenuComboBox);
  return d->DefaultText;
}

// -------------------------------------------------------------------------
void ctkMenuComboBox::setDefaultIcon(const QIcon& newIcon)
{
  Q_D(ctkMenuComboBox);
  d->DefaultIcon = newIcon;
  if (d->IsDefaultIconCurrent)
    {
    d->setCurrentIcon(d->DefaultIcon);
    }
}

// -------------------------------------------------------------------------
QIcon ctkMenuComboBox::defaultIcon()const
{
  Q_D(const ctkMenuComboBox);
  return d->DefaultIcon;
}

// -------------------------------------------------------------------------
void ctkMenuComboBox::setEditableBehavior(ctkMenuComboBox::EditableBehavior edit)
{
  Q_D(ctkMenuComboBox);
  d->EditBehavior = edit;
      this->disconnect(d->MenuComboBox, SIGNAL(popupShown()),
                    d, SLOT(setComboBoxEditable()));
  switch (edit)
  {
    case ctkMenuComboBox::Editable:
      d->MenuComboBox->setContextMenuPolicy(Qt::DefaultContextMenu);
      d->setComboBoxEditable(true);
      break;
    case ctkMenuComboBox::NotEditable:
      d->MenuComboBox->setContextMenuPolicy(Qt::DefaultContextMenu);
      d->setComboBoxEditable(false);
      break;
    case ctkMenuComboBox::EditableOnFocus:
      d->setComboBoxEditable(this->hasFocus());
      // Here we set the context menu policy to fix a crash on the right click.
      // Opening the context menu removes the focus on the line edit,
      // the comboBox becomes not editable, and the line edit is deleted.
      // The opening of the context menu is done in the line edit and lead to
      // a crash because it infers that the line edit is valid. Another fix
      // could be to delete the line edit later (deleteLater()).
      d->MenuComboBox->setContextMenuPolicy(Qt::NoContextMenu);
      break;
    case ctkMenuComboBox::EditableOnPopup:
      d->setComboBoxEditable(false);
      this->connect(d->MenuComboBox, SIGNAL(popupShown()),
                    d, SLOT(setComboBoxEditable()));
      // Same reason as in ctkMenuComboBox::EditableOnFocus.
      d->MenuComboBox->setContextMenuPolicy(Qt::NoContextMenu);
      break;
  }
}

// -------------------------------------------------------------------------
ctkMenuComboBox::EditableBehavior ctkMenuComboBox::editableBehavior()const
{
  Q_D(const ctkMenuComboBox);
  return d->EditBehavior;
}

// -------------------------------------------------------------------------
void ctkMenuComboBox::setSearchIconVisible(bool state)
{
  Q_D(ctkMenuComboBox);
  d->SearchButton->setVisible(state);
}

// -------------------------------------------------------------------------
bool ctkMenuComboBox::isSearchIconVisible() const
{
  Q_D(const ctkMenuComboBox);
  return d->SearchButton->isVisibleTo(const_cast<ctkMenuComboBox*>(this));
}

// -------------------------------------------------------------------------
void ctkMenuComboBox::setToolButtonStyle(Qt::ToolButtonStyle style)
{
  Q_D(ctkMenuComboBox);
  d->SearchButton->setToolButtonStyle(style);
}

// -------------------------------------------------------------------------
Qt::ToolButtonStyle ctkMenuComboBox::toolButtonStyle() const
{
  Q_D(const ctkMenuComboBox);
  return d->SearchButton->toolButtonStyle();
}
// -------------------------------------------------------------------------
void ctkMenuComboBox::setMinimumContentsLength(int characters)
{
  Q_D(ctkMenuComboBox);
  d->MenuComboBox->setMinimumContentsLength(characters);
}

// -------------------------------------------------------------------------
QComboBox* ctkMenuComboBox::menuComboBoxInternal() const
{
  Q_D(const ctkMenuComboBox);
  return d->MenuComboBox;
}

// -------------------------------------------------------------------------
QToolButton* ctkMenuComboBox::toolButtonInternal() const
{
  Q_D(const ctkMenuComboBox);
  return d->SearchButton;
}

// -------------------------------------------------------------------------
ctkCompleter* ctkMenuComboBox::searchCompleter() const
{
  Q_D(const ctkMenuComboBox);
  return d->SearchCompleter;
}

// -------------------------------------------------------------------------
void ctkMenuComboBox::onActionSelected(QAction* action)
{
  Q_D(ctkMenuComboBox);
  /// Set the action selected in the combobox.

  d->IsDefaultTextCurrent = true;
  QString newText = d->DefaultText;
  if (action && !action->text().isEmpty())
    {
    newText = action->text();
    d->IsDefaultTextCurrent = false;
    }
  d->setCurrentText(newText);

  d->IsDefaultIconCurrent = true;
  QIcon newIcon = d->DefaultIcon;
  if (action && !action->icon().isNull())
    {
    d->IsDefaultIconCurrent = false;
    newIcon = action->icon();
    }
  d->setCurrentIcon(newIcon);

  d->MenuComboBox->clearFocus();

  emit ctkMenuComboBox::actionChanged(action);
}

// -------------------------------------------------------------------------
void ctkMenuComboBox::clearActiveAction()
{
  this->onActionSelected(0);
}

// -------------------------------------------------------------------------
void ctkMenuComboBox::onEditingFinished()
{
  Q_D(ctkMenuComboBox);
  if (!d->MenuComboBox->lineEdit())
    {
    return;
    }
  QAction* action = d->actionByTitle(d->MenuComboBox->lineEdit()->text(), d->CompleterMenu);
  if (!action)
    {
    return;
    }
  if (this->isSearchIconVisible())
    {
    d->SearchButton->setChecked(false);
    }

  action->trigger();
}

// -------------------------------------------------------------------------
bool ctkMenuComboBox::eventFilter(QObject* target, QEvent* event)
{
  Q_D(ctkMenuComboBox);

  if (target == d->MenuComboBox)
    {
    if (event->type() == QEvent::Resize)
      {
      this->layout()->invalidate();
      }
    if (event->type() == QEvent::FocusIn &&
        d->EditBehavior == ctkMenuComboBox::EditableOnFocus)
      {
      d->setComboBoxEditable(true);
      }
    if (event->type() == QEvent::FocusOut &&
        (d->EditBehavior == ctkMenuComboBox::EditableOnFocus ||
         d->EditBehavior == ctkMenuComboBox::EditableOnPopup))
      {
      d->setComboBoxEditable(false);
      }
    }
  else if (event->type() == QEvent::ActionAdded)
    {
    QActionEvent* actionEvent = static_cast<QActionEvent *>(event);
    d->addAction(actionEvent->action());
    }
  else if (event->type() == QEvent::ActionRemoved)
    {
    QActionEvent* actionEvent = static_cast<QActionEvent *>(event);
    QAction* action = actionEvent->action();
    // Maybe the action is present multiple times in different submenus
    // Don't remove its entry from the completer model if there are still some action instances
    // in the menus.
    if (!d->actionByTitle(action->text(), this->menu()))
      {
      d->removeActionFromCompleter(action);
      }
    }
  return this->Superclass::eventFilter(target, event);
}
