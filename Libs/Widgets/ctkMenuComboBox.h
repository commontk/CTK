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

#ifndef __ctkMenuComboBox_h
#define __ctkMenuComboBox_h

// Qt includes
#include <QMenu>
#include <QMetaType>
#include <QWidget>
class QComboBox;
class QToolButton;

// CTK includes
#include "ctkWidgetsExport.h"
class ctkCompleter;
class ctkMenuComboBoxPrivate;

/// \ingroup Widgets
/// QComboBox linked with a QMenu. See ctkMenuComboBox::setMenu()
/// ctkMenuComboBox can be editable, disable,
/// editable on focus or editable on double click.
///   if it is editable :
/// the comboBox is always editable, you can filter the Menu or show it.
///   if it is editable on focus - on double click:
/// the combobox become editable when it has the focus in.
/// So ctkMenuComboBox's purpose is to filter a menu, if you edit the current text
/// or show the menu, if you click on the arrow.
///   if it is disabled :
/// the ctkMenuComboBox has the same behavior as a QPushButton. You can't filter the menu.

/// By default ctkMenuComboBox is not editable with the search icon visible.
/// See ctkmenuComboBox::setEditableType() to change the default behavior.
/// and setIconSearchVisible() to show/hide the icon.

class CTK_WIDGETS_EXPORT ctkMenuComboBox : public QWidget
{
  Q_OBJECT
  Q_ENUMS(EditableBehavior)
  /// This property holds the text shown on the combobox when there is no
  /// selected item.
  /// Empty by default.
  Q_PROPERTY(QString defaultText READ defaultText WRITE setDefaultText)
  /// This property holds the icon shown on the combobox when the current item
  /// (QAction) doesn't have any icon associated.
  /// Empty by default
  Q_PROPERTY(QIcon defaultIcon READ defaultIcon WRITE setDefaultIcon)
  /// This property holds the edit behavior of the combobox, it defines what
  /// action is needed to turn the combobox into a search mode where the user
  /// can type the name of the item to select using the combobox line edit.
  /// ctkMenuComboBox::NotEditable by default
  /// \sa EditableType
  Q_PROPERTY(EditableBehavior editBehavior READ editableBehavior WRITE setEditableBehavior)
  /// This property controls whether the search tool button is visible or hidden.
  /// True by default
  Q_PROPERTY(bool searchIconVisible READ isSearchIconVisible WRITE setSearchIconVisible)
  /// This property holds whether the search tool button displays an icon only,
  /// text only, or text beside/below the icon.
  /// The default is Qt::ToolButtonIconOnly.
  /// \sa QToolButton::toolButtonStyle
  Q_PROPERTY(Qt::ToolButtonStyle toolButtonStyle READ toolButtonStyle WRITE setToolButtonStyle)
public:
  enum EditableBehavior{
    NotEditable = 0,
    Editable,
    EditableOnFocus,
    EditableOnPopup
  };

  /// Superclass typedef
  typedef QWidget Superclass;

  ///
  ctkMenuComboBox(QWidget* parent = 0);
  virtual ~ctkMenuComboBox();

  /// Set menu to both the QComboBox and the associated ctkCompleter.
  /// \sa setCompleterMenu(), searchCompleter()
  Q_INVOKABLE void setMenu(QMenu* menu);
  Q_INVOKABLE QMenu* menu()const;

  /// Set a specific menu to the ctkCompleter.
  ///
  /// This is useful when the menu displayed with the combobox is only a subset
  /// of the action that can be searched for.
  /// \sa setMenu(), searchCompleter()
  Q_INVOKABLE void setCompleterMenu(QMenu* menu);
  Q_INVOKABLE QMenu* completerMenu()const;

  void setDefaultText(const QString&);
  QString defaultText()const;

  void setDefaultIcon(const QIcon&);
  QIcon defaultIcon()const;

  void setEditableBehavior(EditableBehavior editBehavior);
  EditableBehavior editableBehavior()const;

  void setSearchIconVisible(bool state);
  bool isSearchIconVisible() const;

  Qt::ToolButtonStyle toolButtonStyle() const;

  /// Set the minimum width of the combobox.
  /// \sa QComboBox::setMinimumContentsLength()
  void setMinimumContentsLength(int characters);

  /// Return the internal combo box
  QComboBox* menuComboBoxInternal() const;

  /// Return the internal tool button
  QToolButton* toolButtonInternal() const;

  /// Return the internal completer
  ctkCompleter* searchCompleter() const;

protected:
  virtual bool eventFilter(QObject* target, QEvent* event);

public Q_SLOTS:
  void clearActiveAction();
  void setToolButtonStyle(Qt::ToolButtonStyle style);

Q_SIGNALS:
  void actionChanged(QAction* action);
  void popupShown();

protected Q_SLOTS:
  /// Change the current text/icon on the QComboBox
  /// And trigger the action.
  /// action selected from the menu.
  void onActionSelected(QAction* action);
  /// action selected from the line edit or the completer.
  void onEditingFinished();

protected:
  QScopedPointer<ctkMenuComboBoxPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkMenuComboBox);
  Q_DISABLE_COPY(ctkMenuComboBox);
};

Q_DECLARE_METATYPE(ctkMenuComboBox::EditableBehavior)

#endif
