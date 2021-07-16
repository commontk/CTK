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
#include <QAction>
#include <QApplication>
#include <QCoreApplication>
#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QDebug>
#include <QDialogButtonBox>
#include <QFile>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QFormLayout>
#include <QListView>
#include <QMenu>
#include <QMessageBox>
#include <QProgressDialog>
#include <QPushButton>
#include <QStringListModel>
#include <QSettings>
#include <QTableView>
#include <QWidgetAction>

// crtkCore includes
#include "ctkUtils.h"

// ctkWidgets includes
#include "ctkDirectoryButton.h"
#include "ctkFileDialog.h"
#include "ctkMessageBox.h"

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMIndexer.h"

// ctkDICOMWidgets includes
#include "ctkDICOMBrowser.h"
#include "ctkDICOMObjectListWidget.h"
#include "ctkDICOMQueryResultsTabWidget.h"
#include "ctkDICOMQueryRetrieveWidget.h"
#include "ctkDICOMQueryWidget.h"
#include "ctkDICOMTableManager.h"
#include "ctkDICOMTableView.h"

#include "ui_ctkDICOMBrowser.h"

class ctkDICOMMetadataDialog : public QDialog
{
public:
  ctkDICOMMetadataDialog(QWidget *parent = 0)
    : QDialog(parent)
  {
    this->setWindowFlags(Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint | Qt::Window);
    this->setModal(true);
    this->setSizeGripEnabled(true);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    this->tagListWidget = new ctkDICOMObjectListWidget();
    layout->addWidget(this->tagListWidget);
  }

  virtual ~ctkDICOMMetadataDialog()
  {
  }

  void setFileList(const QStringList& fileList)
  {
    this->tagListWidget->setFileList(fileList);
  }

  void closeEvent(QCloseEvent *evt)
  {
    // just hide the window when close button is clicked
    evt->ignore();
    this->hide();
  }

  void showEvent(QShowEvent *event)
  {
    QDialog::showEvent(event);
    // QDialog would reset window position and size when shown.
    // Restore its previous size instead (user may look at metadata
    // of different series one after the other and would be inconvenient to
    // set the desired size manually each time).
    if (!savedGeometry.isEmpty())
    {
      this->restoreGeometry(savedGeometry);
    }
  }

  void hideEvent(QHideEvent *event)
  {
    this->savedGeometry = this->saveGeometry();
    QDialog::hideEvent(event);
  }

protected:
  ctkDICOMObjectListWidget* tagListWidget;
  QByteArray savedGeometry;
};

//----------------------------------------------------------------------------
class ctkDICOMBrowserPrivate: public Ui_ctkDICOMBrowser
{
public:
  ctkDICOMBrowser* const q_ptr;
  Q_DECLARE_PUBLIC(ctkDICOMBrowser);

  ctkDICOMBrowserPrivate(ctkDICOMBrowser*, QSharedPointer<ctkDICOMDatabase> database);
  ~ctkDICOMBrowserPrivate();

  void init();

  void importDirectory(QString directory, ctkDICOMBrowser::ImportDirectoryMode mode);

  void importFiles(const QStringList& files, ctkDICOMBrowser::ImportDirectoryMode mode);

  void importOldSettings();

  ctkFileDialog* ImportDialog;
  ctkDICOMMetadataDialog* MetadataDialog;

  ctkDICOMQueryRetrieveWidget* QueryRetrieveWidget;

  QSharedPointer<ctkDICOMDatabase> DICOMDatabase;
  QSharedPointer<ctkDICOMIndexer> DICOMIndexer;
  QProgressDialog *UpdateSchemaProgress;
  QProgressDialog *UpdateDisplayedFieldsProgress;
  QProgressDialog *ExportProgress;

  void showUpdateSchemaDialog();

  // Return a sanitized version of the string that is safe to be used
  // as a filename component.
  // All non-ASCII characters are replaced, becase they may be used on an internal hard disk,
  // but it may not be possible to use them on file systems of an external drive or network storage.
  QString filenameSafeString(const QString& str)
  {
    QString safeStr;
    const QString illegalChars("/\\<>:\"|?*");
    foreach (const QChar& c, str)
    {
      int asciiCode = c.toLatin1();
      if (asciiCode >= 32 && asciiCode <= 127 && !illegalChars.contains(c))
      {
        safeStr.append(c);
      }
      else
      {
        safeStr.append("_");
      }
    }
    // remove leading/trailing whitespaces
    return safeStr.trimmed();
  }


  bool DisplayImportSummary;
  bool ConfirmRemove;
  bool ShemaUpdateAutoCreateDirectory;
  bool SendActionVisible;

  // local count variables to keep track of the number of items
  // added to the database during an import operation
  int PatientsAddedDuringImport;
  int StudiesAddedDuringImport;
  int SeriesAddedDuringImport;
  int InstancesAddedDuringImport;

  // Settings key that stores database directory
  QString DatabaseDirectorySettingsKey;

  // If database directory is specified with relative path then this directory will be used as a base
  QString DatabaseDirectoryBase;

  // Default database path to use if there is nothing in settings
  QString DefaultDatabaseDirectory;
  QString DatabaseDirectory;

  bool BatchUpdateBeforeIndexingUpdate;
};

CTK_GET_CPP(ctkDICOMBrowser, bool, isSendActionVisible, SendActionVisible);
CTK_GET_CPP(ctkDICOMBrowser, QString, databaseDirectoryBase, DatabaseDirectoryBase);
CTK_SET_CPP(ctkDICOMBrowser, const QString&, setDatabaseDirectoryBase, DatabaseDirectoryBase);

//----------------------------------------------------------------------------
// ctkDICOMBrowserPrivate methods

//----------------------------------------------------------------------------
ctkDICOMBrowserPrivate::ctkDICOMBrowserPrivate(ctkDICOMBrowser* parent, QSharedPointer<ctkDICOMDatabase> database)
  : q_ptr(parent)
  , ImportDialog(0)
  , MetadataDialog(0)
  , QueryRetrieveWidget(0)
  , DICOMDatabase(database)
  , DICOMIndexer( QSharedPointer<ctkDICOMIndexer>(new ctkDICOMIndexer) )
  , UpdateSchemaProgress(0)
  , UpdateDisplayedFieldsProgress(0)
  , ExportProgress(0)
  , DisplayImportSummary(true)
  , ConfirmRemove(false)
  , ShemaUpdateAutoCreateDirectory(false)
  , SendActionVisible(false)
  , PatientsAddedDuringImport(0)
  , StudiesAddedDuringImport(0)
  , SeriesAddedDuringImport(0)
  , InstancesAddedDuringImport(0)
  , DefaultDatabaseDirectory("./ctkDICOM-Database")
  , BatchUpdateBeforeIndexingUpdate(false)
{
  if (this->DICOMDatabase.isNull())
  {
    this->DICOMDatabase = QSharedPointer<ctkDICOMDatabase>(new ctkDICOMDatabase);
  }
}

//----------------------------------------------------------------------------
ctkDICOMBrowserPrivate::~ctkDICOMBrowserPrivate()
{
  this->DICOMIndexer->waitForImportFinished();
  if ( UpdateSchemaProgress )
  {
    delete UpdateSchemaProgress;
  }
  if ( UpdateDisplayedFieldsProgress )
  {
    delete UpdateDisplayedFieldsProgress;
  }
  if ( ExportProgress )
  {
    delete ExportProgress;
  }
}

//----------------------------------------------------------------------------
void ctkDICOMBrowserPrivate::showUpdateSchemaDialog()
{
  Q_Q(ctkDICOMBrowser);
  if (UpdateSchemaProgress == 0)
  {
    //
    // Set up the Update Schema Progress Dialog
    //
    UpdateSchemaProgress = new QProgressDialog(
      q->tr("DICOM Schema Update"), "Cancel", 0, 100, q, Qt::WindowTitleHint | Qt::WindowSystemMenuHint);

    // We don't want the progress dialog to resize itself, so we bypass the label by creating our own
    QLabel* progressLabel = new QLabel(q->tr("Initialization..."));
    UpdateSchemaProgress->setLabel(progressLabel);
    UpdateSchemaProgress->setWindowModality(Qt::ApplicationModal);
    UpdateSchemaProgress->setMinimumDuration(0);
    UpdateSchemaProgress->setValue(0);

    q->connect(DICOMDatabase.data(), SIGNAL(schemaUpdateStarted(int)), UpdateSchemaProgress, SLOT(setMaximum(int)));
    q->connect(DICOMDatabase.data(), SIGNAL(schemaUpdateProgress(int)), UpdateSchemaProgress, SLOT(setValue(int)));
    q->connect(DICOMDatabase.data(), SIGNAL(schemaUpdateProgress(QString)), progressLabel, SLOT(setText(QString)));

    // close the dialog
    q->connect(DICOMDatabase.data(), SIGNAL(schemaUpdated()), UpdateSchemaProgress, SLOT(close()));
  }
  UpdateSchemaProgress->show();
}

//----------------------------------------------------------------------------
void ctkDICOMBrowserPrivate::init()
{
  Q_Q(ctkDICOMBrowser);

  qRegisterMetaType<ctkDICOMBrowser::ImportDirectoryMode>("ctkDICOMBrowser::ImportDirectoryMode");

  this->DICOMIndexer->setDatabase(this->DICOMDatabase.data());
  this->DICOMIndexer->setBackgroundImportEnabled(true);

  this->setupUi(q);

  this->ActionSend->setVisible(this->SendActionVisible);

  this->DatabaseDirectoryProblemFrame->hide();
  this->InformationMessageFrame->hide();
  this->ProgressFrame->hide();

  q->connect(this->ProgressCancelButton, SIGNAL(clicked()), DICOMIndexer.data(), SLOT(cancel()));
  q->connect(DICOMIndexer.data(), SIGNAL(progress(int)), q, SLOT(onIndexingProgress(int)));
  q->connect(DICOMIndexer.data(), SIGNAL(progressStep(QString)), q, SLOT(onIndexingProgressStep(QString)));
  q->connect(DICOMIndexer.data(), SIGNAL(progressDetail(QString)), q, SLOT(onIndexingProgressDetail(QString)));
  q->connect(DICOMIndexer.data(), SIGNAL(indexingComplete(int,int,int,int)), q, SLOT(onIndexingComplete(int,int,int,int)));
  q->connect(DICOMIndexer.data(), SIGNAL(updatingDatabase(bool)), q, SLOT(onIndexingUpdatingDatabase(bool)));

  // Signals related to tracking inserts

  q->connect(this->DirectoryButton, SIGNAL(directoryChanged(QString)), q, SLOT(setDatabaseDirectory(QString)));

  q->connect(this->SelectDatabaseDirectoryButton, SIGNAL(clicked()), q, SLOT(selectDatabaseDirectory()));
  q->connect(this->CreateNewDatabaseButton, SIGNAL(clicked()), q, SLOT(createNewDatabaseDirectory()));
  q->connect(this->UpdateDatabaseButton, SIGNAL(clicked()), q, SLOT(updateDatabase()));

  q->connect(this->InformationMessageDismissButton, SIGNAL(clicked()), InformationMessageFrame, SLOT(hide()));

  // Signal for displayed fields update
  q->connect(this->DICOMDatabase.data(), SIGNAL(displayedFieldsUpdateStarted()), q, SLOT(showUpdateDisplayedFieldsDialog()));

  // Set ToolBar button style
  this->ToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

  // Initialize Q/R widget
  this->QueryRetrieveWidget = new ctkDICOMQueryRetrieveWidget();
  this->QueryRetrieveWidget->setWindowModality ( Qt::ApplicationModal );
  this->QueryRetrieveWidget->useProgressDialog(true);

  this->dicomTableManager->setDICOMDatabase(this->DICOMDatabase.data());

  // TableView signals
  q->connect(this->dicomTableManager, SIGNAL(patientsSelectionChanged(const QItemSelection&, const QItemSelection&)),
    this->InformationMessageFrame, SLOT(hide()));
  q->connect(this->dicomTableManager, SIGNAL(studiesSelectionChanged(const QItemSelection&, const QItemSelection&)),
    this->InformationMessageFrame, SLOT(hide()));
  q->connect(this->dicomTableManager, SIGNAL(seriesSelectionChanged(const QItemSelection&, const QItemSelection&)),
    this->InformationMessageFrame, SLOT(hide()));

  q->connect(this->dicomTableManager, SIGNAL(patientsSelectionChanged(const QItemSelection&, const QItemSelection&)),
    q, SLOT(onModelSelected(const QItemSelection&, const QItemSelection&)));
  q->connect(this->dicomTableManager, SIGNAL(studiesSelectionChanged(const QItemSelection&, const QItemSelection&)),
    q, SLOT(onModelSelected(const QItemSelection&, const QItemSelection&)));
  q->connect(this->dicomTableManager, SIGNAL(seriesSelectionChanged(const QItemSelection&, const QItemSelection&)),
    q, SLOT(onModelSelected(const QItemSelection&, const QItemSelection&)));


  // set up context menus for working on selected patients, studies, series
  q->connect(this->dicomTableManager, SIGNAL(patientsRightClicked(const QPoint&)),
          q, SLOT(onPatientsRightClicked(const QPoint&)));
  q->connect(this->dicomTableManager, SIGNAL(studiesRightClicked(const QPoint&)),
          q, SLOT(onStudiesRightClicked(const QPoint&)));
  q->connect(this->dicomTableManager, SIGNAL(seriesRightClicked(const QPoint&)),
          q, SLOT(onSeriesRightClicked(const QPoint&)));

  // Initialize directoryMode widget
  QFormLayout *layout = new QFormLayout;
  QComboBox* importDirectoryModeComboBox = new QComboBox();
  importDirectoryModeComboBox->addItem("Add Link", ctkDICOMBrowser::ImportDirectoryAddLink);
  importDirectoryModeComboBox->addItem("Copy", ctkDICOMBrowser::ImportDirectoryCopy);
  importDirectoryModeComboBox->setToolTip(
        QObject::tr("Indicate if the files should be copied to the local database"
           " directory or if only links should be created ?"));
  layout->addRow(new QLabel("Import Directory Mode:"), importDirectoryModeComboBox);
  layout->setContentsMargins(0, 0, 0, 0);
  QWidget* importDirectoryBottomWidget = new QWidget();
  importDirectoryBottomWidget->setLayout(layout);

  // Default values
  importDirectoryModeComboBox->setCurrentIndex(
        importDirectoryModeComboBox->findData(q->importDirectoryMode()));

  //Initialize import widget
  this->ImportDialog = new ctkFileDialog();
  this->ImportDialog->setBottomWidget(importDirectoryBottomWidget);
  this->ImportDialog->setFileMode(QFileDialog::Directory);
  // XXX Method setSelectionMode must be called after setFileMode
  this->ImportDialog->setSelectionMode(QAbstractItemView::ExtendedSelection);
  this->ImportDialog->setLabelText(QFileDialog::Accept,"Import");
  this->ImportDialog->setWindowTitle("Import DICOM files from directory ...");
  this->ImportDialog->setWindowModality(Qt::ApplicationModal);

  this->MetadataDialog = new ctkDICOMMetadataDialog();
  this->MetadataDialog->setObjectName("DICOMMetadata");
  this->MetadataDialog->setWindowTitle("DICOM File Metadata");

  //connect signal and slots
  q->connect(this->ImportDialog, SIGNAL(filesSelected(QStringList)),
          q,SLOT(onImportDirectoriesSelected(QStringList)));

  q->connect(importDirectoryModeComboBox, SIGNAL(currentIndexChanged(int)),
          q, SLOT(onImportDirectoryComboBoxCurrentIndexChanged(int)));

  q->connect(this->QueryRetrieveWidget, SIGNAL(canceled()), this->QueryRetrieveWidget, SLOT(hide()) );
  q->connect(this->QueryRetrieveWidget, SIGNAL(canceled()), q, SLOT(onQueryRetrieveFinished()) );
}

//----------------------------------------------------------------------------
// ctkDICOMBrowser methods

//----------------------------------------------------------------------------
ctkDICOMBrowser::ctkDICOMBrowser(QWidget* _parent)
  : Superclass(_parent),
  d_ptr(new ctkDICOMBrowserPrivate(this, QSharedPointer<ctkDICOMDatabase>()))
{
  Q_D(ctkDICOMBrowser);
  d->init();
}

//----------------------------------------------------------------------------
ctkDICOMBrowser::ctkDICOMBrowser(QSharedPointer<ctkDICOMDatabase> sharedDatabase, QWidget* _parent)
  : Superclass(_parent),
  d_ptr(new ctkDICOMBrowserPrivate(this, sharedDatabase))
{
  Q_D(ctkDICOMBrowser);
  d->init();
}

//----------------------------------------------------------------------------
ctkDICOMBrowser::~ctkDICOMBrowser()
{
  Q_D(ctkDICOMBrowser);

  d->QueryRetrieveWidget->deleteLater();
  d->ImportDialog->deleteLater();
  d->MetadataDialog->deleteLater();
}

//----------------------------------------------------------------------------
bool ctkDICOMBrowser::displayImportSummary()
{
  Q_D(ctkDICOMBrowser);

  return d->DisplayImportSummary;
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::setDisplayImportSummary(bool onOff)
{
  Q_D(ctkDICOMBrowser);

  d->DisplayImportSummary = onOff;
}

//----------------------------------------------------------------------------
bool ctkDICOMBrowser::confirmRemove()
{
  Q_D(ctkDICOMBrowser);

  return d->ConfirmRemove;
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::setConfirmRemove(bool onOff)
{
  Q_D(ctkDICOMBrowser);

  d->ConfirmRemove = onOff;
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::resetItemsAddedDuringImportCounters()
{
  Q_D(ctkDICOMBrowser);
  d->PatientsAddedDuringImport = 0;
  d->StudiesAddedDuringImport = 0;
  d->SeriesAddedDuringImport = 0;
  d->InstancesAddedDuringImport = 0;
}

//----------------------------------------------------------------------------
int ctkDICOMBrowser::patientsAddedDuringImport()
{
  Q_D(ctkDICOMBrowser);

  return d->PatientsAddedDuringImport;
}

//----------------------------------------------------------------------------
int ctkDICOMBrowser::studiesAddedDuringImport()
{
  Q_D(ctkDICOMBrowser);

  return d->StudiesAddedDuringImport;
}

//----------------------------------------------------------------------------
int ctkDICOMBrowser::seriesAddedDuringImport()
{
  Q_D(ctkDICOMBrowser);

  return d->SeriesAddedDuringImport;
}

//----------------------------------------------------------------------------
int ctkDICOMBrowser::instancesAddedDuringImport()
{
  Q_D(ctkDICOMBrowser);

  return d->InstancesAddedDuringImport;
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::createNewDatabaseDirectory()
{
  Q_D(ctkDICOMBrowser);

  // Use the current database folder as a basis for the new name
  QString baseFolder = this->databaseDirectory();
  if (baseFolder.isEmpty())
  {
    baseFolder = d->DefaultDatabaseDirectory;
  }
  else
  {
    // only use existing folder name as a basis if it is empty or
    // a valid database
    if (!QDir(baseFolder).isEmpty())
    {
      QString databaseFileName = QDir(baseFolder).filePath("ctkDICOM.sql");
      if (!QFile(databaseFileName).exists())
      {
        // current folder is a non-empty and not a DICOM database folder
        // create a subfolder for the new DICOM database based on the name
        // of default database path
        QFileInfo defaultFolderInfo(d->DefaultDatabaseDirectory);
        QString defaultSubfolderName = defaultFolderInfo.fileName();
        if (defaultSubfolderName.isEmpty())
        {
          defaultSubfolderName = defaultFolderInfo.dir().dirName();
        }
        baseFolder += "/" + defaultSubfolderName;
      }
    }
  }
  // Remove existing numerical suffix
  QString separator = "_";
  bool isSuffixValid = false;
  QString suffixStr = baseFolder.split(separator).last();
  int suffixStart = suffixStr.toInt(&isSuffixValid);
  if (isSuffixValid)
  {
    QStringList baseFolderComponents = baseFolder.split(separator);
    baseFolderComponents.removeLast();
    baseFolder = baseFolderComponents.join(separator);
  }
  // Try folder names, starting with the current one,
  // incrementing the original numerical suffix.
  int attemptsCount = 100;
  for (int attempt=0; attempt<attemptsCount; attempt++)
  {
    QString newFolder = baseFolder;
    int suffix = (suffixStart + attempt) % attemptsCount;
    if (suffix)
    {
      newFolder += separator + QString::number(suffix);
    }
    if (!QDir(newFolder).exists())
    {
      if (!QDir().mkpath(newFolder))
      {
        continue;
      }
    }
    if (!QDir(newFolder).isEmpty())
    {
      continue;
    }
    // Folder exists and empty, try to use this
    setDatabaseDirectory(newFolder);
    return;
  }
  std::cerr << "Failed to create new database in folder: " << qPrintable(baseFolder) << "\n";
  d->InformationMessageFrame->hide();
  d->DatabaseDirectoryProblemFrame->show();
  d->DatabaseDirectoryProblemLabel->setText(tr("Failed to create new database in folder %1.").arg(QDir(baseFolder).absolutePath()));
  d->UpdateDatabaseButton->hide();
  d->CreateNewDatabaseButton->show();
  d->SelectDatabaseDirectoryButton->show();
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::updateDatabase()
{
  Q_D(ctkDICOMBrowser);
  d->InformationMessageFrame->hide();
  d->DatabaseDirectoryProblemFrame->hide();
  d->showUpdateSchemaDialog();
  QString dir = this->databaseDirectory();
  // open DICOM database on the directory
  QString databaseFileName = QDir(dir).filePath("ctkDICOM.sql");
  try
  {
    d->DICOMDatabase->openDatabase(databaseFileName);
  }
  catch (std::exception e)
  {
    std::cerr << "Database error: " << qPrintable(d->DICOMDatabase->lastError()) << "\n";
    d->DICOMDatabase->closeDatabase();
    return;
  }
  d->DICOMDatabase->updateSchema();
  // Update GUI
  this->setDatabaseDirectory(dir);
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::setDatabaseDirectory(const QString& directory)
{
  Q_D(ctkDICOMBrowser);
  d->InformationMessageFrame->hide();

  QString absDirectory = ctk::absolutePathFromInternal(directory, d->DatabaseDirectoryBase);

  // close the active DICOM database
  d->DICOMDatabase->closeDatabase();

  // open DICOM database on the directory
  QString databaseFileName = QDir(absDirectory).filePath("ctkDICOM.sql");

  bool success = true;

  if (!QDir(absDirectory).exists()
    || (!QDir(absDirectory).isEmpty() && !QFile(databaseFileName).exists()))
  {
    std::cerr << "Database folder does not contain ctkDICOM.sql file: " << qPrintable(absDirectory) << "\n";
    d->DatabaseDirectoryProblemFrame->show();
    d->DatabaseDirectoryProblemLabel->setText(tr("No valid DICOM database found in folder %1.").arg(absDirectory));
    d->UpdateDatabaseButton->hide();
    d->CreateNewDatabaseButton->show();
    d->SelectDatabaseDirectoryButton->show();
    success = false;
  }

  if (success)
  {
    bool databaseOpenSuccess = false;
    try
    {
      d->DICOMDatabase->openDatabase(databaseFileName);
      databaseOpenSuccess = d->DICOMDatabase->isOpen();
    }
    catch (std::exception e)
    {
      databaseOpenSuccess = false;
    }
    if (!databaseOpenSuccess || d->DICOMDatabase->schemaVersionLoaded().isEmpty())
    {
      std::cerr << "Database error: " << qPrintable(d->DICOMDatabase->lastError()) << "\n";
      d->DICOMDatabase->closeDatabase();
      d->DatabaseDirectoryProblemFrame->show();
      d->DatabaseDirectoryProblemLabel->setText(tr("No valid DICOM database found in folder %1.").arg(absDirectory));
      d->UpdateDatabaseButton->hide();
      d->CreateNewDatabaseButton->show();
      d->SelectDatabaseDirectoryButton->show();
      success = false;
    }
  }

  if (success)
  {
    if (d->DICOMDatabase->schemaVersionLoaded() != d->DICOMDatabase->schemaVersion())
    {
      std::cerr << "Database version mismatch: version of selected database = "
        << qPrintable(d->DICOMDatabase->schemaVersionLoaded())
        << ", version required = " << qPrintable(d->DICOMDatabase->schemaVersion()) << "\n";
      d->DICOMDatabase->closeDatabase();
      d->DatabaseDirectoryProblemFrame->show();
      d->DatabaseDirectoryProblemLabel->setText(
        tr("Incompatible DICOM database version found in folder %1.").arg(absDirectory));
      d->UpdateDatabaseButton->show();
      d->CreateNewDatabaseButton->show();
      d->SelectDatabaseDirectoryButton->show();
      success = false;
    }
  }

  if (success)
  {
    d->DatabaseDirectoryProblemFrame->hide();
  }

  // Save new database directory in this object and in application settings.
  d->DatabaseDirectory = absDirectory;
  if (!d->DatabaseDirectorySettingsKey.isEmpty())
  {
    QSettings settings;
    settings.setValue(d->DatabaseDirectorySettingsKey, ctk::internalPathFromAbsolute(absDirectory, d->DatabaseDirectoryBase));
    settings.sync();
  }

  // pass DICOM database instance to Import widget
  d->QueryRetrieveWidget->setRetrieveDatabase(d->DICOMDatabase);

  // update the button and let any connected slots know about the change
  bool wasBlocked = d->DirectoryButton->blockSignals(true);
  d->DirectoryButton->setDirectory(absDirectory);
  d->DirectoryButton->blockSignals(wasBlocked);

  d->dicomTableManager->updateTableViews();

  emit databaseDirectoryChanged(absDirectory);
}

//----------------------------------------------------------------------------
QString ctkDICOMBrowser::databaseDirectory() const
{
  Q_D(const ctkDICOMBrowser);

  // If override settings is specified then try to get database directory from there first
  return d->DatabaseDirectory;
}

//------------------------------------------------------------------------------
QString ctkDICOMBrowser::databaseDirectorySettingsKey() const
{
  Q_D(const ctkDICOMBrowser);
  return d->DatabaseDirectorySettingsKey;
}

//------------------------------------------------------------------------------
void ctkDICOMBrowser::setDatabaseDirectorySettingsKey(const QString& key)
{
  Q_D(ctkDICOMBrowser);
  d->DatabaseDirectorySettingsKey = key;

  QSettings settings;
  QString databaseDirectory = ctk::absolutePathFromInternal(settings.value(d->DatabaseDirectorySettingsKey, "").toString(), d->DatabaseDirectoryBase);
  this->setDatabaseDirectory(databaseDirectory);
}

//------------------------------------------------------------------------------
void ctkDICOMBrowser::setTagsToPrecache( const QStringList tags)
{
  Q_D(ctkDICOMBrowser);
  d->DICOMDatabase->setTagsToPrecache(tags);
}

//------------------------------------------------------------------------------
const QStringList ctkDICOMBrowser::tagsToPrecache()
{
  Q_D(ctkDICOMBrowser);
  return d->DICOMDatabase->tagsToPrecache();
}

//----------------------------------------------------------------------------
ctkDICOMDatabase* ctkDICOMBrowser::database()
{
  Q_D(ctkDICOMBrowser);
  return d->DICOMDatabase.data();
}

//----------------------------------------------------------------------------
ctkDICOMTableManager* ctkDICOMBrowser::dicomTableManager()
{
  Q_D(ctkDICOMBrowser);
  return d->dicomTableManager;
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::openImportDialog()
{
  Q_D(ctkDICOMBrowser);

  d->ImportDialog->exec();
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::openExportDialog()
{
  // Export selected series
  this->exportSelectedItems(ctkDICOMModel::SeriesType);
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::openSendDialog()
{
  // Export selected series
  emit sendRequested(this->fileListForCurrentSelection(ctkDICOMModel::SeriesType));
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::openQueryDialog()
{
  Q_D(ctkDICOMBrowser);

  // QueryRetrieveWidget is a QWidget not a QDialog, so use this instead of exec
  d->QueryRetrieveWidget->show();
  d->QueryRetrieveWidget->raise();
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::onQueryRetrieveFinished()
{
  emit this->queryRetrieveFinished();
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::onRemoveAction()
{
  Q_D(ctkDICOMBrowser);
  this->removeSelectedItems(ctkDICOMModel::SeriesType);
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::onRepairAction()
{
  Q_D(ctkDICOMBrowser);

  QMessageBox* repairMessageBox;
  repairMessageBox = new QMessageBox;
  repairMessageBox->setWindowTitle("Database Repair");

  QStringList allFiles(d->DICOMDatabase->allFiles());

  QSet<QString> corruptedSeries;
  QHash<QString, QHash<QString, QString> > corruptedSeriesDescriptions;

  QStringList::const_iterator it;
  for (it = allFiles.constBegin(); it!= allFiles.constEnd();++it)
  {
    QString fileName(*it);
    QFile dicomFile(fileName);

    if(!dicomFile.exists())
    {
      QString seriesUid = d->DICOMDatabase->seriesForFile(fileName);
      if (!corruptedSeries.contains(seriesUid))
      {
        corruptedSeries.insert(seriesUid);
        corruptedSeriesDescriptions[seriesUid] = d->DICOMDatabase->descriptionsForFile(fileName);
      }
    }
  }

  if (corruptedSeries.size() == 0)
  {
    bool wasBatchUpdate = d->dicomTableManager->setBatchUpdate(true);
    d->DICOMDatabase->updateDisplayedFields();
    d->dicomTableManager->setBatchUpdate(wasBatchUpdate);

    repairMessageBox->setText("All the files in the local database are available.");
    repairMessageBox->addButton(QMessageBox::Ok);
    repairMessageBox->exec();
  }
  else
  {
    repairMessageBox->addButton(QMessageBox::Yes);
    repairMessageBox->addButton(QMessageBox::No);
    repairMessageBox->addButton(QMessageBox::YesToAll);
    repairMessageBox->addButton(QMessageBox::Cancel);
    QSet<QString>::iterator i;
    bool yesToAll = false;
    for (i = corruptedSeries.begin(); i != corruptedSeries.end(); ++i)
    {
      QStringList fileList (d->DICOMDatabase->filesForSeries(*i));
      QString unavailableFileNames;
      QStringList::const_iterator it;
      QHash<QString, QString> descriptions = corruptedSeriesDescriptions[*i];
      for (it = fileList.constBegin(); it!= fileList.constEnd();++it)
      {
        unavailableFileNames.append(*it+"\n");
      }

      if (!yesToAll)
      {
        repairMessageBox->setText("The files for the following series are not available on the disk: \nPatient Name: "
          + descriptions["PatientsName"] + "\n" +
          "Study Desciption: " + descriptions["StudyDescription"] + "\n" +
          "Series Desciption: " + descriptions["SeriesDescription"] + "\n" +
          "Do you want to remove the series from the DICOM database? ");

        repairMessageBox->setDetailedText(unavailableFileNames);

        int selection = repairMessageBox->exec();
        if (selection == QMessageBox::No)
        {
          continue;
        }
        else if (selection == QMessageBox::YesToAll)
        {
          yesToAll = true;
        }
        else if (selection == QMessageBox::Cancel)
        {
          break;
        }
      }

      d->DICOMDatabase->removeSeries(*i);
      d->dicomTableManager->updateTableViews();
    }

    bool wasBatchUpdate = d->dicomTableManager->setBatchUpdate(true);
    d->DICOMDatabase->updateDisplayedFields();
    d->dicomTableManager->setBatchUpdate(wasBatchUpdate);
  }

  repairMessageBox->deleteLater();

  // Force refresh of table views
  d->DICOMDatabase->databaseChanged();
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::onImportDirectoriesSelected(QStringList directories)
{
  Q_D(ctkDICOMBrowser);
  this->importDirectories(directories, this->importDirectoryMode());

  // Clear selection
  d->ImportDialog->clearSelection();
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::onImportDirectoryComboBoxCurrentIndexChanged(int index)
{
  Q_D(ctkDICOMBrowser);
  Q_UNUSED(index);
  if (!(d->ImportDialog->options() & QFileDialog::DontUseNativeDialog))
  {
    return;  // Native dialog does not support modifying or getting widget elements.
  }
  QComboBox* comboBox = d->ImportDialog->bottomWidget()->findChild<QComboBox*>();
  ctkDICOMBrowser::ImportDirectoryMode mode =
      static_cast<ctkDICOMBrowser::ImportDirectoryMode>(comboBox->itemData(index).toInt());
  this->setImportDirectoryMode(mode);
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::importFiles(const QStringList& files, ctkDICOMBrowser::ImportDirectoryMode mode)
{
  Q_D(ctkDICOMBrowser);
  if (!d->DICOMDatabase || !d->DICOMIndexer)
  {
    qWarning() << Q_FUNC_INFO << " failed: database or indexer is invalid";
    return;
  }
  d->importFiles(files, mode);
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::importDirectories(QStringList directories, ctkDICOMBrowser::ImportDirectoryMode mode)
{
  Q_D(ctkDICOMBrowser);
  if (!d->DICOMDatabase || !d->DICOMIndexer)
  {
    qWarning() << Q_FUNC_INFO << " failed: database or indexer is invalid";
    return;
  }
  foreach (const QString& directory, directories)
  {
    d->importDirectory(directory, mode);
  }
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::importDirectory(QString directory, ctkDICOMBrowser::ImportDirectoryMode mode)
{
  Q_D(ctkDICOMBrowser);
  d->importDirectory(directory, mode);
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::onImportDirectory(QString directory, ctkDICOMBrowser::ImportDirectoryMode mode)
{
  this->importDirectory(directory, mode);
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::waitForImportFinished()
{
  Q_D(ctkDICOMBrowser);
  d->DICOMIndexer->waitForImportFinished();
}

//----------------------------------------------------------------------------
void ctkDICOMBrowserPrivate::importDirectory(QString directory, ctkDICOMBrowser::ImportDirectoryMode mode)
{
  if (!QDir(directory).exists())
  {
    return;
  }
  // Start background indexing
  this->DICOMIndexer->addDirectory(directory, mode == ctkDICOMBrowser::ImportDirectoryCopy);
}

//----------------------------------------------------------------------------
void ctkDICOMBrowserPrivate::importFiles(const QStringList& files, ctkDICOMBrowser::ImportDirectoryMode mode)
{
  // Start background indexing
  this->DICOMIndexer->addListOfFiles(files, mode == ctkDICOMBrowser::ImportDirectoryCopy);
}

//----------------------------------------------------------------------------
void ctkDICOMBrowserPrivate::importOldSettings()
{
  // Backward compatibility
  QSettings settings;
  int dontConfirmCopyOnImport = settings.value("MainWindow/DontConfirmCopyOnImport", static_cast<int>(QMessageBox::InvalidRole)).toInt();
  if (dontConfirmCopyOnImport == QMessageBox::AcceptRole)
  {
    settings.setValue("DICOM/ImportDirectoryMode", static_cast<int>(ctkDICOMBrowser::ImportDirectoryCopy));
  }
  settings.remove("MainWindow/DontConfirmCopyOnImport");
}

//----------------------------------------------------------------------------
ctkFileDialog* ctkDICOMBrowser::importDialog() const
{
  Q_D(const ctkDICOMBrowser);
  return d->ImportDialog;
}

//----------------------------------------------------------------------------
ctkDICOMBrowser::ImportDirectoryMode ctkDICOMBrowser::importDirectoryMode()const
{
  Q_D(const ctkDICOMBrowser);
  ctkDICOMBrowserPrivate* mutable_d = const_cast<ctkDICOMBrowserPrivate*>(d);
  mutable_d->importOldSettings();
  QSettings settings;
  return static_cast<ctkDICOMBrowser::ImportDirectoryMode>(settings.value(
    "DICOM/ImportDirectoryMode", static_cast<int>(ctkDICOMBrowser::ImportDirectoryAddLink)).toInt() );
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::setImportDirectoryMode(ctkDICOMBrowser::ImportDirectoryMode mode)
{
  Q_D(ctkDICOMBrowser);

  QSettings settings;
  settings.setValue("DICOM/ImportDirectoryMode", static_cast<int>(mode));
  if (!d->ImportDialog)
  {
    return;
  }
  if (!(d->ImportDialog->options() & QFileDialog::DontUseNativeDialog))
  {
    return;  // Native dialog does not support modifying or getting widget elements.
  }
  QComboBox* comboBox = d->ImportDialog->bottomWidget()->findChild<QComboBox*>();
  comboBox->setCurrentIndex(comboBox->findData(mode));
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::onModelSelected(const QItemSelection &item1, const QItemSelection &item2)
{
  Q_UNUSED(item1);
  Q_UNUSED(item2);
  Q_D(ctkDICOMBrowser);
  d->ActionRemove->setEnabled(true);
  d->ActionSend->setEnabled(true);
  d->ActionExport->setEnabled(true);
}

//----------------------------------------------------------------------------
bool ctkDICOMBrowser::confirmDeleteSelectedUIDs(QStringList uids)
{
  Q_D(ctkDICOMBrowser);

  if (uids.isEmpty())
  {
    return false;
  }

  ctkMessageBox confirmDeleteDialog;
  QString message("Do you want to delete the following selected items?");

  // add the information about the selected UIDs
  int numUIDs = uids.size();
  for (int i = 0; i < numUIDs; ++i)
  {
    QString uid = uids.at(i);

    // try using the given UID to find a descriptive string
    QString patientName = d->DICOMDatabase->nameForPatient(uid);
    QString studyDescription = d->DICOMDatabase->descriptionForStudy(uid);
    QString seriesDescription = d->DICOMDatabase->descriptionForSeries(uid);

    if (!patientName.isEmpty())
    {
      message += QString("\n") + patientName;
    }
    else if (!studyDescription.isEmpty())
    {
      message += QString("\n") + studyDescription;
    }
    else if (!seriesDescription.isEmpty())
    {
      message += QString("\n") + seriesDescription;
    }
    else
    {
      // if all other descriptors are empty, use the UID
      message += QString("\n") + uid;
    }
  }
  confirmDeleteDialog.setText(message);
  confirmDeleteDialog.setIcon(QMessageBox::Question);

  confirmDeleteDialog.addButton("Delete", QMessageBox::AcceptRole);
  confirmDeleteDialog.addButton("Cancel", QMessageBox::RejectRole);
  confirmDeleteDialog.setDontShowAgainSettingsKey( "MainWindow/DontConfirmDeleteSelected");

  int response = confirmDeleteDialog.exec();

  if (response == QMessageBox::AcceptRole)
  {
    return true;
  }
  else
  {
    return false;
  }
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::onPatientsRightClicked(const QPoint &point)
{
  Q_D(ctkDICOMBrowser);

  // get the list of patients that are selected
  QStringList selectedPatientsUIDs = d->dicomTableManager->currentPatientsSelection();
  int numPatients = selectedPatientsUIDs.size();
  if (numPatients == 0)
  {
    qDebug() << "No patients selected!";
    return;
  }

  QMenu *patientsMenu = new QMenu(d->dicomTableManager);
  QString selectedPatientsCount;
  if (numPatients > 1)
  {
    selectedPatientsCount = QString(" %1 selected patients").arg(numPatients);
  }

  QString metadataString = QString("View DICOM metadata");
  if (numPatients > 1)
  {
    metadataString += QString(" of") + selectedPatientsCount;
  }
  QAction *metadataAction = new QAction(metadataString, patientsMenu);
  patientsMenu->addAction(metadataAction);

  QString deleteString = QString("Delete") + selectedPatientsCount;
  QAction *deleteAction = new QAction(deleteString, patientsMenu);
  patientsMenu->addAction(deleteAction);

  QString exportString = QString("Export%1 to file system").arg(selectedPatientsCount);
  QAction *exportAction = new QAction(exportString, patientsMenu);
  patientsMenu->addAction(exportAction);

  QString sendString = QString("Send%1 to DICOM server").arg(selectedPatientsCount);
  QAction* sendAction = new QAction(sendString, patientsMenu);
  if (this->isSendActionVisible())
  {
    patientsMenu->addAction(sendAction);
  }

  // the table took care of mapping it to a global position so that the
  // menu will pop up at the correct place over this table.
  QAction *selectedAction = patientsMenu->exec(point);

  if (selectedAction == metadataAction)
  {
    this->showMetadata(this->fileListForCurrentSelection(ctkDICOMModel::PatientType));
  }
  else if (selectedAction == deleteAction)
  {
    this->removeSelectedItems(ctkDICOMModel::PatientType);
  }
  else if (selectedAction == exportAction)
  {
    this->exportSelectedItems(ctkDICOMModel::PatientType);
  }
  else if (selectedAction == sendAction)
  {
    emit sendRequested(this->fileListForCurrentSelection(ctkDICOMModel::PatientType));
  }
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::onStudiesRightClicked(const QPoint &point)
{
  Q_D(ctkDICOMBrowser);

  // get the list of studies that are selected
  QStringList selectedStudiesUIDs = d->dicomTableManager->currentStudiesSelection();
  int numStudies = selectedStudiesUIDs.size();
  if (numStudies == 0)
  {
    qDebug() << "No studies selected!";
    return;
  }

  QMenu *studiesMenu = new QMenu(d->dicomTableManager);
  QString selectedStudiesCount;
  if (numStudies > 1)
  {
    selectedStudiesCount = QString(" %1 selected studies").arg(numStudies);
  }

  QString metadataString = QString("View DICOM metadata");
  if (numStudies > 1)
  {
    metadataString += QString(" of") + selectedStudiesCount;
  }
  QAction *metadataAction = new QAction(metadataString, studiesMenu);
  studiesMenu->addAction(metadataAction);

  QString deleteString = QString("Delete") + selectedStudiesCount;
  QAction *deleteAction = new QAction(deleteString, studiesMenu);
  studiesMenu->addAction(deleteAction);

  QString exportString = QString("Export%1 to file system").arg(selectedStudiesCount);
  QAction *exportAction = new QAction(exportString, studiesMenu);
  studiesMenu->addAction(exportAction);

  QString sendString = QString("Send%1 to DICOM server").arg(selectedStudiesCount);
  QAction* sendAction = new QAction(sendString, studiesMenu);
  if (this->isSendActionVisible())
  {
    studiesMenu->addAction(sendAction);
  }

  // the table took care of mapping it to a global position so that the
  // menu will pop up at the correct place over this table.
  QAction *selectedAction = studiesMenu->exec(point);

  if (selectedAction == metadataAction)
  {
    this->showMetadata(this->fileListForCurrentSelection(ctkDICOMModel::StudyType));
  }
  else if (selectedAction == deleteAction
      && this->confirmDeleteSelectedUIDs(selectedStudiesUIDs))
  {
    foreach (const QString& uid, selectedStudiesUIDs)
    {
      d->DICOMDatabase->removeStudy(uid);
      d->dicomTableManager->updateTableViews();
    }
  }
  else if (selectedAction == exportAction)
  {
    this->exportSelectedItems(ctkDICOMModel::StudyType);
  }
  else if (selectedAction == sendAction)
  {
    emit sendRequested(this->fileListForCurrentSelection(ctkDICOMModel::StudyType));
  }
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::onSeriesRightClicked(const QPoint &point)
{
  Q_D(ctkDICOMBrowser);

  // get the list of series that are selected
  QStringList selectedSeriesUIDs = d->dicomTableManager->currentSeriesSelection();
  int numSeries = selectedSeriesUIDs.size();
  if (numSeries == 0)
  {
    qDebug() << "No series selected!";
    return;
  }

  QMenu *seriesMenu = new QMenu(d->dicomTableManager);
  QString selectedSeriesCount;
  if (numSeries > 1)
  {
    selectedSeriesCount = QString(" %1 selected series").arg(numSeries);

  }
  QString metadataString = QString("View DICOM metadata");
  if (numSeries > 1)
  {
    metadataString += QString(" of") + selectedSeriesCount;
  }
  QAction *metadataAction = new QAction(metadataString, seriesMenu);
  seriesMenu->addAction(metadataAction);

  QString deleteString = QString("Delete") + selectedSeriesCount;
  QAction *deleteAction = new QAction(deleteString, seriesMenu);
  seriesMenu->addAction(deleteAction);

  QString exportString = QString("Export%1 to file system").arg(selectedSeriesCount);
  QAction *exportAction = new QAction(exportString, seriesMenu);
  seriesMenu->addAction(exportAction);

  QString sendString = QString("Send%1 to DICOM server").arg(selectedSeriesCount);
  QAction* sendAction = new QAction(sendString, seriesMenu);
  if (this->isSendActionVisible())
  {
    seriesMenu->addAction(sendAction);
  }

  // the table took care of mapping it to a global position so that the
  // menu will pop up at the correct place over this table.
  QAction *selectedAction = seriesMenu->exec(point);

  if (selectedAction == metadataAction)
  {
    this->showMetadata(this->fileListForCurrentSelection(ctkDICOMModel::SeriesType));
  }
  else if (selectedAction == deleteAction
      && this->confirmDeleteSelectedUIDs(selectedSeriesUIDs))
  {
    foreach (const QString& uid, selectedSeriesUIDs)
    {
      d->DICOMDatabase->removeSeries(uid);
      d->dicomTableManager->updateTableViews();
    }
  }
  else if (selectedAction == exportAction)
  {
    this->exportSelectedItems(ctkDICOMModel::SeriesType);
  }
  else if (selectedAction == sendAction)
  {
    emit sendRequested(this->fileListForCurrentSelection(ctkDICOMModel::SeriesType));
  }

}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::exportSeries(QString dirPath, QStringList uids)
{
  Q_D(ctkDICOMBrowser);

  foreach (const QString& uid, uids)
  {
    QStringList filesForSeries = d->DICOMDatabase->filesForSeries(uid);

    // Use the first file to get the overall series information
    QString firstFilePath = filesForSeries[0];
    QHash<QString,QString> descriptions (d->DICOMDatabase->descriptionsForFile(firstFilePath));
    QString patientName = descriptions["PatientsName"];
    QString patientIDTag = QString("0010,0020");
    QString patientID = d->DICOMDatabase->fileValue(firstFilePath, patientIDTag);
    QString studyDescription = descriptions["StudyDescription"];
    QString seriesDescription = descriptions["SeriesDescription"];
    QString studyDateTag = QString("0008,0020");
    QString studyDate = d->DICOMDatabase->fileValue(firstFilePath,studyDateTag);
    QString seriesNumberTag = QString("0020,0011");
    QString seriesNumber = d->DICOMDatabase->fileValue(firstFilePath,seriesNumberTag);

    QString sep = "/";
    QString nameSep = "-";
    QString destinationDir = dirPath + sep + d->filenameSafeString(patientID);
    if (!patientName.isEmpty())
    {
      destinationDir += nameSep + d->filenameSafeString(patientName);
    }
    destinationDir += sep + d->filenameSafeString(studyDate);
    if (!studyDescription.isEmpty())
    {
      destinationDir += nameSep + d->filenameSafeString(studyDescription);
    }
    destinationDir += sep + d->filenameSafeString(seriesNumber);
    if (!seriesDescription.isEmpty())
    {
      destinationDir += nameSep + d->filenameSafeString(seriesDescription);
    }
    destinationDir += sep;


    // create the destination directory if necessary
    if (!QDir().exists(destinationDir))
    {
      if (!QDir().mkpath(destinationDir))
      {
        QString errorString =
          QString("Unable to create export destination directory:\n\n")
          + destinationDir
          + QString("\n\nHalting export.");
        ctkMessageBox createDirectoryErrorMessageBox;
        createDirectoryErrorMessageBox.setText(errorString);
        createDirectoryErrorMessageBox.setIcon(QMessageBox::Warning);
        createDirectoryErrorMessageBox.exec();
        return;
      }
    }

    // show progress
    if (d->ExportProgress == 0)
    {
      d->ExportProgress = new QProgressDialog(this->tr("DICOM Export"), "Close", 0, 100, this, Qt::WindowTitleHint | Qt::WindowSystemMenuHint);
      d->ExportProgress->setWindowModality(Qt::ApplicationModal);
      d->ExportProgress->setMinimumDuration(0);
    }
    QLabel *exportLabel = new QLabel(this->tr("Exporting series ") + seriesNumber);
    d->ExportProgress->setLabel(exportLabel);
    d->ExportProgress->setValue(0);

    int fileNumber = 0;
    int numFiles = filesForSeries.size();
    d->ExportProgress->setMaximum(numFiles);
    foreach (const QString& filePath, filesForSeries)
    {
      // File name example: my/destination/folder/000001.dcm
      QString destinationFileName = QStringLiteral("%1%2.dcm").arg(destinationDir).arg(fileNumber, 6, 10, QLatin1Char('0'));

      if (!QFile::exists(filePath))
      {
        d->ExportProgress->setValue(numFiles);
        QString errorString = QString("Export source file not found:\n\n")
          + filePath
          + QString("\n\nHalting export.\n\nError may be fixed via Repair.");
        ctkMessageBox copyErrorMessageBox;
        copyErrorMessageBox.setText(errorString);
        copyErrorMessageBox.setIcon(QMessageBox::Warning);
        copyErrorMessageBox.exec();
        return;
      }
      if (QFile::exists(destinationFileName))
      {
        d->ExportProgress->setValue(numFiles);
        QString errorString = QString("Export destination file already exists:\n\n")
          + destinationFileName
          + QString("\n\nHalting export.");
        ctkMessageBox copyErrorMessageBox;
        copyErrorMessageBox.setText(errorString);
        copyErrorMessageBox.setIcon(QMessageBox::Warning);
        copyErrorMessageBox.exec();
        return;
      }

      bool copyResult = QFile::copy(filePath, destinationFileName);
      if (!copyResult)
      {
        d->ExportProgress->setValue(numFiles);
        QString errorString = QString("Failed to copy\n\n")
          + filePath
          + QString("\n\nto\n\n")
          + destinationFileName
          + QString("\n\nHalting export.");
        ctkMessageBox copyErrorMessageBox;
        copyErrorMessageBox.setText(errorString);
        copyErrorMessageBox.setIcon(QMessageBox::Warning);
        copyErrorMessageBox.exec();
        return;
        }

      fileNumber++;
      d->ExportProgress->setValue(fileNumber);
    }
    d->ExportProgress->setValue(numFiles);
  }
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::showUpdateDisplayedFieldsDialog()
{
  Q_D(ctkDICOMBrowser);
  if (d->UpdateDisplayedFieldsProgress == 0)
  {
    //
    // Set up the Update Schema Progress Dialog
    //
    d->UpdateDisplayedFieldsProgress = new QProgressDialog(this, Qt::WindowTitleHint | Qt::WindowSystemMenuHint);

    // We don't want the progress dialog to resize itself, so we bypass the label by creating our own
    d->UpdateDisplayedFieldsProgress->setLabelText("Updating database displayed fields...");
    d->UpdateDisplayedFieldsProgress->setWindowModality(Qt::ApplicationModal);
    d->UpdateDisplayedFieldsProgress->setMinimumDuration(0);
    d->UpdateDisplayedFieldsProgress->setMaximum(5);
    d->UpdateDisplayedFieldsProgress->setValue(0);

    connect(d->DICOMDatabase.data(), SIGNAL(displayedFieldsUpdateProgress(int)), d->UpdateDisplayedFieldsProgress, SLOT(setValue(int)));
    connect(d->DICOMDatabase.data(), SIGNAL(displayedFieldsUpdated()), d->UpdateDisplayedFieldsProgress, SLOT(close()));
  }
  d->UpdateDisplayedFieldsProgress->show();
  QApplication::processEvents();
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::setToolbarVisible(bool state)
{
  Q_D(ctkDICOMBrowser);
  d->ToolBar->setVisible(state);
}

//----------------------------------------------------------------------------
bool ctkDICOMBrowser::isToolbarVisible() const
{
  Q_D(const ctkDICOMBrowser);
  return d->ToolBar->isVisible();
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::setSendActionVisible(bool visible)
{
  Q_D(ctkDICOMBrowser);
  d->SendActionVisible = visible;
  d->ActionSend->setVisible(visible);
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::setDatabaseDirectorySelectorVisible(bool state)
{
  Q_D(ctkDICOMBrowser);
  d->DirectoryButton->setVisible(state);
}

//----------------------------------------------------------------------------
bool ctkDICOMBrowser::isDatabaseDirectorySelectorVisible() const
{
  Q_D(const ctkDICOMBrowser);
  return d->DirectoryButton->isVisible();
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::selectDatabaseDirectory()
{
  Q_D(const ctkDICOMBrowser);
  d->InformationMessageFrame->hide();
  d->DatabaseDirectoryProblemFrame->hide();
  d->DirectoryButton->browse();
}

//----------------------------------------------------------------------------
QStringList ctkDICOMBrowser::fileListForCurrentSelection(ctkDICOMModel::IndexType level)
{
  Q_D(const ctkDICOMBrowser);

  QStringList selectedStudyUIDs;
  if (level == ctkDICOMModel::PatientType)
  {
    QStringList uids = d->dicomTableManager->currentPatientsSelection();
    foreach(const QString& uid, uids)
    {
      selectedStudyUIDs << d->DICOMDatabase->studiesForPatient(uid);
    }
  }
  if (level == ctkDICOMModel::StudyType)
  {
    selectedStudyUIDs = d->dicomTableManager->currentStudiesSelection();
  }

  QStringList selectedSeriesUIDs;
  if (level == ctkDICOMModel::SeriesType)
  {
    selectedSeriesUIDs = d->dicomTableManager->currentSeriesSelection();
  }
  else
  {
    foreach(const QString& uid, selectedStudyUIDs)
    {
      selectedSeriesUIDs << d->DICOMDatabase->seriesForStudy(uid);
    }
  }

  QStringList fileList;
  foreach(const QString& selectedSeriesUID, selectedSeriesUIDs)
  {
    fileList << d->DICOMDatabase->filesForSeries(selectedSeriesUID);
  }
  return fileList;
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::showMetadata(const QStringList& fileList)
{
  Q_D(const ctkDICOMBrowser);
  d->MetadataDialog->setFileList(fileList);
  d->MetadataDialog->show();
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::exportSelectedItems(ctkDICOMModel::IndexType level)
{
  Q_D(const ctkDICOMBrowser);
  ctkFileDialog* directoryDialog = new ctkFileDialog();
  directoryDialog->setOption(QFileDialog::ShowDirsOnly);
  directoryDialog->setFileMode(QFileDialog::DirectoryOnly);
  bool res = directoryDialog->exec();
  if (!res)
  {
    delete directoryDialog;
    return;
  }
  QStringList dirs = directoryDialog->selectedFiles();
  delete directoryDialog;
  QString dirPath = dirs[0];

  QStringList selectedStudyUIDs;
  if (level == ctkDICOMModel::PatientType)
  {
    QStringList uids = d->dicomTableManager->currentPatientsSelection();
    foreach(const QString & uid, uids)
    {
      selectedStudyUIDs << d->DICOMDatabase->studiesForPatient(uid);
    }
  }
  if (level == ctkDICOMModel::StudyType)
  {
    selectedStudyUIDs = d->dicomTableManager->currentStudiesSelection();
  }

  QStringList selectedSeriesUIDs;
  if (level == ctkDICOMModel::SeriesType)
  {
    selectedSeriesUIDs = d->dicomTableManager->currentSeriesSelection();
  }
  else
  {
    foreach(const QString & uid, selectedStudyUIDs)
    {
      selectedSeriesUIDs << d->DICOMDatabase->seriesForStudy(uid);
    }
  }

  this->exportSeries(dirPath, selectedSeriesUIDs);
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::removeSelectedItems(ctkDICOMModel::IndexType level)
{
  Q_D(const ctkDICOMBrowser);
  QStringList selectedPatientUIDs;
  QStringList selectedStudyUIDs;
  if (level == ctkDICOMModel::PatientType)
  {
    selectedPatientUIDs = d->dicomTableManager->currentPatientsSelection();
    if (!this->confirmDeleteSelectedUIDs(selectedPatientUIDs))
    {
      return;
    }
    foreach(const QString & uid, selectedPatientUIDs)
    {
      selectedStudyUIDs << d->DICOMDatabase->studiesForPatient(uid);
    }
  }
  if (level == ctkDICOMModel::StudyType)
  {
    selectedStudyUIDs = d->dicomTableManager->currentStudiesSelection();
    if (!this->confirmDeleteSelectedUIDs(selectedStudyUIDs))
    {
      return;
    }
  }

  QStringList selectedSeriesUIDs;
  if (level == ctkDICOMModel::SeriesType)
  {
    selectedSeriesUIDs = d->dicomTableManager->currentSeriesSelection();
    if (!this->confirmDeleteSelectedUIDs(selectedSeriesUIDs))
    {
      return;
    }
  }
  else
  {
    foreach(const QString & uid, selectedStudyUIDs)
    {
      selectedSeriesUIDs << d->DICOMDatabase->seriesForStudy(uid);
    }
  }

  foreach(const QString & uid, selectedSeriesUIDs)
  {
    d->DICOMDatabase->removeSeries(uid);
  }
  foreach(const QString & uid, selectedStudyUIDs)
  {
    d->DICOMDatabase->removeStudy(uid);
  }
  foreach(const QString & uid, selectedPatientUIDs)
  {
    d->DICOMDatabase->removePatient(uid);
  }
  // Update the table views
  d->dicomTableManager->updateTableViews();
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::onIndexingProgress(int percent)
{
  Q_D(const ctkDICOMBrowser);
  d->ProgressFrame->show();
  d->ProgressBar->setValue(percent);
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::onIndexingProgressStep(const QString& step)
{
  Q_D(const ctkDICOMBrowser);
  d->ProgressLabel->setText(step);
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::onIndexingProgressDetail(const QString& detail)
{
  Q_D(const ctkDICOMBrowser);
  if (detail.isEmpty())
  {
    d->ProgressDetailLineEdit->hide();
  }
  else
  {
    d->ProgressDetailLineEdit->setText(detail);
    d->ProgressDetailLineEdit->show();
  }
}


//----------------------------------------------------------------------------
void ctkDICOMBrowser::onIndexingUpdatingDatabase(bool updating)
{
  Q_D(ctkDICOMBrowser);
  if (updating)
  {
    d->BatchUpdateBeforeIndexingUpdate = d->dicomTableManager->setBatchUpdate(true);
  }
  else
  {
    d->dicomTableManager->setBatchUpdate(d->BatchUpdateBeforeIndexingUpdate);
  }
}

//----------------------------------------------------------------------------
void ctkDICOMBrowser::onIndexingComplete(int patientsAdded, int studiesAdded, int seriesAdded, int imagesAdded)
{
  Q_D(ctkDICOMBrowser);

  d->PatientsAddedDuringImport += patientsAdded;
  d->StudiesAddedDuringImport += studiesAdded;
  d->SeriesAddedDuringImport += seriesAdded;
  d->InstancesAddedDuringImport += imagesAdded;

  d->ProgressFrame->hide();
  d->ProgressDetailLineEdit->hide();

  if (d->DisplayImportSummary)
  {
    QString message = QString("Import completed: added %1 patients, %2 studies, %3 series, %4 instances.")
      .arg(QString::number(patientsAdded))
      .arg(QString::number(studiesAdded))
      .arg(QString::number(seriesAdded))
      .arg(QString::number(imagesAdded));
    d->InformationMessageLabel->setText(message);
    d->InformationMessageFrame->show();
  }

  d->dicomTableManager->updateTableViews();

  // allow users of this widget to know that the process has finished
  emit directoryImported();
}
