/*=========================================================================

  Library:   CTK

  Copyright (c) Isomics Inc.

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
#include <QLabel>
#include <QDir>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QResource>
#include <QSettings>
#include <QVBoxLayout>

// CTK widget includes
#include <ctkCollapsibleGroupBox.h>
#include <ctkDirectoryButton.h>
#include <ctkDICOMVisualBrowserWidget.h>

// STD includes
#include <iostream>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  app.setOrganizationName("commontk");
  app.setOrganizationDomain("commontk.org");
  app.setApplicationName("ctkDICOM");

  QSettings settings;
  QString databaseDirectory;

  // set up the database
  if (argc > 1)
  {
    QString directory(argv[1]);
    settings.setValue("DatabaseDirectory", directory);
    settings.sync();
  }

  if (settings.value("DatabaseDirectory", "") == "")
  {
    databaseDirectory = QString("./ctkDICOM-Database");
    std::cerr << "No DatabaseDirectory on command line or in settings.  Using \"" << qPrintable(databaseDirectory) << "\".\n";
  }
  else
  {
    databaseDirectory = settings.value("DatabaseDirectory", "").toString();
  }

  QDir qdir(databaseDirectory);
  if (!qdir.exists(databaseDirectory))
  {
    if (!qdir.mkpath(databaseDirectory))
    {
      std::cerr << "Could not create database directory \"" << qPrintable(databaseDirectory) << "\".\n";
      return EXIT_FAILURE;
    }
  }

  // set up Qt resource files
  QResource::registerResource("./Resources/ctkDICOM.qrc");

  QWidget mainWidget;
  mainWidget.setObjectName(QString::fromUtf8("MainWidget"));
  mainWidget.setWindowTitle(QString::fromUtf8("DICOM Visual Browser"));

  QVBoxLayout mainLayout;
  mainLayout.setObjectName(QString::fromUtf8("mainLayout"));
  mainLayout.setContentsMargins(1, 1, 1, 1);

  QHBoxLayout topLayout;
  topLayout.setObjectName(QString::fromUtf8("topLayout"));
  topLayout.setContentsMargins(1, 1, 1, 1);

  QLabel databaseNameLabel;
  databaseNameLabel.setObjectName(QString::fromUtf8("DatabaseNameLabel"));
  databaseNameLabel.setMaximumSize(QSize(100, 30));
  topLayout.addWidget(&databaseNameLabel);

  ctkDirectoryButton directoryButton;
  directoryButton.setObjectName(QString::fromUtf8("DirectoryButton"));
  directoryButton.setMinimumSize(QSize(200, 30));
  directoryButton.setDirectory(databaseDirectory);

  topLayout.addWidget(&directoryButton);

  mainLayout.addLayout(&topLayout);

  ctkDICOMVisualBrowserWidget DICOMVisualBrowser;
  DICOMVisualBrowser.setObjectName(QString::fromUtf8("DICOMVisualBrowser"));
  DICOMVisualBrowser.setDatabaseDirectorySettingsKey("DatabaseDirectory");
  DICOMVisualBrowser.setMinimumSize(QSize(1000, 1000));
  DICOMVisualBrowser.setDatabaseDirectory(databaseDirectory);

  QObject::connect(&directoryButton, SIGNAL(directoryChanged(const QString&)),
                   &DICOMVisualBrowser, SLOT(setDatabaseDirectory(const QString&)));

  mainLayout.addWidget(&DICOMVisualBrowser);
  mainWidget.setLayout(&mainLayout);
  mainWidget.show();
  DICOMVisualBrowser.onShowPatients();

  return app.exec();
}
