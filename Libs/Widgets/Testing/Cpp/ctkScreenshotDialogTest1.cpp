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
#include <QDir>
#include <QTemporaryDir>
#include <QTimer>

// CTK includes
#include "ctkScreenshotDialog.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkScreenshotDialogTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QWidget topLevel;

  ctkScreenshotDialog screenshotDialog;

  // Check default values
  if (screenshotDialog.widgetToGrab() != 0 ||
      screenshotDialog.baseFileName() != "Untitled" ||
      !screenshotDialog.directory().isEmpty() ||
      screenshotDialog.delay() != 0)
  {
    std::cerr << "ctkScreenshotDialog, bad default values: "
                << screenshotDialog.widgetToGrab() << " "
                << qPrintable(screenshotDialog.baseFileName()) << " "
                << qPrintable(screenshotDialog.directory()) << " "
                <<  screenshotDialog.delay() << std::endl;
    return EXIT_FAILURE;
  }

  screenshotDialog.setWidgetToGrab(&topLevel);
  if (screenshotDialog.widgetToGrab() != &topLevel)
  {
    std::cerr << "ctkScreenshotDialog::setWidgetToGrab failed: "
              << screenshotDialog.widgetToGrab() << " instead of  "
              << &topLevel << std::endl;
    return EXIT_FAILURE;
  }

  screenshotDialog.setBaseFileName("screenshot");
  if (screenshotDialog.baseFileName() != "screenshot")
  {
    std::cerr << "ctkScreenshotDialog::setBaseFileName failed: "
              << qPrintable(screenshotDialog.baseFileName()) << " instead of "
              << "\"screenshot\"." << std::endl;
    return EXIT_FAILURE;
  }

  screenshotDialog.setDirectory("..");
  if (screenshotDialog.directory() != "..")
  {
    std::cerr << "ctkScreenshotDialog::setDirectory failed: "
              << qPrintable(screenshotDialog.directory()) << " instead of "
              << ".." << std::endl;
    return EXIT_FAILURE;
  }

  // Use a temporary directory for the actual screenshot to avoid overwrite
  // confirmation dialogs that would block as a nested modal dialog.
  QTemporaryDir tempDir;
  if (!tempDir.isValid())
  {
    std::cerr << "Failed to create temporary directory." << std::endl;
    return EXIT_FAILURE;
  }
  screenshotDialog.setDirectory(tempDir.path());

  screenshotDialog.setDelay(1);
  if (screenshotDialog.delay() != 1)
  {
    std::cerr << "ctkScreenshotDialog::setDelay failed: "
              << screenshotDialog.delay() << " instead of 2." << std::endl;
    return EXIT_FAILURE;
  }

  topLevel.show();

  if (argc < 2 || QString(argv[1]) != "-I" )
  {
    // Close the modal dialog after the screenshot is taken.
    // QApplication::quit() does not close modal dialogs in Qt6,
    // so we close the dialog directly.
    QTimer* closeTimer = new QTimer(&screenshotDialog);
    closeTimer->setSingleShot(true);
    closeTimer->setInterval(1400);
    QObject::connect(closeTimer, &QTimer::timeout, &screenshotDialog, &QDialog::reject);
    closeTimer->start();
  }

  screenshotDialog.saveScreenshot(); // 1000msecs should be enough
  return screenshotDialog.exec();
}
