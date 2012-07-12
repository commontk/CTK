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

#ifndef __ctkDirectoryListWidget_p_h
#define __ctkDirectoryListWidget_p_h

// Qt includes
#include <QObject>
#include <QStringList>

// CTK includes
#include "ctkDirectoryListWidget.h"
#include "ui_ctkDirectoryListWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Widgets

class ctkDirectoryListWidgetPrivate : public QObject, public Ui_ctkDirectoryListWidget
{
  Q_OBJECT
  Q_DECLARE_PUBLIC(ctkDirectoryListWidget);
protected:
  ctkDirectoryListWidget* const q_ptr;
public:
  explicit ctkDirectoryListWidgetPrivate(ctkDirectoryListWidget& object);
  virtual ~ctkDirectoryListWidgetPrivate();

  void init();
  void setupUi(QWidget * parent);

  void setDirectoryList(const QStringList& list);
  QStringList directoryList() const;

public Q_SLOTS:
  void onAddClicked();
  void onRemoveClicked();
  void onExpandClicked();
  void onDirectoryListChanged();

public:
};

#endif
