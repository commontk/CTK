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

// Qt includes
#include <QApplication>
#include <QCheckBox>
#include <QSettings>
#include <QStyle>
#include <QTimer>

// CTK includes
#include "ctkMessageBox.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkMessageBoxDontShowAgainTest(int argc, char * argv [] )
{
  QApplication app(argc, argv);
  /// set the names for QSettings to work
  app.setOrganizationName("CommonToolKit");
  app.setOrganizationDomain("www.commontk.org");
  app.setApplicationName("CTK");

  ctkMessageBox confirmDialog;

  // Test default values.
  if (confirmDialog.isDontShowAgainVisible() != false ||
      confirmDialog.dontShowAgain() != false ||
      confirmDialog.dontShowAgainSettingsKey().isEmpty() != true)
    {
    std::cerr << "ctkMessageBox default values failed" << std::endl;
    return EXIT_FAILURE;
    }
  confirmDialog.setText("Are you sure you want to exit?");
  confirmDialog.setIcon(QMessageBox::Question);
  confirmDialog.setDontShowAgainVisible(true);

  QSettings settings;
  settings.setValue("DontShow", true);

  confirmDialog.setDontShowAgainSettingsKey("DontShow");
  if (confirmDialog.dontShowAgainSettingsKey() != "DontShow")
    {
    std::cerr << "ctkMessageBox::setDontShowAgainSettingsKey failed:"
              << confirmDialog.dontShowAgainSettingsKey().toStdString() << std::endl;
    return EXIT_FAILURE;
    }
  if (confirmDialog.dontShowAgain() != true)
    {
    std::cerr << "ctkMessageBox::setDontShowAgainSettingsKey failed:"
              << confirmDialog.dontShowAgain() << std::endl;
    return EXIT_FAILURE;
    }

  // exec() should return automatically because DontShowAgain is true
  if (confirmDialog.exec() != QDialog::Accepted)
    {
    std::cerr << "ctkMessageBox::exec failed:" << std::endl;
    return EXIT_FAILURE;
    }

  // test the static version
  if (ctkMessageBox::confirmExit("DontShow") != true)
    {
    std::cerr << "ctkMessageBox::confirmExit failed:" << std::endl;
    return EXIT_FAILURE;
    }

  confirmDialog.setDontShowAgain(false);

  // modal dialog
  confirmDialog.open();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &confirmDialog, SLOT(accept()));
    }
  return app.exec();
}
