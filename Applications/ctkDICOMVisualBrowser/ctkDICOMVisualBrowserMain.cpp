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
#include <QCheckBox>
#include <QLabel>
#include <QDir>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QResource>
#include <QSettings>
#include <QStyle>
#include <QStyleFactory>
#include <QVBoxLayout>

// CTK widget includes
#include <ctkCollapsibleGroupBox.h>
#include <ctkDirectoryButton.h>
#include <ctkDICOMVisualBrowserWidget.h>

// STD includes
#include <iostream>

namespace
{

//----------------------------------------------------------------------------
QPalette darkPalette()
{
  QPalette palette;
  palette.setColor(QPalette::Window, QColor(50, 50, 50));
  palette.setColor(QPalette::WindowText, Qt::white);
  palette.setColor(QPalette::Base, QColor(30, 30, 30));
  palette.setColor(QPalette::AlternateBase, QColor(50, 50, 50));
  palette.setColor(QPalette::ToolTipBase, QColor(255, 255, 220));
  palette.setColor(QPalette::ToolTipText, Qt::black);
  palette.setColor(QPalette::Text, Qt::white);
  palette.setColor(QPalette::Button, QColor(50, 50, 50));
  palette.setColor(QPalette::ButtonText, Qt::white);
  palette.setColor(QPalette::BrightText, QColor(255, 80, 80));
  palette.setColor(QPalette::Light, QColor(70, 70, 70));
  palette.setColor(QPalette::Midlight, QColor(60, 60, 60));
  palette.setColor(QPalette::Mid, QColor(40, 40, 40));
  palette.setColor(QPalette::Dark, QColor(30, 30, 30));
  palette.setColor(QPalette::Shadow, Qt::black);
  palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
  palette.setColor(QPalette::HighlightedText, Qt::white);
  palette.setColor(QPalette::Link, QColor(42, 130, 218));
  palette.setColor(QPalette::LinkVisited, QColor(120, 100, 220));
  palette.setColor(QPalette::PlaceholderText, QColor(160, 160, 160));
  foreach (QPalette::ColorRole role, QList<QPalette::ColorRole>()
           << QPalette::WindowText << QPalette::Text << QPalette::ButtonText)
  {
    palette.setColor(QPalette::Disabled, role, QColor(127, 127, 127));
  }
  return palette;
}

} // end of anonymous namespace

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
  mainWidget.resize(1600, 1200);

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

  // Light and dark styles, switched while the browser is shown: the browser follows
  // the palette of the application. The dark style uses Fusion, which paints with the
  // palette, while the style of the platform may not.
  const QString lightStyleName = app.style()->objectName();
  const QPalette lightPalette = app.palette();
  QCheckBox darkModeCheckBox(QObject::tr("Dark mode"));
  darkModeCheckBox.setObjectName(QString::fromUtf8("DarkModeCheckBox"));
  darkModeCheckBox.setToolTip(QObject::tr("Switch between the light and dark styles of the application"));
  QObject::connect(&darkModeCheckBox, &QCheckBox::toggled, [&](bool dark)
  {
    app.setStyle(QStyleFactory::create(dark ? QString("Fusion") : lightStyleName));
    app.setPalette(dark ? darkPalette() : lightPalette);
    settings.setValue("DarkMode", dark);
  });
  darkModeCheckBox.setChecked(settings.value("DarkMode", false).toBool());
  topLayout.addWidget(&darkModeCheckBox);

  mainLayout.addLayout(&topLayout);

  ctkDICOMVisualBrowserWidget DICOMVisualBrowser;
  DICOMVisualBrowser.setObjectName(QString::fromUtf8("DICOMVisualBrowser"));
  DICOMVisualBrowser.setDatabaseDirectorySettingsKey("DatabaseDirectory");
  DICOMVisualBrowser.setDatabaseDirectory(databaseDirectory);

  QObject::connect(&directoryButton, SIGNAL(directoryChanged(QString)),
                   &DICOMVisualBrowser, SLOT(setDatabaseDirectory(QString)));

  mainLayout.addWidget(&DICOMVisualBrowser);
  mainWidget.setLayout(&mainLayout);
  mainWidget.show();

  return app.exec();
}
