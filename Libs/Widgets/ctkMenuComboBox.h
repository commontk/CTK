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

// CTK includes
#include "ctkWidgetsExport.h"

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
  Q_PROPERTY(QString defaultText READ defaultText WRITE setDefaultText)
  Q_PROPERTY(QIcon defaultIcon READ defaultIcon WRITE setDefaultIcon)
  Q_PROPERTY(EditableBehavior editBehavior READ editableBehavior WRITE setEditableBehavior)
  Q_PROPERTY(bool searchIconVisible READ isSearchIconVisible WRITE setSearchIconVisible)

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

  /// Add a menu to the QcomboBox and set a QCompleter
  void setMenu(QMenu* menu);
  QMenu* menu()const;

  /// Empty by default
  /// set the first default text.
  void setDefaultText(const QString&);
  QString defaultText()const;

  /// Empty by default
  /// if a QAction doesn't have icon in the menu, the comboBox takes the defaultIcon.
  void setDefaultIcon(const QIcon&);
  QIcon defaultIcon()const;

  /// set/get the editableType; See enum EditableType for more details.
  void setEditableBehavior(EditableBehavior editBehavior);
  EditableBehavior editableBehavior()const;

  /// set the icon search visible
  void setSearchIconVisible(bool state);
  bool isSearchIconVisible() const;

  /// See QComboBox::setMinimumContentsLength()
  void setMinimumContentsLength(int characters);

protected:
  virtual bool eventFilter(QObject* target, QEvent* event);
  virtual void resizeEvent(QResizeEvent *event);

public Q_SLOTS:
  void clearActiveAction();

Q_SIGNALS:
  void actionChanged(QAction* action);

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
