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

#ifndef __ctkMenuComboBox_p_h
#define __ctkMenuComboBox_p_h

// Qt includes
#include <QComboBox>
#include <QPointer>

// CTK includes
#include "ctkMenuComboBox.h"
class ctkCompleter;
class QToolButton;

/// \ingroup Widgets
class ctkMenuComboBoxInternal: public QComboBox
{
  Q_OBJECT
public:
  /// Superclass typedef
  typedef QComboBox Superclass;

  ctkMenuComboBoxInternal();
  virtual ~ctkMenuComboBoxInternal();
  virtual void showPopup();

  virtual QSize minimumSizeHint()const;
Q_SIGNALS:
  void popupShown();
public:
  QPointer<QMenu>  Menu;
};

// -------------------------------------------------------------------------
/// \ingroup Widgets
class ctkMenuComboBoxPrivate: public QObject
{
  Q_OBJECT
  Q_DECLARE_PUBLIC(ctkMenuComboBox);
protected:
  ctkMenuComboBox* const q_ptr;
public:
  ctkMenuComboBoxPrivate(ctkMenuComboBox& object);
  void init();
  QAction* actionByTitle(const QString& text, const QMenu* parentMenu);
  void setCurrentText(const QString& newCurrentText);
  QString currentText()const;

  void setCurrentIcon(const QIcon& newCurrentIcon);
  QIcon currentIcon()const;

  void addAction(QAction* action);
  void addMenuToCompleter(QMenu* menu);
  void addActionToCompleter(QAction* action);

  void removeAction(QAction* action);
  void removeMenuFromCompleter(QMenu* menu);
  void removeActionFromCompleter(QAction* action);

public Q_SLOTS:
  void setComboBoxEditable(bool editable = true);
  void onCompletion(const QString& text);

protected:
  QIcon         DefaultIcon;
  QString       DefaultText;
  bool          IsDefaultTextCurrent;
  bool          IsDefaultIconCurrent;

  ctkMenuComboBox::EditableBehavior EditBehavior;

  ctkMenuComboBoxInternal*    MenuComboBox;
  ctkCompleter*               SearchCompleter;
  QPointer<QMenu>             CompleterMenu;
  QToolButton*                SearchButton;
};

#endif

