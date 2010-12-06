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

// Qt includes
#include <QApplication>
#include <QCheckBox>
#include <QSettings>
#include <QStyle>
#include <QTimer>

// CTK includes
#include "ctkConfirmExitDialog.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkConfirmExitDialogTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);
  /// set the names for QSettings to work
  app.setOrganizationName("CommonToolKit");
  app.setOrganizationDomain("www.commontk.org");
  app.setApplicationName("CTK");

  ctkConfirmExitDialog confirmDialog;
  if (confirmDialog.dontShowAnymore() != false)
    {
    std::cerr << "ctkConfirmExitDialog::dontShowAnymore failed" << std::endl;
    return EXIT_FAILURE;
    }
  confirmDialog.setText("Are you sure you want to exit?");
  confirmDialog.setPixmap(confirmDialog.style()->standardPixmap(QStyle::SP_MessageBoxQuestion));
  
  QSettings settings;
  settings.setValue("DontShow", true);
  
  confirmDialog.setDontShowAnymoreSettingsKey("DontShow");
  if (confirmDialog.dontShowAnymoreSettingsKey() != "DontShow")
    {
    std::cerr << "ctkConfirmExitDialog::setDontShowAnymoreSettingsKey failed:"
              << confirmDialog.dontShowAnymoreSettingsKey().toStdString() << std::endl;
    return EXIT_FAILURE;
    }
  if (confirmDialog.dontShowAnymore() != true)
    {
    std::cerr << "ctkConfirmExitDialog::setDontShowAnymoreSettingsKey failed:"
              << confirmDialog.dontShowAnymore() << std::endl;
    return EXIT_FAILURE;
    }
  
  // exec() should return automatically because DontShowAnymore is true
  if (confirmDialog.exec() != QDialog::Accepted)
    {
    std::cerr << "ctkConfirmExitDialog::exec failed:" << std::endl;
    return EXIT_FAILURE;
    }
  
  // test the static version
  if (ctkConfirmExitDialog::confirmExit("DontShow") != true)
    {
    std::cerr << "ctkConfirmExitDialog::confirmExit failed:" << std::endl;
    return EXIT_FAILURE;
    }

  confirmDialog.setDontShowAnymore(false);
  // modal dialog
  confirmDialog.open();

  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &confirmDialog, SLOT(accept()));
    }
  return app.exec();
}
