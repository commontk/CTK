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

  This file was originally developed by Davide Punzo, punzodavide@hotmail.it,
  and development was supported by the Program for Intelligent Image-Guided Interventions (PI3).

=========================================================================*/

// Qt includes
#include <QApplication>
#include <QFile>
#include <QIcon>
#include <QTimer>
#include <QRect>
#include <QStyleOptionViewItem>

// ctkCore includes
#include <ctkCoreTestingMacros.h>

// STD includes
#include <iostream>

// ctkDICOMWidgets includes
#include "ctkDICOMSeriesDelegate.h"

int ctkDICOMSeriesDelegateTest1(int argc, char* argv[])
{
  QApplication app(argc, argv);

  QStringList arguments = app.arguments();
  QString testName = arguments.takeFirst();
  Q_UNUSED(testName);
  bool interactive = arguments.removeOne("-I");

  // Create delegate
  ctkDICOMSeriesDelegate delegate;

  // Test default values
  CHECK_INT(delegate.spacing(), 4);
  CHECK_INT(delegate.cornerRadius(), 8);

  // Test setting spacing
  delegate.setSpacing(10);
  CHECK_INT(delegate.spacing(), 10);

  // Test setting corner radius
  delegate.setCornerRadius(12);
  CHECK_INT(delegate.cornerRadius(), 12);

  // Test size hint with invalid index
  QStyleOptionViewItem option;
  QModelIndex index;
  QSize size = delegate.sizeHint(option, index);
  CHECK_BOOL(size.isValid(), true);

  // The corners of the thumbnail each have their own badge: the query/retrieve
  // status in the lower left, the selection in the upper right, the context menu
  // button in the lower right
  const QRect itemRect(0, 0, 200, 220);
  const QRect statusRect = delegate.statusButtonRect(itemRect, index);
  const QRect selectionRect = delegate.selectionBadgeRect(itemRect, index);
  const QRect contextMenuRect = delegate.contextMenuButtonRect(itemRect, index);

  CHECK_BOOL(statusRect.isValid(), true);
  CHECK_BOOL(selectionRect.isValid(), true);
  CHECK_BOOL(statusRect.left() < selectionRect.left(), true);
  CHECK_BOOL(statusRect.top() > selectionRect.top(), true);
  // The selection badge sits above the context menu button, on the same side
  CHECK_INT(selectionRect.left(), contextMenuRect.left());
  CHECK_BOOL(selectionRect.top() < contextMenuRect.top(), true);
  // and the status badge faces it across the thumbnail
  CHECK_INT(statusRect.top(), contextMenuRect.top());
  // The badges do not overlap each other
  CHECK_BOOL(statusRect.intersects(selectionRect), false);
  CHECK_BOOL(statusRect.intersects(contextMenuRect), false);
  CHECK_BOOL(selectionRect.intersects(contextMenuRect), false);
  // and a click in the lower left corner hits the status badge
  CHECK_BOOL(delegate.isStatusButtonAt(statusRect.center(), itemRect, index), true);
  CHECK_BOOL(delegate.isStatusButtonAt(selectionRect.center(), itemRect, index), false);

  // Every icon the delegate can draw must be in the resources: a missing one is not
  // reported anywhere, it just leaves the badge empty
  QStringList delegateIcons;
  delegateIcons << ":/Icons/cloud.svg"           // queried, still on the server
                << ":/Icons/cloud_download.svg"  // retrieve in progress
                << ":/Icons/accept2.svg"         // retrieved
                << ":/Icons/cloud_alert.svg"     // retrieve failed
                << ":/Icons/loaded.svg"          // loaded by the application
                << ":/Icons/more_vert.svg"       // context menu button
                << ":/Icons/radio_button_checked.svg" // selected
                << ":/Icons/grid.svg"
                << ":/Icons/stack.svg";
  foreach (const QString& iconPath, delegateIcons)
  {
    if (!QFile::exists(iconPath) || QIcon(iconPath).pixmap(16, 16).isNull())
    {
      std::cerr << "Line " << __LINE__ << " - " << qPrintable(iconPath)
                << " is not in the widget resources" << std::endl;
      return EXIT_FAILURE;
    }
  }

  if (!interactive)
  {
    QTimer::singleShot(200, &app, SLOT(quit()));
  }

  return app.exec();
}
