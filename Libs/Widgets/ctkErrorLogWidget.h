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

#ifndef __ctkErrorLogWidget_h
#define __ctkErrorLogWidget_h

// Qt includes
#include <QWidget>

// CTK includes
#include "ctkWidgetsExport.h"

class ctkErrorLogAbstractModel;
class ctkErrorLogWidgetPrivate;
class QAbstractButton;
class QItemSelection;
class QModelIndex;

/// \ingroup Widgets
class CTK_WIDGETS_EXPORT ctkErrorLogWidget : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(Qt::Orientation layoutOrientation READ layoutOrientation WRITE setLayoutOrientation)
public:
  typedef QWidget Superclass;
  explicit ctkErrorLogWidget(QWidget* parentWidget = 0);
  virtual ~ctkErrorLogWidget();

  ctkErrorLogAbstractModel* errorLogModel()const;
  Q_INVOKABLE void setErrorLogModel(ctkErrorLogAbstractModel * newErrorLogModel);

  /// Hide table column identified by /a columnId.
  /// \sa ctkErrorLogModel::ColumnsIds
  Q_INVOKABLE void setColumnHidden(int columnId, bool hidden) const;

  /// This property describes how the list of errors and error description are organized.
  /// The orientation must be Qt::Horizontal (the widgets are side-by-side) or Qt::Vertical (the widgets are above-under).
  /// The default is Qt::Vertical.
  Qt::Orientation layoutOrientation() const;

Q_SIGNALS:

  /// Emitted if the user interacted with the widget to view messages
  /// (scrolled the message list, changed filter criteria, etc.).
  /// This can be useful if the application shows a "new message" notification
  /// when a new message is logged: the application can hide the notification
  /// when this signal is emitted, because it indicates that the user had a look
  /// at the messages.
  void userViewed();

public Q_SLOTS:
  void setLayoutOrientation(Qt::Orientation orientation);

  void setAllEntriesVisible(bool visibility = true);

  void setErrorEntriesVisible(bool visibility);

  void setWarningEntriesVisible(bool visibility);

  void setInfoEntriesVisible(bool visibility);
  
  void setUnknownEntriesVisible(bool visibility);

protected Q_SLOTS:
  void onRowsInserted(const QModelIndex &parent, int first, int last);

  void onLogLevelFilterChanged();

  void removeEntries();

  void onSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected);

protected:
  bool eventFilter(QObject* target, QEvent* event);

  QScopedPointer<ctkErrorLogWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkErrorLogWidget);
  Q_DISABLE_COPY(ctkErrorLogWidget);
};

#endif
