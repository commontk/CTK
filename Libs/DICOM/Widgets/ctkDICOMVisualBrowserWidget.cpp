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
#include <QCalendarWidget>
#include <QCloseEvent>
#include <QDebug>
#include <QDate>
#include <QDateEdit>
#include <QEvent>
#include <QFormLayout>
#include <QKeyEvent>
#include <QMap>
#include <QMenu>
#include <QProgressBar>
#include <QProgressDialog>
#include <QScrollArea>
#include <QScrollBar>
#include <QTableWidget>
#include <QTimer>
#include <QToolButton>

// CTK includes
#include <ctkBasePopupWidget.h>
#include <ctkCheckBox.h>
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
#include "ctkDICOMModalities.h"
#include "ctkDICOMScheduler.h"
#include "ctkDICOMScheduler.h"
#include "ctkDICOMServer.h"
#include "ctkDICOMThumbnailGenerator.h"
#include "ctkUtils.h"

// ctkDICOMWidgets includes
#include "ctkDICOMMetadataDialog.h"
#include "ctkDICOMPatientDelegate.h"
#include "ctkDICOMPatientModel.h"
#include "ctkDICOMPatientFilterProxyModel.h"
#include "ctkDICOMPatientView.h"
#include "ctkDICOMVisualBrowserWidget.h"
#include "ctkDICOMSeriesModel.h"
#include "ctkDICOMSeriesTableView.h"
#include "ctkDICOMServerNodeWidget2.h"
#include "ctkDICOMStudyModel.h"
#include "ctkDICOMStudyMergedFilterProxyModel.h"
#include "ctkDICOMStudyListView.h"
#include "ctkDICOMVisualBrowserWidget.h"
#include "ui_ctkDICOMVisualBrowserWidget.h"

static ctkLogger logger("org.commontk.DICOM.Widgets.DICOMVisualBrowserWidget");

QColor ctkDICOMVisualBrowserWidgetDefaultColor(Qt::white);
QColor ctkDICOMVisualBrowserWidgetDarkModeDefaultColor(50, 50, 50);
QColor ctkDICOMVisualBrowserWidgetWarningColor(Qt::darkYellow);
int ctkDICOMVisualBrowserWidgetThumbnailSizePixelsNone = 0;
int ctkDICOMVisualBrowserWidgetThumbnailSizePixelsSmall = 128;
int ctkDICOMVisualBrowserWidgetThumbnailSizePixelsMedium = 192;
int ctkDICOMVisualBrowserWidgetThumbnailSizePixelsLarge = 256;

//----------------------------------------------------------------------------
class ctkDICOMVisualBrowserWidgetPrivate : public Ui_ctkDICOMVisualBrowserWidget
{
  Q_DECLARE_PUBLIC(ctkDICOMVisualBrowserWidget);

protected:
  ctkDICOMVisualBrowserWidget* const q_ptr;

public:
  ctkDICOMVisualBrowserWidgetPrivate(ctkDICOMVisualBrowserWidget& obj);
  ~ctkDICOMVisualBrowserWidgetPrivate();

  void init();
  void disconnectScheduler();
  void connectScheduler();
  void importDirectory(QString directory, ctkDICOMVisualBrowserWidget::ImportDirectoryMode mode);
  void importFiles(const QStringList& files, ctkDICOMVisualBrowserWidget::ImportDirectoryMode mode);
  void importOldSettings();
  void showUpdateSchemaDialog();
  void updateModalityCheckableComboBox();
  void createPatients(bool queryRetrieve = false,
                      const QStringList& queriedPatientIDs = QStringList(),
                      bool isImport = false);
  bool areFiltersEmpty();
  void resetFilters();
  void updateUIAfterFilters();
  void updateFiltersWarnings();
  void setBackgroundColorToFilterWidgets(bool warning = false);
  void setBackgroundColorToWidget(QColor color, QWidget* widget);
  void updateFiltersLayoutOrientation();
  void retrieveSeries();
  bool updateServer(ctkDICOMServer* server);
  QString findPatientUIDByPatientID(const QString& patientID);
  QStringList filterPatientList(const QStringList& patientList,
                                const QMap<QString, QVariant>& filters);
  QStringList filterStudyList(const QStringList& studyList,
                              const QMap<QString, QVariant>& filters);
  QStringList filterSeriesList(const QStringList& seriesList,
                               const QMap<QString, QVariant>& filters);
  int computeThumbnailSizeInPixels(ctkDICOMVisualBrowserWidget::ThumbnailSizePresetOption sizeOption);
  void updateThumbnailSizeForCurrentStudyView(ctkDICOMVisualBrowserWidget::ThumbnailSizePresetOption sizeOption);

  // Utility function to convert DateType enum to number of days
  static int getNDaysFromFilteringDate(ctkDICOMVisualBrowserWidget::DateType dateType);

  // Return a sanitized version of the string that is safe to be used
  // as a filename component.
  // All non-ASCII characters are replaced, because they may be used on an internal hard disk,
  // but it may not be possible to use them on file systems of an external drive or network storage.
  QString filenameSafeString(const QString& str);

  // Utility function to add a widget to a grid layout with specified row/column/span
  void setGridWidget(QGridLayout* layout, QWidget* widget, int row, int col, int rowSpan = 1, int colSpan = 1);

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
  QSharedPointer<ctkDICOMPatientModel> PatientModel;
  QSharedPointer<ctkDICOMPatientFilterProxyModel> PatientFilterProxyModel;

  QString FilteringPatientID;
  QString FilteringPatientName;

  QString FilteringStudyDescription;
  ctkDICOMVisualBrowserWidget::DateType FilteringDate;
  QDate FilteringStartDate;
  QDate FilteringEndDate;
  QWidget *CustomDateRangeWidget;
  QDateEdit* FilteringStartDateEdit;
  QDateEdit* FilteringEndDateEdit;

  QString FilteringSeriesDescription;
  QStringList PreviousFilteringModalities;
  QStringList FilteringModalities;

  int NumberOfOpenedStudiesPerPatient;
  ctkDICOMVisualBrowserWidget::ThumbnailSizePresetOption ThumbnailSizePreset;
  bool SendActionVisible;
  bool DeleteActionVisible;
  bool IsGUIUpdating;
  bool IsGUIHorizontal;
  bool IsLoading;
  QString SelectedPatientUID;

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

  this->PatientModel = QSharedPointer<ctkDICOMPatientModel>(new ctkDICOMPatientModel);
  this->PatientFilterProxyModel = QSharedPointer<ctkDICOMPatientFilterProxyModel>(new ctkDICOMPatientFilterProxyModel);

  this->MetadataDialog = new ctkDICOMMetadataDialog();
  this->MetadataDialog->setObjectName("DICOMMetadata");
  this->MetadataDialog->setWindowTitle(ctkDICOMVisualBrowserWidget::tr("DICOM File Metadata"));

  this->NumberOfOpenedStudiesPerPatient = 2;
  this->ThumbnailSizePreset = ctkDICOMVisualBrowserWidget::Small;
  this->SendActionVisible = false;
  this->DeleteActionVisible = true;

  this->FilteringDate = ctkDICOMVisualBrowserWidget::Any;
  this->CustomDateRangeWidget = nullptr;
  this->FilteringStartDateEdit = nullptr;
  this->FilteringEndDateEdit = nullptr;
  this->FilteringModalities = ctkDICOMModalities::AllModalities;

  this->PatientsAddedDuringImport = 0;
  this->StudiesAddedDuringImport = 0;
  this->SeriesAddedDuringImport = 0;
  this->InstancesAddedDuringImport = 0;
  this->ImportDialog = nullptr;

  this->IsGUIUpdating = false;
  this->IsGUIHorizontal = true;
  this->IsLoading = false;

  this->ExportProgress = nullptr;
  this->UpdateSchemaProgress = nullptr;
}

//----------------------------------------------------------------------------
ctkDICOMVisualBrowserWidgetPrivate::~ctkDICOMVisualBrowserWidgetPrivate()
{
  if (this->ImportDialog)
  {
    this->ImportDialog->deleteLater();
  }
  if (this->MetadataDialog)
  {
    this->MetadataDialog->deleteLater();
  }
  if (this->CustomDateRangeWidget)
  {
    this->CustomDateRangeWidget->deleteLater();
  }
  if (this->FilteringStartDateEdit)
  {
    this->FilteringStartDateEdit->deleteLater();
  }
  if (this->FilteringEndDateEdit)
  {
    this->FilteringEndDateEdit->deleteLater();
  }
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

  // Install event filters on search boxes to prevent Enter key from collapsing group boxes in Qt6
  // We filter on the parent widget to catch events after they've been processed by search boxes
  this->SearchPatientsCollapsibleGroupBox->installEventFilter(q);

  // To Do: add an option to disable styling
  this->FilteringModalityCheckableComboBox->setStyleSheet("combobox-popup: 0;");

  QStringList allModalities = ctkDICOMModalities::AllModalities;
  QStringList commonModalities = ctkDICOMModalities::CommonImagingModalities;

  QSet<QString> otherModalitiesSet = QSet<QString>(allModalities.begin(), allModalities.end());
  otherModalitiesSet.subtract(QSet<QString>(commonModalities.begin(), commonModalities.end()));
  otherModalitiesSet.remove("Any");
  QStringList otherModalities = otherModalitiesSet.values();
  otherModalities.sort();

  this->FilteringModalityCheckableComboBox->setMaxVisibleItems(20);
  this->FilteringModalityCheckableComboBox->addItem("Any");
  this->FilteringModalityCheckableComboBox->insertSeparator(1);
  this->FilteringModalityCheckableComboBox->addItems(commonModalities);
  this->FilteringModalityCheckableComboBox->insertSeparator(commonModalities.length() + 2);
  this->FilteringModalityCheckableComboBox->addItems(otherModalities);

  this->updateModalityCheckableComboBox();
  QObject::connect(this->FilteringModalityCheckableComboBox, SIGNAL(checkedIndexesChanged()),
                    q, SLOT(onFilteringModalityCheckableComboBoxChanged()));

  QListView* modalityListView = qobject_cast<QListView*>(this->FilteringModalityCheckableComboBox->view());
  if (modalityListView)
  {
    modalityListView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  }

  QObject::connect(this->FilteringModalityCheckableComboBox, SIGNAL(returnRelease()),
                   q, SLOT(onQueryPatients()));

  QObject::connect(this->FilteringDateComboBox, SIGNAL(currentIndexChanged(int)),
                   q, SLOT(onFilteringDateComboBoxChanged(int)));

  QObject::connect(this->FilteringDateComboBox, SIGNAL(returnRelease()),
                   q, SLOT(onQueryPatients()));

  // Create popup widget for custom date range
  ctkPopupWidget* dateRangePopup = new ctkPopupWidget(this->FilteringDateComboBox);
  dateRangePopup->setAutoShow(false);
  dateRangePopup->setAutoHide(false);
  dateRangePopup->setAnimationEffect(ctkBasePopupWidget::FadeEffect);
  dateRangePopup->setOrientation(Qt::Vertical);
  dateRangePopup->setHorizontalDirection(Qt::LeftToRight);
  dateRangePopup->setVerticalDirection(ctkBasePopupWidget::TopToBottom);

  // Add the CustomDateRangeWidget to the popup using a layout
  QVBoxLayout* popupLayout = new QVBoxLayout(dateRangePopup);
  popupLayout->setContentsMargins(0, 0, 0, 0);
  this->CustomDateRangeWidget = new QWidget(q);
  this->CustomDateRangeWidget->setParent(dateRangePopup);
  this->CustomDateRangeWidget->setVisible(true);
  popupLayout->addWidget(this->CustomDateRangeWidget);

  // Store popup in a member variable for later access
  q->setProperty("dateRangePopup", QVariant::fromValue(static_cast<QWidget*>(dateRangePopup)));

  // Add a layout to CustomDateRangeWidget and insert labels, date edits, and popup calendars
  QHBoxLayout* customDateLayout = new QHBoxLayout(this->CustomDateRangeWidget);
  customDateLayout->setContentsMargins(0, 0, 0, 0);

  QLabel* fromLabel = new QLabel(ctkDICOMVisualBrowserWidget::tr("From:"), this->CustomDateRangeWidget);
  this->FilteringStartDateEdit = new QDateEdit(this->CustomDateRangeWidget);
  this->FilteringStartDateEdit->setCalendarPopup(true);
  this->FilteringStartDateEdit->setMinimumDate(QDate());
  this->FilteringStartDateEdit->setDate(QDate()); // Set to null date (empty)
  this->FilteringStartDateEdit->setSpecialValueText(" "); // Show empty when null
  this->FilteringStartDateEdit->setDisplayFormat("dd MMM yyyy");

  QLabel* toLabel = new QLabel(ctkDICOMVisualBrowserWidget::tr("To:"), this->CustomDateRangeWidget);
  this->FilteringEndDateEdit = new QDateEdit(this->CustomDateRangeWidget);
  this->FilteringEndDateEdit->setCalendarPopup(true);
  this->FilteringEndDateEdit->setMinimumDate(QDate());
  this->FilteringEndDateEdit->setDate(QDate());   // Set to null date (empty)
  this->FilteringEndDateEdit->setSpecialValueText(" ");   // Show empty when null
  this->FilteringEndDateEdit->setDisplayFormat("dd MMM yyyy");

  customDateLayout->addWidget(fromLabel);
  customDateLayout->addWidget(this->FilteringStartDateEdit);
  customDateLayout->addSpacing(10);
  customDateLayout->addWidget(toLabel);
  customDateLayout->addWidget(this->FilteringEndDateEdit);

  QObject::connect(this->FilteringStartDateEdit, SIGNAL(dateChanged(QDate)),
                   q, SLOT(onFilteringStartDateChanged(QDate)));
  QObject::connect(this->FilteringEndDateEdit, SIGNAL(dateChanged(QDate)),
                   q, SLOT(onFilteringEndDateChanged(QDate)));

  QObject::connect(this->SearchPushButton, SIGNAL(clicked()),
                   q, SLOT(onQueryPatients()));

  // Setup Patient View with Model/View/Delegate architecture
  this->PatientModel->setDicomDatabase(this->DicomDatabase);
  this->PatientModel->setScheduler(this->Scheduler);
  this->PatientModel->setNumberOfOpenedStudiesPerPatient(this->NumberOfOpenedStudiesPerPatient);

  // Create and set the delegate for patient rendering
  ctkDICOMPatientDelegate* patientDelegate = new ctkDICOMPatientDelegate(q);
  this->PatientView->setItemDelegate(patientDelegate);

  this->PatientView->setDisplayMode(ctkDICOMPatientView::TabMode);

  // Create filter proxy model for patients
  this->PatientFilterProxyModel->setSourceModel(this->PatientModel.data());

  // Set display mode on proxy to match view's display mode
  this->PatientFilterProxyModel->setDisplayMode(
    this->PatientView->displayMode() == ctkDICOMPatientView::TabMode
      ? ctkDICOMPatientFilterProxyModel::TabMode
      : ctkDICOMPatientFilterProxyModel::ListMode
  );

  // Set the proxy model on the view
  this->PatientView->setModel(this->PatientFilterProxyModel.data());

  // Connect display mode changes to update proxy model
  QObject::connect(this->PatientView, &ctkDICOMPatientView::displayModeChanged,
                   q, &ctkDICOMVisualBrowserWidget::onPatientViewDisplayModeChanged);

  // Connect patient context menu (3-dot button or right-click)
  QObject::connect(this->PatientView, &ctkDICOMPatientView::patientContextMenuRequested,
                   q, &ctkDICOMVisualBrowserWidget::showPatientContextMenu);

  // Connect study context menu - when study header is clicked, all series of that study are selected
  QObject::connect(this->PatientView->studyListView(), &ctkDICOMStudyListView::studyContextMenuRequested,
                   q, &ctkDICOMVisualBrowserWidget::showStudyContextMenu);

  // Connect series context menu - need to forward through StudyListView
  QObject::connect(this->PatientView->studyListView(), &ctkDICOMStudyListView::seriesContextMenuRequested,
                   q, &ctkDICOMVisualBrowserWidget::showSeriesContextMenu);

  // Connect series activation (double-click) signal
  QObject::connect(this->PatientView->studyListView(), &ctkDICOMStudyListView::seriesActivated,
                   q, &ctkDICOMVisualBrowserWidget::onSeriesDoubleClicked);

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

  bool shouldCheckAllCommon = this->PreviousFilteringModalities.contains("Any") &&
                              !this->FilteringModalities.contains("Any");
  bool shouldUncheckAll = this->PreviousFilteringModalities == ctkDICOMModalities::CommonImagingModalities;
  bool shouldCheckAll = !this->PreviousFilteringModalities.contains("Any") &&
                         this->FilteringModalities.contains("Any");
  if (shouldUncheckAll)
  {
    this->FilteringModalities.clear();
  }
  else if (shouldCheckAll)
  {
    this->FilteringModalities.clear();
    this->FilteringModalities.append("Any");
    for (int index = 0; index < this->FilteringModalityCheckableComboBox->count(); ++index)
    {
      QModelIndex modelIndex = model->index(index, 0);
      this->FilteringModalityCheckableComboBox->setCheckState(modelIndex, Qt::CheckState::Checked);
      if (index > 0)
      {
        QVariant value = model->data(modelIndex);
        this->FilteringModalities.append(value.toString());
      }
    }
    this->FilteringModalityCheckableComboBox->blockSignals(wasBlocking);
    return;
  }
  else if (shouldCheckAllCommon)
  {
    this->FilteringModalities = ctkDICOMModalities::CommonImagingModalities;
  }

  // First, uncheck all items
  for (int index = 0; index < this->FilteringModalityCheckableComboBox->count(); ++index)
  {
    QModelIndex modelIndex = model->index(index, 0);
    this->FilteringModalityCheckableComboBox->setCheckState(modelIndex, Qt::CheckState::Unchecked);
  }

  // Then check only the modalities in FilteringModalities list
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

  // Update the "Any" checkbox state
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
    if (!this->FilteringModalities.contains("Any"))
    {
      this->FilteringModalities.append("Any");
    }
  }

  this->FilteringModalityCheckableComboBox->blockSignals(wasBlocking);
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidgetPrivate::createPatients(bool queryRetrieve,
                                                        const QStringList& queriedPatientIDs,
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

  if (!this->PatientModel)
  {
    logger.error("createPatients failed, no PatientModel has been set. \n");
    return;
  }

  QStringList patientList = this->DicomDatabase->patients();
  if (patientList.count() == 0)
  {
    // Clear the model by refreshing with empty database
    this->PatientModel->refresh();
    return;
  }
  else if (patientList.count() > 5)
  {
    this->PatientView->setDisplayMode(ctkDICOMPatientView::ListMode);
  }
  else
  {
    this->PatientView->setDisplayMode(ctkDICOMPatientView::TabMode);
  }

  this->IsGUIUpdating = true;

  // Set all filters on the model - it will handle filtering internally
  this->PatientModel->setPatientIDFilter(this->FilteringPatientID);
  this->PatientModel->setPatientNameFilter(this->FilteringPatientName);
  this->PatientModel->setStudyDescriptionFilter(this->FilteringStudyDescription);
  this->PatientModel->setSeriesDescriptionFilter(this->FilteringSeriesDescription);
  this->PatientModel->setDateFilter(static_cast<ctkDICOMPatientModel::DateType>(this->FilteringDate));
  this->PatientModel->setModalityFilter(this->FilteringModalities);
  this->PatientModel->setThumbnailSize(this->computeThumbnailSizeInPixels(this->ThumbnailSizePreset));

  // Refresh the model to populate with filtered patients
  this->PatientModel->refresh();

  QString patientUIDToShow;
  // If this is an import and we have new patients, select the most recent one
  if (isImport && patientList.count() > 0)
  {
    QMap<QString, QDateTime> patientsInsertDateTimeList;
    foreach (QString patientUID, patientList)
    {
      QString patientID = this->DicomDatabase->fieldForPatient("PatientID", patientUID);
      QString patientName = this->DicomDatabase->fieldForPatient("PatientsName", patientUID);

      // Apply the same filters as the model
      if ((!this->FilteringPatientID.isEmpty() && !patientID.contains(this->FilteringPatientID, Qt::CaseInsensitive)) ||
          (!this->FilteringPatientName.isEmpty() && !patientName.contains(this->FilteringPatientName, Qt::CaseInsensitive)))
      {
        continue;
      }

      QDateTime patientInsertDateTime = this->DicomDatabase->insertDateTimeForPatient(patientUID);
      patientsInsertDateTimeList[patientUID] = patientInsertDateTime;
    }

    if (patientsInsertDateTimeList.count() > 0)
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

      patientUIDToShow = list.last().first;
    }
  }
  else if (!this->SelectedPatientUID.isEmpty())
  {
    // Restore previous selection if it still exists
    patientUIDToShow = this->SelectedPatientUID;
  }
  else
  {
    // Select the first patient by default
    patientUIDToShow = patientList.first();
  }

  this->PatientView->selectPatientUID(patientUIDToShow);

  q->onServersSettingsChanged();

  if (queryRetrieve && this->Scheduler &&
      this->Scheduler->queryRetrieveServersCount() > 0)
  {
    bool studiesQueried = false;
    if (queriedPatientIDs.empty())
    {
      QModelIndex modelIndex = this->PatientModel->indexFromPatientUID(patientUIDToShow);
      QString patientID = this->PatientModel->patientID(modelIndex);
      studiesQueried = this->PatientModel->queryStudies(patientID);
    }
    else
    {
      foreach (QString patientID, queriedPatientIDs)
      {
        if (this->PatientModel->queryStudies(patientID))
        {
          studiesQueried = true;
        }
      }
    }
    if (!studiesQueried)
    {
      this->SearchPushButton->setIcon(QIcon(":/Icons/query_failed.svg"));
      QString warningText = ctkDICOMVisualBrowserWidget::tr(
        "No server is configured for query/retrieve operations. "
        "The server settings section is below.\n"
        "Check also the allowed servers at patient level."
      );
      logger.warn(warningText);
      this->WarningPushButton->setText(warningText);
      this->WarningPushButton->show();
      q->openServerSettingsSection();
    }
  }
  this->IsGUIUpdating = false;
}

//----------------------------------------------------------------------------
bool ctkDICOMVisualBrowserWidgetPrivate::areFiltersEmpty()
{
  return this->FilteringPatientID.isEmpty() &&
         this->FilteringPatientName.isEmpty() &&
         this->FilteringStudyDescription.isEmpty() &&
         this->FilteringSeriesDescription.isEmpty() &&
         this->FilteringDate == ctkDICOMVisualBrowserWidget::Any &&
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
  this->FilteringDate = ctkDICOMVisualBrowserWidget::Any;
  this->FilteringDateComboBox->setCurrentIndex(static_cast<int>(this->FilteringDate));
  this->PatientModel->setDateFilter(static_cast<ctkDICOMPatientModel::DateType>(this->FilteringDate));
  this->PreviousFilteringModalities = ctkDICOMModalities::AllModalities;
  this->FilteringModalities = ctkDICOMModalities::AllModalities;

  this->updateModalityCheckableComboBox();

  this->setBackgroundColorToFilterWidgets();
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidgetPrivate::updateUIAfterFilters()
{
  QModelIndexList selectedIndexes = this->PatientView->selectionModel()->selectedIndexes();
  if (selectedIndexes.empty())
  {
    this->PatientView->selectPatientUID("");
  }

  this->PatientView->refreshLayout();
  this->PatientView->studyListView()->clearSelection();
  this->PatientView->studyListView()->refreshLayout();
  this->PatientView->studyListView()->refreshSeriesLayout();
  this->updateFiltersWarnings();
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidgetPrivate::updateFiltersWarnings()
{
  if (!this->DicomDatabase)
  {
    logger.error("updateFiltersWarnings failed, no DICOM database has been set. \n");
    return;
  }

  if (!this->PatientModel)
  {
    logger.error("updateFiltersWarnings failed, no PatientModel has been set. \n");
    return;
  }

  // Reset all filter widgets to default color
  this->setBackgroundColorToFilterWidgets();

  QColor warningColor = ctkDICOMVisualBrowserWidgetWarningColor;

  // Check if there are any patients in the database at all
  QStringList allPatients = this->DicomDatabase->patients();
  if (allPatients.count() == 0)
  {
    this->setBackgroundColorToWidget(warningColor, this->FilteringPatientIDSearchBox);
    this->setBackgroundColorToWidget(warningColor, this->FilteringPatientNameSearchBox);
    return;
  }

  QStringList filteredPatientUIDs = this->PatientModel->filteredPatientUIDs();
  if (filteredPatientUIDs.count() == 0)
  {
    // No patients match the patient-level filters
    this->setBackgroundColorToWidget(warningColor, this->FilteringPatientIDSearchBox);
    this->setBackgroundColorToWidget(warningColor, this->FilteringPatientNameSearchBox);
    return;
  }

  int filteredStudiesCount = 0;
  foreach (const QString& patientUID, filteredPatientUIDs)
  {
    filteredStudiesCount += this->PatientModel->filteredStudiesCountForPatient(patientUID);
  }

  if (filteredStudiesCount == 0)
  {
    this->setBackgroundColorToWidget(warningColor, this->FilteringDateComboBox);
    this->setBackgroundColorToWidget(warningColor, this->FilteringStudyDescriptionSearchBox);
    return;
  }

  int filteredSeriesCount = 0;
  foreach (const QString& patientUID, filteredPatientUIDs)
  {
    filteredSeriesCount += this->PatientModel->filteredSeriesCountForPatient(patientUID);
  }

  if (filteredSeriesCount == 0)
  {
    this->setBackgroundColorToWidget(warningColor, this->FilteringSeriesDescriptionSearchBox);
    this->setBackgroundColorToWidget(warningColor, this->FilteringModalityCheckableComboBox);
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
void ctkDICOMVisualBrowserWidgetPrivate::updateFiltersLayoutOrientation()
{
  Q_Q(ctkDICOMVisualBrowserWidget);

  QGridLayout* searchGridLayout = qobject_cast<QGridLayout*>(this->SearchPatientsCollapsibleGroupBox->layout());
  if (!searchGridLayout)
  {
    return;
  }
  QGridLayout* actionGridLayout = qobject_cast<QGridLayout*>(this->ActionsCollapsibleGroupBox->layout());
  if (!actionGridLayout)
  {
    return;
  }
  ctkCollapsibleGroupBox * storageCollapsibleGroupBox = this->ServerNodeWidget->storageCollapsibleGroupBox();
  QGridLayout* storageGridLayout = qobject_cast<QGridLayout*>(storageCollapsibleGroupBox->layout());
  if (!storageGridLayout)
  {
    return;
  }
  if (this->IsGUIUpdating)
  {
    return;
  }

  this->IsGUIUpdating = true;

  // Use a threshold width approach
  const int thresholdWidth = 800;
  int availableWidth = q->width();
  bool horizontal = true;
  if (availableWidth < thresholdWidth)
  {
    horizontal = false;
  }

  if (this->IsGUIHorizontal == horizontal)
  {
    this->IsGUIUpdating = false;
    return;
  }

  this->IsGUIHorizontal = horizontal;

  // Widget pointers (must match UI file order)
  QWidget* patientIDBox = this->FilteringPatientIDSearchBox;
  QWidget* patientNameBox = this->FilteringPatientNameSearchBox;
  QWidget* studyDescriptionBox = this->FilteringStudyDescriptionSearchBox;
  QWidget* seriesDescriptionBox = this->FilteringSeriesDescriptionSearchBox;
  QWidget* modalityLabel = this->FilteringModalityLabel;
  QWidget* modalityCombo = this->FilteringModalityCheckableComboBox;
  QWidget* dateLabel = this->FilteringDateLabel;
  QWidget* dateCombo = this->FilteringDateComboBox;

  // Batch layout changes for performance
  q->setUpdatesEnabled(false);

  if (!this->IsGUIHorizontal)
  {
    this->setGridWidget(searchGridLayout, patientIDBox, 0, 0);
    this->setGridWidget(searchGridLayout, patientNameBox, 1, 0);
    this->setGridWidget(searchGridLayout, studyDescriptionBox, 2, 0);
    this->setGridWidget(searchGridLayout, seriesDescriptionBox, 3, 0);
    this->setGridWidget(searchGridLayout, modalityLabel, 5, 0);
    this->setGridWidget(searchGridLayout, modalityCombo, 6, 0);
    this->setGridWidget(searchGridLayout, dateLabel, 7, 0);
    this->setGridWidget(searchGridLayout, dateCombo, 8, 0);
  }
  else
  {
    this->setGridWidget(searchGridLayout, patientIDBox, 0, 0);
    this->setGridWidget(searchGridLayout, patientNameBox, 0, 1);
    this->setGridWidget(searchGridLayout, studyDescriptionBox, 1, 0);
    this->setGridWidget(searchGridLayout, seriesDescriptionBox, 1, 1);
    this->setGridWidget(searchGridLayout, modalityLabel, 0, 2);
    this->setGridWidget(searchGridLayout, modalityCombo, 1, 2);
    this->setGridWidget(searchGridLayout, dateLabel, 0, 3);
    this->setGridWidget(searchGridLayout, dateCombo, 1, 3);
  }

  QWidget* searchPushButton = this->SearchPushButton;
  QWidget* closePushButton = this->ClosePushButton;
  QWidget* importPushButton = this->ImportPushButton;
  ctkDynamicSpacer *actionsDynamicSpacer = this->ActionsDynamicSpacer;

  if (!this->IsGUIHorizontal)
  {
    this->setGridWidget(actionGridLayout, searchPushButton, 0, 0);
    this->setGridWidget(actionGridLayout, closePushButton, 1, 0);
    this->setGridWidget(actionGridLayout, importPushButton, 2, 0);
    this->setGridWidget(actionGridLayout, actionsDynamicSpacer, 3, 0);
  }
  else
  {
    this->setGridWidget(actionGridLayout, searchPushButton, 0, 0, 2, 1);
    this->setGridWidget(actionGridLayout, closePushButton, 0, 1);
    this->setGridWidget(actionGridLayout, importPushButton, 1, 1);
    this->setGridWidget(actionGridLayout, actionsDynamicSpacer, 2, 0);
  }

  // Get widgets from the storageGridLayout by their object names
  QLabel* storageEnabledLabel = storageCollapsibleGroupBox->findChild<QLabel*>("StorageEnabledLabel");
  ctkCheckBox* storageEnabledCheckBox = storageCollapsibleGroupBox->findChild<ctkCheckBox*>("StorageEnabledCheckBox");
  QLabel* storageAETitleLabel = storageCollapsibleGroupBox->findChild<QLabel*>("StorageAETitleLabel");
  QLineEdit* storageAETitle = storageCollapsibleGroupBox->findChild<QLineEdit*>("StorageAETitle");
  QLabel* storagePortLabel = storageCollapsibleGroupBox->findChild<QLabel*>("StoragePortLabel");
  QLineEdit* storagePort = storageCollapsibleGroupBox->findChild<QLineEdit*>("StoragePort");
  QLabel* storageStatusLabel = storageCollapsibleGroupBox->findChild<QLabel*>("StorageStatusLabel");
  QLabel* storageStatusValueLabel = storageCollapsibleGroupBox->findChild<QLabel*>("StorageStatusValueLabel");

  if (!this->IsGUIHorizontal)
  {
    this->setGridWidget(storageGridLayout, storageEnabledLabel, 0, 0);
    this->setGridWidget(storageGridLayout, storageEnabledCheckBox, 0, 1);
    this->setGridWidget(storageGridLayout, storageAETitleLabel, 1, 0);
    this->setGridWidget(storageGridLayout, storageAETitle, 1, 1);
    this->setGridWidget(storageGridLayout, storagePortLabel, 2, 0);
    this->setGridWidget(storageGridLayout, storagePort, 2, 1);
    this->setGridWidget(storageGridLayout, storageStatusLabel, 3, 0);
    this->setGridWidget(storageGridLayout, storageStatusValueLabel, 3, 1);
  }
  else
  {
    this->setGridWidget(storageGridLayout, storageEnabledLabel, 0, 0);
    this->setGridWidget(storageGridLayout, storageEnabledCheckBox, 0, 1);
    this->setGridWidget(storageGridLayout, storageAETitleLabel, 0, 2);
    this->setGridWidget(storageGridLayout, storageAETitle, 0, 3);
    this->setGridWidget(storageGridLayout, storagePortLabel, 0, 4);
    this->setGridWidget(storageGridLayout, storagePort, 0, 5);
    this->setGridWidget(storageGridLayout, storageStatusLabel, 0, 6);
    this->setGridWidget(storageGridLayout, storageStatusValueLabel, 0, 7);
  }

  q->setUpdatesEnabled(true);
  this->IsGUIUpdating = false;
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidgetPrivate::retrieveSeries()
{
  // TODO: Implement series retrieve logic with model/view architecture
}

//----------------------------------------------------------------------------
QString ctkDICOMVisualBrowserWidgetPrivate::findPatientUIDByPatientID(const QString& patientID)
{
  if (!this->DicomDatabase || patientID.isEmpty())
  {
    return QString();
  }

  // Search through database for patient with this ID
  QStringList patientList = this->DicomDatabase->patients();
  foreach (QString patientUID, patientList)
  {
    QString currentPatientID = this->DicomDatabase->fieldForPatient("PatientID", patientUID);
    if (currentPatientID == patientID)
    {
      return patientUID;
    }
  }

  return QString();
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

  foreach (QString patientUID, patientList)
  {
    bool filtered = false;
    for (QString key : filters.keys())
    {
      QString filter = this->DicomDatabase->fieldForPatient(key, patientUID);
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

    filteredPatientList.append(patientUID);
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
int ctkDICOMVisualBrowserWidgetPrivate::computeThumbnailSizeInPixels(ctkDICOMVisualBrowserWidget::ThumbnailSizePresetOption sizeOption)
{
  if (sizeOption == ctkDICOMVisualBrowserWidget::None)
  {
    return ctkDICOMVisualBrowserWidgetThumbnailSizePixelsNone;
  }
  else if (sizeOption == ctkDICOMVisualBrowserWidget::Small)
  {
    return ctkDICOMVisualBrowserWidgetThumbnailSizePixelsSmall;
  }
  else if (sizeOption == ctkDICOMVisualBrowserWidget::Medium)
  {
    return ctkDICOMVisualBrowserWidgetThumbnailSizePixelsMedium;
  }
  else if (sizeOption == ctkDICOMVisualBrowserWidget::Large)
  {
    return ctkDICOMVisualBrowserWidgetThumbnailSizePixelsLarge;
  }

  return ctkDICOMVisualBrowserWidgetThumbnailSizePixelsSmall;
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidgetPrivate::updateThumbnailSizeForCurrentStudyView(ctkDICOMVisualBrowserWidget::ThumbnailSizePresetOption sizeOption)
{
  Q_Q(ctkDICOMVisualBrowserWidget);

  ctkDICOMPatientView* patientView = q->patientView();
  ctkDICOMStudyListView* studyView = nullptr;
  QModelIndex firstVisibleIndex;
  if (patientView)
  {
    studyView = patientView->studyListView();
    if (studyView)
    {
      QRect viewportRect = studyView->viewport()->rect();
      firstVisibleIndex = studyView->indexAt(QPoint(viewportRect.left(), viewportRect.top()));
    }
  }

  q->setThumbnailSizePreset(sizeOption);

  // Restore the first visible item and current index after thumbnail size change
  if (studyView && firstVisibleIndex.isValid())
  {
    studyView->scrollTo(firstVisibleIndex, QAbstractItemView::PositionAtTop);
  }
}

//----------------------------------------------------------------------------
int ctkDICOMVisualBrowserWidgetPrivate::getNDaysFromFilteringDate(ctkDICOMVisualBrowserWidget::DateType dateType)
{
  switch (dateType)
  {
    case ctkDICOMVisualBrowserWidget::Any:
      return -1;
    case ctkDICOMVisualBrowserWidget::Today:
      return 0;
    case ctkDICOMVisualBrowserWidget::Yesterday:
      return 1;
    case ctkDICOMVisualBrowserWidget::LastWeek:
      return 7;
    case ctkDICOMVisualBrowserWidget::LastMonth:
      return 30;
    case ctkDICOMVisualBrowserWidget::LastYear:
      return 365;
    case ctkDICOMVisualBrowserWidget::CustomRange:
      return -1;
    default:
      return -1;
  }
}

//----------------------------------------------------------------------------
QString ctkDICOMVisualBrowserWidgetPrivate::filenameSafeString(const QString& str)
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

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidgetPrivate::setGridWidget(QGridLayout* layout, QWidget* widget, int row, int col, int rowSpan, int colSpan)
{
  int idx = layout->indexOf(widget);
  if (idx >= 0) {
    int curRow, curCol, curRowSpan, curColSpan;
    layout->getItemPosition(idx, &curRow, &curCol, &curRowSpan, &curColSpan);
    if (curRow == row && curCol == col && curRowSpan == rowSpan && curColSpan == colSpan) return;
    layout->removeWidget(widget);
  }
  layout->addWidget(widget, row, col, rowSpan, colSpan);
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
CTK_GET_CPP(ctkDICOMVisualBrowserWidget, ctkDICOMVisualBrowserWidget::DateType, filteringDate, FilteringDate);
CTK_GET_CPP(ctkDICOMVisualBrowserWidget, QString, filteringSeriesDescription, FilteringSeriesDescription);
CTK_GET_CPP(ctkDICOMVisualBrowserWidget, QStringList, filteringModalities, FilteringModalities);
CTK_SET_CPP(ctkDICOMVisualBrowserWidget, int, setNumberOfOpenedStudiesPerPatient, NumberOfOpenedStudiesPerPatient);
CTK_GET_CPP(ctkDICOMVisualBrowserWidget, int, numberOfOpenedStudiesPerPatient, NumberOfOpenedStudiesPerPatient);
CTK_GET_CPP(ctkDICOMVisualBrowserWidget, ctkDICOMVisualBrowserWidget::ThumbnailSizePresetOption, thumbnailSizePreset, ThumbnailSizePreset);
CTK_SET_CPP(ctkDICOMVisualBrowserWidget, bool, setSendActionVisible, SendActionVisible);
CTK_GET_CPP(ctkDICOMVisualBrowserWidget, bool, isSendActionVisible, SendActionVisible);
CTK_SET_CPP(ctkDICOMVisualBrowserWidget, bool, setDeleteActionVisible, DeleteActionVisible);
CTK_GET_CPP(ctkDICOMVisualBrowserWidget, bool, isDeleteActionVisible, DeleteActionVisible);

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::setThumbnailSizePreset(ctkDICOMVisualBrowserWidget::ThumbnailSizePresetOption thumbnailSizePreset)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  if (d->ThumbnailSizePreset == thumbnailSizePreset)
  {
    return;
  }

  d->ThumbnailSizePreset = thumbnailSizePreset;
  if (d->PatientModel)
  {
    d->PatientModel->setThumbnailSize(d->computeThumbnailSizeInPixels(thumbnailSizePreset));
  }
}

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
void ctkDICOMVisualBrowserWidget::setFilteringDate(DateType filteringDate)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->FilteringDate = filteringDate;
  d->FilteringDateComboBox->setCurrentIndex(static_cast<int>(d->FilteringDate));
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

//------------------------------------------------------------------------------
ctkDICOMPatientView* ctkDICOMVisualBrowserWidget::patientView() const
{
  Q_D(const ctkDICOMVisualBrowserWidget);
  return d->PatientView;
}

//------------------------------------------------------------------------------
ctkDICOMPatientModel* ctkDICOMVisualBrowserWidget::patientModel() const
{
  Q_D(const ctkDICOMVisualBrowserWidget);
  return d->PatientModel.data();
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

  // Clean model first (this will clean all study models and series models)
  d->PatientModel->clean();
  // Then clean the view (this will clean study view and all series views)
  d->PatientView->clean();
  d->createPatients();
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

  // allow users of this widget to know that the process has finished
  emit directoryImported();

  d->createPatients(false, QStringList(), true);
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
  this->openServerSettingsSection();
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::openServerSettingsSection()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->JobsCollapsibleGroupBox->setChecked(false);
  d->AdvancedCollapsibleGroupBox->setChecked(true);
  // Ensure the splitter shows the AdvancedCollapsibleGroupBox
  if (d->AdvancedCollapsibleGroupBox->parentWidget())
  {
    QSplitter* splitter = qobject_cast<QSplitter*>(d->AdvancedCollapsibleGroupBox->parentWidget());
    if (splitter)
    {
      int index = splitter->indexOf(d->AdvancedCollapsibleGroupBox);
      if (index >= 0)
      {
        int height = splitter->size().height() * 0.5;
        splitter->setSizes(QList<int>() << height << height);
      }
    }
  }
  d->ServersSettingsCollapsibleGroupBox->setChecked(true);
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::showMetadata(const QStringList& fileList)
{
  Q_D(const ctkDICOMVisualBrowserWidget);
  d->MetadataDialog->setFileList(fileList);
  d->MetadataDialog->show();
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onFilteringPatientIDChanged()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->FilteringPatientID = d->FilteringPatientIDSearchBox->text();
  if (!d->PatientModel || !d->PatientView || !d->PatientView->studyListView())
  {
    return;
  }

  d->PatientModel->setPatientIDFilter(d->FilteringPatientID);
  d->updateUIAfterFilters();
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onFilteringPatientNameChanged()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->FilteringPatientName = d->FilteringPatientNameSearchBox->text();
  if (!d->PatientModel || !d->PatientView || !d->PatientView->studyListView())
  {
    return;
  }

  d->PatientModel->setPatientNameFilter(d->FilteringPatientName);
  d->updateUIAfterFilters();
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onFilteringStudyDescriptionChanged()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->FilteringStudyDescription = d->FilteringStudyDescriptionSearchBox->text();
  if (!d->PatientModel || !d->PatientView || !d->PatientView->studyListView())
  {
    return;
  }

  d->PatientModel->setStudyDescriptionFilter(d->FilteringStudyDescription);
  d->updateUIAfterFilters();
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onFilteringSeriesDescriptionChanged()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->FilteringSeriesDescription = d->FilteringSeriesDescriptionSearchBox->text();
  if (!d->PatientModel || !d->PatientView || !d->PatientView->studyListView())
  {
    return;
  }

  d->PatientModel->setSeriesDescriptionFilter(d->FilteringSeriesDescription);
  d->updateUIAfterFilters();
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

  if (!d->PatientModel || !d->PatientView || !d->PatientView->studyListView())
  {
    return;
  }

  d->PatientModel->setModalityFilter(d->FilteringModalities);
  d->updateUIAfterFilters();
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onFilteringDateComboBoxChanged(int index)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->FilteringDate = static_cast<DateType>(index);

  // Retrieve the popup widget
  ctkPopupWidget* dateRangePopup = qobject_cast<ctkPopupWidget*>(this->property("dateRangePopup").value<QWidget*>());

  // Show popup for custom date range selection, hide for other options
  if (d->FilteringDate == CustomRange)
  {
    if (dateRangePopup)
    {
      dateRangePopup->showPopup();
    }
    d->FilteringStartDate = d->FilteringStartDateEdit->date();
    d->FilteringEndDate = d->FilteringEndDateEdit->date();
  }
  else
  {
    // Hide popup when selecting any other date filter option
    if (dateRangePopup)
    {
      dateRangePopup->hidePopup();
    }
  }

  if (!d->PatientModel || !d->PatientView || !d->PatientView->studyListView())
  {
    return;
  }

  d->PatientModel->setDateFilter(static_cast<ctkDICOMPatientModel::DateType>(index));
  d->updateUIAfterFilters();
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onFilteringStartDateChanged(const QDate& date)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->FilteringStartDate = date;

  if (d->FilteringDate != CustomRange)
  {
    return;
  }
  if (!d->PatientModel || !d->PatientView || !d->PatientView->studyListView())
  {
    return;
  }
  // Only apply filter if both dates are valid
  if (d->FilteringStartDate.isValid() && d->FilteringEndDate.isValid())
  {
    d->PatientModel->setCustomDateRange(d->FilteringStartDate, d->FilteringEndDate);
    d->updateUIAfterFilters();
  }
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onFilteringEndDateChanged(const QDate& date)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  d->FilteringEndDate = date;

  if (d->FilteringDate != CustomRange)
  {
    return;
  }
  if (!d->PatientModel || !d->PatientView || !d->PatientView->studyListView())
  {
    return;
  }

  // Only apply filter if both dates are valid
  if (d->FilteringStartDate.isValid() && d->FilteringEndDate.isValid())
  {
    d->PatientModel->setCustomDateRange(d->FilteringStartDate, d->FilteringEndDate);
    d->updateUIAfterFilters();
  }
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onQueryPatients()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  if (d->IsGUIUpdating)
  {
    return;
  }

  if (!d->DicomDatabase)
  {
    logger.error("onQueryPatients failed, no DICOM database has been set. \n");
    return;
  }

  // Stop any fetching task.
  this->onStop();
  d->SearchPushButton->setIcon(QIcon(":/Icons/query.svg"));
  d->WarningPushButton->hide();
  d->updateFiltersWarnings();

  bool filtersAreEmpty = d->areFiltersEmpty();
  bool hasActiveQueryRetrieveServer = d->Scheduler->queryRetrieveServersCount() > 0;
  bool localDatabaseIsEmpty = (d->DicomDatabase->patients().count() == 0);

  if (!hasActiveQueryRetrieveServer)
  {
    d->SearchPushButton->setIcon(QIcon(":/Icons/query_failed.svg"));
    QString warningText = tr("No server is configured for query/retrieve operations.\n"
                              "The server settings section is below.");
    logger.warn(warningText);
    d->WarningPushButton->setText(warningText);
    d->WarningPushButton->show();
    this->openServerSettingsSection();
    return;
  }

  if (filtersAreEmpty && localDatabaseIsEmpty)
  {
    d->setBackgroundColorToFilterWidgets(true);
    d->SearchPushButton->setIcon(QIcon(":/Icons/query_failed.svg"));
    QString warningText = tr("No filters are set and no patients are found in the local database.\n"
                              "Please specify at least one filter to query the servers.");
    logger.warn(warningText);
    d->WarningPushButton->setText(warningText);
    d->WarningPushButton->show();
    return;
  }

  // Save current patient selection
  if (d->PatientView)
  {
    QString patientUID = d->PatientView->currentPatientUID();
    if (patientUID.isEmpty())
    {
      if (d->PatientModel->rowCount() > 0)
      {
        QModelIndex firstPatientIndex = d->PatientModel->index(0, 0);
        patientUID = d->PatientModel->patientUID(firstPatientIndex);
        d->PatientView->selectPatientUID(patientUID);
      }
    }
    d->SelectedPatientUID = patientUID;
  }

  d->WarningPushButton->hide();
  d->updateFiltersWarnings();

  d->SearchPushButton->setIcon(QIcon(":/Icons/wait.svg"));
  if (!filtersAreEmpty)
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

    int nDays = d->getNDaysFromFilteringDate(d->FilteringDate);
    if (nDays != -1)
    {
      QDate endDate = QDate::currentDate();
      QString formattedEndDate = endDate.toString("yyyyMMdd");

      QDate startDate = endDate.addDays(-nDays);
      QString formattedStartDate = startDate.toString("yyyyMMdd");

      parameters["StartDate"] = formattedStartDate;
      parameters["EndDate"] = formattedEndDate;
    }
    else if (d->FilteringDate == ctkDICOMVisualBrowserWidget::CustomRange)
    {
      QString formattedStartDate = d->FilteringStartDate.toString("yyyyMMdd");
      QString formattedEndDate = d->FilteringEndDate.toString("yyyyMMdd");

      parameters["StartDate"] = formattedStartDate;
      parameters["EndDate"] = formattedEndDate;
    }

    d->Scheduler->setFilters(parameters);
    d->Scheduler->queryPatients(QThread::NormalPriority);
  }
  else
  {
    d->createPatients(true, QStringList(), false);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::updateGUIFromScheduler(QList<QVariant> datas)
{
  Q_D(ctkDICOMVisualBrowserWidget);

  if (!d->Scheduler || !d->PatientModel || !d->PatientView)
  {
    return;
  }

  bool updatePatients = false;
  QStringList queriedPatientIDs;
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
      d->PatientModel->updateGUIFromScheduler(data);
    }

    if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryStudies ||
      td.JobType == ctkDICOMJobResponseSet::JobType::QuerySeries)
    {
      if (td.JobType == ctkDICOMJobResponseSet::JobType::QuerySeries &&
          d->PatientView->currentPatientUID().isEmpty())
      {
        d->PatientView->selectPatientUID(d->SelectedPatientUID);
        d->SelectedPatientUID = d->PatientView->currentPatientUID();
        d->PatientModel->refreshPatient(d->SelectedPatientUID);
        d->PatientModel->updateAllowedServersFromDB(d->SelectedPatientUID);
      }
      d->updateFiltersWarnings();
      continue;
    }
    else if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryInstances)
    {
      // NOTE: if running query jobs, wait to open the first N studies of the patient.
      if (d->Scheduler->getJobsByDICOMUIDs({td.PatientID}).count() == 0)
      {
        // Wait for the proxy model to finish sorting before opening studies
        // The proxy model sorts asynchronously (due to dynamic sort filter)
        // Since the studies are already in the model from QueryStudies, we just need
        // to wait for the proxy model to complete its sorting after the data changes
        ctkDICOMStudyListView* studyListView = d->PatientView->studyListView();

        // Use a lambda to capture the number of studies to open
        int numberOfStudies = this->numberOfOpenedStudiesPerPatient();

        // Use QTimer::singleShot with a small delay to ensure the proxy model has finished sorting
        // The proxy model processes data changes in the event loop, so we defer our action
        // to the next event loop iteration
        QTimer::singleShot(100, studyListView, [studyListView, numberOfStudies]() {
          studyListView->onNumberOfOpenedStudiesChanged(numberOfStudies);
        });
      }
    }
    else if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryPatients)
    {
      d->updateFiltersWarnings();
      if (td.NumberOfDataSets == 0)
      {
        d->WarningPushButton->setText(tr("The patients query provided no results. Please refine your filters in the search section."));
        d->WarningPushButton->show();
        d->SearchPushButton->setIcon(QIcon(":/Icons/query_failed.svg"));
      }
      else
      {
        d->WarningPushButton->hide();
        queriedPatientIDs.append(td.QueriedPatientIDs);
        updatePatients = true;
      }
    }
  }

  if (updatePatients)
  {
    d->createPatients(true, queriedPatientIDs, false);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onJobStarted(QList<QVariant> datas)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  if (!d->PatientModel)
  {
    return;
  }

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
      d->SearchPushButton->setIcon(QIcon(":/Icons/wait.svg"));
      continue;
    }

    if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryStudies ||
        td.JobType == ctkDICOMJobResponseSet::JobType::QuerySeries ||
        td.JobType == ctkDICOMJobResponseSet::JobType::QueryInstances ||
        td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
        td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries)
    {
      d->PatientModel->onJobStarted(data);
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onJobUserStopped(QList<QVariant> datas)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  if (!d->PatientModel)
  {
    return;
  }

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
      d->SearchPushButton->setIcon(QIcon(":/Icons/query_failed.svg"));
      continue;
    }

    if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryStudies ||
        td.JobType == ctkDICOMJobResponseSet::JobType::QuerySeries ||
        td.JobType == ctkDICOMJobResponseSet::JobType::QueryInstances ||
        td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
        td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries)
    {
      d->PatientModel->onJobUserStopped(data);
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onJobFailed(QList<QVariant> datas)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  if (!d->PatientModel)
  {
    return;
  }

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
      d->SearchPushButton->setIcon(QIcon(":/Icons/query_failed.svg"));
      d->WarningPushButton->setText(tr("The patients query failed. Please check the servers settings."));
      d->WarningPushButton->show();
      continue;
    }

    if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryStudies ||
        td.JobType == ctkDICOMJobResponseSet::JobType::QuerySeries ||
        td.JobType == ctkDICOMJobResponseSet::JobType::QueryInstances ||
        td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
        td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries)
    {
      d->PatientModel->onJobFailed(data);
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onJobFinished(QList<QVariant> datas)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  if (!d->PatientModel)
  {
    return;
  }

  foreach (QVariant data, datas)
  {
    ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
    if (td.JobUID.isEmpty())
    {
      continue;
    }

    if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryPatients ||
        td.JobType == ctkDICOMJobResponseSet::JobType::QueryStudies)
    {
      d->updateFiltersWarnings();
      d->SearchPushButton->setIcon(QIcon(":/Icons/query_success.svg"));
    }
    if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryStudies ||
        td.JobType == ctkDICOMJobResponseSet::JobType::QuerySeries ||
        td.JobType == ctkDICOMJobResponseSet::JobType::QueryInstances ||
        td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
        td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries ||
        td.JobType == ctkDICOMJobResponseSet::JobType::ThumbnailGenerator)
    {
      d->PatientModel->onJobFinished(data);
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onServersSettingsChanged()
{
  Q_D(ctkDICOMVisualBrowserWidget);
  if (!d->PatientModel)
  {
    return;
  }

  // Update allowed servers from database for all patients when server settings change
  QStringList patientUIDs = d->PatientModel->filteredPatientUIDs();
  foreach (const QString& patientUID, patientUIDs)
  {
    d->PatientModel->updateAllowedServersFromDB(patientUID);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::patientSelectedOnJobList(const QString& patientID,
                                                           const QString& patientName,
                                                           const QString& patientBirthDate)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  Q_UNUSED(patientName);
  Q_UNUSED(patientBirthDate);

  if (!d->PatientView || !d->DicomDatabase)
  {
    return;
  }

  QString patientUID = d->findPatientUIDByPatientID(patientID);
  if (patientUID.isEmpty())
  {
    return;
  }

  // Select the patient item in the patient view
  d->PatientView->selectPatientUID(patientUID);
  d->PatientView->refreshLayout();
}

//----------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onPatientViewDisplayModeChanged(ctkDICOMPatientView::DisplayMode mode)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  if (!d->PatientFilterProxyModel)
  {
    return;
  }
  d->PatientFilterProxyModel.data()->setDisplayMode(
      mode == ctkDICOMPatientView::TabMode
          ? ctkDICOMPatientFilterProxyModel::TabMode
          : ctkDICOMPatientFilterProxyModel::ListMode
      );
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::showPatientContextMenu(const QPoint& globalPos, const QStringList& selectedPatientUIDs)
{
  if (selectedPatientUIDs.isEmpty())
  {
    return;
  }

  int patientCount = selectedPatientUIDs.count();

  // Create context menu
  QMenu* patientMenu = new QMenu(this);

  // Set window flags to enable rounded corners
  patientMenu->setWindowFlags(patientMenu->windowFlags() | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
  patientMenu->setAttribute(Qt::WA_TranslucentBackground);
  // To Do: add an option to disable styling
  patientMenu->setStyleSheet(
    "QMenu {"
    "  background-color: #ffffff;"
    "  border-radius: 8px;"
    "  border: 1px solid #e0e0e0;"
    "  padding: 4px;"
    "}"
    "QMenu::item {"
    "  padding: 8px 32px 8px 16px;"
    "  border-radius: 4px;"
    "  color: #212121;"
    "  background-color: transparent;"
    "  margin: 2px 4px;"
    "}"
    "QMenu::item:selected {"
    "  background-color: #f5f5f5;"
    "  color: #1976d2;"
    "}"
    "QMenu::separator {"
    "  height: 1px;"
    "  background: #eeeeee;"
    "  margin: 4px 8px;"
    "}"
  );

  // Load action
  QString loadString = patientCount == 1 ? tr("Load patient") :
    tr("Load %1 patients").arg(patientCount);
  QAction* loadAction = new QAction(QIcon(":/Icons/load.svg"), loadString, patientMenu);
  patientMenu->addAction(loadAction);

  // View metadata action
  QString metadataString = patientCount == 1 ? tr("View patient DICOM metadata") :
    tr("View %1 patients DICOM metadata").arg(patientCount);
  QAction* metadataAction = new QAction(QIcon(":/Icons/metadata.svg"), metadataString, patientMenu);
  patientMenu->addAction(metadataAction);

  // Delete action
  QString deleteString = patientCount == 1 ? tr("Delete patient from local database") :
    tr("Delete %1 patients from local database").arg(patientCount);
  QAction* deleteAction = new QAction(QIcon(":/Icons/delete.svg"), deleteString, patientMenu);
  deleteAction->setVisible(this->isDeleteActionVisible());
  patientMenu->addAction(deleteAction);

  // Export action
  QString exportString = patientCount == 1 ? tr("Export patient to file system") :
    tr("Export %1 patients to file system").arg(patientCount);
  QAction* exportAction = new QAction(QIcon(":/Icons/export.svg"), exportString, patientMenu);
  patientMenu->addAction(exportAction);

  // Send action (if enabled)
  QString sendString = patientCount == 1 ? tr("Send patient to DICOM server") :
    tr("Send %1 patients to DICOM server").arg(patientCount);
  QAction* sendAction = new QAction(QIcon(":/Icons/upload.svg"), sendString, patientMenu);
  sendAction->setVisible(this->isSendActionVisible());
  patientMenu->addAction(sendAction);

  // Execute menu
  QAction* selectedAction = patientMenu->exec(globalPos);

  // Handle selected action
  if (selectedAction == loadAction)
  {
    this->onLoadPatients(selectedPatientUIDs);
  }
  else if (selectedAction == metadataAction)
  {
    this->showMetadataForPatients(selectedPatientUIDs);
  }
  else if (selectedAction == deleteAction)
  {
    this->removePatients(selectedPatientUIDs);
  }
  else if (selectedAction == exportAction)
  {
    this->exportPatients(selectedPatientUIDs);
  }
  else if (selectedAction == sendAction)
  {
    // Get all studies for the selected patients and send their files
    QStringList selectedStudyInstanceUIDs = this->studiesForPatients(selectedPatientUIDs);
    QStringList seriesInstanceUIDs = this->seriesForStudies(selectedStudyInstanceUIDs);
    emit this->sendRequested(this->filesForSeries(seriesInstanceUIDs));
  }

  delete patientMenu;
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::showStudyContextMenu(const QPoint& globalPos, const QStringList& selectedStudyInstanceUIDs)
{
  Q_D(ctkDICOMVisualBrowserWidget);

  if (selectedStudyInstanceUIDs.isEmpty() || !d->PatientView)
  {
    return;
  }

  int numberOfStudies = selectedStudyInstanceUIDs.count();

  QMenu* studyMenu = new QMenu(this);

  // Set window flags to enable rounded corners
  studyMenu->setWindowFlags(studyMenu->windowFlags() | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
  studyMenu->setAttribute(Qt::WA_TranslucentBackground);
  // To Do: add an option to disable styling
  studyMenu->setStyleSheet(
    "QMenu {"
    "  background-color: #ffffff;"
    "  border-radius: 8px;"
    "  border: 1px solid #e0e0e0;"
    "  padding: 4px;"
    "}"
    "QMenu::item {"
    "  padding: 8px 32px 8px 16px;"
    "  border-radius: 4px;"
    "  color: #212121;"
    "  background-color: transparent;"
    "  margin: 2px 4px;"
    "}"
    "QMenu::item:selected {"
    "  background-color: #f5f5f5;"
    "  color: #1976d2;"
    "}"
    "QMenu::separator {"
    "  height: 1px;"
    "  background: #eeeeee;"
    "  margin: 4px 8px;"
    "}"
  );

  // Load action
  QString loadString = numberOfStudies == 1 ? tr("Load study") :
    tr("Load %1 studies").arg(numberOfStudies);
  QAction* loadAction = new QAction(QIcon(":/Icons/load.svg"), loadString, studyMenu);
  studyMenu->addAction(loadAction);

  // View metadata action
  QString metadataString = numberOfStudies == 1 ? tr("View study DICOM metadata") :
    tr("View %1 studies DICOM metadata").arg(numberOfStudies);
  QAction* metadataAction = new QAction(QIcon(":/Icons/metadata.svg"), metadataString, studyMenu);
  studyMenu->addAction(metadataAction);

  // Force retrieve action
  QString forceRetrieveString = numberOfStudies == 1 ? tr("Force retrieve study") :
    tr("Force retrieve %1 studies").arg(numberOfStudies);
  QAction* forceRetrieveAction = new QAction(QIcon(":/Icons/download.svg"), forceRetrieveString, studyMenu);
  studyMenu->addAction(forceRetrieveAction);

  // Delete action
  QString deleteString = numberOfStudies == 1 ? tr("Delete study from local database") :
    tr("Delete %1 studies from local database").arg(numberOfStudies);
  QAction* deleteAction = new QAction(QIcon(":/Icons/delete.svg"), deleteString, studyMenu);
  deleteAction->setVisible(this->isDeleteActionVisible());
  studyMenu->addAction(deleteAction);

  // Export action
  QString exportString = numberOfStudies == 1 ? tr("Export study to file system") :
    tr("Export %1 studies to file system").arg(numberOfStudies);
  QAction* exportAction = new QAction(QIcon(":/Icons/export.svg"), exportString, studyMenu);
  studyMenu->addAction(exportAction);

  // Send action
  QString sendString = numberOfStudies == 1 ? tr("Send study to DICOM server") :
    tr("Send %1 studies to DICOM server").arg(numberOfStudies);
  QAction* sendAction = new QAction(QIcon(":/Icons/upload.svg"), sendString, studyMenu);
  sendAction->setVisible(this->isSendActionVisible());
  studyMenu->addAction(sendAction);

  // Execute menu
  QAction* selectedAction = studyMenu->exec(globalPos);

  if (selectedAction == loadAction)
  {
    this->onLoadStudies(selectedStudyInstanceUIDs);
  }
  else if (selectedAction == metadataAction)
  {
    this->showMetadataForStudies(selectedStudyInstanceUIDs);
  }
  else if (selectedAction == forceRetrieveAction)
  {
    this->forceRetrieveStudies(selectedStudyInstanceUIDs);
  }
  else if (selectedAction == deleteAction)
  {
    this->removeStudies(selectedStudyInstanceUIDs);
  }
  else if (selectedAction == exportAction)
  {
    this->exportStudies(selectedStudyInstanceUIDs);
  }
  else if (selectedAction == sendAction)
  {
    // Get all series for the selected studies and send their files
    QStringList seriesInstanceUIDs = this->seriesForStudies(selectedStudyInstanceUIDs);
    emit this->sendRequested(this->filesForSeries(seriesInstanceUIDs));
  }

  delete studyMenu;
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::showSeriesContextMenu(const QPoint& globalPos, const QStringList& selectedSeriesInstanceUIDs)
{
  Q_D(ctkDICOMVisualBrowserWidget);

  if (selectedSeriesInstanceUIDs.isEmpty() || !d->PatientView)
  {
    return;
  }

  int numberOfSelectedSeries = selectedSeriesInstanceUIDs.count();

  QMenu* seriesMenu = new QMenu(this);

  // Set window flags to enable rounded corners
  seriesMenu->setWindowFlags(seriesMenu->windowFlags() | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
  seriesMenu->setAttribute(Qt::WA_TranslucentBackground);
  // To Do: add an option to disable styling
  seriesMenu->setStyleSheet(
    "QMenu {"
    "  background-color: #ffffff;"
    "  border-radius: 8px;"
    "  border: 1px solid #e0e0e0;"
    "  padding: 4px;"
    "}"
    "QMenu::item {"
    "  padding: 8px 32px 8px 16px;"
    "  border-radius: 4px;"
    "  color: #212121;"
    "  background-color: transparent;"
    "  margin: 2px 4px;"
    "}"
    "QMenu::item:selected {"
    "  background-color: #f5f5f5;"
    "  color: #1976d2;"
    "}"
    "QMenu::separator {"
    "  height: 1px;"
    "  background: #eeeeee;"
    "  margin: 4px 8px;"
    "}"
  );

  // Load action
  QString loadString = numberOfSelectedSeries == 1 ? tr("Load series") :
    tr("Load %1 series").arg(numberOfSelectedSeries);
  QAction* loadAction = new QAction(QIcon(":/Icons/load.svg"), loadString, seriesMenu);
  seriesMenu->addAction(loadAction);

  // View metadata action
  QString metadataString = numberOfSelectedSeries == 1 ? tr("View series DICOM metadata") :
    tr("View %1 series DICOM metadata").arg(numberOfSelectedSeries);
  QAction* metadataAction = new QAction(QIcon(":/Icons/metadata.svg"), metadataString, seriesMenu);
  seriesMenu->addAction(metadataAction);

  // Force retrieve action
  QString forceRetrieveString = numberOfSelectedSeries == 1 ? tr("Force retrieve series") :
    tr("Force retrieve %1 series").arg(numberOfSelectedSeries);
  QAction* forceRetrieveAction = new QAction(QIcon(":/Icons/download.svg"), forceRetrieveString, seriesMenu);
  seriesMenu->addAction(forceRetrieveAction);

  // Delete action
  QString deleteString = numberOfSelectedSeries == 1 ? tr("Delete series from local database") :
    tr("Delete %1 series from local database").arg(numberOfSelectedSeries);
  QAction* deleteAction = new QAction(QIcon(":/Icons/delete.svg"), deleteString, seriesMenu);
  deleteAction->setVisible(this->isDeleteActionVisible());
  seriesMenu->addAction(deleteAction);

  // Export action
  QString exportString = numberOfSelectedSeries == 1 ? tr("Export series to file system") :
    tr("Export %1 series to file system").arg(numberOfSelectedSeries);
  QAction* exportAction = new QAction(QIcon(":/Icons/export.svg"), exportString, seriesMenu);
  seriesMenu->addAction(exportAction);

  // Send action
  QString sendString = numberOfSelectedSeries == 1 ? tr("Send series to DICOM server") :
    tr("Send %1 series to DICOM server").arg(numberOfSelectedSeries);
  QAction* sendAction = new QAction(QIcon(":/Icons/upload.svg"), sendString, seriesMenu);
  sendAction->setVisible(this->isSendActionVisible());
  seriesMenu->addAction(sendAction);

  // Execute menu
  QAction* selectedAction = seriesMenu->exec(globalPos);

  if (selectedAction == loadAction)
  {
    this->onLoadSeries(selectedSeriesInstanceUIDs);
  }
  else if (selectedAction == metadataAction)
  {
    this->showMetadataForSeries(selectedSeriesInstanceUIDs);
  }
  else if (selectedAction == forceRetrieveAction)
  {
    this->forceRetrieveSeries(selectedSeriesInstanceUIDs);
  }
  else if (selectedAction == deleteAction)
  {
    this->removeSeries(selectedSeriesInstanceUIDs);
  }
  else if (selectedAction == exportAction)
  {
    this->exportSeries(selectedSeriesInstanceUIDs);
  }
  else if (selectedAction == sendAction)
  {
    emit this->sendRequested(this->filesForSeries(selectedSeriesInstanceUIDs));
  }

  delete seriesMenu;
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onSeriesDoubleClicked(const QString& seriesInstanceUID)
{
  Q_D(ctkDICOMVisualBrowserWidget);

  if (seriesInstanceUID.isEmpty() || !d->PatientView)
  {
    return;
  }

  // Clear all series selections across all studies
  QStringList selectedSeriesInstanceUIDs;
  ctkDICOMStudyListView* studyListView = d->PatientView->studyListView();
  if (studyListView)
  {
    QMap<QString, ctkDICOMSeriesTableView*> allSeriesViews = studyListView->getAllSeriesView();
    for (ctkDICOMSeriesTableView* seriesView : allSeriesViews)
    {
      if (seriesView)
      {
        selectedSeriesInstanceUIDs.append(seriesView->selectedSeriesInstanceUIDs());
      }
    }
  }

  // Load the series
  this->onLoadSeries(selectedSeriesInstanceUIDs);
}

//------------------------------------------------------------------------------
QStringList ctkDICOMVisualBrowserWidget::studiesForPatients(const QStringList& patientUIDs)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  QStringList studyUIDs;

  if (!d->DicomDatabase)
  {
    return studyUIDs;
  }

  foreach (const QString& patientUID, patientUIDs)
  {
    QStringList studiesList = d->DicomDatabase->studiesForPatient(patientUID);
    studiesList.removeAll("");
    studyUIDs << studiesList;
  }

  return studyUIDs;
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onLoadPatients(const QStringList& patientUIDs)
{
  // Get all studies for the patients and load them
  QStringList studyUIDs = this->studiesForPatients(patientUIDs);
  this->onLoadStudies(studyUIDs);
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::showMetadataForPatients(const QStringList& patientUIDs)
{
  // Get all studies for the patients and show their metadata
  QStringList studyUIDs = this->studiesForPatients(patientUIDs);
  this->showMetadataForStudies(studyUIDs);
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::removePatients(const QStringList& patientUIDs)
{
  Q_D(ctkDICOMVisualBrowserWidget);

  if (!d->PatientModel || !d->PatientView ||
      !d->Scheduler || !d->DicomDatabase)
  {
    return;
  }

  // Confirm delete
  if (!this->confirmDeleteSelectedUIDs(patientUIDs))
  {
    return;
  }

  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

  QStringList patientIDs;
  for (const QString& patientUID : patientUIDs)
  {
    QString patientID = d->PatientModel->patientID(d->PatientModel->indexFromPatientUID(patientUID));
    if (!patientID.isEmpty())
    {
      patientIDs << patientID;
    }
  }

  // Stop any running jobs for these studies
  d->Scheduler->stopJobsByDICOMUIDs(patientIDs);

  // Delete patients from database
  QStringList loadedSeriesInstanceUIDs = d->DicomDatabase->loadedSeriesInstanceUIDs();
  foreach (const QString& patientUID, patientUIDs)
  {
    QStringList studyInstanceUIDs = d->DicomDatabase->studiesForPatient(patientUID);
    for (const QString& studyInstanceUID : studyInstanceUIDs)
    {
      QStringList seriesInstanceUIDs = d->DicomDatabase->seriesForStudy(studyInstanceUID);
      for (const QString& seriesInstanceUID : seriesInstanceUIDs)
      {
        loadedSeriesInstanceUIDs.removeAll(seriesInstanceUID);
      }
    }
    d->DicomDatabase->removePatient(patientUID);
  }
  d->DicomDatabase->setLoadedSeriesInstanceUIDs(loadedSeriesInstanceUIDs);

  // Refresh the patient model
  d->PatientModel->refresh();
  d->PatientView->selectPatientUID("");
  QApplication::restoreOverrideCursor();
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::exportPatients(const QStringList& patientUIDs)
{
  // Get all studies for the patients and export them
  QStringList studyUIDs = this->studiesForPatients(patientUIDs);
  this->exportStudies(studyUIDs);
}

//------------------------------------------------------------------------------
QStringList ctkDICOMVisualBrowserWidget::seriesForStudies(const QStringList& studyInstanceUIDs)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  QStringList seriesInstanceUIDs;

  if (!d->DicomDatabase)
  {
    return seriesInstanceUIDs;
  }

  foreach (const QString& studyInstanceUID, studyInstanceUIDs)
  {
    seriesInstanceUIDs << d->DicomDatabase->seriesForStudy(studyInstanceUID);
  }

  return seriesInstanceUIDs;
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onLoadStudies(const QStringList& studyInstanceUIDs)
{
  // Get all series for the studies and load them
  QStringList seriesInstanceUIDs = this->seriesForStudies(studyInstanceUIDs);
  this->onLoadSeries(seriesInstanceUIDs);
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::showMetadataForStudies(const QStringList& studyInstanceUIDs)
{
  // Get all series for the studies and show their metadata
  QStringList seriesInstanceUIDs = this->seriesForStudies(studyInstanceUIDs);
  this->showMetadataForSeries(seriesInstanceUIDs);
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::forceRetrieveStudies(const QStringList& studyInstanceUIDs)
{
  // Get all series for the studies and force retrieve them
  QStringList seriesInstanceUIDs = this->seriesForStudies(studyInstanceUIDs);
  this->forceRetrieveSeries(seriesInstanceUIDs);
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::removeStudies(const QStringList& studyInstanceUIDs)
{
  Q_D(ctkDICOMVisualBrowserWidget);

  // Confirm delete
  if (!this->confirmDeleteSelectedUIDs(studyInstanceUIDs))
  {
    return;
  }

  if (!d->PatientView || !d->DicomDatabase)
  {
    return;
  }

  ctkDICOMStudyListView* studyListView =  d->PatientView->studyListView();
  if (!studyListView)
  {
    return;
  }

  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

  // Stop any running jobs for these studies
  if (d->Scheduler)
  {
    d->Scheduler->stopJobsByDICOMUIDs(QStringList(), studyInstanceUIDs, QStringList());
  }

  // Delete studies from database
  QStringList loadedSeriesInstanceUIDs = d->DicomDatabase->loadedSeriesInstanceUIDs();
  foreach (const QString& studyInstanceUID, studyInstanceUIDs)
  {
    QStringList seriesInstanceUIDs = d->DicomDatabase->seriesForStudy(studyInstanceUID);
    for (const QString& seriesInstanceUID : seriesInstanceUIDs)
    {
      loadedSeriesInstanceUIDs.removeAll(seriesInstanceUID);
    }
    d->DicomDatabase->removeStudy(studyInstanceUID);
  }
  d->DicomDatabase->setLoadedSeriesInstanceUIDs(loadedSeriesInstanceUIDs);

  ctkDICOMStudyModel* studyModel = studyListView->studyModel();
  ctkDICOMStudyMergedFilterProxyModel* studyMergedFilterModel = studyListView->studyMergedFilterProxyModel();
  if (studyModel)
  {
    studyModel->refresh();
    studyModel->refreshStudies();
  }
  else if (studyMergedFilterModel)
  {
    QList<ctkDICOMStudyModel *> sourceModels = studyMergedFilterModel->sourceModels();
    foreach (ctkDICOMStudyModel* sourceModel, sourceModels)
    {
      sourceModel->refresh();
      sourceModel->refreshStudies();
    }
  }
  QString patientUID = d->PatientView->currentPatientUID();
  d->PatientModel->refreshPatient(patientUID);
  QApplication::restoreOverrideCursor();
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::exportStudies(const QStringList& studyInstanceUIDs)
{
  // Get all series for the studies and export them
  QStringList seriesUIDs = this->seriesForStudies(studyInstanceUIDs);
  this->exportSeries(seriesUIDs);
}

//------------------------------------------------------------------------------
QStringList ctkDICOMVisualBrowserWidget::filesForSeries(const QStringList& seriesInstanceUIDs)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  QStringList fileList;

  if (!d->DicomDatabase)
  {
    return fileList;
  }

  foreach (const QString& seriesUID, seriesInstanceUIDs)
  {
    fileList << d->DicomDatabase->filesForSeries(seriesUID);
  }

  return fileList;
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::onLoadSeries(const QStringList& seriesInstanceUIDs)
{
  Q_D(ctkDICOMVisualBrowserWidget);

  if (!d->Scheduler)
  {
    logger.error("onLoadSeries failed, no scheduler has been set. \n");
    return;
  }

  if (!d->DicomDatabase)
  {
    logger.error("onLoadSeries failed, no DicomDatabase has been set. \n");
    return;
  }

  if (seriesInstanceUIDs.isEmpty())
  {
    return;
  }

  // Update UI - disable delete action and search button during load
  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
  bool deleteActionWasVisible = d->DeleteActionVisible;
  d->DeleteActionVisible = false;

  // Get all series models from study list view to potentially stop non-selected series jobs
  QList<QString> allSeriesUIDs;
  if (d->PatientView)
  {
    ctkDICOMStudyListView* studyListView = d->PatientView->studyListView();
    if (studyListView && studyListView->studyModel())
    {
      ctkDICOMStudyModel* studyModel = studyListView->studyModel();
      QStringList studyInstanceUIDs = studyModel->filteredStudyInstanceUIDs();
      foreach (const QString& studyUID, studyInstanceUIDs)
      {
        ctkDICOMSeriesModel* seriesModel = studyModel->seriesModelForStudyInstanceUID(studyUID);
        if (seriesModel)
        {
          allSeriesUIDs.append(seriesModel->filteredSeriesInstanceUIDs());
        }
      }
    }
    else if (studyListView && studyListView->studyMergedFilterProxyModel())
    {
      ctkDICOMStudyMergedFilterProxyModel* studyMergedFilterModel = studyListView->studyMergedFilterProxyModel();
      QList<ctkDICOMStudyModel *> sourceModels = studyMergedFilterModel->sourceModels();
      foreach (ctkDICOMStudyModel* studyModel, sourceModels)
      {
        QStringList studyInstanceUIDs = studyModel->filteredStudyInstanceUIDs();
        foreach (const QString& studyUID, studyInstanceUIDs)
        {
          ctkDICOMSeriesModel* seriesModel = studyModel->seriesModelForStudyInstanceUID(studyUID);
          if (seriesModel)
          {
            allSeriesUIDs.append(seriesModel->filteredSeriesInstanceUIDs());
          }
        }
      }
    }
  }

  // Stop jobs for non-selected series
  QStringList seriesToStop;
  foreach (const QString& seriesInstanceUID, allSeriesUIDs)
  {
    if (!seriesInstanceUIDs.contains(seriesInstanceUID))
    {
      seriesToStop.append(seriesInstanceUID);
    }
  }

  if (!seriesToStop.isEmpty())
  {
    d->Scheduler->stopJobsByDICOMUIDs(QStringList(), QStringList(), seriesToStop);
  }

  // For cloud series that haven't been retrieved or failed, force retrieve if no jobs running
  QStringList seriesToForceRetrieve;
  foreach (const QString& seriesInstanceUID, seriesInstanceUIDs)
  {
    int numberOfRunningJobsPerSeries = d->Scheduler->getJobsByDICOMUIDs({}, {}, {seriesInstanceUID}).count();
    bool seriesIsCloud = false;

    QString studyInstanceUID = d->DicomDatabase->studyForSeries(seriesInstanceUID);
    QString patientUID = d->DicomDatabase->patientForStudy(studyInstanceUID);
    ctkDICOMStudyModel* studyModel = d->PatientModel->studyModelForPatientUID(patientUID);
    if (studyModel)
    {
      ctkDICOMSeriesModel* seriesModel = studyModel->seriesModelForStudyInstanceUID(studyInstanceUID);
      if (seriesModel)
      {
        seriesIsCloud = seriesModel->isSeriesCloud(seriesInstanceUID);
      }
    }

    if (numberOfRunningJobsPerSeries == 0 && seriesIsCloud)
    {
      seriesToForceRetrieve.append(seriesInstanceUID);
    }
  }

  if (!seriesToForceRetrieve.isEmpty())
  {
    this->forceRetrieveSeries(seriesToForceRetrieve);
  }

  // Create progress dialog
  QProgressDialog loadSeriesProgress(
    ctkDICOMVisualBrowserWidget::tr("Retrieving and processing selected series..."),
    ctkDICOMVisualBrowserWidget::tr("Cancel"), 0, 100, this);
  loadSeriesProgress.setWindowModality(Qt::ApplicationModal);
  loadSeriesProgress.setMinimumDuration(1000);
  QProgressBar* bar = new QProgressBar(&loadSeriesProgress);
  bar->setTextVisible(false);
  loadSeriesProgress.setBar(bar);
  loadSeriesProgress.setValue(0);

  // Wait for selected series to be fully retrieved
  bool wait = true;
  int progress = 0;
  while (wait)
  {
    qApp->processEvents();
    d->Scheduler->waitForDone(300);
    progress++;
    if (progress >= 99)
    {
      progress = 0;
    }
    loadSeriesProgress.setValue(progress);

    if (loadSeriesProgress.wasCanceled())
    {
      break;
    }

    // Check if all selected series have been retrieved
    // In the new architecture, we check if instances exist in database
    wait = false;
    if (d->DicomDatabase)
    {
      foreach (const QString& seriesInstanceUID, seriesInstanceUIDs)
      {
        QString studyInstanceUID = d->DicomDatabase->studyForSeries(seriesInstanceUID);
        QString patientUID = d->DicomDatabase->patientForStudy(studyInstanceUID);
        ctkDICOMStudyModel* studyModel = d->PatientModel->studyModelForPatientUID(patientUID);
        if (studyModel)
        {
          ctkDICOMSeriesModel* seriesModel = studyModel->seriesModelForStudyInstanceUID(studyInstanceUID);
          if (seriesModel)
          {
            if (seriesModel->isSeriesCloud(seriesInstanceUID))
            {
              wait = true;
              break;
            }
          }
        }
      }
    }
  }

  // Restore UI
  d->DeleteActionVisible = deleteActionWasVisible;
  QApplication::restoreOverrideCursor();

  // Finalize
  QStringList retrievedSeriesInstanceUIDs;
  if (loadSeriesProgress.wasCanceled())
  {
    // Stop jobs for selected series
    d->Scheduler->stopJobsByDICOMUIDs(QStringList(), QStringList(), seriesInstanceUIDs);
    d->Scheduler->waitForDone(300);
  }
  else
  {
    loadSeriesProgress.close();

    // Collect successfully retrieved series
    if (d->DicomDatabase)
    {
      foreach (const QString& seriesUID, seriesInstanceUIDs)
      {
        // If series has instances, it was successfully retrieved
        if (d->DicomDatabase->instancesForSeries(seriesUID).count() > 0)
        {
          retrievedSeriesInstanceUIDs.append(seriesUID);
        }
      }
    }

    // Emit signal with retrieved series
    if (!retrievedSeriesInstanceUIDs.isEmpty())
    {
      emit seriesRetrieved(retrievedSeriesInstanceUIDs);
      QStringList loadedSeriesInstanceUIDs = d->DicomDatabase->loadedSeriesInstanceUIDs();
      for (const QString& uid : retrievedSeriesInstanceUIDs)
      {
        if (!loadedSeriesInstanceUIDs.contains(uid))
        {
          loadedSeriesInstanceUIDs.append(uid);
        }
      }

      d->DicomDatabase->setLoadedSeriesInstanceUIDs(loadedSeriesInstanceUIDs);
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::showMetadataForSeries(const QStringList& seriesInstanceUIDs)
{
  this->showMetadata(this->filesForSeries(seriesInstanceUIDs));
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::forceRetrieveSeries(const QStringList& seriesInstanceUIDs)
{
  Q_D(ctkDICOMVisualBrowserWidget);

  if (!d->Scheduler)
  {
    logger.error("forceRetrieveSeries failed, no scheduler has been set. \n");
    return;
  }

  if (!d->DicomDatabase)
  {
    logger.error("forceRetrieveSeries failed, no DicomDatabase has been set. \n");
    return;
  }

  // Stop any running jobs for these series
  d->Scheduler->stopJobsByDICOMUIDs(QStringList(), QStringList(), seriesInstanceUIDs);

  // Delete series from database
  QStringList loadedSeriesInstanceUIDs = d->DicomDatabase->loadedSeriesInstanceUIDs();
  foreach (const QString& seriesInstanceUID, seriesInstanceUIDs)
  {
    loadedSeriesInstanceUIDs.removeAll(seriesInstanceUID);
    d->DicomDatabase->removeSeries(seriesInstanceUID, false, false);
  }
  d->DicomDatabase->setLoadedSeriesInstanceUIDs(loadedSeriesInstanceUIDs);

  // Delete series from database
  foreach (const QString& seriesInstanceUID, seriesInstanceUIDs)
  {
    QString studyInstanceUID = d->DicomDatabase->studyForSeries(seriesInstanceUID);
    QString patientUID = d->DicomDatabase->patientForStudy(studyInstanceUID);
    ctkDICOMStudyModel* studyModel = d->PatientModel->studyModelForPatientUID(patientUID);
    if (studyModel)
    {
      ctkDICOMSeriesModel* seriesModel = studyModel->seriesModelForStudyInstanceUID(studyInstanceUID);
      if (seriesModel)
      {
        seriesModel->forceRetrieveSeries(seriesInstanceUID);
      }
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::removeSeries(const QStringList& seriesInstanceUIDs)
{
  Q_D(ctkDICOMVisualBrowserWidget);

  // Confirm delete
  if (!this->confirmDeleteSelectedUIDs(seriesInstanceUIDs))
  {
    return;
  }

  if (!d->PatientView || !d->Scheduler || !d->DicomDatabase)
  {
    return;
  }

  ctkDICOMStudyListView* studyListView = d->PatientView->studyListView();
  if (!studyListView)
  {
    return;
  }

  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

  // Stop any running jobs for these series
  d->Scheduler->stopJobsByDICOMUIDs(QStringList(), QStringList(), seriesInstanceUIDs);

  // Collect affected study UIDs and series models before deleting series
  QMap<QString, ctkDICOMSeriesModel*> affectedSeriesModels;

  // For each series, get its study UID from the model and cache the series model and view
  foreach (const QString& seriesUID, seriesInstanceUIDs)
  {
    ctkDICOMStudyModel* studyModel = studyListView->studyModel();
    ctkDICOMStudyMergedFilterProxyModel* studyMergedFilterModel = studyListView->studyMergedFilterProxyModel();
    if (studyModel)
    {
      QStringList studyInstanceUIDs = studyModel->filteredStudyInstanceUIDs();
      foreach (const QString& studyInstanceUID, studyInstanceUIDs)
      {
        if (affectedSeriesModels.contains(studyInstanceUID))
        {
          // Already cached
          continue;
        }
        ctkDICOMSeriesModel* seriesModel = studyModel->seriesModelForStudyInstanceUID(studyInstanceUID);
        if (seriesModel)
        {
          QModelIndex seriesIndex = seriesModel->indexForSeriesInstanceUID(seriesUID);
          if (seriesIndex.isValid())
          {
            affectedSeriesModels.insert(studyInstanceUID, seriesModel);
          }
        }
      }
    }
    else if (studyMergedFilterModel)
    {
      QList<ctkDICOMStudyModel *> sourceModels = studyMergedFilterModel->sourceModels();
      foreach (ctkDICOMStudyModel* studyModel, sourceModels)
      {
        QStringList studyInstanceUIDs = studyModel->filteredStudyInstanceUIDs();
        foreach (const QString& studyInstanceUID, studyInstanceUIDs)
        {
          if (affectedSeriesModels.contains(studyInstanceUID))
          {
            // Already cached
            continue;
          }
          ctkDICOMSeriesModel* seriesModel = studyModel->seriesModelForStudyInstanceUID(studyInstanceUID);
          if (seriesModel)
          {
            QModelIndex seriesIndex = seriesModel->indexForSeriesInstanceUID(seriesUID);
            if (seriesIndex.isValid())
            {
              affectedSeriesModels.insert(studyInstanceUID, seriesModel);
            }
          }
        }
      }
    }
  }

  // Delete series from database
  QStringList loadedSeriesInstanceUIDs = d->DicomDatabase->loadedSeriesInstanceUIDs();
  foreach (const QString& seriesInstanceUID, seriesInstanceUIDs)
  {
    loadedSeriesInstanceUIDs.removeAll(seriesInstanceUID);
    d->DicomDatabase->removeSeries(seriesInstanceUID);
  }
  d->DicomDatabase->setLoadedSeriesInstanceUIDs(loadedSeriesInstanceUIDs);

  QApplication::restoreOverrideCursor();

  // Refresh the affected series models and update their views' viewports
  foreach (const QString& studyUID, affectedSeriesModels.keys())
  {
    ctkDICOMSeriesModel* seriesModel = affectedSeriesModels.value(studyUID, nullptr);
    if (seriesModel)
    {
      seriesModel->refresh();
    }

    // Also refresh the series view and force layout recalculation
    ctkDICOMSeriesTableView* seriesView = studyListView->getSeriesViewForStudy(studyUID);
    if (seriesView)
    {
      // reset the model to force a full view update
      QAbstractItemModel* oldModel = seriesView->model();
      seriesView->setModel(nullptr);
      seriesView->setModel(oldModel);
    }
  }

  ctkDICOMStudyModel* studyModel = studyListView->studyModel();
  ctkDICOMStudyMergedFilterProxyModel* studyMergedFilterModel = studyListView->studyMergedFilterProxyModel();
  if (studyModel)
  {
    studyModel->refresh();
    studyModel->refreshStudies();
  }
  else if (studyMergedFilterModel)
  {
    QList<ctkDICOMStudyModel *> sourceModels = studyMergedFilterModel->sourceModels();
    foreach (ctkDICOMStudyModel* sourceModel, sourceModels)
    {
      sourceModel->refresh();
      sourceModel->refreshStudies();
    }
  }
  QString patientUID = d->PatientView->currentPatientUID();
  d->PatientModel->refreshPatient(patientUID);
  QApplication::restoreOverrideCursor();
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::exportSeries(const QStringList& seriesInstanceUIDs)
{
  // Show directory selection dialog
  ctkFileDialog* directoryDialog = new ctkFileDialog(this);
  directoryDialog->setOption(QFileDialog::ShowDirsOnly);
  directoryDialog->setFileMode(QFileDialog::Directory);

  if (directoryDialog->exec())
  {
    QStringList dirs = directoryDialog->selectedFiles();
    if (!dirs.isEmpty())
    {
      this->exportSeriesToDirectory(dirs[0], seriesInstanceUIDs);
    }
  }
  delete directoryDialog;
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::exportSeriesToDirectory(const QString& dirPath, const QStringList& uids)
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
void ctkDICOMVisualBrowserWidget::wheelEvent(QWheelEvent *event)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  if (event->modifiers() & Qt::ControlModifier)
  {

    int numDegrees = event->angleDelta().y() / 120;
    int newSize = static_cast<int>(d->ThumbnailSizePreset) + (numDegrees > 0 ? 1 : -1);
    newSize = std::max(0, std::min(newSize, 3)); // Clamp to None = 0, Small=1, Medium=2, Large=3
    d->updateThumbnailSizeForCurrentStudyView(static_cast<ctkDICOMVisualBrowserWidget::ThumbnailSizePresetOption>(newSize));
    event->accept();
    return;
  }

  Superclass::wheelEvent(event);
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::keyPressEvent(QKeyEvent *event)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  if (event->modifiers() & Qt::ControlModifier)
  {
    switch (event->key())
    {
      case Qt::Key_Plus:
        {
          int newSize = static_cast<int>(d->ThumbnailSizePreset) + 1;
          newSize = std::min(newSize, 3); // Clamp to None = 0, Small=1, Medium=2, Large=3
          d->updateThumbnailSizeForCurrentStudyView(static_cast<ctkDICOMVisualBrowserWidget::ThumbnailSizePresetOption>(newSize));
          event->accept();
          return;
        }
      break;
      case Qt::Key_Minus:
        {
          int newSize = static_cast<int>(d->ThumbnailSizePreset) - 1;
          newSize = std::max(newSize, 0);
          d->updateThumbnailSizeForCurrentStudyView(static_cast<ctkDICOMVisualBrowserWidget::ThumbnailSizePresetOption>(newSize));
          event->accept();
          return;
        }
      break;
    }
  }

  Superclass::keyPressEvent(event);
}

//------------------------------------------------------------------------------
bool ctkDICOMVisualBrowserWidget::eventFilter(QObject* object, QEvent* event)
{
  Q_D(ctkDICOMVisualBrowserWidget);

  // Prevent Enter/Return key from collapsing the SearchPatientsCollapsibleGroupBox in Qt6
  // when focus is on one of the search boxes
  if (object == d->SearchPatientsCollapsibleGroupBox &&
      event->type() == QEvent::KeyPress)
  {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
    if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
    {
      // Check if focus is on one of the search boxes
      QWidget* focusWidget = QApplication::focusWidget();
      if (focusWidget == d->FilteringPatientIDSearchBox ||
          focusWidget == d->FilteringPatientNameSearchBox ||
          focusWidget == d->FilteringStudyDescriptionSearchBox ||
          focusWidget == d->FilteringSeriesDescriptionSearchBox)
      {
        // Filter out the event to prevent the group box from handling it
        return true;
      }
    }
  }

  // Pass the event to the base class
  return Superclass::eventFilter(object, event);
}

//------------------------------------------------------------------------------
void ctkDICOMVisualBrowserWidget::resizeEvent(QResizeEvent* event)
{
  Q_D(ctkDICOMVisualBrowserWidget);
  Superclass::resizeEvent(event);
  d->updateFiltersLayoutOrientation();
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
  for (int index = 0; index < numUIDs; ++index)
  {
    QString uid = uids.at(index);

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

  QAbstractButton* deleteButton = confirmDeleteDialog.addButton(tr("Delete"), QMessageBox::AcceptRole);
  confirmDeleteDialog.addButton(tr("Cancel"), QMessageBox::RejectRole);
  confirmDeleteDialog.setDontShowAgainSettingsKey("VisualDICOMBrowser/DontConfirmDeleteSelected");

  int result = confirmDeleteDialog.exec();

  // Check both the result code and the clicked button for robustness
  // When auto-accepting via "Don't show again", result will be QDialog::Accepted
  if (result == QDialog::Accepted || confirmDeleteDialog.clickedButton() == deleteButton)
  {
    return true;
  }
  else
  {
    return false;
  }
}
