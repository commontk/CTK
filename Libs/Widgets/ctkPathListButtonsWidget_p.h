/*=========================================================================

  Library:   CTK

  Copyright (c) University College London.

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

#ifndef __ctkPathListButtonsWidget_p_h
#define __ctkPathListButtonsWidget_p_h

// Qt includes
#include <QObject>
#include <QItemSelection>

// CTK includes
#include "ui_ctkPathListButtonsWidget.h"

class ctkPathListButtonsWidget;
class ctkPathListWidget;

class QFileDialog;

//-----------------------------------------------------------------------------
class ctkPathListButtonsWidgetPrivate : public QObject, public Ui_ctkPathListButtonsWidget
{
  Q_OBJECT
  Q_DECLARE_PUBLIC(ctkPathListButtonsWidget)

protected:

  ctkPathListButtonsWidget* const q_ptr;

public:

  explicit ctkPathListButtonsWidgetPrivate(ctkPathListButtonsWidget& object);
  virtual ~ctkPathListButtonsWidgetPrivate();

  void init();
  void setupUi(QWidget * parent);

public Q_SLOTS:

  void on_AddFilesButton_clicked();
  void on_AddDirButton_clicked();
  void on_RemoveButton_clicked();
  void on_EditButton_clicked();

  void on_PathListWidget_selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

public:

  ctkPathListWidget* PathListWidget;

private:

  QStringList openAddFilesDialog(bool multiple = true);
  QStringList openAddDirDialog();

  void addPathsWithWarningMessage(const QStringList& paths);
};

#endif
