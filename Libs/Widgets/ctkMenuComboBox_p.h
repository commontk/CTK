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
#include <QWeakPointer>

// CTK includes
#include "ctkMenuComboBox.h"
#include "ctkSearchBox.h"

class ctkMenuComboBoxInternal: public QComboBox
{
public:
  /// Superclass typedef
  typedef QComboBox Superclass;

  ctkMenuComboBoxInternal();
  virtual ~ctkMenuComboBoxInternal();
  virtual void showPopup();

  QWeakPointer<QMenu>  Menu;
};

// -------------------------------------------------------------------------
class ctkMenuComboBoxPrivate
{
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

  void setComboBoxEditable(bool);

  void addAction(QAction* action);
  void addMenuToCompleter(QMenu* menu);
  void addActionToCompleter(QAction* action);

  void removeActionToCompleter(QAction* action);

  QIcon         DefaultIcon;
  QString       DefaultText;
  bool          IsDefaultTextCurrent;
  bool          IsDefaultIconCurrent;

  ctkMenuComboBox::EditableBehavior EditBehavior;

  ctkMenuComboBoxInternal*    MenuComboBox;
  QCompleter*                 SearchCompleter;
  QWeakPointer<QMenu>         Menu;
};

#endif

