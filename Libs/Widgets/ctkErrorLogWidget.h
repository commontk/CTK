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
public:
  typedef QWidget Superclass;
  explicit ctkErrorLogWidget(QWidget* parentWidget = 0);
  virtual ~ctkErrorLogWidget();

  ctkErrorLogAbstractModel* errorLogModel()const;
  Q_INVOKABLE void setErrorLogModel(ctkErrorLogAbstractModel * newErrorLogModel);

  /// Hide table column identified by /a columnId.
  /// \sa ctkErrorLogModel::ColumnsIds
  Q_INVOKABLE void setColumnHidden(int columnId, bool hidden) const;

public Q_SLOTS:
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
  QScopedPointer<ctkErrorLogWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkErrorLogWidget);
  Q_DISABLE_COPY(ctkErrorLogWidget);
};

#endif
