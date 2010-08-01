/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

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

#ifndef __ctkActionsWidget_h
#define __ctkActionsWidget_h

// Qt includes
#include <QWidget>

// CTK includes
#include "ctkPimpl.h"
#include "CTKWidgetsExport.h"
class ctkActionsWidgetPrivate;

class QAction;
class QStandardItemModel;
class QStandardItem;
class QTreeView;

class CTK_WIDGETS_EXPORT ctkActionsWidget : public QWidget
{
  Q_OBJECT
  //Q_PROPERTY(bool showActionsWithNoShortcut read showActionsWithNoShortcut write setShowActionsWithNoShortcut)
public:
  explicit ctkActionsWidget(QWidget* parent = 0);
  
  void addAction(QAction* action, const QString& category = QString());
  void addActions(QList<QAction*> actions, const QString& category = QString());

  QStandardItem* groupItem(const QString& category);

  //void setShowActionsWithNoShortcut(bool show);
  //bool showActionsWithNoShortcut()const;

  QStandardItemModel* model()const;
  QTreeView* view()const;
protected slots:
  void updateAction();

private:
  CTK_DECLARE_PRIVATE(ctkActionsWidget);
};

#endif
