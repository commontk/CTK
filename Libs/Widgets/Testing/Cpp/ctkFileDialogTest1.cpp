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

// CTK includes
#include "ctkCoreTestingMacros.h"
#include "ctkFileDialog.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int testSelectionMode(ctkFileDialog* fileDialog)
{
  fileDialog->setSelectionMode(QAbstractItemView::ExtendedSelection);
  CHECK_INT(fileDialog->selectionMode(), static_cast<int>(QAbstractItemView::ExtendedSelection));

  // Due to limitation of QFileDialog API, calling setFileMode resets
  // the selection mode.

  fileDialog->setSelectionMode(QAbstractItemView::ExtendedSelection);
  fileDialog->setFileMode(QFileDialog::Directory);
  CHECK_INT(fileDialog->selectionMode(), static_cast<int>(QAbstractItemView::SingleSelection));

  fileDialog->setSelectionMode(QAbstractItemView::ExtendedSelection);
  fileDialog->setFileMode(QFileDialog::DirectoryOnly);
  CHECK_INT(fileDialog->selectionMode(), static_cast<int>(QAbstractItemView::SingleSelection));

  fileDialog->setSelectionMode(QAbstractItemView::ExtendedSelection);
  fileDialog->setFileMode(QFileDialog::ExistingFile);
  CHECK_INT(fileDialog->selectionMode(), static_cast<int>(QAbstractItemView::SingleSelection));

  fileDialog->setSelectionMode(QAbstractItemView::ExtendedSelection);
  fileDialog->setFileMode(QFileDialog::ExistingFiles);
  CHECK_INT(fileDialog->selectionMode(), static_cast<int>(QAbstractItemView::ExtendedSelection));

  fileDialog->setSelectionMode(QAbstractItemView::ExtendedSelection);
  fileDialog->setFileMode(QFileDialog::AnyFile);
  CHECK_INT(fileDialog->selectionMode(), static_cast<int>(QAbstractItemView::SingleSelection));

  return EXIT_SUCCESS;
}

//-----------------------------------------------------------------------------
int ctkFileDialogTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkFileDialog fileDialog;
  fileDialog.setFileMode(QFileDialog::AnyFile);
  fileDialog.setNameFilter("Images (*.png *.xpm *.jpg)");
  fileDialog.setViewMode(QFileDialog::Detail);
  QCheckBox* checkBox = new QCheckBox;
  fileDialog.setBottomWidget(checkBox, "Foo Bar:");
  if (checkBox != fileDialog.bottomWidget())
    {
    return EXIT_FAILURE;
    }

  CHECK_EXIT_SUCCESS(testSelectionMode(&fileDialog));

  // the following is only in interactive mode
  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    return EXIT_SUCCESS;
    }
  QObject::connect(checkBox, SIGNAL(toggled(bool)),
                   &fileDialog, SLOT(setAcceptButtonEnable(bool)));
  fileDialog.setAcceptButtonEnable(false);
  if (!fileDialog.exec())
    {
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;

}
