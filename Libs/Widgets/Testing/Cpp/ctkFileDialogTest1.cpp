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
#include <QCommandLineParser>

// CTK includes
#include "ctkCoreTestingMacros.h"
#include "ctkFileDialog.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int testSelectionMode(ctkFileDialog* fileDialog, bool supportSelection)
{
  fileDialog->setSelectionMode(QAbstractItemView::ExtendedSelection);
  CHECK_INT(fileDialog->selectionMode(),
            static_cast<int>(
              supportSelection ? QAbstractItemView::ExtendedSelection : QAbstractItemView::NoSelection));

  // Due to limitation of QFileDialog API, calling setFileMode resets
  // the selection mode.

  fileDialog->setSelectionMode(QAbstractItemView::ExtendedSelection);
  fileDialog->setFileMode(QFileDialog::Directory);
  CHECK_INT(fileDialog->selectionMode(),
            static_cast<int>(
              supportSelection ? QAbstractItemView::SingleSelection : QAbstractItemView::NoSelection));

  fileDialog->setSelectionMode(QAbstractItemView::ExtendedSelection);
  fileDialog->setFileMode(QFileDialog::Directory);
  fileDialog->setOption(QFileDialog::ShowDirsOnly);
  CHECK_INT(fileDialog->selectionMode(),
            static_cast<int>(
              supportSelection ? QAbstractItemView::SingleSelection : QAbstractItemView::NoSelection));

  fileDialog->setSelectionMode(QAbstractItemView::ExtendedSelection);
  fileDialog->setFileMode(QFileDialog::ExistingFile);
  CHECK_INT(fileDialog->selectionMode(),
            static_cast<int>(
              supportSelection ? QAbstractItemView::SingleSelection : QAbstractItemView::NoSelection));

  fileDialog->setSelectionMode(QAbstractItemView::ExtendedSelection);
  fileDialog->setFileMode(QFileDialog::ExistingFiles);
  CHECK_INT(fileDialog->selectionMode(),
            static_cast<int>(
              supportSelection ? QAbstractItemView::ExtendedSelection : QAbstractItemView::NoSelection));

  fileDialog->setSelectionMode(QAbstractItemView::ExtendedSelection);
  fileDialog->setFileMode(QFileDialog::AnyFile);
  CHECK_INT(fileDialog->selectionMode(),
            static_cast<int>(
              supportSelection ? QAbstractItemView::SingleSelection : QAbstractItemView::NoSelection));

  return EXIT_SUCCESS;
}

//-----------------------------------------------------------------------------
int ctkFileDialogTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QCommandLineParser parser;
  parser.addOptions(
  {
          {"I", "Run in interactive mode"},
          {"do-not-use-native-dialogs", "Do not use native dialogs"},
  });
  parser.process(app); // Automatically exit if there is a parsing error

  bool skipNativeDialogs = parser.isSet("do-not-use-native-dialogs");
  QApplication::setAttribute(Qt::AA_DontUseNativeDialogs, skipNativeDialogs);

  ctkFileDialog fileDialog;
  CHECK_BOOL(fileDialog.testOption(ctkFileDialog::DontUseNativeDialog), skipNativeDialogs);
  fileDialog.setFileMode(QFileDialog::AnyFile);
  fileDialog.setNameFilter("Images (*.png *.xpm *.jpg)");
  fileDialog.setViewMode(QFileDialog::Detail);
  QCheckBox* checkBox = new QCheckBox;
  fileDialog.setBottomWidget(checkBox, "Foo Bar:");

  // A bottom widget can be associated with the file dialog only
  // if using the non-native dialog.
  bool supportBottomWidget = skipNativeDialogs;

  CHECK_POINTER(fileDialog.bottomWidget(), supportBottomWidget ? checkBox : nullptr);

  CHECK_EXIT_SUCCESS(testSelectionMode(&fileDialog, supportBottomWidget));

  // the following is only in interactive mode
  if (!parser.isSet("I"))
  {
    return EXIT_SUCCESS;
  }

  if (supportBottomWidget)
  {
    QObject::connect(fileDialog.bottomWidget(), SIGNAL(toggled(bool)),
                     &fileDialog, SLOT(setAcceptButtonEnable(bool)));
  }
  fileDialog.setAcceptButtonEnable(false);
  if (!fileDialog.exec())
  {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;

}
