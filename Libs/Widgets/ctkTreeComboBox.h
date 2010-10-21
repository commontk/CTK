/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

#ifndef __ctkTreeComboBox_h
#define __ctkTreeComboBox_h

// Qt includes
#include <QComboBox>

// CTK includes
#include <ctkPimpl.h>

#include "ctkWidgetsExport.h"

class ctkTreeComboBoxPrivate;
class QTreeView;

/// Description:
/// ComboBox that displays the items as a tree view.
/// See below for a use case:
///    ctkTreeComboBox combo;
///    QStandardItemModel model;
///    model.appendRow(new QStandardItem("Test1"));
///    model.item(0)->appendRow(new QStandardItem("Test1.1"));
///    model.item(0)->appendRow(new QStandardItem("Test1.2"));
///    model.item(0)->appendRow(new QStandardItem("Test1.3"));
///    model.appendRow(new QStandardItem("Test2"));
///    model.appendRow(new QStandardItem("Test3"));
///    combo.setModel(&model);
///    combo.show();
/// TODO fix size of the view
class CTK_WIDGETS_EXPORT ctkTreeComboBox : public QComboBox
{
  Q_OBJECT
public:
  typedef QComboBox Superclass;
  explicit ctkTreeComboBox(QWidget* parent = 0);
  virtual ~ctkTreeComboBox();

  virtual bool eventFilter(QObject* object, QEvent* event);
  virtual void hidePopup();
  
  /// ctkTreeComboBox uses a QTreeView for its model view. treeView() is a
  /// utility function that cast QComboBox::view() into a QTreeView.
  /// \sa view()
  QTreeView* treeView()const;

protected:
  virtual void paintEvent(QPaintEvent*);
  
protected slots:
  void resizePopup();
  
protected:
  QScopedPointer<ctkTreeComboBoxPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkTreeComboBox);
  Q_DISABLE_COPY(ctkTreeComboBox);
};

#endif
