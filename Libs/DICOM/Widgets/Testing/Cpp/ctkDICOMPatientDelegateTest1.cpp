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
#include <QTimer>
#include <QStyleOptionViewItem>

// ctkCore includes
#include <ctkCoreTestingMacros.h>

// ctkDICOMWidgets includes
#include "ctkDICOMPatientDelegate.h"

int ctkDICOMPatientDelegateTest1(int argc, char* argv[])
{
  QApplication app(argc, argv);

  QStringList arguments = app.arguments();
  QString testName = arguments.takeFirst();
  Q_UNUSED(testName);
  bool interactive = arguments.removeOne("-I");

  // Create delegate
  ctkDICOMPatientDelegate delegate;

  // Test default values
  CHECK_INT(delegate.spacing(), 4);
  CHECK_INT(delegate.iconSize(), 24);
  CHECK_INT(delegate.cornerRadius(), 8);
  CHECK_INT(delegate.tabModeHeight(), 34);
  CHECK_INT(delegate.patientHeaderHeight(), 100);

  // Test setting spacing
  delegate.setSpacing(10);
  CHECK_INT(delegate.spacing(), 10);

  // Test setting icon size
  delegate.setIconSize(32);
  CHECK_INT(delegate.iconSize(), 32);

  // Test setting corner radius
  delegate.setCornerRadius(12);
  CHECK_INT(delegate.cornerRadius(), 12);

  // Test setting tab mode height
  delegate.setTabModeHeight(60);
  CHECK_INT(delegate.tabModeHeight(), 60);

  // Test setting patient header height
  delegate.setPatientHeaderHeight(100);
  CHECK_INT(delegate.patientHeaderHeight(), 100);

  // Test size hint with invalid index
  QStyleOptionViewItem option;
  QModelIndex index;
  QSize size = delegate.sizeHint(option, index);
  CHECK_BOOL(size.isValid(), true);

  if (!interactive)
  {
    QTimer::singleShot(200, &app, SLOT(quit()));
  }

  return app.exec();
}
