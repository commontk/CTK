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
  and development was supported by the Center for Intelligent Image-guided Interventions (CI3).

=========================================================================*/

// Qt includes
#include <QCloseEvent>
#include <QDebug>
#include <QDate>
#include <QDesktopWidget>
#include <QFormLayout>
#include <QMap>
#include <QMenu>
#include <QProgressDialog>
#include <QScrollArea>
#include <QTableWidget>
#include <QToolButton>

// CTK includes
#include <ctkBasePopupWidget.h>
#include <ctkDirectoryButton.h>
#include <ctkFileDialog.h>
#include <ctkLogger.h>
#include <ctkMessageBox.h>
#include <ctkPopupWidget.h>

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMIndexer.h"
#include "ctkDICOMJob.h"
#include "ctkDICOMJobResponseSet.h"
#include "ctkDICOMScheduler.h"
#include "ctkDICOMScheduler.h"
#include "ctkDICOMServer.h"
#include "ctkDICOMThumbnailGenerator.h"
#include "ctkUtils.h"

// ctkDICOMWidgets includes
#include "ctkDICOMObjectListWidget.h"
#include "ctkDICOMVisualBrowserWidget.h"
#include "ctkDICOMSeriesItemWidget.h"
#include "ctkDICOMServerNodeWidget2.h"
#include "ctkDICOMVisualBrowserWidget.h"
#include "ui_ctkDICOMVisualBrowserWidget.h"

static ctkLogger logger("org.commontk.DICOM.Widgets.DICOMVisualBrowserWidget");

QColor ctkDICOMVisualBrowserWidgetDefaultColor(Qt::white);
QColor ctkDICOMVisualBrowserWidgetDarkModeDefaultColor(50, 50, 50);
QColor ctkDICOMVisualBrowserWidgetWarningColor(Qt::darkYellow);

class ctkDICOMMetadataDialog : public QDialog
{
public:
  ctkDICOMMetadataDialog(QWidget* parent = 0)
    : QDialog(parent)
  {
    this->setWindowFlags(Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint | Qt::Window);
    this->setModal(true);
    this->setSizeGripEnabled(true);
    QVBoxLayout* layout = new QVBoxLayout(this);
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

  void closeEvent(QCloseEvent* evt)
  {
    // just hide the window when close button is clicked
    evt->ignore();
    this->hide();
  }

  void showEvent(QShowEvent* event)
  {
    QDialog::showEvent(event);
    // QDialog would reset window position and size when shown.
    // Restore its previous size instead (user may look at metadata
    // of different series one after the other and would be inconvenient to
    // set the desired size manually each time).
    if (!this->savedGeometry.isEmpty())
    {
      this->restoreGeometry(this->savedGeometry);
      if (this->isMaximized())
      {
        this->setGeometry(QApplication::desktop()->availableGeometry(this));
      }
    }
  }

  void hideEvent(QHideEvent* event)
  {
    this->savedGeometry = this->saveGeometry();
    QDialog::hideEvent(event);
  }

protected:
  ctkDICOMObjectListWidget* tagListWidget;
  QByteArray savedGeometry;
};

//----------------------------------------------------------------------------
class ctkDICOMVisualBrowserWidgetPrivate : public Ui_ctkDICOMVisualBrowserWidget
{
  Q_DECLARE_PUBLIC(ctkDICOMVisualBrowserWidget);

protected:
  ctkDICOMVisualBrowserWidget* const q_ptr;
  QToolButton* patientsTabMenuToolButton;

public:
  ctkDICOMVisualBrowserWidgetPrivate(ctkDICOMVisualBrowserWidget& obj);
  ~ctkDICOMVisualBrowserWidgetPrivate();

  void init();
  void disconnectScheduler();
  void connectScheduler();
  void importDirectory(QString directory, ctkDICOMVisualBrowserWidget::ImportDirectoryMode mode);
  void importFiles(const QStringList& files, ctkDICOMVisualBrowserWidget::ImportDirectoryMode mode);
  void importOldSettings();
  void configureSearchIcon();
  void showUpdateSchemaDialog();
  void updateModalityCheckableComboBox();
  void createPatients(bool queryRetrieve = true,
                      bool isImport = false);
  bool areFiltersEmpty();
  void resetFilters();
  void updateFiltersWarnings();
  void setBackgroundColorToFilterWidgets(bool warning = false);
  void setBackgroundColorToWidget(QColor color, QWidget* widget);
  void retrieveSeries();
  bool updateServer(ctkDICOMServer* server);
  void removeAllPatientItemWidgets();
  QString findPatientItemFromPatientInfo(const QString& patientID = "",
                                         const QString& patientName = "",
                                         const QString& patientBirthDate = "");
  int findPatientTabIndexFromPatientItem(const QString& patientItem);
  void updateSeriesTablesSelection(ctkDICOMSeriesItemWidget* selectedSeriesItemWidget);
  QStringList getPatientItemsFromWidgets(ctkDICOMModel::IndexType level,
                                         QList<QWidget*> selectedWidgets);
  QStringList getPatientUIDsFromWidgets(ctkDICOMModel::IndexType level,
                                        QList<QWidget*> selectedWidgets);
  QStringList getStudyUIDsFromWidgets(ctkDICOMModel::IndexType level,
                                      QList<QWidget*> selectedWidgets);
  QStringList getSeriesUIDsFromWidgets(ctkDICOMModel::IndexType level,
                                       QList<QWidget*> selectedWidgets);
  QStringList filterPatientList(const QStringList& patientList,
                                const QMap<QString, QVariant>& filters);
  QStringList filterStudyList(const QStringList& patientList,
                              const QMap<QString, QVariant>& filters);
  QStringList filterSeriesList(const QStringList& patientList,
                               const QMap<QString, QVariant>& filters);
  ctkDICOMStudyItemWidget* getCurrentPatientStudyWidgetByUIDs(const QString& studyInstanceUID);
  ctkDICOMSeriesItemWidget* getCurrentPatientSeriesWidgetByUIDs(const QString& studyInstanceUID,
                                                                const QString& seriesInstanceUID);
  void getPatientsMetadata(bool queryRetrieve = true);
  void updateTabVisibilityForPatientItemWidgets();
  void updateTabVisibilityForPatientItemWidget(ctkDICOMPatientItemWidget* patientItemWidget);

  // Return a sanitized version of the string that is safe to be used
  // as a filename component.
  // All non-ASCII characters are replaced, because they may be used on an internal hard disk,
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

  // local count variables to keep track of the number of items
  // added to the database during an import operation
  int PatientsAddedDuringImport;
  int StudiesAddedDuringImport;
  int SeriesAddedDuringImport;
  int InstancesAddedDuringImport;
  ctkFileDialog* ImportDialog;
  ctkDICOMMetadataDialog* MetadataDialog;

  // Settings key that stores database directory
  QString DatabaseDirectorySettingsKey;

  // If database directory is specified with relative path then this directory will be used as a base
  QString DatabaseDirectoryBase;

  // Default database path to use if there is nothing in settings
  QString DefaultDatabaseDirectory;
  QString DatabaseDirectory;

  QSharedPointer<ctkDICOMDatabase> DicomDatabase;
  QSharedPointer<ctkDICOMThumbnailGenerator> ThumbnailGenerator;
  QSharedPointer<ctkDICOMScheduler> Scheduler;
  QSharedPointer<ctkDICOMIndexer> Indexer;

  QString FilteringPatientID;
  QString FilteringPatientName;

  QString FilteringStudyDescription;
  ctkDICOMPatientItemWidget::DateType FilteringDate;

  QString FilteringSeriesDescription;
  QStringList PreviousFilteringModalities;
  QStringList FilteringModalities;

  int NumberOfOpenedStudiesPerPatient;
  ctkDICOMStudyItemWidget::ThumbnailSizeOption ThumbnailSize;
  bool SendActionVisible;
  bool DeleteActionVisible;
  bool IsGUIUpdating;
  bool IsLoading;
  QString SelectedPatientItem;

  QProgressDialog* UpdateSchemaProgress;
  QProgressDialog* ExportProgress;
};

CTK_GET_CPP(ctkDICOMVisualBrowserWidget, QString, databaseDirectoryBase, DatabaseDirectoryBase);
CTK_SET_CPP(ctkDICOMVisualBrowserWidget, const QString&, setDatabaseDirectoryBase, DatabaseDirectoryBase);

//----------------------------------------------------------------------------
// ctkDICOMVisualBrowserWidgetPrivate methods

//----------------------------------------------------------------------------
ctkDICOMVisualBrowserWidgetPrivate::ctkDICOMVisualBrowserWidgetPrivate(ctkDICOMVisualBrowserWidget& obj)
  : q_ptr(&obj)
{
  this->ThumbnailGenerator = QSharedPointer<ctkDICOMThumbnailGenerator>(new ctkDICOMThumbnailGenerator);

  this->DicomDatabase = QSharedPointer<ctkDICOMDatabase>(new ctkDICOMDatabase);
  this->DicomDatabase->setThumbnailGenerator(ThumbnailGenerator.data());

  this->Scheduler = QSharedPointer<ctkDICOMScheduler>(new ctkDICOMScheduler);
  this->Scheduler->setDicomDatabase(this->DicomDatabase);

  this->Indexer = QSharedPointer<ctkDICOMIndexer>(new ctkDICOMIndexer);
  this->Indexer->setDatabase(this->DicomDatabase.data());
  this->Indexer->setBackgroundImportEnabled(true);

  this->MetadataDialog = new ctkDICOMMetadataDialog();
  this->MetadataDialog->setObjectName("DICOMMetadata");
  this->MetadataDialog->setWindowTitle(ctkDICOMVisualBrowserWidget::tr("DICOM File Metadata"));

  this->NumberOfOpenedStudiesPerPatient = 2;
  this->ThumbnailSize = ctkDICOMStudyItemWidget::ThumbnailSizeOption::Medium;
  this->SendActionVisible = false;
  this->DeleteActionVisible = true;

  this->FilteringDate = ctkDICOMPatientItemWidget::DateType::Any;

  this->FilteringModalities.append("Any");
  this->FilteringModalities.append("CR");
  this->FilteringModalities.append("CT");
  this->FilteringModalities.append("MR");
  this->FilteringModalities.append("NM");
  this->FilteringModalities.append("US");
  this->FilteringModalities.append("PT");
  this->FilteringModalities.append("XA");

  this->PatientsAddedDuringImport = 0;
  this->StudiesAddedDuringImport = 0;
  this->SeriesAddedDuringImport = 0;
  this->InstancesAddedDuringImport = 0;
  this->ImportDialog = nullptr;

  this->IsGUIUpdating = false;
  this->IsLoading = false;

  this->ExportProgress = nullptr;
  this->UpdateSchemaProgress = nullptr;
}

//----------------------------------------------------------------------------
ctkDICOMVisualBrowserWidgetPrivate::~ctkDICOMVisualBrowserWidgetPrivate()
{
  this->ImportDialog->deleteLater();
  this->MetadataDialog->deleteLater();
  this->removeAllPatientItemWidgets();
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidgetPrivate::init()
{
  Q_Q(ctkDICOMVisualBrowserWidget);
  this->setupUi(q);

  this->DatabaseDirectoryProblemFrame->hide();
  QObject::connect(this->SelectDatabaseDirectoryButton, SIGNAL(clicked()),
                   q, SLOT(selectDatabaseDirectory()));
  QObject::connect(this->CreateNewDatabaseButton, SIGNAL(clicked()),
                   q, SLOT(createNewDatabaseDirectory()));
  QObject::connect(this->UpdateDatabaseButton, SIGNAL(clicked()),
                   q, SLOT(updateDatabase()));

  this->WarningPushButton->hide();
  QObject::connect(this->WarningPushButton, SIGNAL(clicked()),
                   q, SLOT(onWarningPushButtonClicked()));

  QObject::connect(this->FilteringPatientIDSearchBox, SIGNAL(textChanged(QString)),
                   q, SLOT(onFilteringPatientIDChanged()));

  QObject::connect(this->FilteringPatientIDSearchBox, SIGNAL(returnPressed()),
                   q, SLOT(onQueryPatients()));

  QObject::connect(this->FilteringPatientNameSearchBox, SIGNAL(textChanged(QString)),
                   q, SLOT(onFilteringPatientNameChanged()));

  QObject::connect(this->FilteringPatientNameSearchBox, SIGNAL(returnPressed()),
                   q, SLOT(onQueryPatients()));

  QObject::connect(this->FilteringStudyDescriptionSearchBox, SIGNAL(textChanged(QString)),
                   q, SLOT(onFilteringStudyDescriptionChanged()));

  QObject::connect(this->FilteringStudyDescriptionSearchBox, SIGNAL(returnPressed()),
                   q, SLOT(onQueryPatients()));

  QObject::connect(this->FilteringSeriesDescriptionSearchBox, SIGNAL(textChanged(QString)),
                   q, SLOT(onFilteringSeriesDescriptionChanged()));

  QObject::connect(this->FilteringSeriesDescriptionSearchBox, SIGNAL(returnPressed()),
                   q, SLOT(onQueryPatients()));

  QObject::connect(this->FilteringModalityCheckableComboBox, SIGNAL(checkedIndexesChanged()),
                   q, SLOT(onFilteringModalityCheckableComboBoxChanged()));
  this->updateModalityCheckableComboBox();

  QObject::connect(this->FilteringModalityCheckableComboBox, SIGNAL(returnRelease()),
                   q, SLOT(onQueryPatients()));

  QObject::connect(this->FilteringDateComboBox, SIGNAL(currentIndexChanged(int)),
                   q, SLOT(onFilteringDateComboBoxChanged(int)));

  QObject::connect(this->FilteringDateComboBox, SIGNAL(returnRelease()),
                   q, SLOT(onQueryPatients()));

  QObject::connect(this->SearchMenuButton, SIGNAL(clicked()),
                   q, SLOT(onQueryPatients()));

  // Add query/retrieve options menu to SearchMenuButton button
  QMenu *queryRetrieveButtonMenu = new QMenu(ctkDICOMVisualBrowserWidget::tr("Show options"), this->SearchMenuButton);
  queryRetrieveButtonMenu->setToolTipsVisible(true);
  this->SearchMenuButton->setMenu(queryRetrieveButtonMenu);
  this->SearchMenuButton->setFixedWidth(this->SearchMenuButton->width());

  QAction *toggleQueryRetrieveAction = new QAction(ctkDICOMVisualBrowserWidget::tr("Query/Retrieve from servers"), queryRetrieveButtonMenu);
  toggleQueryRetrieveAction->setToolTip(ctkDICOMVisualBrowserWidget::tr("If enabled, the widget will also conduct queries and retrieve the data."));
  toggleQueryRetrieveAction->setCheckable(true);
  QSettings settings;
  if (settings.value("DICOM/QueryRetrieveEnabled", "") == "")
  {
    settings.setValue("DICOM/QueryRetrieveEnabled", true);
    settings.sync();
  }
  toggleQueryRetrieveAction->setChecked(settings.value("DICOM/QueryRetrieveEnabled", "").toBool());
  queryRetrieveButtonMenu->addAction(toggleQueryRetrieveAction);
  this->configureSearchIcon();

  QObject::connect(toggleQueryRetrieveAction, SIGNAL(toggled(bool)),
                   q, SLOT(onQueryRetrieveOptionToggled(bool)));

  this->PatientsTabWidget->clear();

  // setup patients menu
  this->patientsTabMenuToolButton = new QToolButton(q);
  this->patientsTabMenuToolButton->setObjectName("patientsTabMenuToolButton");
  this->patientsTabMenuToolButton->setCheckable(false);
  this->patientsTabMenuToolButton->setChecked(false);
  this->patientsTabMenuToolButton->setIcon(QIcon(":/Icons/more_vert.svg"));
  this->patientsTabMenuToolButton->hide();

  QObject::connect(this->patientsTabMenuToolButton, SIGNAL(clicked()),
                   q, SLOT(onPatientsTabMenuToolButtonClicked()));

  this->PatientsTabWidget->setCornerWidget(this->patientsTabMenuToolButton, Qt::TopLeftCorner);

  QObject::connect(this->PatientsTabWidget, SIGNAL(currentChanged(int)),
                   q, SLOT(onPatientItemChanged(int)));

  QObject::connect(this->PatientsTabWidget, SIGNAL(tabBarClicked(int)),
                   q, SLOT(onOperationStatusTabBarItemClicked(int)));

  QObject::connect(this->ServerNodeWidget, SIGNAL(serversSettingsChanged()),
                   q, SLOT(onServersSettingsChanged()));

  QObject::connect(this->ClosePushButton, SIGNAL(clicked()),
                   q, SLOT(onClose()));

  QObject::connect(this->ImportPushButton, SIGNAL(clicked()),
                   q, SLOT(openImportDialog()));

  // Initialize directoryMode widget
  QFormLayout* layout = new QFormLayout;
  QComboBox* importDirectoryModeComboBox = new QComboBox();
  importDirectoryModeComboBox->addItem(ctkDICOMVisualBrowserWidget::tr("Add Link"), static_cast<int>(ctkDICOMVisualBrowserWidget::ImportDirectoryAddLink));
  importDirectoryModeComboBox->addItem(ctkDICOMVisualBrowserWidget::tr("Copy"), static_cast<int>(ctkDICOMVisualBrowserWidget::ImportDirectoryCopy));
  importDirectoryModeComboBox->setToolTip(
        ctkDICOMVisualBrowserWidget::tr("Indicate if the files should be copied to the local database"
           " directory or if only links should be created ?"));
  layout->addRow(new QLabel(ctkDICOMVisualBrowserWidget::tr("Import Directory Mode:")), importDirectoryModeComboBox);
  layout->setContentsMargins(0, 0, 0, 0);
  QWidget* importDirectoryBottomWidget = new QWidget();
  importDirectoryBottomWidget->setLayout(layout);

  // Default values
  importDirectoryModeComboBox->setCurrentIndex(
        importDirectoryModeComboBox->findData(static_cast<int>(q->importDirectoryMode())));

  // Initialize import widget
  this->ImportDialog = new ctkFileDialog();
  this->ImportDialog->setBottomWidget(importDirectoryBottomWidget);
  this->ImportDialog->setFileMode(QFileDialog::Directory);
  // XXX Method setSelectionMode must be called after setFileMode
  this->ImportDialog->setSelectionMode(QAbstractItemView::ExtendedSelection);
  this->ImportDialog->setLabelText(QFileDialog::Accept, ctkDICOMVisualBrowserWidget::tr("Import"));
  this->ImportDialog->setWindowTitle(ctkDICOMVisualBrowserWidget::tr("Import DICOM files from directory ..."));
  this->ImportDialog->setWindowModality(Qt::ApplicationModal);

  QObject::connect(this->ImportDialog, SIGNAL(filesSelected(QStringList)),
                   q, SLOT(onImportDirectoriesSelected(QStringList)));

  QObject::connect(importDirectoryModeComboBox, SIGNAL(currentIndexChanged(int)),
                   q, SLOT(onImportDirectoryComboBoxCurrentIndexChanged(int)));

  this->ProgressFrame->hide();

  this->ServersSettingsCollapsibleGroupBox->setCollapsed(true);
  this->JobsCollapsibleGroupBox->setCollapsed(true);
  this->AdvancedCollapsibleGroupBox->setCollapsed(true);

  this->ServerNodeWidget->setScheduler(this->Scheduler);
  this->JobListWidget->setScheduler(this->Scheduler);
  this->connectScheduler();

  QObject::connect(this->JobListWidget, SIGNAL(patientSelected(QString, QString, QString)),
                   q, SLOT(patientSelectedOnJobList(QString, QString, QString)));

  QObject::connect(this->ProgressCancelButton, SIGNAL(clicked()), this->Indexer.data(), SLOT(cancel()));
  QObject::connect(this->Indexer.data(), SIGNAL(progress(int)), q, SLOT(onIndexingProgress(int)));
  QObject::connect(this->Indexer.data(), SIGNAL(progressStep(QString)), q, SLOT(onIndexingProgressStep(QString)));
  QObject::connect(this->Indexer.data(), SIGNAL(progressDetail(QString)), q, SLOT(onIndexingProgressDetail(QString)));
  QObject::connect(this->Indexer.data(), SIGNAL(indexingComplete(int, int, int, int)), q, SLOT(onIndexingComplete(int, int, int, int)));
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidgetPrivate::disconnectScheduler()
{
  Q_Q(ctkDICOMVisualBrowserWidget);
  if (!this->Scheduler)
  {
    return;
  }

  QObject::disconnect(this->Scheduler.data(), SIGNAL(progressJobDetail(QList<QVariant>)),
                      q, SLOT(updateGUIFromScheduler(QList<QVariant>)));
  QObject::disconnect(this->Scheduler.data(), SIGNAL(jobStarted(QList<QVariant>)),
                      q, SLOT(onJobStarted(QList<QVariant>)));
  QObject::disconnect(this->Scheduler.data(), SIGNAL(jobUserStopped(QList<QVariant>)),
                      q, SLOT(onJobUserStopped(QList<QVariant>)));
  QObject::disconnect(this->Scheduler.data(), SIGNAL(jobFailed(QList<QVariant>)),
                      q, SLOT(onJobFailed(QList<QVariant>)));
  QObject::disconnect(this->Scheduler.data(), SIGNAL(jobFinished(QList<QVariant>)),
                      q, SLOT(onJobFinished(QList<QVariant>)));
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidgetPrivate::connectScheduler()
{
  Q_Q(ctkDICOMVisualBrowserWidget);
  if (!this->Scheduler)
  {
    return;
  }

  QObject::connect(this->Scheduler.data(), SIGNAL(progressJobDetail(QList<QVariant>)),
                   q, SLOT(updateGUIFromScheduler(QList<QVariant>)));
  QObject::connect(this->Scheduler.data(), SIGNAL(jobStarted(QList<QVariant>)),
                   q, SLOT(onJobStarted(QList<QVariant>)));
  QObject::connect(this->Scheduler.data(), SIGNAL(jobUserStopped(QList<QVariant>)),
                   q, SLOT(onJobUserStopped(QList<QVariant>)));
  QObject::connect(this->Scheduler.data(), SIGNAL(jobFailed(QList<QVariant>)),
                   q, SLOT(onJobFailed(QList<QVariant>)));
  QObject::connect(this->Scheduler.data(), SIGNAL(jobFinished(QList<QVariant>)),
                   q, SLOT(onJobFinished(QList<QVariant>)));
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidgetPrivate::importDirectory(QString directory, ctkDICOMVisualBrowserWidget::ImportDirectoryMode mode)
{
  if (!this->DicomDatabase)
  {
    logger.error("importDirectory failed, no DICOM Database has been set. \n");
    return;
  }

  if (!this->Scheduler || !this->Indexer)
  {
    logger.error("importDirectory failed, no task pool has been set. \n");
    return;
  }

  if (!QDir(directory).exists())
  {
    logger.error(QString("importDirectory failed, input directory %1 does not exist. \n").arg(directory));
    return;
  }
  // Start background indexing
  this->Indexer->addDirectory(directory, mode == ctkDICOMVisualBrowserWidget::ImportDirectoryCopy);
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidgetPrivate::importFiles(const QStringList& files, ctkDICOMVisualBrowserWidget::ImportDirectoryMode mode)
{
  if (!this->DicomDatabase)
  {
    logger.error("importFiles failed, no DICOM Database has been set. \n");
    return;
  }

  if (!this->Scheduler || !this->Indexer)
  {
    logger.error("importFiles failed, no task pool has been set. \n");
    return;
  }

  // Start background indexing
  this->Indexer->addListOfFiles(files, mode == ctkDICOMVisualBrowserWidget::ImportDirectoryCopy);
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidgetPrivate::importOldSettings()
{
  // Backward compatibility
  QSettings settings;
  int dontConfirmCopyOnImport = settings.value("MainWindow/DontConfirmCopyOnImport", static_cast<int>(QMessageBox::InvalidRole)).toInt();
  if (dontConfirmCopyOnImport == QMessageBox::AcceptRole)
  {
    settings.setValue("DICOM/ImportDirectoryMode", static_cast<int>(ctkDICOMVisualBrowserWidget::ImportDirectoryCopy));
  }
  settings.remove("MainWindow/DontConfirmCopyOnImport");
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidgetPrivate::configureSearchIcon()
{
  QSettings settings;
  bool queryRetrieveEnabled = settings.value("DICOM/QueryRetrieveEnabled", "").toBool();
  if (queryRetrieveEnabled)
  {
    this->SearchMenuButton->setIcon(QIcon(":/Icons/query.svg"));
  }
  else
  {
    this->SearchMenuButton->setIcon(QIcon(":/Icons/search_local.svg"));
  }
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidgetPrivate::showUpdateSchemaDialog()
{
  Q_Q(ctkDICOMVisualBrowserWidget);
  if (this->UpdateSchemaProgress == 0)
  {
    //
    // Set up the Update Schema Progress Dialog
    //
    this->UpdateSchemaProgress = new QProgressDialog(
      ctkDICOMVisualBrowserWidget::tr("DICOM Schema Update"), ctkDICOMVisualBrowserWidget::tr("Cancel"), 0, 100, q, Qt::WindowTitleHint | Qt::WindowSystemMenuHint);

    // We don't want the progress dialog to resize itself, so we bypass the label by creating our own
    QLabel* progressLabel = new QLabel(ctkDICOMVisualBrowserWidget::tr("Initialization..."));
    this->UpdateSchemaProgress->setLabel(progressLabel);
    this->UpdateSchemaProgress->setWindowModality(Qt::ApplicationModal);
    this->UpdateSchemaProgress->setMinimumDuration(0);
    this->UpdateSchemaProgress->setValue(0);

    q->connect(DicomDatabase.data(), SIGNAL(schemaUpdateStarted(int)),
               this->UpdateSchemaProgress, SLOT(setMaximum(int)));
    q->connect(DicomDatabase.data(), SIGNAL(schemaUpdateProgress(int)),
               this->UpdateSchemaProgress, SLOT(setValue(int)));
    q->connect(DicomDatabase.data(), SIGNAL(schemaUpdateProgress(QString)),
               progressLabel, SLOT(setText(QString)));

    // close the dialog
    q->connect(this->DicomDatabase.data(), SIGNAL(schemaUpdated()),
               this->UpdateSchemaProgress, SLOT(close()));
  }
  this->UpdateSchemaProgress->show();
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidgetPrivate::updateModalityCheckableComboBox()
{
  QAbstractItemModel* model = this->FilteringModalityCheckableComboBox->checkableModel();
  int wasBlocking = this->FilteringModalityCheckableComboBox->blockSignals(true);

  if ((!this->PreviousFilteringModalities.contains("Any") &&
       this->FilteringModalities.contains("Any")) ||
      this->FilteringModalities.count() == 0)
  {
    this->FilteringModalities.clear();
    this->FilteringModalities.append("Any");
    this->FilteringModalities.append("CR");
    this->FilteringModalities.append("CT");
    this->FilteringModalities.append("MR");
    this->FilteringModalities.append("NM");
    this->FilteringModalities.append("US");
    this->FilteringModalities.append("PT");
    this->FilteringModalities.append("XA");

    for (int filterIndex = 0; filterIndex < this->FilteringModalityCheckableComboBox->count(); ++filterIndex)
    {
      QModelIndex modelIndex = model->index(filterIndex, 0);
      this->FilteringModalityCheckableComboBox->setCheckState(modelIndex, Qt::CheckState::Checked);
    }
    this->FilteringModalityCheckableComboBox->blockSignals(wasBlocking);
    return;
  }

  for (int i = 0; i < this->FilteringModalityCheckableComboBox->count(); ++i)
  {
    QModelIndex modelIndex = model->index(i, 0);
    this->FilteringModalityCheckableComboBox->setCheckState(modelIndex, Qt::CheckState::Unchecked);
  }

  foreach (QString modality, this->FilteringModalities)
  {
    QModelIndexList indexList = model->match(model->index(0, 0), 0, modality);
    if (indexList.length() == 0)
    {
      continue;
    }

    QModelIndex index = indexList[0];
    this->FilteringModalityCheckableComboBox->setCheckState(index, Qt::CheckState::Checked);
  }

  if (this->FilteringModalityCheckableComboBox->allChecked())
  {
    this->FilteringModalityCheckableComboBox->blockSignals(wasBlocking);
    return;
  }

  int anyCheckState = Qt::CheckState::Unchecked;
  QModelIndex anyModelIndex = model->index(0, 0);
  for (int filterIndex = 1; filterIndex < this->FilteringModalityCheckableComboBox->count(); ++filterIndex)
  {
    QModelIndex modelIndex = model->index(filterIndex, 0);
    if (this->FilteringModalityCheckableComboBox->checkState(modelIndex) != Qt::CheckState::Checked)
    {
      anyCheckState = Qt::CheckState::PartiallyChecked;
      break;
    }
  }

  if (anyCheckState == Qt::CheckState::PartiallyChecked)
  {
    this->FilteringModalityCheckableComboBox->setCheckState(anyModelIndex, Qt::CheckState::PartiallyChecked);
    this->FilteringModalities.removeAll("Any");
  }
  else
  {
    this->FilteringModalityCheckableComboBox->setCheckState(anyModelIndex, Qt::CheckState::Checked);
    this->FilteringModalities.append("Any");
  }

  this->FilteringModalityCheckableComboBox->blockSignals(wasBlocking);
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidgetPrivate::createPatients(bool queryRetrieve,
                                                        bool isImport)
{
  Q_Q(ctkDICOMVisualBrowserWidget);
  if (this->IsGUIUpdating)
  {
    return;
  }

  if (!this->DicomDatabase)
  {
    logger.error("createPatients failed, no DICOM database has been set. \n");
    return;
  }

  QStringList patientList = this->DicomDatabase->patients();
  if (patientList.count() == 0)
  {
    this->patientsTabMenuToolButton->hide();
    return;
  }

  this->patientsTabMenuToolButton->show();
  this->IsGUIUpdating = true;
  int wasBlocking = this->PatientsTabWidget->blockSignals(true);
  QMap<QString, QDateTime> patientsInsertDateTimeList;
  foreach (QString patientItem, patientList)
  {
    QString patientID = this->DicomDatabase->fieldForPatient("PatientID", patientItem);
    QString patientName = this->DicomDatabase->fieldForPatient("PatientsName", patientItem);
    patientName.replace(R"(^)", R"( )");
    int index = this->findPatientTabIndexFromPatientItem(patientItem);
    if (index != -1)
    {
      continue;
    }

    // Filter with patientID and patientsName
    if ((!this->FilteringPatientID.isEmpty() && !patientID.contains(this->FilteringPatientID, Qt::CaseInsensitive)) ||
        (!this->FilteringPatientName.isEmpty() && !patientName.contains(this->FilteringPatientName, Qt::CaseInsensitive)))
    {
      continue;
    }

    index = q->addPatientItemWidget(patientItem);

    QDateTime patientInsertDateTime = this->DicomDatabase->insertDateTimeForPatient(patientItem);
    patientsInsertDateTimeList[patientItem] = patientInsertDateTime;
  }

  int selectedIndex = this->findPatientTabIndexFromPatientItem(this->SelectedPatientItem);
  if (isImport && patientsInsertDateTimeList.count() > 0)
  {
    QList<QPair<QString, QDateTime>> list;
    for (QMap<QString, QDateTime>::const_iterator it = patientsInsertDateTimeList.cbegin();
      it != patientsInsertDateTimeList.cend(); ++it)
    {
      list.append(qMakePair(it.key(), it.value()));
    }

    std::sort(list.begin(), list.end(), [](const QPair<QString, QDateTime>& a, const QPair<QString, QDateTime>& b)
    {
      return a.second < b.second;
    });

    QString mostRecentPatientItem = list.last().first;
    selectedIndex = this->findPatientTabIndexFromPatientItem(mostRecentPatientItem);
  }
  this->PatientsTabWidget->setCurrentIndex(selectedIndex);
  this->PatientsTabWidget->blockSignals(wasBlocking);
  this->IsGUIUpdating = false;

  this->getPatientsMetadata(queryRetrieve);
}

//----------------------------------------------------------------------------
bool ctkDICOMVisualBrowserWidgetPrivate::areFiltersEmpty()
{
  return this->FilteringPatientID.isEmpty() &&
         this->FilteringPatientName.isEmpty() &&
         this->FilteringStudyDescription.isEmpty() &&
         this->FilteringSeriesDescription.isEmpty() &&
         this->FilteringDate == ctkDICOMPatientItemWidget::DateType::Any &&
         this->FilteringModalities.contains("Any");
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidgetPrivate::resetFilters()
{
  this->FilteringPatientID = "";
  this->FilteringPatientIDSearchBox->setText("");
  this->FilteringPatientName = "";
  this->FilteringPatientNameSearchBox->setText("");
  this->FilteringStudyDescription = "";
  this->FilteringStudyDescriptionSearchBox->setText("");
  this->FilteringSeriesDescription = "";
  this->FilteringSeriesDescriptionSearchBox->setText("");
  this->FilteringDate = ctkDICOMPatientItemWidget::DateType::Any;
  this->FilteringDateComboBox->setCurrentIndex(this->FilteringDate);

  this->FilteringModalities.append("Any");
  this->FilteringModalities.append("CR");
  this->FilteringModalities.append("CT");
  this->FilteringModalities.append("MR");
  this->FilteringModalities.append("NM");
  this->FilteringModalities.append("US");
  this->FilteringModalities.append("PT");
  this->FilteringModalities.append("XA");

  this->updateModalityCheckableComboBox();

  this->setBackgroundColorToFilterWidgets();
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidgetPrivate::updateFiltersWarnings()
{
  if (!this->DicomDatabase)
  {
    logger.error("updateFiltersWarnings failed, no DICOM database has been set. \n");
    return;
  }

  // Loop over all the data in the dicom database and apply the filters.
  // If there are no series, highlight which are the filters that produce no results
  this->setBackgroundColorToFilterWidgets();

  QColor color = ctkDICOMVisualBrowserWidgetWarningColor;
  QStringList patientList = this->DicomDatabase->patients();
  if (patientList.count() == 0)
  {
    this->setBackgroundColorToWidget(color, this->FilteringPatientIDSearchBox);
    this->setBackgroundColorToWidget(color, this->FilteringPatientNameSearchBox);
    return;
  }

  QMap<QString, QVariant> filters;
  filters.insert("PatientsName", this->FilteringPatientName);
  filters.insert("PatientID", this->FilteringPatientID);
  QStringList filteredPatientList = this->filterPatientList(patientList, filters);
  if (filteredPatientList.count() == 0)
  {
    this->setBackgroundColorToWidget(color, this->FilteringPatientIDSearchBox);
    this->setBackgroundColorToWidget(color, this->FilteringPatientNameSearchBox);
    return;
  }

  QStringList studiesList;
  foreach (QString patientItem, filteredPatientList)
  {
    studiesList.append(this->DicomDatabase->studiesForPatient(patientItem));
  }

  filters.clear();
  filters.insert("StudyDate", QString::number(ctkDICOMPatientItemWidget::getNDaysFromFilteringDate(this->FilteringDate)));
  filters.insert("StudyDescription", this->FilteringStudyDescription);

  QStringList filteredStudyList = this->filterStudyList(studiesList, filters);
  if (filteredStudyList.count() == 0)
  {
    this->setBackgroundColorToWidget(color, this->FilteringDateComboBox);
    this->setBackgroundColorToWidget(color, this->FilteringStudyDescriptionSearchBox);
    return;
  }

  QStringList seriesList;
  foreach (QString studyItem, filteredStudyList)
  {
    QString studyInstanceUID = this->DicomDatabase->fieldForStudy("StudyInstanceUID", studyItem);
    seriesList.append(this->DicomDatabase->seriesForStudy(studyInstanceUID));
  }

  filters.clear();
  filters.insert("Modality", this->FilteringModalities);
  filters.insert("SeriesDescription", this->FilteringSeriesDescription);

  QStringList filteredSeriesList = this->filterSeriesList(seriesList, filters);
  if (filteredSeriesList.count() == 0)
  {
    this->setBackgroundColorToWidget(color, this->FilteringSeriesDescriptionSearchBox);
    this->setBackgroundColorToWidget(color, this->FilteringModalityCheckableComboBox);
  }
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidgetPrivate::setBackgroundColorToFilterWidgets(bool warning)
{
  Q_Q(ctkDICOMVisualBrowserWidget);
  if (warning)
  {
    this->setBackgroundColorToWidget(ctkDICOMVisualBrowserWidgetWarningColor, this->FilteringPatientIDSearchBox);
    this->setBackgroundColorToWidget(ctkDICOMVisualBrowserWidgetWarningColor, this->FilteringPatientNameSearchBox);
    this->setBackgroundColorToWidget(ctkDICOMVisualBrowserWidgetWarningColor, this->FilteringDateComboBox);
    this->setBackgroundColorToWidget(ctkDICOMVisualBrowserWidgetWarningColor, this->FilteringStudyDescriptionSearchBox);
    this->setBackgroundColorToWidget(ctkDICOMVisualBrowserWidgetWarningColor, this->FilteringSeriesDescriptionSearchBox);
    this->setBackgroundColorToWidget(ctkDICOMVisualBrowserWidgetWarningColor, this->FilteringModalityCheckableComboBox);
  }
  else
  {
    QColor color(ctkDICOMVisualBrowserWidgetDefaultColor);
    QColor visualDICOMBrowserColor = q->palette().color(QPalette::Normal, q->backgroundRole());
    if (visualDICOMBrowserColor.lightnessF() < 0.5)
    {
      color = ctkDICOMVisualBrowserWidgetDarkModeDefaultColor;
    }
    this->setBackgroundColorToWidget(color, this->FilteringPatientIDSearchBox);
    this->setBackgroundColorToWidget(color, this->FilteringPatientNameSearchBox);
    this->setBackgroundColorToWidget(color, this->FilteringDateComboBox);
    this->setBackgroundColorToWidget(color, this->FilteringStudyDescriptionSearchBox);
    this->setBackgroundColorToWidget(color, this->FilteringSeriesDescriptionSearchBox);
    this->setBackgroundColorToWidget(color, this->FilteringModalityCheckableComboBox);
  }
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidgetPrivate::setBackgroundColorToWidget(QColor color,
                                                                    QWidget* widget)
{
  if (!widget)
  {
    return;
  }

  QPalette pal = widget->palette();
  QComboBox* comboBox = qobject_cast<QComboBox*>(widget);
  if (comboBox)
  {
    pal.setColor(QPalette::Button, color);
  }
  else
  {
    pal.setColor(widget->backgroundRole(), color);
  }
  widget->setPalette(pal);
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidgetPrivate::retrieveSeries()
{
  Q_Q(ctkDICOMVisualBrowserWidget);
  if (!this->Scheduler)
  {
    logger.error("retrieveSeries failed, no task pool has been set. \n");
    return;
  }

  if (this->IsLoading)
  {
    return;
  }

  ctkDICOMPatientItemWidget* currentPatientItemWidget =
    qobject_cast<ctkDICOMPatientItemWidget*>(this->PatientsTabWidget->currentWidget());
  if (!currentPatientItemWidget)
  {
    return;
  }

  this->IsLoading = true;

  // Get only the selected series widgets
  QList<ctkDICOMSeriesItemWidget*> selectedSeriesWidgetsList;
  QList<ctkDICOMStudyItemWidget*> studyItemWidgetsList = currentPatientItemWidget->studyItemWidgetsList();
  foreach (ctkDICOMStudyItemWidget* studyItemWidget, studyItemWidgetsList)
  {
    QTableWidget* seriesListTableWidget = studyItemWidget->seriesListTableWidget();
    QModelIndexList indexList = seriesListTableWidget->selectionModel()->selectedIndexes();
    foreach (QModelIndex index, indexList)
    {
      ctkDICOMSeriesItemWidget* seriesItemWidget = qobject_cast<ctkDICOMSeriesItemWidget*>
        (seriesListTableWidget->cellWidget(index.row(), index.column()));
      if (!seriesItemWidget)
      {
        continue;
      }

      selectedSeriesWidgetsList.append(seriesItemWidget);
    }
  }

  if (selectedSeriesWidgetsList.count() == 0)
  {
    this->IsLoading = false;
    return;
  }

  // Get all series widgets
  QList<ctkDICOMSeriesItemWidget*> seriesWidgetsList;
  for (int patientIndex = 0; patientIndex < this->PatientsTabWidget->count(); ++patientIndex)
  {
    ctkDICOMPatientItemWidget* patientItemWidget =
      qobject_cast<ctkDICOMPatientItemWidget*>(this->PatientsTabWidget->widget(patientIndex));
    if (!patientItemWidget)
    {
      continue;
    }

    QList<ctkDICOMStudyItemWidget*> studyItemWidgetsList = patientItemWidget->studyItemWidgetsList();
    foreach (ctkDICOMStudyItemWidget* studyItemWidget, studyItemWidgetsList)
    {
      QTableWidget* seriesListTableWidget = studyItemWidget->seriesListTableWidget();
      for (int row = 0; row < seriesListTableWidget->rowCount(); row++)
      {
        for (int column = 0; column < seriesListTableWidget->columnCount(); column++)
        {
          ctkDICOMSeriesItemWidget* seriesItemWidget =
            qobject_cast<ctkDICOMSeriesItemWidget*>(seriesListTableWidget->cellWidget(row, column));
          if (!seriesItemWidget)
          {
            continue;
          }

          seriesWidgetsList.append(seriesItemWidget);
        }
      }
    }
  }

  // Update UI
  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

  bool deleteActionWasVisible = this->DeleteActionVisible;
  this->DeleteActionVisible = false;

  bool queryPatientButtonWasEnabled = this->SearchMenuButton->isEnabled();
  this->SearchMenuButton->setEnabled(false);

  // Set a flag in non selected series widget to stop any jobs creation
  // and stop all the jobs connected to the widgets
  QStringList seriesInstanceUIDsToStop;
  foreach (ctkDICOMSeriesItemWidget* seriesItemWidget, seriesWidgetsList)
  {
    if (!seriesItemWidget)
    {
      continue;
    }

    if (!selectedSeriesWidgetsList.contains(seriesItemWidget))
    {
      seriesItemWidget->setStopJobs(true);
      seriesInstanceUIDsToStop.append(seriesItemWidget->seriesInstanceUID());
    }
  }

  this->Scheduler->stopJobsByDICOMUIDs({}, {}, seriesInstanceUIDsToStop);
  this->Scheduler->waitForDone(300);

  // Check all the selected series widgets. If any widgets is not fully retrieved or the retrieve failed,
  // and no jobs are running for the series, force the retrieve.
  foreach (ctkDICOMSeriesItemWidget* seriesItemWidget, selectedSeriesWidgetsList)
  {
    if (!seriesItemWidget)
    {
      continue;
    }

    if (!seriesItemWidget->isCloud() && !seriesItemWidget->retrieveFailed())
    {
      continue;
    }

    if (this->Scheduler->getJobsByDICOMUIDs({}, {}, {seriesItemWidget->seriesInstanceUID()}).count() == 0)
    {
      seriesItemWidget->forceRetrieve();
    }
  }

  // Create a progress dialog to show the progress of the retrieve
  QProgressDialog loadSeriesProgress(
    ctkDICOMVisualBrowserWidget::tr("Retrieving and processing selected series..."),
    ctkDICOMVisualBrowserWidget::tr("Cancel"), 0, 100, q);
  loadSeriesProgress.setWindowModality(Qt::ApplicationModal);
  loadSeriesProgress.setMinimumDuration(1000);

  QProgressBar *bar = new QProgressBar(&loadSeriesProgress);
  bar->setTextVisible(false);
  loadSeriesProgress.setBar(bar);
  loadSeriesProgress.setValue(0);

  // Wait for the selected series widgets to be fully retrieved
  bool wait = true;
  int progress = 0;
  while (wait)
  {
    qApp->processEvents();
    this->Scheduler->waitForDone(300);

    progress++;
    if (progress == 99)
    {
      progress = 0;
    }
    loadSeriesProgress.setValue(progress);

    wait = false;
    foreach (ctkDICOMSeriesItemWidget* seriesItemWidget, selectedSeriesWidgetsList)
    {
      if (!seriesItemWidget)
      {
        continue;
      }

      if (loadSeriesProgress.wasCanceled())
      {
        break;
      }

      if (seriesItemWidget->isCloud() && !seriesItemWidget->retrieveFailed())
      {
        wait = true;
        break;
      }
    }

    if (loadSeriesProgress.wasCanceled())
    {
      break;
    }
  }

  // Update UI
  this->updateFiltersWarnings();
  this->configureSearchIcon();

  this->IsLoading = false;
  this->DeleteActionVisible = deleteActionWasVisible;
  this->SearchMenuButton->setEnabled(queryPatientButtonWasEnabled);
  QApplication::restoreOverrideCursor();


  // Finalize the retrieve
  QStringList selectedSeriesInstanceUIDs;
  if (loadSeriesProgress.wasCanceled())
    {
    // It was canceled -> stop the jobs connected to the selected series widgets
    foreach (ctkDICOMSeriesItemWidget* seriesItemWidget, selectedSeriesWidgetsList)
    {
      if (!seriesItemWidget)
      {
        continue;
      }

      selectedSeriesInstanceUIDs.append(seriesItemWidget->seriesInstanceUID());
    }

    this->Scheduler->stopJobsByDICOMUIDs({}, {}, selectedSeriesInstanceUIDs);
    this->Scheduler->waitForDone(300);
    }
  else
    {
    loadSeriesProgress.close();
    foreach (ctkDICOMSeriesItemWidget* seriesItemWidget, selectedSeriesWidgetsList)
    {
      // If the series was not fully retrieved or the retrieve failed -> skip
      if (!seriesItemWidget || seriesItemWidget->isCloud() || seriesItemWidget->retrieveFailed())
      {
        continue;
      }

      // If the series has only metadata -> skip
      if (this->DicomDatabase->instancesForSeries(seriesItemWidget->seriesInstanceUID()).count() == 0)
      {
        continue;
      }

      selectedSeriesInstanceUIDs.append(seriesItemWidget->seriesInstanceUID());
    }

    q->emit seriesRetrieved(selectedSeriesInstanceUIDs);
  }

  // Re-allow all seriesItemWidgets to run jobs
  foreach (ctkDICOMSeriesItemWidget* seriesItemWidget, seriesWidgetsList)
  {
    if (!seriesItemWidget)
    {
      continue;
    }

    seriesItemWidget->setStopJobs(false);
  }
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidgetPrivate::removeAllPatientItemWidgets()
{
  Q_Q(ctkDICOMVisualBrowserWidget);

  int wasBlocking = this->PatientsTabWidget->blockSignals(true);
  for (int patientIndex = 0; patientIndex < this->PatientsTabWidget->count(); ++patientIndex)
  {
    ctkDICOMPatientItemWidget* patientItemWidget =
      qobject_cast<ctkDICOMPatientItemWidget*>(this->PatientsTabWidget->widget(patientIndex));
    if (!patientItemWidget)
    {
      continue;
    }

    this->PatientsTabWidget->removeTab(patientIndex);
    q->disconnect(patientItemWidget, SIGNAL(customContextMenuRequested(const QPoint&)),
                  q, SLOT(showPatientContextMenu(const QPoint&)));

    QList<ctkDICOMStudyItemWidget*> studyItemWidgets = patientItemWidget->studyItemWidgetsList();
    foreach (ctkDICOMStudyItemWidget* studyItemWidget, studyItemWidgets)
    {
      q->disconnect(studyItemWidget->seriesListTableWidget(), SIGNAL(itemDoubleClicked(QTableWidgetItem*)),
                    q, SLOT(onLoad()));
    }

    delete patientItemWidget;
    patientIndex--;
  }
  this->PatientsTabWidget->blockSignals(wasBlocking);
}

//----------------------------------------------------------------------------
QString ctkDICOMVisualBrowserWidgetPrivate::findPatientItemFromPatientInfo(const QString &patientID,
                                                                           const QString &patientName,
                                                                           const QString &patientBirthDate)
{
  QString patientItem;
  if (patientID.isEmpty() && patientName.isEmpty() && patientBirthDate.isEmpty())
  {
    return patientItem;
  }

  for (int index = 0; index < this->PatientsTabWidget->count(); ++index)
  {
    ctkDICOMPatientItemWidget* patientItemWidget =
      qobject_cast<ctkDICOMPatientItemWidget*>(this->PatientsTabWidget->widget(index));
    if (!patientItemWidget)
    {
      continue;
    }

    if ((patientID.isEmpty() || patientItemWidget->patientID() == patientID) &&
      (patientName.isEmpty() || patientItemWidget->patientName() == patientName) &&
      (patientBirthDate.isEmpty() || patientItemWidget->patientBirthDate() == patientBirthDate))
    {
      return this->PatientsTabWidget->tabWhatsThis(index);
    }
  }

  return patientItem;
}

//----------------------------------------------------------------------------
int ctkDICOMVisualBrowserWidgetPrivate::findPatientTabIndexFromPatientItem(const QString& patientItem)
{
  int patientIndex = -1;
  for (int index = 0; index < this->PatientsTabWidget->count(); ++index)
  {
    if (patientItem == this->PatientsTabWidget->tabWhatsThis(index))
    {
      patientIndex = index;
      break;
    }
  }

  return patientIndex;
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidgetPrivate::updateSeriesTablesSelection(ctkDICOMSeriesItemWidget* selectedSeriesItemWidget)
{
  if (!selectedSeriesItemWidget)
  {
    return;
  }

  ctkDICOMPatientItemWidget* currentPatientItemWidget =
    qobject_cast<ctkDICOMPatientItemWidget*>(this->PatientsTabWidget->currentWidget());
  if (!currentPatientItemWidget)
  {
    return;
  }

  QList<ctkDICOMStudyItemWidget*> studyItemWidgetsList = currentPatientItemWidget->studyItemWidgetsList();
  foreach (ctkDICOMStudyItemWidget* studyItemWidget, studyItemWidgetsList)
  {
    if (!studyItemWidget)
    {
      continue;
    }
    QTableWidget* seriesListTableWidget = studyItemWidget->seriesListTableWidget();
    QList<QTableWidgetItem*> selectedItems = seriesListTableWidget->selectedItems();
    foreach (QTableWidgetItem* selectedItem, selectedItems)
    {
      if (!selectedItem)
      {
        continue;
      }

      int row = selectedItem->row();
      int column = selectedItem->column();
      ctkDICOMSeriesItemWidget* seriesItemWidget =
          qobject_cast<ctkDICOMSeriesItemWidget*>(seriesListTableWidget->cellWidget(row, column));

      if (seriesItemWidget == selectedSeriesItemWidget)
      {
        seriesListTableWidget->itemClicked(selectedItem);
        return;
      }
    }
  }
}

//----------------------------------------------------------------------------
QStringList ctkDICOMVisualBrowserWidgetPrivate::getPatientItemsFromWidgets(ctkDICOMModel::IndexType level,
                                                                           QList<QWidget *> selectedWidgets)
{
  QStringList selectedPatientItems;

  if (!this->DicomDatabase)
  {
    return selectedPatientItems;
  }

  foreach (QWidget* selectedWidget, selectedWidgets)
  {
    if (!selectedWidget)
    {
      continue;
    }

    if (level == ctkDICOMModel::PatientType)
    {
      ctkDICOMPatientItemWidget* patientItemWidget =
        qobject_cast<ctkDICOMPatientItemWidget*>(selectedWidget);
      if (patientItemWidget)
      {
        QString patientItem = patientItemWidget->patientItem();
        selectedPatientItems << patientItem;
      }
    }
  }

  return selectedPatientItems;
}

//----------------------------------------------------------------------------
QStringList ctkDICOMVisualBrowserWidgetPrivate::getPatientUIDsFromWidgets(ctkDICOMModel::IndexType level,
                                                                          QList<QWidget*> selectedWidgets)
{
  QStringList selectedPatientUIDs;

  if (!this->DicomDatabase)
  {
    return selectedPatientUIDs;
  }

  foreach (QWidget* selectedWidget, selectedWidgets)
  {
    if (!selectedWidget)
    {
      continue;
    }

    if (level == ctkDICOMModel::PatientType)
    {
      ctkDICOMPatientItemWidget* patientItemWidget =
        qobject_cast<ctkDICOMPatientItemWidget*>(selectedWidget);
      if (patientItemWidget)
      {
        QString patientID = patientItemWidget->patientID();
        selectedPatientUIDs << patientID;
      }
    }
  }

  return selectedPatientUIDs;
}

//----------------------------------------------------------------------------
QStringList ctkDICOMVisualBrowserWidgetPrivate::getStudyUIDsFromWidgets(ctkDICOMModel::IndexType level,
                                                                        QList<QWidget*> selectedWidgets)
{
  QStringList selectedStudyUIDs;

  if (!this->DicomDatabase)
  {
    return selectedStudyUIDs;
  }

  foreach (QWidget* selectedWidget, selectedWidgets)
  {
    if (!selectedWidget)
    {
      continue;
    }

    if (level == ctkDICOMModel::PatientType)
    {
      ctkDICOMPatientItemWidget* patientItemWidget =
        qobject_cast<ctkDICOMPatientItemWidget*>(selectedWidget);
      if (patientItemWidget)
      {
        selectedStudyUIDs << this->DicomDatabase->studiesForPatient(patientItemWidget->patientItem());
      }
    }
    else if (level == ctkDICOMModel::StudyType)
    {
      ctkDICOMStudyItemWidget* studyItemWidget =
        qobject_cast<ctkDICOMStudyItemWidget*>(selectedWidget);
      if (studyItemWidget)
      {
        selectedStudyUIDs << studyItemWidget->studyInstanceUID();
      }
    }
  }

  return selectedStudyUIDs;
}

//----------------------------------------------------------------------------
QStringList ctkDICOMVisualBrowserWidgetPrivate::getSeriesUIDsFromWidgets(ctkDICOMModel::IndexType level,
                                                                         QList<QWidget*> selectedWidgets)
{
  QStringList selectedStudyUIDs;
  QStringList selectedSeriesUIDs;

  if (!this->DicomDatabase)
  {
    return selectedSeriesUIDs;
  }

  foreach (QWidget* selectedWidget, selectedWidgets)
  {
    if (!selectedWidget)
    {
      continue;
    }

    if (level == ctkDICOMModel::PatientType)
    {
      ctkDICOMPatientItemWidget* patientItemWidget =
        qobject_cast<ctkDICOMPatientItemWidget*>(selectedWidget);
      if (patientItemWidget)
      {
        selectedStudyUIDs << this->DicomDatabase->studiesForPatient(patientItemWidget->patientItem());
      }
    }
    else if (level == ctkDICOMModel::StudyType)
    {
      ctkDICOMStudyItemWidget* studyItemWidget =
        qobject_cast<ctkDICOMStudyItemWidget*>(selectedWidget);
      if (studyItemWidget)
      {
        selectedStudyUIDs << studyItemWidget->studyInstanceUID();
      }
    }

    if (level == ctkDICOMModel::SeriesType)
    {
      ctkDICOMSeriesItemWidget* seriesItemWidget =
        qobject_cast<ctkDICOMSeriesItemWidget*>(selectedWidget);
      if (seriesItemWidget)
      {
        selectedSeriesUIDs << seriesItemWidget->seriesInstanceUID();
      }
    }
    else
    {
      foreach (const QString& uid, selectedStudyUIDs)
      {
        selectedSeriesUIDs << this->DicomDatabase->seriesForStudy(uid);
      }
    }
  }

  return selectedSeriesUIDs;
}

//----------------------------------------------------------------------------
QStringList ctkDICOMVisualBrowserWidgetPrivate::filterPatientList(const QStringList& patientList,
                                                                  const QMap<QString, QVariant>& filters)
{
  QStringList filteredPatientList;
  if (!this->DicomDatabase)
  {
    logger.error("filterPatientList failed, no DICOM Database has been set. \n");
    return filteredPatientList;
  }

  foreach (QString patientItem, patientList)
  {
    bool filtered = false;
    for (QString key : filters.keys())
    {
      QString filter = this->DicomDatabase->fieldForPatient(key, patientItem);
      QString filterValue = filters.value(key).toString();
      if (!filter.contains(filterValue, Qt::CaseInsensitive))
      {
        filtered = true;
        break;
      }
    }

    if (filtered)
    {
      continue;
    }

    filteredPatientList.append(patientItem);
  }

  return filteredPatientList;
}

//----------------------------------------------------------------------------
QStringList ctkDICOMVisualBrowserWidgetPrivate::filterStudyList(const QStringList& studyList,
                                                                const QMap<QString, QVariant>& filters)
{
  QStringList filteredStudyList;
  if (!this->DicomDatabase)
  {
    logger.error("filterStudyList failed, no DICOM Database has been set. \n");
    return filteredStudyList;
  }

  foreach (QString studyItem, studyList)
  {
    bool filtered = false;
    for (QString key : filters.keys())
    {
      QString filter = this->DicomDatabase->fieldForStudy(key, studyItem);
      QString filterValue = filters.value(key).toString();
      if (key == "StudyDate")
      {
        int nDays = filterValue.toInt();
        if (nDays != -1)
        {
          QDate endDate = QDate::currentDate();
          QDate startDate = endDate.addDays(-nDays);
          filter.replace(QString("-"), QString(""));
          QDate studyDate = QDate::fromString(filter, "yyyyMMdd");
          if (studyDate < startDate || studyDate > endDate)
          {
            filtered = true;
            break;
          }
        }
      }
      else if (!filter.contains(filterValue, Qt::CaseInsensitive))
      {
        filtered = true;
        break;
      }
    }

    if (filtered)
    {
      continue;
    }

    filteredStudyList.append(studyItem);
  }

  return filteredStudyList;
}

//----------------------------------------------------------------------------
QStringList ctkDICOMVisualBrowserWidgetPrivate::filterSeriesList(const QStringList& seriesList,
                                                                 const QMap<QString, QVariant>& filters)
{
  QStringList filteredSeriesList;
  if (!this->DicomDatabase)
  {
    logger.error("filterSeriesList failed, no DICOM Database has been set. \n");
    return filteredSeriesList;
  }

  foreach (QString seriesItem, seriesList)
  {
    bool filtered = false;
    for (QString key : filters.keys())
    {
      QString filter = this->DicomDatabase->fieldForSeries(key, seriesItem);
      if (key == "Modality")
      {
        QStringList filterValues = filters.value(key).toStringList();
        if (!filterValues.contains("Any") && !filterValues.contains(filter))
        {
          filtered = true;
          break;
        }
      }
      else
      {
        QString filterValue = filters.value(key).toString();
        if (!filter.contains(filterValue, Qt::CaseInsensitive))
        {
          filtered = true;
          break;
        }
      }
    }

    if (filtered)
    {
      continue;
    }

    filteredSeriesList.append(seriesItem);
  }

  return filteredSeriesList;
}

//----------------------------------------------------------------------------
ctkDICOMStudyItemWidget* ctkDICOMVisualBrowserWidgetPrivate::getCurrentPatientStudyWidgetByUIDs(const QString& studyInstanceUID)
{
  ctkDICOMPatientItemWidget* patientItemWidget =
    qobject_cast<ctkDICOMPatientItemWidget*>(this->PatientsTabWidget->currentWidget());
  if (!patientItemWidget)
  {
    return nullptr;
  }

  QList<ctkDICOMStudyItemWidget*> studyItemWidgetsList = patientItemWidget->studyItemWidgetsList();
  foreach (ctkDICOMStudyItemWidget* studyItemWidget, studyItemWidgetsList)
  {
    if (!studyItemWidget || studyItemWidget->studyInstanceUID() != studyInstanceUID)
    {
      continue;
    }

    return studyItemWidget;
  }

  return nullptr;
}

//----------------------------------------------------------------------------
ctkDICOMSeriesItemWidget* ctkDICOMVisualBrowserWidgetPrivate::getCurrentPatientSeriesWidgetByUIDs(const QString& studyInstanceUID,
                                                                                                  const QString& seriesInstanceUID)
{
  ctkDICOMStudyItemWidget* studyItemWidget = this->getCurrentPatientStudyWidgetByUIDs(studyInstanceUID);
  if (!studyItemWidget)
  {
    return nullptr;
  }

  QList<ctkDICOMSeriesItemWidget*> seriesItemWidgetsList = studyItemWidget->seriesItemWidgetsList();
  foreach (ctkDICOMSeriesItemWidget* seriesItemWidget, seriesItemWidgetsList)
  {
    if (!seriesItemWidget || seriesItemWidget->seriesInstanceUID() != seriesInstanceUID)
    {
      continue;
    }

    return seriesItemWidget;
  }

  return nullptr;
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidgetPrivate::getPatientsMetadata(bool queryRetrieve)
{
  ctkDICOMPatientItemWidget* currentPatientItemWidget =
    qobject_cast<ctkDICOMPatientItemWidget*>(this->PatientsTabWidget->currentWidget());

  for (int patientIndex = 0; patientIndex < this->PatientsTabWidget->count(); ++patientIndex)
  {
    ctkDICOMPatientItemWidget* patientItemWidget =
      qobject_cast<ctkDICOMPatientItemWidget*>(this->PatientsTabWidget->widget(patientIndex));
    if (!patientItemWidget)
    {
      continue;
    }

    QSettings settings;
    bool queryRetrieveEnabled = settings.value("DICOM/QueryRetrieveEnabled", "").toBool();
    bool query = queryRetrieve && queryRetrieveEnabled;
    bool retrieve = false;
    if (patientItemWidget == currentPatientItemWidget)
    {
      retrieve = queryRetrieve && queryRetrieveEnabled;
    }

    patientItemWidget->generateStudies(query, retrieve);
  }
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidgetPrivate::updateTabVisibilityForPatientItemWidgets()
{
  for (int patientIndex = 0; patientIndex < this->PatientsTabWidget->count(); ++patientIndex)
  {
    ctkDICOMPatientItemWidget* patientItemWidget =
      qobject_cast<ctkDICOMPatientItemWidget*>(this->PatientsTabWidget->widget(patientIndex));
    if (!patientItemWidget)
    {
      continue;
    }
    this->updateTabVisibilityForPatientItemWidget(patientItemWidget);
  }
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidgetPrivate::updateTabVisibilityForPatientItemWidget(ctkDICOMPatientItemWidget *patientItemWidget)
{
  if (this->IsGUIUpdating)
  {
    return;
  }

  if(this->areFiltersEmpty())
  {
    return;
  }

  int index = this->findPatientTabIndexFromPatientItem(patientItemWidget->patientItem());
  if (index == -1)
  {
    return;
  }

  this->IsGUIUpdating = true;
  int wasBlocking = this->PatientsTabWidget->blockSignals(true);

  QString patientItem;
  ctkDICOMPatientItemWidget* currentPatientItemWidget =
    qobject_cast<ctkDICOMPatientItemWidget*>(this->PatientsTabWidget->currentWidget());
  if (currentPatientItemWidget)
  {
    patientItem = currentPatientItemWidget->patientItem();
  }

  bool patientVisible = false;
  QStringList studiesList = this->DicomDatabase->studiesForPatient(patientItemWidget->patientItem());

  QList<ctkDICOMStudyItemWidget*> studyItemWidgetsList = patientItemWidget->studyItemWidgetsList();
  foreach (ctkDICOMStudyItemWidget* studyItemWidget, studyItemWidgetsList)
  {
    if (!studyItemWidget)
    {
      continue;
    }

    if (studyItemWidget->filteredSeriesCount() != 0)
    {
      patientVisible = true;
      break;
    }
  }

#if QT_VERSION <= QT_VERSION_CHECK(5, 13, 0)
  this->PatientsTabWidget->setTabEnabled(index, patientVisible);
  this->PatientsTabWidget->setTabText(index, patientVisible ? patientItemWidget->patientName() : "");
  this->PatientsTabWidget->setTabIcon(index, patientVisible ? QIcon(":/Icons/patient.svg") : QIcon());
#else
  this->PatientsTabWidget->setTabVisible(index, patientVisible);
#endif

  int selectedIndex = this->findPatientTabIndexFromPatientItem(patientItem);
  if (selectedIndex == index && !patientVisible)
  {
    this->PatientsTabWidget->setCurrentIndex(0);
  }
  else if (selectedIndex != -1)
  {
    this->PatientsTabWidget->setCurrentIndex(selectedIndex);
  }
  this->PatientsTabWidget->blockSignals(wasBlocking);
  this->IsGUIUpdating = false;
}

//----------------------------------------------------------------------------
// ctkDICOMVisualBrowserWidget methods

//----------------------------------------------------------------------------
ctkDICOMVisualBrowserWidget::ctkDICOMVisualBrowserWidget(QWidget* parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new ctkDICOMVisualBrowserWidgetPrivate(*this))
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->init();
}

//----------------------------------------------------------------------------
ctkDICOMVisualBrowserWidget::~ctkDICOMVisualBrowserWidget() = default;

//----------------------------------------------------------------------------
CTK_GET_CPP(ctkDICOMVisualBrowserWidget, QString, databaseDirectory, DatabaseDirectory);
CTK_GET_CPP(ctkDICOMVisualBrowserWidget, QString, databaseDirectorySettingsKey, DatabaseDirectorySettingsKey);
CTK_GET_CPP(ctkDICOMVisualBrowserWidget, QString, filteringPatientID, FilteringPatientID);
CTK_GET_CPP(ctkDICOMVisualBrowserWidget, QString, filteringPatientName, FilteringPatientName);
CTK_GET_CPP(ctkDICOMVisualBrowserWidget, QString, filteringStudyDescription, FilteringStudyDescription);
CTK_GET_CPP(ctkDICOMVisualBrowserWidget, ctkDICOMPatientItemWidget::DateType, filteringDate, FilteringDate);
CTK_GET_CPP(ctkDICOMVisualBrowserWidget, QString, filteringSeriesDescription, FilteringSeriesDescription);
CTK_GET_CPP(ctkDICOMVisualBrowserWidget, QStringList, filteringModalities, FilteringModalities);
CTK_SET_CPP(ctkDICOMVisualBrowserWidget, int, setNumberOfOpenedStudiesPerPatient, NumberOfOpenedStudiesPerPatient);
CTK_GET_CPP(ctkDICOMVisualBrowserWidget, int, numberOfOpenedStudiesPerPatient, NumberOfOpenedStudiesPerPatient);
CTK_SET_CPP(ctkDICOMVisualBrowserWidget, const ctkDICOMStudyItemWidget::ThumbnailSizeOption&, setThumbnailSize, ThumbnailSize);
CTK_GET_CPP(ctkDICOMVisualBrowserWidget, ctkDICOMStudyItemWidget::ThumbnailSizeOption, thumbnailSize, ThumbnailSize);
CTK_SET_CPP(ctkDICOMVisualBrowserWidget, bool, setSendActionVisible, SendActionVisible);
CTK_GET_CPP(ctkDICOMVisualBrowserWidget, bool, isSendActionVisible, SendActionVisible);
CTK_SET_CPP(ctkDICOMVisualBrowserWidget, bool, setDeleteActionVisible, DeleteActionVisible);
CTK_GET_CPP(ctkDICOMVisualBrowserWidget, bool, isDeleteActionVisible, DeleteActionVisible);

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::setDatabaseDirectorySettingsKey(const QString& key)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->DatabaseDirectorySettingsKey = key;

  QSettings settings;
  QString databaseDirectory = ctk::absolutePathFromInternal(settings.value(d->DatabaseDirectorySettingsKey, "").toString(), d->DatabaseDirectoryBase);
  this->setDatabaseDirectory(databaseDirectory);
}

//----------------------------------------------------------------------------
static void skipDelete(QObject* obj)
{
  Q_UNUSED(obj);
  // this deleter does not delete the object from memory
  // useful if the pointer is not owned by the smart pointer
}

//----------------------------------------------------------------------------
ctkDICOMScheduler* ctkDICOMVisualBrowserWidget::scheduler() const
{
  Q_D(const ctkDICOMVisualBrowserWidget);
  return d->Scheduler.data();
}

//----------------------------------------------------------------------------
QSharedPointer<ctkDICOMScheduler> ctkDICOMVisualBrowserWidget::schedulerShared() const
{
  Q_D(const ctkDICOMVisualBrowserWidget);
  return d->Scheduler;
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::setScheduler(ctkDICOMScheduler& Scheduler)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->disconnectScheduler();
  d->Scheduler = QSharedPointer<ctkDICOMScheduler>(&Scheduler, skipDelete);
  d->ServerNodeWidget->setScheduler(d->Scheduler);
  d->JobListWidget->setScheduler(d->Scheduler);
  d->connectScheduler();
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::setScheduler(QSharedPointer<ctkDICOMScheduler> Scheduler)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->disconnectScheduler();
  d->Scheduler = Scheduler;
  d->ServerNodeWidget->setScheduler(d->Scheduler);
  d->JobListWidget->setScheduler(d->Scheduler);
  d->connectScheduler();
}

//----------------------------------------------------------------------------
ctkDICOMDatabase* ctkDICOMVisualBrowserWidget::dicomDatabase() const
{
  Q_D(const ctkDICOMVisualBrowserWidget);
  return d->DicomDatabase.data();
}

//----------------------------------------------------------------------------
QSharedPointer<ctkDICOMDatabase> ctkDICOMVisualBrowserWidget::dicomDatabaseShared() const
{
  Q_D(const ctkDICOMVisualBrowserWidget);
  return d->DicomDatabase;
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::setTagsToPrecache(const QStringList& tags)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  if (!d->DicomDatabase)
  {
    logger.error("setTagsToPrecache failed, no DICOM Database has been set. \n");
    return;
  }

  d->DicomDatabase->setTagsToPrecache(tags);
}

//----------------------------------------------------------------------------
const QStringList ctkDICOMVisualBrowserWidget::tagsToPrecache()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  if (!d->DicomDatabase)
  {
    logger.error("Get tagsToPrecache failed, no DICOM Database has been set. \n");
    return QStringList();
  }

  return d->DicomDatabase->tagsToPrecache();
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::setStorageAETitle(const QString& storageAETitle)
{
  Q_D(const ctkDICOMVisualBrowserWidget);
  d->ServerNodeWidget->setStorageAETitle(storageAETitle);
}

//----------------------------------------------------------------------------
QString ctkDICOMVisualBrowserWidget::storageAETitle() const
{
  Q_D(const ctkDICOMVisualBrowserWidget);
  return d->ServerNodeWidget->storageAETitle();
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::setStoragePort(int storagePort)
{
  Q_D(const ctkDICOMVisualBrowserWidget);
  d->ServerNodeWidget->setStoragePort(storagePort);
}

//----------------------------------------------------------------------------
int ctkDICOMVisualBrowserWidget::storagePort() const
{
  Q_D(const ctkDICOMVisualBrowserWidget);
  return d->ServerNodeWidget->storagePort();
}

//----------------------------------------------------------------------------
int ctkDICOMVisualBrowserWidget::serversCount()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  return d->ServerNodeWidget->serversCount();
}

//----------------------------------------------------------------------------
ctkDICOMServer* ctkDICOMVisualBrowserWidget::server(int id)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  return d->ServerNodeWidget->server(id);
}

//----------------------------------------------------------------------------
ctkDICOMServer* ctkDICOMVisualBrowserWidget::server(const QString& connectionName)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  return d->ServerNodeWidget->server(connectionName);
}

//----------------------------------------------------------------------------
int ctkDICOMVisualBrowserWidget::addServer(ctkDICOMServer* server)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  return d->ServerNodeWidget->addServer(server);
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::removeServer(const QString& connectionName)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  return d->ServerNodeWidget->removeServer(connectionName);
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::removeServer(int id)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  return d->ServerNodeWidget->removeServer(id);
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::removeAllServers()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  return d->ServerNodeWidget->removeAllServers();
}

//----------------------------------------------------------------------------
QString ctkDICOMVisualBrowserWidget::getServerNameFromIndex(int id)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  return d->ServerNodeWidget->getServerNameFromIndex(id);
}

//----------------------------------------------------------------------------
int ctkDICOMVisualBrowserWidget::getServerIndexFromName(const QString& connectionName)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  return d->ServerNodeWidget->getServerIndexFromName(connectionName);
}

//------------------------------------------------------------------------------
ctkDICOMJobListWidget *ctkDICOMVisualBrowserWidget::jobListWidget()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  return d->JobListWidget;
}

//------------------------------------------------------------------------------
ctkCollapsibleGroupBox *ctkDICOMVisualBrowserWidget::jobListGroupBox()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  return d->JobsCollapsibleGroupBox;
}

//------------------------------------------------------------------------------
ctkDICOMServerNodeWidget2 *ctkDICOMVisualBrowserWidget::serverSettingsWidget()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  return d->ServerNodeWidget;
}

//------------------------------------------------------------------------------
ctkCollapsibleGroupBox* ctkDICOMVisualBrowserWidget::serverSettingsGroupBox()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  return d->ServersSettingsCollapsibleGroupBox;
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::setFilteringPatientID(const QString& filteringPatientID)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->FilteringPatientID = filteringPatientID;
  d->FilteringPatientIDSearchBox->setText(d->FilteringPatientID);
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::setFilteringPatientName(const QString& filteringPatientName)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->FilteringPatientName = filteringPatientName;
  d->FilteringPatientNameSearchBox->setText(d->FilteringPatientName);
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::setFilteringStudyDescription(const QString& filteringStudyDescription)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->FilteringStudyDescription = filteringStudyDescription;
  d->FilteringStudyDescriptionSearchBox->setText(d->FilteringStudyDescription);
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::setFilteringDate(const ctkDICOMPatientItemWidget::DateType& filteringDate)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->FilteringDate = filteringDate;
  d->FilteringDateComboBox->setCurrentIndex(d->FilteringDate);
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::setFilteringSeriesDescription(const QString& filteringSeriesDescription)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->FilteringSeriesDescription = filteringSeriesDescription;
  d->FilteringSeriesDescriptionSearchBox->setText(d->FilteringSeriesDescription);
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::setFilteringModalities(const QStringList& filteringModalities)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->FilteringModalities = filteringModalities;
  d->updateModalityCheckableComboBox();
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::resetFilters()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->resetFilters();
}

//----------------------------------------------------------------------------
int ctkDICOMVisualBrowserWidget::addPatientItemWidget(const QString& patientItem)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  if (!d->DicomDatabase)
  {
    logger.error("addPatientItemWidget failed, no DICOM database has been set. \n");
    return -1;
  }

  QString patientName = d->DicomDatabase->fieldForPatient("PatientsName", patientItem);
  patientName.replace(R"(^)", R"( )");
  QString patientID = d->DicomDatabase->fieldForPatient("PatientID", patientItem);
  QString date = d->DicomDatabase->fieldForPatient("PatientsBirthDate", patientItem);
  date.replace(QString("-"), QString(""));
  date = QDate::fromString(date, "yyyyMMdd").toString();
  QString sex = d->DicomDatabase->fieldForPatient("PatientsSex", patientItem);

  ctkDICOMPatientItemWidget* patientItemWidget = new ctkDICOMPatientItemWidget(this);
  patientItemWidget->setDicomDatabase(d->DicomDatabase);
  patientItemWidget->setScheduler(d->Scheduler);
  patientItemWidget->setPatientItem(patientItem);
  patientItemWidget->setPatientID(patientID);
  patientItemWidget->setPatientName(patientName);
  patientItemWidget->setPatientBirthDate(date);
  patientItemWidget->setPatientSex(sex);
  patientItemWidget->setFilteringStudyDescription(d->FilteringStudyDescription);
  patientItemWidget->setFilteringDate(d->FilteringDate);
  patientItemWidget->setFilteringSeriesDescription(d->FilteringSeriesDescription);
  patientItemWidget->setFilteringModalities(d->FilteringModalities);
  patientItemWidget->setThumbnailSize(d->ThumbnailSize);
  patientItemWidget->setNumberOfOpenedStudiesPerPatient(d->NumberOfOpenedStudiesPerPatient);
  patientItemWidget->setContextMenuPolicy(Qt::CustomContextMenu);
  this->connect(patientItemWidget, SIGNAL(customContextMenuRequested(const QPoint&)),
                this, SLOT(showPatientContextMenu(const QPoint&)));
  this->connect(patientItemWidget, SIGNAL(updateGUIFinished()),
                this, SLOT(onPatientUpdateGUIFinished()));

  int index = d->PatientsTabWidget->addTab(patientItemWidget, QIcon(":/Icons/patient.svg"), patientName);
  d->PatientsTabWidget->setTabWhatsThis(index, patientItem);

  return index;
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::removePatientItemWidget(const QString& patientItem)
{
  Q_D(ctkDICOMVisualBrowserWidget);

  for (int patientIndex = 0; patientIndex < d->PatientsTabWidget->count(); ++patientIndex)
  {
    ctkDICOMPatientItemWidget* patientItemWidget =
      qobject_cast<ctkDICOMPatientItemWidget*>(d->PatientsTabWidget->widget(patientIndex));

    if (!patientItemWidget || patientItemWidget->patientItem() != patientItem)
    {
      continue;
    }

    d->PatientsTabWidget->removeTab(patientIndex);
    this->disconnect(patientItemWidget, SIGNAL(customContextMenuRequested(const QPoint&)),
                     this, SLOT(showPatientContextMenu(const QPoint&)));
    delete patientItemWidget;
    break;
  }
}

//------------------------------------------------------------------------------
ctkDICOMPatientItemWidget *ctkDICOMVisualBrowserWidget::patientItemWidgetByPatientItem(const QString &patientItem)
{
  Q_D(ctkDICOMVisualBrowserWidget);

  for (int patientIndex = 0; patientIndex < d->PatientsTabWidget->count(); ++patientIndex)
  {
    ctkDICOMPatientItemWidget* patientItemWidget =
      qobject_cast<ctkDICOMPatientItemWidget*>(d->PatientsTabWidget->widget(patientIndex));
    if (!patientItemWidget)
    {
      continue;
    }

    if (patientItemWidget->patientItem() != patientItem)
    {
      continue;
    }

    return patientItemWidget;
  }

  return nullptr;
}

//------------------------------------------------------------------------------
ctkDICOMPatientItemWidget *ctkDICOMVisualBrowserWidget::patientItemWidgetByPatientID(const QString &patientID)
{
  Q_D(ctkDICOMVisualBrowserWidget);

  for (int patientIndex = 0; patientIndex < d->PatientsTabWidget->count(); ++patientIndex)
  {
    ctkDICOMPatientItemWidget* patientItemWidget =
      qobject_cast<ctkDICOMPatientItemWidget*>(d->PatientsTabWidget->widget(patientIndex));
    if (!patientItemWidget)
    {
      continue;
    }

    if (patientItemWidget->patientID() != patientID)
    {
      continue;
    }

    return patientItemWidget;
  }

  return nullptr;
}

//------------------------------------------------------------------------------
ctkDICOMPatientItemWidget* ctkDICOMVisualBrowserWidget::patientItemWidgetByPatientName(const QString& patientName)
{
  Q_D(ctkDICOMVisualBrowserWidget);

  for (int patientIndex = 0; patientIndex < d->PatientsTabWidget->count(); ++patientIndex)
  {
    ctkDICOMPatientItemWidget* patientItemWidget =
      qobject_cast<ctkDICOMPatientItemWidget*>(d->PatientsTabWidget->widget(patientIndex));
    if (!patientItemWidget)
    {
      continue;
    }

    if (patientItemWidget->patientName() != patientName)
    {
      continue;
    }

    return patientItemWidget;
  }

  return nullptr;
}

//------------------------------------------------------------------------------
QTabWidget* ctkDICOMVisualBrowserWidget::patientsTabWidget()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  return d->PatientsTabWidget;
}

//------------------------------------------------------------------------------
int ctkDICOMVisualBrowserWidget::patientsAddedDuringImport()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  return d->PatientsAddedDuringImport;
}

//------------------------------------------------------------------------------
int ctkDICOMVisualBrowserWidget::studiesAddedDuringImport()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  return d->StudiesAddedDuringImport;
}

//------------------------------------------------------------------------------
int ctkDICOMVisualBrowserWidget::seriesAddedDuringImport()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  return d->SeriesAddedDuringImport;
}

//------------------------------------------------------------------------------
int ctkDICOMVisualBrowserWidget::instancesAddedDuringImport()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  return d->InstancesAddedDuringImport;
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::resetItemsAddedDuringImportCounters()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->PatientsAddedDuringImport = 0;
  d->StudiesAddedDuringImport = 0;
  d->SeriesAddedDuringImport = 0;
  d->InstancesAddedDuringImport = 0;
}

//------------------------------------------------------------------------------
ctkDICOMVisualBrowserWidget::ImportDirectoryMode ctkDICOMVisualBrowserWidget::importDirectoryMode() const
{
  Q_D(const ctkDICOMVisualBrowserWidget);
  ctkDICOMVisualBrowserWidgetPrivate* mutable_d = const_cast<ctkDICOMVisualBrowserWidgetPrivate*>(d);
  mutable_d->importOldSettings();
  QSettings settings;
  return static_cast<ctkDICOMVisualBrowserWidget::ImportDirectoryMode>(settings.value(
    "DICOM/ImportDirectoryMode", static_cast<int>(ctkDICOMVisualBrowserWidget::ImportDirectoryAddLink)).toInt());
}

//------------------------------------------------------------------------------
ctkFileDialog* ctkDICOMVisualBrowserWidget::importDialog() const
{
  Q_D(const ctkDICOMVisualBrowserWidget);
  return d->ImportDialog;
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::setImportDirectoryMode(ImportDirectoryMode mode)
{
  Q_D(ctkDICOMVisualBrowserWidget);

  QSettings settings;
  settings.setValue("DICOM/ImportDirectoryMode", static_cast<int>(mode));
  if (!d->ImportDialog)
  {
    return;
  }
  if (!(d->ImportDialog->options() & QFileDialog::DontUseNativeDialog))
  {
    return; // Native dialog does not support modifying or getting widget elements.
  }
  QComboBox* comboBox = d->ImportDialog->bottomWidget()->findChild<QComboBox*>();
  comboBox->setCurrentIndex(comboBox->findData(static_cast<int>(mode)));
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::setDatabaseDirectory(const QString& directory)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  if (!d->DicomDatabase)
  {
    logger.error("setDatabaseDirectory failed, no DICOM database has been set. \n");
    return;
  }

  QString absDirectory = ctk::absolutePathFromInternal(directory, d->DatabaseDirectoryBase);

  // close the active DICOM database
  d->DicomDatabase->closeDatabase();

  // open DICOM database on the directory
  QString databaseFileName = QDir(absDirectory).filePath("ctkDICOM.sql");

  bool success = true;
  if (!QDir(absDirectory).exists()
    || (!ctk::isDirEmpty(QDir(absDirectory)) && !QFile(databaseFileName).exists()))
  {
    logger.warn("Database folder does not contain ctkDICOM.sql file: " + absDirectory + "\n");
    d->DatabaseDirectoryProblemFrame->show();
    d->DatabaseDirectoryProblemLabel->setText(
      //: %1 is the folder path
      tr("No valid DICOM database found in folder %1.").arg(absDirectory)
    );
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
      d->DicomDatabase->openDatabase(databaseFileName);
      databaseOpenSuccess = d->DicomDatabase->isOpen();
    }
    catch (const std::exception& e)
    {
      Q_UNUSED(e);
      databaseOpenSuccess = false;
    }
    if (!databaseOpenSuccess || d->DicomDatabase->schemaVersionLoaded().isEmpty())
    {
      logger.warn(tr("Database error: %1 \n").arg(d->DicomDatabase->lastError()));
      d->DicomDatabase->closeDatabase();
      d->DatabaseDirectoryProblemFrame->show();
      d->DatabaseDirectoryProblemLabel->setText(
        //: %1 is the folder path
        tr("No valid DICOM database found in folder %1.").arg(absDirectory)
      );
      d->UpdateDatabaseButton->hide();
      d->CreateNewDatabaseButton->show();
      d->SelectDatabaseDirectoryButton->show();
      success = false;
    }
  }

  if (success)
  {
    if (d->DicomDatabase->schemaVersionLoaded() != d->DicomDatabase->schemaVersion())
    {
      logger.warn(QString("Database version mismatch: version of selected database = %1, version required = %2 \n")
        .arg(d->DicomDatabase->schemaVersionLoaded()).arg(d->DicomDatabase->schemaVersion()));
      d->DicomDatabase->closeDatabase();
      d->DatabaseDirectoryProblemFrame->show();
      d->DatabaseDirectoryProblemLabel->setText(
        //: %1 is the folder path
        tr("Incompatible DICOM database version found in folder %1.").arg(absDirectory)
      );
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
  emit databaseDirectoryChanged(absDirectory);
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::openImportDialog()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->ProgressFrame->show();
  d->ProgressDetailLineEdit->hide();
  int dialogCode = d->ImportDialog->exec();
  if (dialogCode == QDialog::Rejected)
  {
    d->ProgressFrame->hide();
  }
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::importDirectories(const QStringList& directories, ImportDirectoryMode mode)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  this->resetItemsAddedDuringImportCounters();
  d->ProgressFrame->show();
  foreach (const QString& directory, directories)
  {
    d->importDirectory(directory, mode);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::importDirectory(const QString& directory, ImportDirectoryMode mode)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  this->resetItemsAddedDuringImportCounters();
  d->ProgressFrame->show();
  d->importDirectory(directory, mode);
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::importFiles(const QStringList& files, ImportDirectoryMode mode)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  this->resetItemsAddedDuringImportCounters();
  d->ProgressFrame->show();
  d->importFiles(files, mode);
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::waitForImportFinished()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  if (!d->Scheduler || !d->Indexer)
  {
    logger.error("waitForImportFinished failed, no task pool has been set. \n");
    return;
  }
  d->Indexer->waitForImportFinished();
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onIndexingProgress(int percent)
{
  Q_D(const ctkDICOMVisualBrowserWidget);
  d->ProgressBar->setValue(percent);
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onIndexingProgressStep(const QString& step)
{
  Q_D(const ctkDICOMVisualBrowserWidget);
  d->ProgressLabel->setText(step);
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onIndexingProgressDetail(const QString& detail)
{
  Q_D(const ctkDICOMVisualBrowserWidget);
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

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onIndexingComplete(int patientsAdded, int studiesAdded, int seriesAdded, int imagesAdded)
{
  Q_D(ctkDICOMVisualBrowserWidget);

  d->PatientsAddedDuringImport += patientsAdded;
  d->StudiesAddedDuringImport += studiesAdded;
  d->SeriesAddedDuringImport += seriesAdded;
  d->InstancesAddedDuringImport += imagesAdded;

  d->ProgressFrame->hide();
  d->ProgressDetailLineEdit->hide();
  d->configureSearchIcon();

  // allow users of this widget to know that the process has finished
  emit directoryImported();

  d->createPatients(false, true);
  d->setBackgroundColorToFilterWidgets();
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::selectDatabaseDirectory()
{
  Q_D(const ctkDICOMVisualBrowserWidget);
  d->DatabaseDirectoryProblemFrame->hide();
  ctkDirectoryButton directoryButton(this);
  directoryButton.setDirectory(d->DatabaseDirectory);
  QString dir = directoryButton.browse();
  this->setDatabaseDirectory(dir);
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::createNewDatabaseDirectory()
{
  Q_D(ctkDICOMVisualBrowserWidget);

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
    if (!ctk::isDirEmpty(QDir(baseFolder)))
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
  for (int attempt = 0; attempt < attemptsCount; attempt++)
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
    if (!ctk::isDirEmpty(QDir(newFolder)))
    {
      continue;
    }
    // Folder exists and empty, try to use this
    setDatabaseDirectory(newFolder);
    return;
  }
  std::cerr << "Failed to create new database in folder: " << qPrintable(baseFolder) << "\n";
  d->DatabaseDirectoryProblemFrame->show();
  d->DatabaseDirectoryProblemLabel->setText(
    //: %1 is the folder path
    tr("Failed to create new database in folder %1.").arg(QDir(baseFolder).absolutePath())
  );
  d->UpdateDatabaseButton->hide();
  d->CreateNewDatabaseButton->show();
  d->SelectDatabaseDirectoryButton->show();
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::updateDatabase()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->DatabaseDirectoryProblemFrame->hide();
  d->showUpdateSchemaDialog();
  QString dir = this->databaseDirectory();
  // open DICOM database on the directory
  QString databaseFileName = QDir(dir).filePath("ctkDICOM.sql");
  try
  {
    d->DicomDatabase->openDatabase(databaseFileName);
  }
  catch (const std::exception& e)
  {
    Q_UNUSED(e);
    std::cerr << "Database error: " << qPrintable(d->DicomDatabase->lastError()) << "\n";
    d->DicomDatabase->closeDatabase();
    return;
  }
  d->DicomDatabase->updateSchema();
  // Update GUI
  this->setDatabaseDirectory(dir);
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onWarningPushButtonClicked()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->WarningPushButton->hide();
  d->JobsCollapsibleGroupBox->setChecked(true);
  d->AdvancedCollapsibleGroupBox->setChecked(true);
}

//------------------------------------------------------------------------------
QStringList ctkDICOMVisualBrowserWidget::fileListForCurrentSelection(ctkDICOMModel::IndexType level,
                                                                     const QList<QWidget*>& selectedWidgets)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  if (!d->DicomDatabase)
  {
    logger.error("fileListForCurrentSelection failed, no DICOM database has been set. \n");
    return QStringList();
  }

  QStringList selectedSeriesUIDs = d->getSeriesUIDsFromWidgets(level, selectedWidgets);

  QStringList fileList;
  foreach (const QString& selectedSeriesUID, selectedSeriesUIDs)
  {
    fileList << d->DicomDatabase->filesForSeries(selectedSeriesUID);
  }
  return fileList;
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::showMetadata(const QStringList& fileList)
{
  Q_D(const ctkDICOMVisualBrowserWidget);
  d->MetadataDialog->setFileList(fileList);
  d->MetadataDialog->show();
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::forceSeriesRetrieve(const QList<QWidget *> &selectedWidgets)
{
  foreach (QWidget* selectedWidget, selectedWidgets)
  {
    if (!selectedWidget)
    {
      continue;
    }
    ctkDICOMSeriesItemWidget* seriesItemWidget =
      qobject_cast<ctkDICOMSeriesItemWidget*>(selectedWidget);
    if (seriesItemWidget)
    {
      seriesItemWidget->forceRetrieve();
    }

    ctkDICOMStudyItemWidget* studyItemWidget =
        qobject_cast<ctkDICOMStudyItemWidget*>(selectedWidget);
    if (studyItemWidget)
    {
      QList<ctkDICOMSeriesItemWidget*> seriesItemWidgetsList = studyItemWidget->seriesItemWidgetsList();
      foreach (ctkDICOMSeriesItemWidget* seriesItemWidget, seriesItemWidgetsList)
      {
        if (!seriesItemWidget)
        {
          continue;
        }

        seriesItemWidget->forceRetrieve();
      }
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::removeSelectedItems(ctkDICOMModel::IndexType level,
                                                      const QList<QWidget*>& selectedWidgets)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  if (!d->DicomDatabase)
  {
    logger.error("removeSelectedItems failed, no DICOM database has been set. \n");
    return;
  }

  QStringList selectedPatientItems;
  QStringList selectedPatientUIDs;
  QStringList selectedStudyUIDs;
  QStringList selectedSeriesUIDs;

  if (level == ctkDICOMModel::RootType)
  {
    for (int patientIndex = 0; patientIndex < d->PatientsTabWidget->count(); ++patientIndex)
    {
      ctkDICOMPatientItemWidget* patientItemWidget =
        qobject_cast<ctkDICOMPatientItemWidget*>(d->PatientsTabWidget->widget(patientIndex));
      if (!patientItemWidget)
      {
        continue;
      }
      QString patientItem = patientItemWidget->patientItem();
      QString patientID = patientItemWidget->patientID();
      selectedStudyUIDs << d->DicomDatabase->studiesForPatient(patientItem);
      selectedPatientUIDs << patientID;
      selectedPatientItems << patientItem;
    }

    if (!this->confirmDeleteSelectedUIDs(selectedPatientUIDs))
    {
      return;
    }

    d->removeAllPatientItemWidgets();
  }
  else if (level == ctkDICOMModel::PatientType)
  {
    selectedPatientUIDs = d->getPatientUIDsFromWidgets(ctkDICOMModel::PatientType, selectedWidgets);
    selectedPatientItems = d->getPatientItemsFromWidgets(ctkDICOMModel::PatientType, selectedWidgets);
    if (!this->confirmDeleteSelectedUIDs(selectedPatientUIDs))
    {
      return;
    }
  }
  else if (level == ctkDICOMModel::StudyType)
  {
    selectedStudyUIDs = d->getStudyUIDsFromWidgets(ctkDICOMModel::StudyType, selectedWidgets);
    if (!this->confirmDeleteSelectedUIDs(selectedStudyUIDs))
    {
      return;
    }
  }
  else if (level == ctkDICOMModel::SeriesType)
  {
    selectedSeriesUIDs = d->getSeriesUIDsFromWidgets(ctkDICOMModel::SeriesType, selectedWidgets);
    if (!this->confirmDeleteSelectedUIDs(selectedSeriesUIDs))
    {
      return;
    }
  }

  foreach (QWidget* selectedWidget, selectedWidgets)
  {
    if (!selectedWidget)
    {
      continue;
    }
    else if (level == ctkDICOMModel::PatientType)
    {
      ctkDICOMPatientItemWidget* patientItemWidget =
        qobject_cast<ctkDICOMPatientItemWidget*>(selectedWidget);
      if (patientItemWidget)
      {
        QString patientItem = patientItemWidget->patientItem();
        selectedStudyUIDs << d->DicomDatabase->studiesForPatient(patientItem);

        this->removePatientItemWidget(patientItem);
      }
    }
    else if (level == ctkDICOMModel::StudyType)
    {
      ctkDICOMStudyItemWidget* studyItemWidget =
        qobject_cast<ctkDICOMStudyItemWidget*>(selectedWidget);
      if (studyItemWidget)
      {
        ctkDICOMPatientItemWidget* patientItemWidget =
          qobject_cast<ctkDICOMPatientItemWidget*>(d->PatientsTabWidget->currentWidget());
        if (patientItemWidget)
        {
          patientItemWidget->removeStudyItemWidget(studyItemWidget->studyItem());
        }
      }
    }

    if (level == ctkDICOMModel::SeriesType)
    {
      ctkDICOMSeriesItemWidget* seriesItemWidget =
        qobject_cast<ctkDICOMSeriesItemWidget*>(selectedWidget);
      if (seriesItemWidget)
      {
        ctkDICOMPatientItemWidget* patientItemWidget =
          qobject_cast<ctkDICOMPatientItemWidget*>(d->PatientsTabWidget->currentWidget());
        if (patientItemWidget)
        {
          QList<ctkDICOMStudyItemWidget*> studyItemWidgetsList = patientItemWidget->studyItemWidgetsList();
          foreach (ctkDICOMStudyItemWidget* studyItemWidget, studyItemWidgetsList)
          {
            if (!studyItemWidget || studyItemWidget->studyInstanceUID() != seriesItemWidget->studyInstanceUID())
            {
              continue;
            }

            studyItemWidget->removeSeriesItemWidget(seriesItemWidget->seriesItem());
            break;
          }
        }
      }
    }
    else
    {
      foreach (const QString& uid, selectedStudyUIDs)
      {
        selectedSeriesUIDs << d->DicomDatabase->seriesForStudy(uid);
      }
    }
  }

  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
  d->Scheduler->stopJobsByDICOMUIDs(selectedPatientUIDs,
                                    selectedStudyUIDs,
                                    selectedSeriesUIDs);

  foreach (const QString& uid, selectedSeriesUIDs)
  {
    d->DicomDatabase->removeSeries(uid, false, true);
  }
  foreach (const QString& uid, selectedStudyUIDs)
  {
    d->DicomDatabase->removeStudy(uid, true);
  }
  foreach (const QString& uid, selectedPatientItems)
  {
    d->DicomDatabase->removePatient(uid, true);
  }
  QApplication::restoreOverrideCursor();
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onFilteringPatientIDChanged()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->FilteringPatientID = d->FilteringPatientIDSearchBox->text();
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onFilteringPatientNameChanged()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->FilteringPatientName = d->FilteringPatientNameSearchBox->text();
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onFilteringStudyDescriptionChanged()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->FilteringStudyDescription = d->FilteringStudyDescriptionSearchBox->text();
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onFilteringSeriesDescriptionChanged()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->FilteringSeriesDescription = d->FilteringSeriesDescriptionSearchBox->text();
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onFilteringModalityCheckableComboBoxChanged()
{
  Q_D(ctkDICOMVisualBrowserWidget);

  d->PreviousFilteringModalities = d->FilteringModalities;
  d->FilteringModalities.clear();
  QModelIndexList indexList = d->FilteringModalityCheckableComboBox->checkedIndexes();
  foreach (QModelIndex index, indexList)
  {
    QVariant value = d->FilteringModalityCheckableComboBox->checkableModel()->data(index);
    d->FilteringModalities.append(value.toString());
  }
  d->updateModalityCheckableComboBox();
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onFilteringDateComboBoxChanged(int index)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->FilteringDate = static_cast<ctkDICOMPatientItemWidget::DateType>(index);
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onShowPatients()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  if (d->IsGUIUpdating)
  {
    return;
  }

  if (!d->DicomDatabase)
  {
    logger.error("onQueryPatient failed, no DICOM database has been set. \n");
    return;
  }

  // Stop any fetching task.
  this->onStop();

  // Save current patient selection
  QString patientItem;
  ctkDICOMPatientItemWidget* currentPatientItemWidget =
    qobject_cast<ctkDICOMPatientItemWidget*>(d->PatientsTabWidget->currentWidget());
  if (currentPatientItemWidget)
  {
    d->SelectedPatientItem = currentPatientItemWidget->patientItem();
  }

  // Clear the UI.
  d->removeAllPatientItemWidgets();

  if (d->DicomDatabase->patients().count() == 0)
  {
    d->setBackgroundColorToFilterWidgets(true);

    d->WarningPushButton->setText(tr("No patients have been found in the local database."));
    d->WarningPushButton->show();
    d->patientsTabMenuToolButton->hide();
    return;
  }
  else
  {
    d->WarningPushButton->hide();
  }

  d->createPatients(false);
  d->updateFiltersWarnings();
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onQueryPatients()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  if (d->IsGUIUpdating)
  {
    return;
  }

  QSettings settings;
  bool queryRetrieveEnabled = settings.value("DICOM/QueryRetrieveEnabled", "").toBool();
  if (!queryRetrieveEnabled)
    {
    this->onShowPatients();
    return;
    }

  if (!d->DicomDatabase)
  {
    logger.error("onQueryPatient failed, no DICOM database has been set. \n");
    return;
  }

  if (!d->Scheduler)
  {
    logger.error("onQueryPatient failed, no scheduler has been set. \n");
    return;
  }

  // Stop any fetching task.
  this->onStop();

  // Save current patient selection
  QString patientItem;
  ctkDICOMPatientItemWidget* currentPatientItemWidget =
    qobject_cast<ctkDICOMPatientItemWidget*>(d->PatientsTabWidget->currentWidget());
  if (currentPatientItemWidget)
  {
    d->SelectedPatientItem = currentPatientItemWidget->patientItem();
  }

  // Clear the UI.
  d->removeAllPatientItemWidgets();

  bool filtersEmpty = d->areFiltersEmpty();

  if (d->DicomDatabase->patients().count() == 0 &&
      (filtersEmpty || d->Scheduler->queryRetrieveServersCount() == 0))
  {
    d->setBackgroundColorToFilterWidgets(true);

    d->WarningPushButton->setText(tr("No filters or query/retrieve servers have been set and"
                                     " no patients have been found in the local database."
                                     "\nPlease set at least one filter to query the servers and "
                                     "check that at least one server has the Query/Retrieve property toggled."));
    d->WarningPushButton->show();
    d->patientsTabMenuToolButton->hide();
    return;
  }
  else
  {
    d->WarningPushButton->hide();
  }

  d->createPatients();

  if (filtersEmpty || (d->Scheduler->queryRetrieveServersCount() == 0))
  {
    d->updateFiltersWarnings();
  }
  else if (d->Scheduler->queryRetrieveServersCount() > 0)
  {
    QMap<QString, QVariant> parameters;
    parameters["Name"] = d->FilteringPatientName;
    parameters["ID"] = d->FilteringPatientID;
    parameters["Study"] = d->FilteringStudyDescription;
    parameters["Series"] = d->FilteringSeriesDescription;
    if (!d->FilteringModalities.contains("Any"))
    {
      parameters["Modalities"] = d->FilteringModalities;
    }

    int nDays = ctkDICOMPatientItemWidget::getNDaysFromFilteringDate(d->FilteringDate);
    if (nDays != -1)
    {
      QDate endDate = QDate::currentDate();
      QString formattedEndDate = endDate.toString("yyyyMMdd");

      QDate startDate = endDate.addDays(-nDays);
      QString formattedStartDate = startDate.toString("yyyyMMdd");

      parameters["StartDate"] = formattedStartDate;
      parameters["EndDate"] = formattedEndDate;
    }

    d->Scheduler->setFilters(parameters);
    d->Scheduler->queryPatients(QThread::NormalPriority);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onQueryRetrieveOptionToggled(bool toggled)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  QSettings settings;
  settings.setValue("DICOM/QueryRetrieveEnabled", toggled);
  d->configureSearchIcon();
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::updateGUIFromScheduler(QList<QVariant> datas)
{
  Q_D(ctkDICOMVisualBrowserWidget);

  bool updatePatients = false;
  foreach (QVariant data, datas)
  {
    ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
    if (td.JobUID.isEmpty())
    {
      d->updateFiltersWarnings();
      continue;
    }

    if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryStudies ||
        td.JobType == ctkDICOMJobResponseSet::JobType::QuerySeries ||
        td.JobType == ctkDICOMJobResponseSet::JobType::QueryInstances ||
        td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
        td.JobType == ctkDICOMJobResponseSet::JobType::StoreSOPInstance ||
        td.JobType == ctkDICOMJobResponseSet::JobType::ThumbnailGenerator ||
        td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries)
    {
      ctkDICOMPatientItemWidget* patientItemWidget = this->patientItemWidgetByPatientID(td.PatientID);
      if (patientItemWidget)
      {
        patientItemWidget->updateGUIFromScheduler(data);
      }
    }

    if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryStudies ||
      td.JobType == ctkDICOMJobResponseSet::JobType::QuerySeries)
    {
      d->updateFiltersWarnings();
      continue;
    }
    else if (td.JobType != ctkDICOMJobResponseSet::JobType::QueryPatients)
    {
      continue;
    }

    d->updateFiltersWarnings();
    if (td.NumberOfDataSets == 0)
    {
      d->WarningPushButton->setText(tr("The patients query provided no results. Please refine your filters."));
      d->WarningPushButton->show();
      d->SearchMenuButton->setIcon(QIcon(":/Icons/query_failed.svg"));
    }
    else
    {
      d->WarningPushButton->hide();
      updatePatients = true;
    }
  }

  if (updatePatients)
  {
    QSettings settings;
    bool queryRetrieveEnabled = settings.value("DICOM/QueryRetrieveEnabled", "").toBool();
    d->createPatients(queryRetrieveEnabled);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onJobStarted(QList<QVariant> datas)
{
  Q_D(ctkDICOMVisualBrowserWidget);

  foreach (QVariant data, datas)
  {
    ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
    if (td.JobUID.isEmpty())
    {
      continue;
    }

    if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryPatients)
    {
      d->updateFiltersWarnings();
      d->SearchMenuButton->setIcon(QIcon(":/Icons/wait.svg"));
    }
    else if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryStudies)
    {
      QString patientItem = d->findPatientItemFromPatientInfo(td.PatientID);
      int patientIndex = d->findPatientTabIndexFromPatientItem(patientItem);
      ctkDICOMPatientItemWidget* patientItemWidget =
        qobject_cast<ctkDICOMPatientItemWidget*>(d->PatientsTabWidget->widget(patientIndex));
      if (patientItemWidget)
      {
        patientItemWidget->setOperationStatus(ctkDICOMPatientItemWidget::InProgress);
      }
      d->PatientsTabWidget->setTabIcon(patientIndex, QIcon(":/Icons/patient_pending.svg"));
    }

    if (td.JobType == ctkDICOMJobResponseSet::JobType::QuerySeries ||
        td.JobType == ctkDICOMJobResponseSet::JobType::QueryInstances ||
        td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
        td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries)
    {
      ctkDICOMPatientItemWidget* patientItemWidget = this->patientItemWidgetByPatientID(td.PatientID);
      if (patientItemWidget)
      {
        patientItemWidget->onJobStarted(data);
      }
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onJobUserStopped(QList<QVariant> datas)
{
  Q_D(ctkDICOMVisualBrowserWidget);

  foreach (QVariant data, datas)
  {
    ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
    if (td.JobUID.isEmpty())
    {
      continue;
    }

    if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryPatients)
    {
      d->updateFiltersWarnings();
      d->SearchMenuButton->setIcon(QIcon(":/Icons/query_failed.svg"));
    }
    else if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryStudies)
    {
      QString patientItem = d->findPatientItemFromPatientInfo(td.PatientID);
      int patientIndex = d->findPatientTabIndexFromPatientItem(patientItem);
      ctkDICOMPatientItemWidget* patientItemWidget =
        qobject_cast<ctkDICOMPatientItemWidget*>(d->PatientsTabWidget->widget(patientIndex));
      if (patientItemWidget)
      {
        patientItemWidget->setOperationStatus(ctkDICOMPatientItemWidget::Failed);
      }
      d->PatientsTabWidget->setTabIcon(patientIndex, QIcon(":/Icons/patient_failed.svg"));
    }

    if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryStudies ||
        td.JobType == ctkDICOMJobResponseSet::JobType::QuerySeries ||
        td.JobType == ctkDICOMJobResponseSet::JobType::QueryInstances ||
        td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
        td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries)
    {
      ctkDICOMPatientItemWidget* patientItemWidget = this->patientItemWidgetByPatientID(td.PatientID);
      if (patientItemWidget)
      {
        patientItemWidget->onJobUserStopped(data);
      }
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onJobFailed(QList<QVariant> datas)
{
  Q_D(ctkDICOMVisualBrowserWidget);

  foreach (QVariant data, datas)
  {
    ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
    if (td.JobUID.isEmpty())
    {
      continue;
    }

    if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryPatients)
    {
      d->updateFiltersWarnings();
      d->SearchMenuButton->setIcon(QIcon(":/Icons/query_failed.svg"));
      d->WarningPushButton->setText(tr("The patients query failed. Please check the servers settings."));
      d->WarningPushButton->show();
    }
    else if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryStudies)
    {
      QString patientItem = d->findPatientItemFromPatientInfo(td.PatientID);
      int patientIndex = d->findPatientTabIndexFromPatientItem(patientItem);
      ctkDICOMPatientItemWidget* patientItemWidget =
        qobject_cast<ctkDICOMPatientItemWidget*>(d->PatientsTabWidget->widget(patientIndex));
      if (patientItemWidget)
      {
        patientItemWidget->setOperationStatus(ctkDICOMPatientItemWidget::Failed);
      }
      d->PatientsTabWidget->setTabIcon(patientIndex, QIcon(":/Icons/patient_failed.svg"));
    }

    if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryStudies ||
        td.JobType == ctkDICOMJobResponseSet::JobType::QuerySeries ||
        td.JobType == ctkDICOMJobResponseSet::JobType::QueryInstances ||
        td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
        td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries)
    {
      ctkDICOMPatientItemWidget* patientItemWidget = this->patientItemWidgetByPatientID(td.PatientID);
      if (patientItemWidget)
      {
        patientItemWidget->onJobFailed(data);
      }
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onJobFinished(QList<QVariant> datas)
{
  Q_D(ctkDICOMVisualBrowserWidget);

  foreach (QVariant data, datas)
  {
    ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
    if (td.JobUID.isEmpty())
    {
      continue;
    }

    if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryPatients)
    {
      d->updateFiltersWarnings();
      d->SearchMenuButton->setIcon(QIcon(":/Icons/query_success.svg"));
    }
    else if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryStudies)
    {
      QString patientItem = d->findPatientItemFromPatientInfo(td.PatientID);
      int patientIndex = d->findPatientTabIndexFromPatientItem(patientItem);
      ctkDICOMPatientItemWidget* patientItemWidget =
        qobject_cast<ctkDICOMPatientItemWidget*>(d->PatientsTabWidget->widget(patientIndex));
      if (patientItemWidget)
      {
        patientItemWidget->setOperationStatus(ctkDICOMPatientItemWidget::Completed);
      }
      d->PatientsTabWidget->setTabIcon(patientIndex, QIcon(":/Icons/patient_success.svg"));
    }

    if (td.JobType == ctkDICOMJobResponseSet::JobType::QuerySeries ||
        td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
        td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries)
    {
      ctkDICOMPatientItemWidget* patientItemWidget = this->patientItemWidgetByPatientID(td.PatientID);
      if (patientItemWidget)
      {
        patientItemWidget->onJobFinished(data);
      }
    }
    else if (td.JobType == ctkDICOMJobResponseSet::JobType::Inserter)
    {
      for (int patientIndex = 0; patientIndex < d->PatientsTabWidget->count(); ++patientIndex)
      {
        ctkDICOMPatientItemWidget* patientItemWidget =
          qobject_cast<ctkDICOMPatientItemWidget*>(d->PatientsTabWidget->widget(patientIndex));
        if (patientItemWidget)
        {
          patientItemWidget->onInserterJobFinished(data);
        }
      }
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onPatientItemChanged(int index)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  ctkDICOMPatientItemWidget* patientItem =
    qobject_cast<ctkDICOMPatientItemWidget*>(d->PatientsTabWidget->widget(index));
  if (!patientItem || patientItem->patientItem().isEmpty())
  {
    return;
  }

  QSettings settings;
  bool queryRetrieveEnabled = settings.value("DICOM/QueryRetrieveEnabled", "").toBool();
  patientItem->generateStudies(queryRetrieveEnabled, queryRetrieveEnabled);
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onOperationStatusTabBarItemClicked(int index)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  ctkDICOMPatientItemWidget* patientItemWidget =
    qobject_cast<ctkDICOMPatientItemWidget*>(d->PatientsTabWidget->widget(index));
  if (!patientItemWidget)
  {
    return;
  }

  ctkDICOMPatientItemWidget::OperationStatus status = patientItemWidget->operationStatus();
  if (status == ctkDICOMPatientItemWidget::InProgress)
  {
    d->Scheduler->stopJobsByDICOMUIDs(QStringList(patientItemWidget->patientID()));
  }
  else if (status > ctkDICOMPatientItemWidget::InProgress)
  {
    if (!d->Scheduler->retryJob(patientItemWidget->stoppedJobUID()))
    {
    logger.info(QString("Unable to restart Job job %1 (patient level) because the job has been fully cleared from the system. "
                        "Please initiate a new job if further processing is required.").arg(patientItemWidget->stoppedJobUID()));
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onServersSettingsChanged()
{
  Q_D(ctkDICOMVisualBrowserWidget);

  for (int patientIndex = 0; patientIndex < d->PatientsTabWidget->count(); ++patientIndex)
  {
    ctkDICOMPatientItemWidget* patientItemWidget =
        qobject_cast<ctkDICOMPatientItemWidget*>(d->PatientsTabWidget->widget(patientIndex));
    if (!patientItemWidget)
    {
      continue;
    }

    patientItemWidget->updateAllowedServersUIFromDB();
  }
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::patientSelectedOnJobList(const QString& patientID,
                                                           const QString& patientName,
                                                           const QString& patientBirthDate)
{
  Q_D(ctkDICOMVisualBrowserWidget);

  QString patientItem = d->findPatientItemFromPatientInfo(patientID, patientName, patientBirthDate);
  int selectedIndex = d->findPatientTabIndexFromPatientItem(patientItem);
  int wasBlocking = d->PatientsTabWidget->blockSignals(true);
  d->PatientsTabWidget->setCurrentIndex(selectedIndex);
  d->PatientsTabWidget->blockSignals(wasBlocking);
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::showPatientContextMenu(const QPoint& point)
{
  Q_D(ctkDICOMVisualBrowserWidget);

  ctkDICOMPatientItemWidget* patientItemWidget =
    qobject_cast<ctkDICOMPatientItemWidget*>(d->PatientsTabWidget->currentWidget());
  if (!patientItemWidget)
  {
    return;
  }

  QList<QWidget*> selectedWidgets;
  selectedWidgets.append(patientItemWidget);

  QPoint globalPos = patientItemWidget->mapToGlobal(point);
  QMenu* patientMenu = new QMenu();

  QString loadString = tr("Load patient files");
  QAction* loadAction = new QAction(loadString, patientMenu);
  patientMenu->addAction(loadAction);

  QString metadataString = tr("View patient DICOM metadata");
  QAction* metadataAction = new QAction(metadataString, patientMenu);
  patientMenu->addAction(metadataAction);

  QString deleteString = tr("Delete patient from local database");
  QAction* deleteAction = new QAction(deleteString, patientMenu);
  patientMenu->addAction(deleteAction);
  deleteAction->setVisible(this->isDeleteActionVisible());

  QString exportString = tr("Export patient to file system");
  QAction* exportAction = new QAction(exportString, patientMenu);
  patientMenu->addAction(exportAction);

  QString sendString = tr("Send patient to DICOM server");
  QAction* sendAction = new QAction(sendString, patientMenu);
  sendAction->setVisible(this->isSendActionVisible());
  patientMenu->addAction(sendAction);

  QAction* selectedAction = patientMenu->exec(globalPos);
  if (selectedAction == loadAction)
  {
    // first select all the series for all studies
    ctkDICOMPatientItemWidget* currentPatientItemWidget =
        qobject_cast<ctkDICOMPatientItemWidget*>(d->PatientsTabWidget->currentWidget());
    if (currentPatientItemWidget)
    {
      currentPatientItemWidget->setSelection(true);
    }

    this->onLoad();
  }
  else if (selectedAction == metadataAction)
  {
    this->showMetadata(this->fileListForCurrentSelection(ctkDICOMModel::PatientType, selectedWidgets));
  }
  else if (selectedAction == deleteAction)
  {
    this->removeSelectedItems(ctkDICOMModel::PatientType, selectedWidgets);
  }
  else if (selectedAction == exportAction)
  {
    this->exportSelectedItems(ctkDICOMModel::PatientType, selectedWidgets);
  }
  else if (selectedAction == sendAction)
  {
    emit sendRequested(this->fileListForCurrentSelection(ctkDICOMModel::PatientType, selectedWidgets));
  }
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::showStudyContextMenu(const QPoint& point)
{
  Q_D(ctkDICOMVisualBrowserWidget);

  ctkDICOMStudyItemWidget* studyItemWidget =
    qobject_cast<ctkDICOMStudyItemWidget*>(QObject::sender());
  if (!studyItemWidget)
  {
    return;
  }

  studyItemWidget->setSelection(true);

  ctkDICOMPatientItemWidget* currentPatientItemWidget =
    qobject_cast<ctkDICOMPatientItemWidget*>(d->PatientsTabWidget->currentWidget());
  if (!currentPatientItemWidget)
  {
    return;
  }

  QList<ctkDICOMStudyItemWidget*> studyItemWidgetsList = currentPatientItemWidget->studyItemWidgetsList();
  QList<QWidget*> selectedWidgets;
  foreach (ctkDICOMStudyItemWidget* studyItemWidget, studyItemWidgetsList)
  {
    if (!studyItemWidget || !studyItemWidget->selection())
    {
      continue;
    }

    selectedWidgets.append(studyItemWidget);
  }

  int numberOfSelectedStudies = selectedWidgets.count();

  QPoint globalPos = studyItemWidget->mapToGlobal(point);
  QMenu* studyMenu = new QMenu();

  QString loadString = numberOfSelectedStudies == 1 ? tr("Load study") :
    tr("Load %1 studies").arg(numberOfSelectedStudies);
  QAction* loadAction = new QAction(loadString, studyMenu);
  studyMenu->addAction(loadAction);

  QString metadataString = numberOfSelectedStudies == 1 ? tr("View study DICOM metadata") :
    tr("View %1 studies DICOM metadata").arg(numberOfSelectedStudies);
  QAction* metadataAction = new QAction(metadataString, studyMenu);
  studyMenu->addAction(metadataAction);

  QString forceRetrieveString = numberOfSelectedStudies == 1 ? tr("Force retrieve series") :
    tr("Force retrieve series for %1 studies").arg(numberOfSelectedStudies);
  QAction *forceRetrieveAction = new QAction(forceRetrieveString, studyMenu);
  studyMenu->addAction(forceRetrieveAction);

  QString deleteString = numberOfSelectedStudies == 1 ? tr("Delete study from local database") :
    tr("Delete %1 studies from local database").arg(numberOfSelectedStudies);
  QAction* deleteAction = new QAction(deleteString, studyMenu);
  studyMenu->addAction(deleteAction);
  deleteAction->setVisible(this->isDeleteActionVisible());

  QString exportString = numberOfSelectedStudies == 1 ? tr("Export study to file system") :
    tr("Export %1 studies to file system").arg(numberOfSelectedStudies);
  QAction* exportAction = new QAction(exportString, studyMenu);
  studyMenu->addAction(exportAction);

  QString sendString = numberOfSelectedStudies == 1 ? tr("Send study to DICOM server") :
    tr("Send %1 studies to DICOM server").arg(numberOfSelectedStudies);
  QAction* sendAction = new QAction(sendString, studyMenu);
  sendAction->setVisible(this->isSendActionVisible());
  studyMenu->addAction(sendAction);

  QAction* selectedAction = studyMenu->exec(globalPos);
  if (selectedAction == loadAction)
  {
    this->onLoad();
  }
  else if (selectedAction == metadataAction)
  {
    this->showMetadata(this->fileListForCurrentSelection(ctkDICOMModel::StudyType, selectedWidgets));
  }
  else if (selectedAction == forceRetrieveAction)
  {
    this->forceSeriesRetrieve(selectedWidgets);
  }
  else if (selectedAction == deleteAction)
  {
    this->removeSelectedItems(ctkDICOMModel::StudyType, selectedWidgets);
  }
  else if (selectedAction == exportAction)
  {
    this->exportSelectedItems(ctkDICOMModel::StudyType, selectedWidgets);
  }
  else if (selectedAction == sendAction)
  {
    emit sendRequested(this->fileListForCurrentSelection(ctkDICOMModel::StudyType, selectedWidgets));
  }
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::showSeriesContextMenu(const QPoint& point)
{
  Q_D(ctkDICOMVisualBrowserWidget);

  ctkDICOMSeriesItemWidget* selectedSeriesItemWidget =
    qobject_cast<ctkDICOMSeriesItemWidget*>(QObject::sender());
  if (!selectedSeriesItemWidget)
  {
    return;
  }

  ctkDICOMPatientItemWidget* currentPatientItemWidget =
    qobject_cast<ctkDICOMPatientItemWidget*>(d->PatientsTabWidget->currentWidget());
  if (!currentPatientItemWidget)
  {
    return;
  }

  d->updateSeriesTablesSelection(selectedSeriesItemWidget);
  QList<ctkDICOMStudyItemWidget*> studyItemWidgetsList = currentPatientItemWidget->studyItemWidgetsList();
  QList<QWidget*> selectedWidgets;
  foreach (ctkDICOMStudyItemWidget* studyItemWidget, studyItemWidgetsList)
  {
    if (!studyItemWidget)
    {
      continue;
    }
    QTableWidget* seriesListTableWidget = studyItemWidget->seriesListTableWidget();
    QList<QTableWidgetItem*> selectedItems = seriesListTableWidget->selectedItems();
    foreach (QTableWidgetItem* selectedItem, selectedItems)
    {
      if (!selectedItem)
      {
        continue;
      }

      int row = selectedItem->row();
      int column = selectedItem->column();
      ctkDICOMSeriesItemWidget* seriesItemWidget =
        qobject_cast<ctkDICOMSeriesItemWidget*>(seriesListTableWidget->cellWidget(row, column));

      selectedWidgets.append(seriesItemWidget);
    }
  }

  int numberOfSelectedSeries = selectedWidgets.count();

  QPoint globalPos = selectedSeriesItemWidget->mapToGlobal(point);
  QMenu* seriesMenu = new QMenu();

  QString loadString = numberOfSelectedSeries == 1 ? tr("Load series") :
    tr("Load %1 series").arg(numberOfSelectedSeries);
  QAction *loadAction = new QAction(loadString, seriesMenu);
  seriesMenu->addAction(loadAction);

  QString metadataString = numberOfSelectedSeries == 1 ? tr("View series DICOM metadata") :
    tr("View %1 series DICOM metadata").arg(numberOfSelectedSeries);
  QAction *metadataAction = new QAction(metadataString, seriesMenu);
  seriesMenu->addAction(metadataAction);

  QString forceRetrieveString = numberOfSelectedSeries == 1 ? tr("Force retrieve series") :
    tr("Force retrieve for %1 series").arg(numberOfSelectedSeries);
  QAction *forceRetrieveAction = new QAction(forceRetrieveString, seriesMenu);
  seriesMenu->addAction(forceRetrieveAction);

  QString deleteString = numberOfSelectedSeries == 1 ? tr("Delete series from local database") :
    tr("Delete %1 series from local database").arg(numberOfSelectedSeries);
  QAction *deleteAction = new QAction(deleteString, seriesMenu);
  seriesMenu->addAction(deleteAction);
  deleteAction->setVisible(this->isDeleteActionVisible());

  QString exportString = numberOfSelectedSeries == 1 ? tr("Export series to file system") :
    tr("Export %1 series to file system").arg(numberOfSelectedSeries);
  QAction *exportAction = new QAction(exportString, seriesMenu);
  seriesMenu->addAction(exportAction);

  QString sendString = numberOfSelectedSeries == 1 ? tr("Send series to DICOM server") :
    tr("Send %1 series to DICOM server").arg(numberOfSelectedSeries);
  QAction* sendAction = new QAction(sendString, seriesMenu);
  sendAction->setVisible(this->isSendActionVisible());
  seriesMenu->addAction(sendAction);

  QAction* selectedAction = seriesMenu->exec(globalPos);
  if (selectedAction == loadAction)
  {
    this->onLoad();
  }
  else if (selectedAction == metadataAction)
  {
    this->showMetadata(this->fileListForCurrentSelection(ctkDICOMModel::SeriesType, selectedWidgets));
  }
  else if (selectedAction == forceRetrieveAction)
  {
    this->forceSeriesRetrieve(selectedWidgets);
  }
  else if (selectedAction == deleteAction)
  {
    this->removeSelectedItems(ctkDICOMModel::SeriesType, selectedWidgets);
  }
  else if (selectedAction == exportAction)
  {
    this->exportSelectedItems(ctkDICOMModel::SeriesType, selectedWidgets);
  }
  else if (selectedAction == sendAction)
  {
    emit sendRequested(this->fileListForCurrentSelection(ctkDICOMModel::SeriesType, selectedWidgets));
  }
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onPatientsTabMenuToolButtonClicked()
{
  Q_D(ctkDICOMVisualBrowserWidget);

  QPoint globalPos = this->mapToGlobal(d->patientsTabMenuToolButton->geometry().bottomLeft());
  globalPos.setY(globalPos.y() + d->patientsTabMenuToolButton->height() * 3);
  globalPos.setX(globalPos.x());

  QMenu* patientMenu = new QMenu();
  patientMenu->move(globalPos);
  for (int patientIndex = 0; patientIndex < d->PatientsTabWidget->count(); ++patientIndex)
  {
    ctkDICOMPatientItemWidget* patientItemWidget =
        qobject_cast<ctkDICOMPatientItemWidget*>(d->PatientsTabWidget->widget(patientIndex));

#if QT_VERSION <= QT_VERSION_CHECK(5, 13, 0)
    if (!patientItemWidget || d->PatientsTabWidget->tabText(patientIndex).isEmpty())
#else
    if (!patientItemWidget || !d->PatientsTabWidget->isTabVisible(patientIndex))
#endif
    {
      continue;
    }

    QString patientName = patientItemWidget->patientName();
    QAction* changePatientAction = new QAction(patientName, patientMenu);
    if (patientItemWidget == d->PatientsTabWidget->currentWidget())
    {
      changePatientAction->setIcon(QIcon(":Icons/patient.svg"));
      QFont font(changePatientAction->font());
      font.setBold(true);
      changePatientAction->setFont(font);
    }
    patientMenu->addAction(changePatientAction);
  }

  patientMenu->addSeparator();
  QString deleteString = tr("Delete all Patients from local database");
  QAction* deleteAction = new QAction(deleteString, patientMenu);
  deleteAction->setIcon(QIcon(":Icons/delete.svg"));
  patientMenu->addAction(deleteAction);
  deleteAction->setVisible(this->isDeleteActionVisible());

  QAction* selectedAction = patientMenu->exec(globalPos);
  if (selectedAction == deleteAction)
  {
    this->removeSelectedItems(ctkDICOMModel::RootType);
    d->patientsTabMenuToolButton->hide();
  }
  else if (selectedAction)
  {
    QString patientName = selectedAction->text();
    ctkDICOMPatientItemWidget* patientItemWidget = this->patientItemWidgetByPatientName(patientName);
    if (patientItemWidget)
    {
      d->PatientsTabWidget->setCurrentWidget(patientItemWidget);
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onPatientUpdateGUIFinished()
{
  Q_D(ctkDICOMVisualBrowserWidget);

  ctkDICOMPatientItemWidget* patientItemWidget =
    qobject_cast<ctkDICOMPatientItemWidget*>(QObject::sender());
  if (!patientItemWidget)
  {
    return;
  }

  d->updateTabVisibilityForPatientItemWidget(patientItemWidget);
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::exportSelectedItems(ctkDICOMModel::IndexType level,
                                                      const QList<QWidget*>& selectedWidgets)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  if (!d->DicomDatabase)
  {
    logger.error("exportSelectedItems failed, no DICOM database has been set. \n");
    return;
  }

  ctkFileDialog* directoryDialog = new ctkFileDialog();
  directoryDialog->setOption(QFileDialog::ShowDirsOnly);
  directoryDialog->setFileMode(QFileDialog::Directory);
  directoryDialog->setOption(QFileDialog::ShowDirsOnly);
  bool res = directoryDialog->exec();
  if (!res)
  {
    delete directoryDialog;
    return;
  }
  QStringList dirs = directoryDialog->selectedFiles();
  delete directoryDialog;
  QString dirPath = dirs[0];

  QStringList selectedSeriesUIDs = d->getSeriesUIDsFromWidgets(level, selectedWidgets);

  this->exportSeries(dirPath, selectedSeriesUIDs);
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::exportSeries(const QString& dirPath, const QStringList& uids)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  if (!d->DicomDatabase)
  {
    logger.error("exportSeries failed, no DICOM database has been set. \n");
    return;
  }

  foreach (const QString& uid, uids)
  {
    QStringList filesForSeries = d->DicomDatabase->filesForSeries(uid);

    // Use the first file to get the overall series information
    QString firstFilePath = filesForSeries[0];
    QHash<QString, QString> descriptions(d->DicomDatabase->descriptionsForFile(firstFilePath));
    QString patientName = descriptions["PatientsName"];
    QString patientIDTag = QString("0010,0020");
    QString patientID = d->DicomDatabase->fileValue(firstFilePath, patientIDTag);
    QString studyDescription = descriptions["StudyDescription"];
    QString seriesDescription = descriptions["SeriesDescription"];
    QString studyDateTag = QString("0008,0020");
    QString studyDate = d->DicomDatabase->fileValue(firstFilePath, studyDateTag);
    QString seriesNumberTag = QString("0020,0011");
    QString seriesNumber = d->DicomDatabase->fileValue(firstFilePath, seriesNumberTag);

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
        //: %1 is the destination directory
        QString errorString = tr("Unable to create export destination directory:\n\n%1"
            "\n\nHalting export.")
            .arg(destinationDir);
        ctkMessageBox createDirectoryErrorMessageBox(this);
        createDirectoryErrorMessageBox.setText(errorString);
        createDirectoryErrorMessageBox.setIcon(QMessageBox::Warning);
        createDirectoryErrorMessageBox.exec();
        return;
      }
    }

    // show progress
    if (d->ExportProgress == 0)
    {
      d->ExportProgress = new QProgressDialog(tr("DICOM Export"), tr("Close"), 0, 100, this, Qt::WindowTitleHint | Qt::WindowSystemMenuHint);
      d->ExportProgress->setWindowModality(Qt::ApplicationModal);
      d->ExportProgress->setMinimumDuration(0);
    }
    QLabel* exportLabel = new QLabel(
      //: %1 is the series number
      tr("Exporting series %1").arg(seriesNumber)
    );
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
        //: %1 is the file path
        QString errorString = tr("Export source file not found:\n\n%1"
          "\n\nHalting export.\n\nError may be fixed via Repair.")
          .arg(filePath);
        ctkMessageBox copyErrorMessageBox;
        copyErrorMessageBox.setText(errorString);
        copyErrorMessageBox.setIcon(QMessageBox::Warning);
        copyErrorMessageBox.exec();
        return;
      }
      if (QFile::exists(destinationFileName))
      {
        d->ExportProgress->setValue(numFiles);
        //: %1 is the destination file name
        QString errorString = tr("Export destination file already exists:\n\n%1"
          "\n\nHalting export.")
          .arg(destinationFileName);
        ctkMessageBox copyErrorMessageBox(this);
        copyErrorMessageBox.setText(errorString);
        copyErrorMessageBox.setIcon(QMessageBox::Warning);
        copyErrorMessageBox.exec();
        return;
      }

      bool copyResult = QFile::copy(filePath, destinationFileName);
      if (!copyResult)
      {
        d->ExportProgress->setValue(numFiles);
        //: %1 and %2 refers to source and destination file paths
        QString errorString = tr("Failed to copy\n\n%1\n\nto\n\n%2"
          "\n\nHalting export.")
          .arg(filePath)
          .arg(destinationFileName);
        ctkMessageBox copyErrorMessageBox(this);
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

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onImportDirectoriesSelected(const QStringList& directories)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->ImportDialog->hide();
  this->importDirectories(directories, this->importDirectoryMode());
  d->updateFiltersWarnings();

  // Clear selection
  d->ImportDialog->clearSelection();
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onImportDirectoryComboBoxCurrentIndexChanged(int index)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  Q_UNUSED(index);
  if (!(d->ImportDialog->options() & QFileDialog::DontUseNativeDialog))
  {
    return; // Native dialog does not support modifying or getting widget elements.
  }
  QComboBox* comboBox = d->ImportDialog->bottomWidget()->findChild<QComboBox*>();
  ctkDICOMVisualBrowserWidget::ImportDirectoryMode mode =
      static_cast<ctkDICOMVisualBrowserWidget::ImportDirectoryMode>(comboBox->itemData(index).toInt());
  this->setImportDirectoryMode(mode);
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onClose()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  if (d->IsGUIUpdating)
  {
    return;
  }

  this->onStop(true, true);
  this->close();
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onLoad()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  if (d->IsGUIUpdating)
  {
    return;
  }

  d->retrieveSeries();
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onStop(bool stopPersistentTasks, bool removeJobs)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  if (!d->Scheduler)
  {
    return;
  }

  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
  d->Scheduler->stopAllJobs(stopPersistentTasks, removeJobs);
  d->updateFiltersWarnings();
  d->ProgressFrame->hide();
  QApplication::restoreOverrideCursor();
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::closeEvent(QCloseEvent* event)
{
  this->onStop(true, true);
  event->accept();
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::keyPressEvent(QKeyEvent *event)
{
  Q_D(ctkDICOMVisualBrowserWidget);

  if (event->key() == Qt::Key_Tab || event->key() == Qt::Key_Backtab)
  {
    int currentIndex = d->PatientsTabWidget->currentIndex();
    if (event->modifiers() & Qt::ControlModifier &&
      !(event->modifiers() & Qt::ShiftModifier))
    {
    d->PatientsTabWidget->setCurrentIndex(currentIndex + 1);
    }
    else if ((event->modifiers() & Qt::ControlModifier) &&
      (event->modifiers() & Qt::ShiftModifier))
    {
    d->PatientsTabWidget->setCurrentIndex(currentIndex - 1);
    }
    else
    {
      QWidget::keyPressEvent(event);
    }
  }
  else
  {
    QWidget::keyPressEvent(event);
  }
}

//------------------------------------------------------------------------------
bool ctkDICOMVisualBrowserWidget::confirmDeleteSelectedUIDs(const QStringList& uids)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  if (!d->DicomDatabase)
  {
    logger.error("confirmDeleteSelectedUIDs failed, no DICOM database has been set. \n");
    return false;
  }

  if (uids.isEmpty())
  {
    return false;
  }

  ctkMessageBox confirmDeleteDialog(this);
  QString message = tr("Do you want to delete the following selected items from the LOCAL database? \n"
                       "The data will not be deleted from the PACs server. \n");

  // add the information about the selected UIDs
  int numUIDs = uids.size();
  for (int i = 0; i < numUIDs; ++i)
  {
    QString uid = uids.at(i);

    // try using the given UID to find a descriptive string
    QString patientName = d->DicomDatabase->nameForPatient(uid);
    QString studyDescription = d->DicomDatabase->descriptionForStudy(uid);
    QString seriesDescription = d->DicomDatabase->descriptionForSeries(uid);

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

  confirmDeleteDialog.addButton(tr("Delete"), QMessageBox::AcceptRole);
  confirmDeleteDialog.addButton(tr("Cancel"), QMessageBox::RejectRole);
  confirmDeleteDialog.setDontShowAgainSettingsKey("VisualDICOMBrowser/DontConfirmDeleteSelected");

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
