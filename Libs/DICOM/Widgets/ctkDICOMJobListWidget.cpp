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

//Qt includes
#include <QStandardItemModel>
#include <QStyledItemDelegate>

// CTK includes
#include <ctkLogger.h>

// ctkDICOMCore includes
#include <ctkDICOMJob.h>
#include "ctkDICOMJobResponseSet.h"
#include "ctkDICOMScheduler.h"

// ctkDICOMWidgets includes
#include "ctkDICOMJobListWidget.h"
#include "ui_ctkDICOMJobListWidget.h"

static ctkLogger logger("org.commontk.DICOM.Widgets.DICOMJobListWidget");

//----------------------------------------------------------------------------
class ProgressBarDelegate : public QStyledItemDelegate
{
public:
  using QStyledItemDelegate::QStyledItemDelegate;
  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
  {
    QList<QVariant> data = index.data().toList();
    if (data.count() != 2)
    {
      return;
    }

    int progress = ceil(float(data.at(0).toInt()) / data.at(1).toInt() * 100);
    progress = progress < 0 ? 0 : progress;
    progress = progress > 100 ? 100 : progress;

    QStyleOptionProgressBar progressBarOption;
    int width = option.rect.width();
    int height = option.rect.height() - 4;
    int x = option.rect.left();
    int y = option.rect.top() + 2;
    progressBarOption.rect.setRect(x, y, width, height);
    progressBarOption.minimum = 0;
    progressBarOption.maximum = 100;
    progressBarOption.progress = progress;
    progressBarOption.text = QString::number(progress) + "%";
    progressBarOption.textVisible = true;

    if (progress != -1)
    {
      QApplication::style()->drawControl(QStyle::CE_ProgressBar,
                                         &progressBarOption, painter);
    }
  }
};

//----------------------------------------------------------------------------
class QCenteredItemModel : public QStandardItemModel
{
public:
  using QStandardItemModel::QStandardItemModel;

  enum JobStatus{
    Initialized,
    Queued,
    Running,
    UserStopped,
    AttemptFailed,
    Failed,
    Completed
  }; Q_ENUM(JobStatus);

  enum Columns{
    JobType,
    Status,
    Progress,
    CreationDateTime,
    StartDateTime,
    CompletionDateTime,
    DICOMLevel,
    PatientID,
    PatientName,
    PatientBirthDate,
    StudyInstanceUID,
    SeriesInstanceUID,
    SOPInstanceUID,
    Connection,
    JobUID,
    JobClass,
    JobThread,
    JobLogging,
  }; Q_ENUM(Columns);

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  QString getJobTypeAsString(QString jobClass, ctkDICOMJob::DICOMLevels dicomLevel);
  void addJob(const ctkDICOMJobDetail &td, ctkDICOMDatabase* database);
  void updateJobStatus(const ctkDICOMJobDetail &td, const JobStatus &status);
  void updateProgressBar(const ctkDICOMJobDetail &td, ctkDICOMDatabase* database);
  void setProgressBar(int row, const ctkDICOMJobDetail &td, ctkDICOMDatabase* database);
  void clearCompletedJobs();
  static Columns getColumnIndexFromString(QString columnString);
  static QString getColumnStringFromIndex(Columns columnIndex);
};

//----------------------------------------------------------------------------
QVariant QCenteredItemModel::data(const QModelIndex &index, int role) const
{
  if (role == Qt::TextAlignmentRole)
  {
    return static_cast<Qt::Alignment::Int>(Qt::AlignCenter);
  }
  else
  {
    return QStandardItemModel::data(index, role);
  }
}

//----------------------------------------------------------------------------
QString QCenteredItemModel::getJobTypeAsString(QString jobClass, ctkDICOMJob::DICOMLevels dicomLevel)
{
  if (jobClass == "ctkDICOMQueryJob")
  {
    switch (dicomLevel)
    {
      case ctkDICOMJob::DICOMLevels::None:
        logger.warn("ctkDICOMScheduler : DICOMLevels was not set.");
        return "";
      case ctkDICOMJob::DICOMLevels::Patients:
        return ctkDICOMJobListWidget::tr("Query patients");
      case ctkDICOMJob::DICOMLevels::Studies:
        return ctkDICOMJobListWidget::tr("Query studies");
      case ctkDICOMJob::DICOMLevels::Series:
        return ctkDICOMJobListWidget::tr("Query series");
      case ctkDICOMJob::DICOMLevels::Instances:
        return ctkDICOMJobListWidget::tr("Query instances");
    }
  }
  else if (jobClass == "ctkDICOMRetrieveJob")
  {
    switch (dicomLevel)
    {
      case ctkDICOMJob::DICOMLevels::None:
        logger.warn("ctkDICOMScheduler : DICOMLevels was not set.");
        return "";
      case ctkDICOMJob::DICOMLevels::Patients:
        return ctkDICOMJobListWidget::tr("Retrieve patients");
      case ctkDICOMJob::DICOMLevels::Studies:
        return ctkDICOMJobListWidget::tr("Retrieve studies");
      case ctkDICOMJob::DICOMLevels::Series:
        return ctkDICOMJobListWidget::tr("Retrieve series");
      case ctkDICOMJob::DICOMLevels::Instances:
        return ctkDICOMJobListWidget::tr("Retrieve instances");
    }
  }
  else if (jobClass == "ctkDICOMStorageListenerJob")
  {
    return ctkDICOMJobListWidget::tr("Storage listener");
  }
  else if (jobClass == "ctkDICOMEchoJob")
  {
    return ctkDICOMJobListWidget::tr("Echo server");
  }
  else if (jobClass == "ctkDICOMInserterJob")
  {
    return ctkDICOMJobListWidget::tr("Inserter");
  }

  return QString();
}

//----------------------------------------------------------------------------
void QCenteredItemModel::addJob(const ctkDICOMJobDetail &td,
                                ctkDICOMDatabase *database)
{
  if (!database)
  {
    return;
  }

  int row = 0; // add the job to the top
  this->insertRow(row);

  QString jobType = this->getJobTypeAsString(td.JobClass, td.DICOMLevel);
  this->setData(this->index(row, Columns::JobType), jobType);
  this->setData(this->index(row, Columns::JobType), jobType, Qt::ToolTipRole);

  QIcon statusIcon = QIcon(":/Icons/pending.svg");
  QString statusText = ctkDICOMJobListWidget::tr("initialized");
  QStandardItem *statusItem = new QStandardItem(QString("statusItem"));
  statusItem->setIcon(statusIcon);
  this->setItem(row, Columns::Status, statusItem);
  this->setData(this->index(row, Columns::Status), statusText);
  this->setData(this->index(row, Columns::Status), statusText, Qt::ToolTipRole);

  QList<QVariant> data;
  data.append(0);
  data.append(100);
  if (td.JobClass == "ctkDICOMStorageListenerJob")
  {
    data[0] = -1;
  }
  this->setData(this->index(row, Columns::Progress), data);

  this->setData(this->index(row, Columns::CreationDateTime), td.CreationDateTime);
  this->setData(this->index(row, Columns::CreationDateTime), td.CreationDateTime, Qt::ToolTipRole);

  this->setData(this->index(row, Columns::StartDateTime), QString("-"));
  this->setData(this->index(row, Columns::CompletionDateTime), QString("-"));

  QString DICOMLevel;
  if (td.JobClass != "ctkDICOMStorageListenerJob")
  {
    DICOMLevel = QMetaEnum::fromType<ctkDICOMJob::DICOMLevels>().valueToKey(td.DICOMLevel);
  }
  this->setData(this->index(row, Columns::DICOMLevel), DICOMLevel);

  this->setData(this->index(row, Columns::PatientID), td.PatientID);
  this->setData(this->index(row, Columns::PatientID), td.PatientID, Qt::ToolTipRole);

  QStringList patients = database->patients();
  QString patientItem;
  foreach (QString patient, patients)
  {
    QString patientID = database->fieldForPatient("PatientID", patient);
    if (patientID == td.PatientID)
    {
      patientItem = patient;
      break;
    }
  }
  QString patientName = database->fieldForPatient("PatientsName", patientItem);
  patientName.replace(R"(^)", R"( )");
  this->setData(this->index(row, Columns::PatientName), patientName);
  this->setData(this->index(row, Columns::PatientName), patientName, Qt::ToolTipRole);

  QString date = database->fieldForPatient("PatientsBirthDate", patientItem);
  date.replace(QString("-"), QString(""));
  date = QDate::fromString(date, "yyyyMMdd").toString();

  this->setData(this->index(row, Columns::PatientBirthDate), date);
  this->setData(this->index(row, Columns::PatientBirthDate), date, Qt::ToolTipRole);

  this->setData(this->index(row, Columns::StudyInstanceUID), td.StudyInstanceUID);
  this->setData(this->index(row, Columns::SeriesInstanceUID), td.SeriesInstanceUID);
  this->setData(this->index(row, Columns::SOPInstanceUID), td.SOPInstanceUID);

  this->setData(this->index(row, Columns::Connection), td.ConnectionName);
  this->setData(this->index(row, Columns::Connection), td.ConnectionName, Qt::ToolTipRole);

  this->setData(this->index(row, Columns::JobUID), td.JobUID);
  this->setData(this->index(row, Columns::JobClass), td.JobClass);
  this->setData(this->index(row, Columns::JobThread), td.RunningThreadID);
  this->setData(this->index(row, Columns::JobLogging), td.Logging);
}

//----------------------------------------------------------------------------
void QCenteredItemModel::updateJobStatus(const ctkDICOMJobDetail &td, const JobStatus &status)
{
  QList<QStandardItem*> list = this->findItems(td.JobUID, Qt::MatchExactly, Columns::JobUID);
  if (list.empty())
  {
    return;
  }

  int row = list.first()->row();
  QIcon statusIcon;
  QString statusText;
  if (status == Queued)
  {
    statusIcon = QIcon(":/Icons/pending.svg");
    statusText = ctkDICOMJobListWidget::tr("queued");
  }
  else if (status == Running)
  {
    statusIcon = QIcon(":/Icons/pending.svg");
    statusText = ctkDICOMJobListWidget::tr("in-progress");
    if (td.JobClass == "ctkDICOMQueryJob")
    {
      QList<QVariant> data;
      data.append(20);
      data.append(100);
      this->setData(this->index(row, Columns::Progress), data);
    }
  }
  else if (status == Failed)
  {
    statusIcon = QIcon(":/Icons/error.svg");
    statusText = ctkDICOMJobListWidget::tr("failed");
  }
  else if (status == UserStopped)
  {
    statusIcon = QIcon(":/Icons/error.svg");
    statusText = ctkDICOMJobListWidget::tr("user-stopped");
  }
  else if (status == AttemptFailed)
  {
    statusIcon = QIcon(":/Icons/error.svg");
    statusText = ctkDICOMJobListWidget::tr("attempt-failed");
  }
  else if (status == Completed)
  {
    statusIcon = QIcon(":/Icons/accept.svg");
    statusText = ctkDICOMJobListWidget::tr("completed");
    QList<QVariant> data;
    data.append(100);
    data.append(100);
    this->setData(this->index(row, Columns::Progress), data);
  }

  QStandardItem *statusItem = new QStandardItem(QString("statusItem"));
  statusItem->setIcon(statusIcon);
  this->setItem(row, Columns::Status, statusItem);
  this->setData(this->index(row, Columns::Status), statusText);
  this->setData(this->index(row, Columns::Status), statusText, Qt::ToolTipRole);

  if (status == Running)
  {
    this->setData(this->index(row, Columns::StartDateTime), td.CreationDateTime);
    this->setData(this->index(row, Columns::StartDateTime), td.CreationDateTime, Qt::ToolTipRole);
  }
  else
  {
    this->setData(this->index(row, Columns::CompletionDateTime), td.CompletionDateTime);
    this->setData(this->index(row, Columns::CompletionDateTime), td.CompletionDateTime, Qt::ToolTipRole);
  }

  this->setData(this->index(row, Columns::JobThread), td.RunningThreadID);
  this->setData(this->index(row, Columns::JobThread), td.RunningThreadID, Qt::ToolTipRole);
  this->setData(this->index(row, Columns::JobLogging), td.Logging);
}

//----------------------------------------------------------------------------
void QCenteredItemModel::updateProgressBar(const ctkDICOMJobDetail &td, ctkDICOMDatabase *database)
{
  if (td.JobType != ctkDICOMJobResponseSet::JobType::RetrieveSeries &&
      td.JobType != ctkDICOMJobResponseSet::JobType::StoreSOPInstance)
  {
    return;
  }

  QList<QStandardItem*> itemList = this->findItems(td.JobUID, Qt::MatchExactly, Columns::JobUID);
  if (itemList.empty())
  {
    return;
  }

  int row = itemList.first()->row();
  QString jobClass = this->index(row, QCenteredItemModel::Columns::JobClass).data().toString();
  if (jobClass == "ctkDICOMStorageListenerJob")
  {
    itemList = QList<QStandardItem*>();
    if (!td.SeriesInstanceUID.isEmpty())
    {
      itemList = this->findItems(td.SeriesInstanceUID, Qt::MatchExactly, Columns::SeriesInstanceUID);
    }

    foreach (QStandardItem* item, itemList)
    {
      int row = item->row();
      this->setProgressBar(row, td, database);
    }
  }
  else
  {
    this->setProgressBar(row, td, database);
  }
}

//----------------------------------------------------------------------------
void QCenteredItemModel::setProgressBar(int row, const ctkDICOMJobDetail &td, ctkDICOMDatabase *database)
{
  if (!database)
  {
    return;
  }

  QString seriesInstanceUID = this->index(row, QCenteredItemModel::Columns::SeriesInstanceUID).data().toString();
  QString studyInstanceUID = this->index(row, QCenteredItemModel::Columns::StudyInstanceUID).data().toString();

  if (seriesInstanceUID != td.SeriesInstanceUID ||
      studyInstanceUID != td.StudyInstanceUID)
  {
    return;
  }

  QList<QVariant> data = this->index(row, QCenteredItemModel::Columns::Progress).data().toList();
  if (data.count() != 2)
  {
    return;
  }

  int progress = data.at(0).toInt();
  progress += 1;
  data[0] = progress;
  int numberOfInstances = data.at(1).toInt();
  numberOfInstances = database->instancesForSeries(td.SeriesInstanceUID).count();
  data[1] = numberOfInstances;
  this->setData(this->index(row, Columns::Progress), data);
}

//----------------------------------------------------------------------------
void QCenteredItemModel::clearCompletedJobs()
{
#if (QT_VERSION >= QT_VERSION_CHECK(5,15,0))
  QList<QStandardItem*> attemptFailedList = this->findItems(ctkDICOMJobListWidget::tr("attempt-failed"), Qt::MatchRegularExpression, Columns::Status);
  QList<QStandardItem*> completedList = this->findItems(ctkDICOMJobListWidget::tr("completed"), Qt::MatchRegularExpression, Columns::Status);
#else
  QList<QStandardItem*> attemptFailedList = this->findItems(ctkDICOMJobListWidget::tr("attempt-failed"), Qt::MatchRegExp, Columns::Status);
  QList<QStandardItem*> completedList = this->findItems(ctkDICOMJobListWidget::tr("completed"), Qt::MatchRegExp, Columns::Status);
#endif

  completedList.append(attemptFailedList);
  foreach (QStandardItem* item, completedList)
  {
    this->removeRow(item->row());
  }
}

//----------------------------------------------------------------------------
QCenteredItemModel::Columns QCenteredItemModel::getColumnIndexFromString(QString columnString)
{
  if (columnString == ctkDICOMJobListWidget::tr("Type"))
  {
    return Columns::JobType;
  }
  else if (columnString == ctkDICOMJobListWidget::tr("Status"))
  {
    return Columns::Status;
  }
  else if (columnString == ctkDICOMJobListWidget::tr("Progress"))
  {
    return Columns::Progress;
  }
  else if (columnString == ctkDICOMJobListWidget::tr("Time and Date"))
  {
    return Columns::CreationDateTime;
  }
  else if (columnString == ctkDICOMJobListWidget::tr("Starting Time and Date"))
  {
    return Columns::StartDateTime;
  }
  else if (columnString == ctkDICOMJobListWidget::tr("Completion Time and Date"))
  {
    return Columns::CompletionDateTime;
  }
  else if (columnString == ctkDICOMJobListWidget::tr("DICOM Level"))
  {
    return Columns::DICOMLevel;
  }
  else if (columnString == ctkDICOMJobListWidget::tr("Patient ID"))
  {
    return Columns::PatientID;
  }
  else if (columnString == ctkDICOMJobListWidget::tr("Patient Name"))
  {
    return Columns::PatientName;
  }
  else if (columnString == ctkDICOMJobListWidget::tr("Birth Date"))
  {
    return Columns::PatientBirthDate;
  }
  else if (columnString == ctkDICOMJobListWidget::tr("Study UID"))
  {
    return Columns::StudyInstanceUID;
  }
  else if (columnString == ctkDICOMJobListWidget::tr("Series UID"))
  {
    return Columns::SeriesInstanceUID;
  }
  else if (columnString == ctkDICOMJobListWidget::tr("SOP UID"))
  {
    return Columns::SOPInstanceUID;
  }
  else if (columnString == ctkDICOMJobListWidget::tr("Connection"))
  {
    return Columns::Connection;
  }
  else if (columnString == ctkDICOMJobListWidget::tr("Job UID"))
  {
    return Columns::JobUID;
  }
  else if (columnString == ctkDICOMJobListWidget::tr("Class"))
  {
    return Columns::JobClass;
  }
  else if (columnString == ctkDICOMJobListWidget::tr("Thread"))
  {
    return Columns::JobThread;
  }
  else if (columnString == ctkDICOMJobListWidget::tr("Logging"))
  {
    return Columns::JobLogging;
  }
  else
  {
    return Columns::JobClass;
  }
}

//----------------------------------------------------------------------------
QString QCenteredItemModel::getColumnStringFromIndex(Columns columnIndex)
{
  switch (columnIndex)
  {
    case Columns::JobType:
      return ctkDICOMJobListWidget::tr("Type");
    case Columns::Status:
      return ctkDICOMJobListWidget::tr("Status");
    case Columns::Progress:
      return ctkDICOMJobListWidget::tr("Progress");
    case Columns::CreationDateTime:
      return ctkDICOMJobListWidget::tr("Time and Date");
    case Columns::StartDateTime:
      return ctkDICOMJobListWidget::tr("Starting Time and Date");
    case Columns::CompletionDateTime:
      return ctkDICOMJobListWidget::tr("Completion Time and Date");
    case Columns::DICOMLevel:
      return ctkDICOMJobListWidget::tr("DICOM Level");
    case Columns::PatientID:
      return ctkDICOMJobListWidget::tr("Patient ID");
    case Columns::PatientName:
      return ctkDICOMJobListWidget::tr("Patient Name");
    case Columns::PatientBirthDate:
      return ctkDICOMJobListWidget::tr("Birth Date");
    case Columns::StudyInstanceUID:
      return ctkDICOMJobListWidget::tr("Study UID");
    case Columns::SeriesInstanceUID:
      return ctkDICOMJobListWidget::tr("Series UID");
    case Columns::SOPInstanceUID:
      return ctkDICOMJobListWidget::tr("SOP UID");
    case Columns::Connection:
      return ctkDICOMJobListWidget::tr("Connection");
    case Columns::JobUID:
      return ctkDICOMJobListWidget::tr("Job UID");
    case Columns::JobClass:
      return ctkDICOMJobListWidget::tr("Class");
    case Columns::JobThread:
      return ctkDICOMJobListWidget::tr("Thread");
    case Columns::JobLogging:
      return ctkDICOMJobListWidget::tr("Logging");
    default:
      return QString();
  }
}

//----------------------------------------------------------------------------
class ctkDICOMJobListWidgetPrivate: public Ui_ctkDICOMJobListWidget
{
  Q_DECLARE_PUBLIC( ctkDICOMJobListWidget );

protected:
  ctkDICOMJobListWidget* const q_ptr;

public:
  ctkDICOMJobListWidgetPrivate(ctkDICOMJobListWidget& obj);
  ~ctkDICOMJobListWidgetPrivate();

  void init();
  void disconnectScheduler();
  void connectScheduler();
  void setFilterKeyColumn(QString);
  void updateJobsDetailsWidget();
  void retryJobs();

  QSharedPointer<ctkDICOMScheduler> Scheduler;
  QSharedPointer<QSortFilterProxyModel> proxyModel;
  QSharedPointer<QSortFilterProxyModel> showCompletedProxyModel;
  QSharedPointer<QCenteredItemModel> dataModel;
};

//----------------------------------------------------------------------------
// ctkDICOMJobListWidgetPrivate methods

//----------------------------------------------------------------------------
ctkDICOMJobListWidgetPrivate::ctkDICOMJobListWidgetPrivate(ctkDICOMJobListWidget& obj)
  : q_ptr(&obj)
{
  this->Scheduler = nullptr;
  this->proxyModel = nullptr;
  this->showCompletedProxyModel = nullptr;
  this->dataModel = nullptr;
}

//----------------------------------------------------------------------------
ctkDICOMJobListWidgetPrivate::~ctkDICOMJobListWidgetPrivate()
{
}

//----------------------------------------------------------------------------
void ctkDICOMJobListWidgetPrivate::init()
{
  Q_Q(ctkDICOMJobListWidget);
  this->setupUi(q);

  QStringList filteringColumnNames;
  filteringColumnNames.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::JobType));
  filteringColumnNames.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::Status));
  filteringColumnNames.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::CreationDateTime));
  filteringColumnNames.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::PatientID));
  filteringColumnNames.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::PatientName));
  filteringColumnNames.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::PatientBirthDate));
  filteringColumnNames.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::Connection));

  this->FilterColumnComboBox->addItems(filteringColumnNames);

  QStringList allColumnNames;
  allColumnNames.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::JobType));
  allColumnNames.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::Status));
  allColumnNames.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::Progress));
  allColumnNames.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::CreationDateTime));
  allColumnNames.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::StartDateTime));
  allColumnNames.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::CompletionDateTime));
  allColumnNames.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::DICOMLevel));
  allColumnNames.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::PatientID));
  allColumnNames.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::PatientName));
  allColumnNames.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::PatientBirthDate));
  allColumnNames.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::StudyInstanceUID));
  allColumnNames.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::SeriesInstanceUID));
  allColumnNames.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::SOPInstanceUID));
  allColumnNames.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::Connection));
  allColumnNames.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::JobUID));
  allColumnNames.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::JobClass));
  allColumnNames.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::JobThread));
  allColumnNames.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::JobLogging));

  this->dataModel = QSharedPointer<QCenteredItemModel>(new QCenteredItemModel(0, allColumnNames.count(), q));
  this->dataModel->setHorizontalHeaderLabels(allColumnNames);

  this->proxyModel = QSharedPointer<QSortFilterProxyModel>(new QSortFilterProxyModel);
  this->proxyModel->setSourceModel(this->dataModel.data());

  this->showCompletedProxyModel = QSharedPointer<QSortFilterProxyModel>(new QSortFilterProxyModel);
  this->showCompletedProxyModel->setSourceModel(this->proxyModel.data());
  this->showCompletedProxyModel->setFilterKeyColumn(QCenteredItemModel::Columns::Status);

  this->JobsView->setAlternatingRowColors(false);
  this->JobsView->setModel(this->showCompletedProxyModel.data());
  this->JobsView->setItemDelegateForColumn(QCenteredItemModel::Columns::Progress, new ProgressBarDelegate(q));

  this->JobsView->setColumnHidden(QCenteredItemModel::Columns::StartDateTime, true);
  this->JobsView->setColumnHidden(QCenteredItemModel::Columns::CompletionDateTime, true);
  this->JobsView->setColumnHidden(QCenteredItemModel::Columns::DICOMLevel, true);
  this->JobsView->setColumnHidden(QCenteredItemModel::Columns::StudyInstanceUID, true);
  this->JobsView->setColumnHidden(QCenteredItemModel::Columns::SeriesInstanceUID, true);
  this->JobsView->setColumnHidden(QCenteredItemModel::Columns::SOPInstanceUID, true);
  this->JobsView->setColumnHidden(QCenteredItemModel::Columns::JobUID, true);
  this->JobsView->setColumnHidden(QCenteredItemModel::Columns::JobClass, true);
  this->JobsView->setColumnHidden(QCenteredItemModel::Columns::JobThread, true);
  this->JobsView->setColumnHidden(QCenteredItemModel::Columns::JobLogging, true);

  QObject::connect(this->JobsView->selectionModel(), &QItemSelectionModel::selectionChanged,
                   q, &ctkDICOMJobListWidget::onJobsViewSelectionChanged);

  QObject::connect(this->FilterLineEdit, SIGNAL(textChanged(QString)),
                   q, SLOT(onFilterTextChanged(QString)));
  QObject::connect(this->FilterColumnComboBox, SIGNAL(currentTextChanged(QString)),
                   q, SLOT(onFilterColumnChanged(QString)));

  this->StopButton->setEnabled(false);
  this->RetryButton->setEnabled(false);

  QObject::connect(this->SelectAllPushButton, SIGNAL(clicked()),
                   q, SLOT(onSelectAllButtonClicked()));
  QObject::connect(this->StopButton, SIGNAL(clicked()),
                   q, SLOT(onStopButtonClicked()));
  QObject::connect(this->RetryButton, SIGNAL(clicked()),
                   q, SLOT(onRetryButtonClicked()));
  QObject::connect(this->ResetFiltersPushButton, SIGNAL(clicked()),
                   q, SLOT(onResetFiltersButtonClicked()));
  QObject::connect(this->ShowCompletedButton, SIGNAL(toggled(bool)),
                   q, SLOT(onShowCompletedButtonToggled(bool)));
  q->onShowCompletedButtonToggled(false);
  QObject::connect(this->ClearCompletedCompletedButton, SIGNAL(clicked()),
                   q, SLOT(onClearCompletedButtonClicked()));
  QObject::connect(this->ClearAllPushButton, SIGNAL(clicked()),
                   q, SLOT(onClearAllButtonClicked()));

  this->FiltersCollapsibleGroupBox->setCollapsed(true);
  this->DetailsCollapsibleGroupBox->setCollapsed(true);
}

//----------------------------------------------------------------------------
void ctkDICOMJobListWidgetPrivate::disconnectScheduler()
{
  Q_Q(ctkDICOMJobListWidget);
  if (!this->Scheduler)
  {
    return;
  }

  ctkDICOMJobListWidget::disconnect(this->Scheduler.data(), SIGNAL(jobInitialized(QVariant)),
                                    q, SLOT(onJobInitialized(QVariant)));
  ctkDICOMJobListWidget::disconnect(this->Scheduler.data(), SIGNAL(jobQueued(QVariant)),
                                    q, SLOT(onJobQueued(QVariant)));
  ctkDICOMJobListWidget::disconnect(this->Scheduler.data(), SIGNAL(jobStarted(QList<QVariant>)),
                                    q, SLOT(onJobStarted(QList<QVariant>)));
  ctkDICOMJobListWidget::disconnect(this->Scheduler.data(), SIGNAL(jobUserStopped(QList<QVariant>)),
                                    q, SLOT(onJobUserStopped(QList<QVariant>)));
  ctkDICOMJobListWidget::disconnect(this->Scheduler.data(), SIGNAL(jobFinished(QList<QVariant>)),
                                    q, SLOT(onJobFinished(QList<QVariant>)));
  ctkDICOMJobListWidget::disconnect(this->Scheduler.data(), SIGNAL(jobAttemptFailed(QList<QVariant>)),
                                    q, SLOT(onJobAttemptFailed(QList<QVariant>)));
  ctkDICOMJobListWidget::disconnect(this->Scheduler.data(), SIGNAL(jobFailed(QList<QVariant>)),
                                    q, SLOT(onJobFailed(QList<QVariant>)));
  ctkDICOMJobListWidget::disconnect(this->Scheduler.data(), SIGNAL(progressJobDetail(QList<QVariant>)),
                                    q, SLOT(onProgressJobDetail(QList<QVariant>)));
}

//----------------------------------------------------------------------------
void ctkDICOMJobListWidgetPrivate::connectScheduler()
{
  Q_Q(ctkDICOMJobListWidget);
  if (!this->Scheduler)
  {
    return;
  }

  ctkDICOMJobListWidget::connect(this->Scheduler.data(), SIGNAL(jobInitialized(QVariant)),
                                 q, SLOT(onJobInitialized(QVariant)));
  ctkDICOMJobListWidget::connect(this->Scheduler.data(), SIGNAL(jobQueued(QVariant)),
                                 q, SLOT(onJobQueued(QVariant)));
  ctkDICOMJobListWidget::connect(this->Scheduler.data(), SIGNAL(jobStarted(QList<QVariant>)),
                                 q, SLOT(onJobStarted(QList<QVariant>)));
  ctkDICOMJobListWidget::connect(this->Scheduler.data(), SIGNAL(jobUserStopped(QList<QVariant>)),
                                 q, SLOT(onJobUserStopped(QList<QVariant>)));
  ctkDICOMJobListWidget::connect(this->Scheduler.data(), SIGNAL(jobFinished(QList<QVariant>)),
                                 q, SLOT(onJobFinished(QList<QVariant>)));
  ctkDICOMJobListWidget::connect(this->Scheduler.data(), SIGNAL(jobAttemptFailed(QList<QVariant>)),
                                 q, SLOT(onJobAttemptFailed(QList<QVariant>)));
  ctkDICOMJobListWidget::connect(this->Scheduler.data(), SIGNAL(jobFailed(QList<QVariant>)),
                                 q, SLOT(onJobFailed(QList<QVariant>)));
  ctkDICOMJobListWidget::connect(this->Scheduler.data(), SIGNAL(progressJobDetail(QList<QVariant>)),
                                 q, SLOT(onProgressJobDetail(QList<QVariant>)));
}

//----------------------------------------------------------------------------
void ctkDICOMJobListWidgetPrivate::setFilterKeyColumn(QString text)
{
  int columnIndex = QCenteredItemModel::getColumnIndexFromString(text);
  this->proxyModel->setFilterKeyColumn(columnIndex);
}

//----------------------------------------------------------------------------
void ctkDICOMJobListWidgetPrivate::updateJobsDetailsWidget()
{
  Q_Q(ctkDICOMJobListWidget);

  this->DetailsTextBrowser->clear();

  QItemSelectionModel *select = this->JobsView->selectionModel();
  if (!select->hasSelection())
  {
    return;
  }

  QString detailsText;
  QModelIndexList selectedRows = select->selectedRows();
  int count = 0;
  foreach (QModelIndex rowIndex, selectedRows)
  {
    if (count != 0)
    {
      detailsText.append(QString("\n =============================================================== \n"));
    }

    int row = rowIndex.row();
    QString jobType = this->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::JobType).data().toString();
    QString status = this->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::Status).data().toString();
    QString creationDateTime = this->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::CreationDateTime).data().toString();
    QString startDateTime = this->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::StartDateTime).data().toString();
    QString completionDateTime = this->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::CompletionDateTime).data().toString();
    QString dicomLevel = this->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::DICOMLevel).data().toString();
    QString patientID = this->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::PatientID).data().toString();
    QString patientName = this->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::PatientName).data().toString();
    QString patientBirthDate = this->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::PatientBirthDate).data().toString();
    QString studyInstanceUID = this->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::StudyInstanceUID).data().toString();
    QString seriesInstanceUID = this->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::SeriesInstanceUID).data().toString();
    QString sopInstanceUID = this->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::SOPInstanceUID).data().toString();
    QString connection = this->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::Connection).data().toString();
    QString jobUID = this->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::JobUID).data().toString();
    QString jobClass = this->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::JobClass).data().toString();
    QString thread = this->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::JobThread).data().toString();
    QString logging = this->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::JobLogging).data().toString();

    if (!jobType.isEmpty())
    {
      detailsText.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::JobType));
      detailsText.append(QString(": ") + jobType + QString("\n"));
    }
    if (!jobUID.isEmpty())
    {
      detailsText.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::JobUID));
      detailsText.append(QString(": ") + jobUID + QString("\n"));
    }
    if (!jobClass.isEmpty())
    {
      detailsText.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::JobClass));
      detailsText.append(QString(": ") + jobClass + QString("\n"));
    }
    if (!connection.isEmpty())
    {
      detailsText.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::Connection));
      detailsText.append(QString(": ") + connection + QString("\n"));
    }
    if (!status.isEmpty())
    {
      detailsText.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::Status));
      detailsText.append(QString(": ") + status + QString("\n"));
    }
    if (!creationDateTime.isEmpty())
    {
      detailsText.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::CreationDateTime));
      detailsText.append(QString(": ") + creationDateTime + QString("\n"));
    }
    if (!startDateTime.isEmpty())
    {
      detailsText.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::StartDateTime));
      detailsText.append(QString(": ") + startDateTime + QString("\n"));
    }
    if (!completionDateTime.isEmpty())
    {
      detailsText.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::CompletionDateTime));
      detailsText.append(QString(": ") + completionDateTime + QString("\n"));
    }
    if (!dicomLevel.isEmpty() && dicomLevel != "None")
    {
      detailsText.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::DICOMLevel));
      detailsText.append(QString(": ") + dicomLevel + QString("\n"));
    }
    if (!patientID.isEmpty())
    {
      detailsText.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::PatientID));
      detailsText.append(QString(": ") + patientID + QString("\n"));
    }
    if (!patientName.isEmpty())
    {
      detailsText.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::PatientName));
      detailsText.append(QString(": ") + patientName + QString("\n"));
    }
    if (!patientBirthDate.isEmpty())
    {
      detailsText.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::PatientBirthDate));
      detailsText.append(QString(": ") + patientBirthDate + QString("\n"));
    }
    if (!studyInstanceUID.isEmpty())
    {
      detailsText.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::StudyInstanceUID));
      detailsText.append(QString(": ") + studyInstanceUID + QString("\n"));
    }
    if (!seriesInstanceUID.isEmpty())
    {
      detailsText.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::SeriesInstanceUID));
      detailsText.append(QString(": ") + seriesInstanceUID + QString("\n"));
    }
    if (!sopInstanceUID.isEmpty())
    {
      detailsText.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::SOPInstanceUID));
      detailsText.append(QString(": ") + sopInstanceUID + QString("\n"));
    }
    if (!thread.isEmpty())
    {
      detailsText.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::JobThread));
      detailsText.append(QString(": ") + thread + QString("\n"));
    }
    if (!logging.isEmpty())
    {
      detailsText.append(QCenteredItemModel::getColumnStringFromIndex(QCenteredItemModel::Columns::JobLogging));
      detailsText.append(QString(":\n") + logging + QString("\n"));
    }

    if (count == 0)
    {
      q->patientSelected(patientID, patientName, patientBirthDate);
    }
    count++;
  }

  this->DetailsTextBrowser->setPlainText(detailsText);
}

//----------------------------------------------------------------------------
void ctkDICOMJobListWidgetPrivate::retryJobs()
{
  QMap<QString, ctkDICOMJobDetail> jobsUIDsToRetry;
  QItemSelectionModel *select = this->JobsView->selectionModel();
  QModelIndexList selectedRows = select->selectedRows();
  foreach (QModelIndex rowIndex, selectedRows)
  {
    int row = rowIndex.row();
    QString status = this->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::Status).data().toString();
    QString jobClass = this->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::JobClass).data().toString();
    QString jobUID = this->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::JobUID).data().toString();

    if (status != ctkDICOMJobListWidget::tr("failed") && status != ctkDICOMJobListWidget::tr("user-stopped"))
    {
      continue;
    }

    if (jobClass != "ctkDICOMQueryJob" &&  jobClass != "ctkDICOMRetrieveJob")
    {
      continue;
    }

    ctkDICOMJobDetail jobDetail;
    jobDetail.JobClass = this->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::JobClass).data().toString();

    QString DICOMLevelString = this->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::DICOMLevel).data().toString();
    ctkDICOMJob::DICOMLevels DICOMLevel = ctkDICOMJob::DICOMLevels::None;
    if (DICOMLevelString == "Patients")
    {
      DICOMLevel = ctkDICOMJob::DICOMLevels::Patients;
    }
    else if (DICOMLevelString == "Studies")
    {
      DICOMLevel = ctkDICOMJob::DICOMLevels::Studies;
    }
    else if (DICOMLevelString == "Series")
    {
      DICOMLevel = ctkDICOMJob::DICOMLevels::Series;
    }
    else if (DICOMLevelString == "Instances")
    {
      DICOMLevel = ctkDICOMJob::DICOMLevels::Instances;
    }

    jobDetail.DICOMLevel = DICOMLevel;
    jobDetail.PatientID = this->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::PatientID).data().toString();
    jobDetail.StudyInstanceUID = this->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::StudyInstanceUID).data().toString();
    jobDetail.SeriesInstanceUID = this->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::SeriesInstanceUID).data().toString();
    jobDetail.SOPInstanceUID = this->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::SOPInstanceUID).data().toString();
    jobDetail.ConnectionName = this->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::Connection).data().toString();
    jobsUIDsToRetry.insert(jobUID, jobDetail);
  }

  // remove duplicate jobs (e.g., in the selected list there is multiple
  // entries of the same job canceled/failed, we don't want running multiple
  // times the same jobs)
  QMap<QString, ctkDICOMJobDetail> filteredJobsUIDsToRetry;
  foreach (QString jobUID, jobsUIDsToRetry.keys())
  {
    ctkDICOMJobDetail td = jobsUIDsToRetry.value(jobUID);
    bool duplicate = false;
    foreach (QString filteredJobUID, filteredJobsUIDsToRetry.keys())
    {
      if (filteredJobUID == jobUID)
      {
        continue;
      }

      ctkDICOMJobDetail filteredTD = jobsUIDsToRetry.value(filteredJobUID);
      if (td.JobClass == filteredTD.JobClass &&
          td.DICOMLevel == filteredTD.DICOMLevel &&
          td.PatientID == filteredTD.PatientID &&
          td.StudyInstanceUID == filteredTD.StudyInstanceUID &&
          td.SeriesInstanceUID == filteredTD.SeriesInstanceUID &&
          td.SOPInstanceUID == filteredTD.SOPInstanceUID &&
          td.ConnectionName == filteredTD.ConnectionName)
      {
        duplicate = true;
        break;
      }
    }

    if (duplicate)
    {
      continue;
    }
    else
    {
      filteredJobsUIDsToRetry.insert(jobUID, td);
    }
  }

  this->Scheduler->runJobs(filteredJobsUIDsToRetry);
  this->JobsView->clearSelection();
}

//----------------------------------------------------------------------------
// ctkDICOMJobListWidget methods

//----------------------------------------------------------------------------
ctkDICOMJobListWidget::ctkDICOMJobListWidget(QWidget* parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new ctkDICOMJobListWidgetPrivate(*this))
{
  Q_D(ctkDICOMJobListWidget);
  d->init();
}

//----------------------------------------------------------------------------
ctkDICOMJobListWidget::~ctkDICOMJobListWidget()
{
}

//----------------------------------------------------------------------------
static void skipDelete(QObject* obj)
{
  Q_UNUSED(obj);
  // this deleter does not delete the object from memory
  // useful if the pointer is not owned by the smart pointer
}

//----------------------------------------------------------------------------
ctkDICOMScheduler* ctkDICOMJobListWidget::scheduler()const
{
  Q_D(const ctkDICOMJobListWidget);
  return d->Scheduler.data();
}

//----------------------------------------------------------------------------
QSharedPointer<ctkDICOMScheduler> ctkDICOMJobListWidget::schedulerShared()const
{
  Q_D(const ctkDICOMJobListWidget);
  return d->Scheduler;
}

//----------------------------------------------------------------------------
void ctkDICOMJobListWidget::setScheduler(ctkDICOMScheduler& scheduler)
{
  Q_D(ctkDICOMJobListWidget);
  d->disconnectScheduler();
  d->Scheduler = QSharedPointer<ctkDICOMScheduler>(&scheduler, skipDelete);
  d->connectScheduler();
}

//----------------------------------------------------------------------------
void ctkDICOMJobListWidget::setScheduler(QSharedPointer<ctkDICOMScheduler> scheduler)
{
  Q_D(ctkDICOMJobListWidget);
  d->disconnectScheduler();
  d->Scheduler = scheduler;
  d->connectScheduler();
}

//----------------------------------------------------------------------------
void ctkDICOMJobListWidget::onJobInitialized(QVariant data)
{
  Q_D(ctkDICOMJobListWidget);
  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();

  if(td.JobClass.isEmpty())
  {
    return;
  }

  d->dataModel->addJob(td, d->Scheduler->dicomDatabase());
}

//----------------------------------------------------------------------------
void ctkDICOMJobListWidget::onJobQueued(QVariant data)
{
  Q_D(ctkDICOMJobListWidget);
  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();

  if(td.JobClass.isEmpty())
  {
    return;
  }

  d->dataModel->updateJobStatus(td, QCenteredItemModel::Queued);
}

//----------------------------------------------------------------------------
void ctkDICOMJobListWidget::onJobStarted(QList<QVariant> datas)
{
  Q_D(ctkDICOMJobListWidget);
  foreach (QVariant data, datas)
  {
    ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();

    if(td.JobClass.isEmpty())
    {
      continue;
    }

    d->dataModel->updateJobStatus(td, QCenteredItemModel::Running);
  }
}

//----------------------------------------------------------------------------
void ctkDICOMJobListWidget::onJobFinished(QList<QVariant> datas)
{
  Q_D(ctkDICOMJobListWidget);
  foreach (QVariant data, datas)
  {
    ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();

    if(td.JobClass.isEmpty())
    {
      continue;
    }

    d->dataModel->updateJobStatus(td, QCenteredItemModel::Completed);
  }
}

//----------------------------------------------------------------------------
void ctkDICOMJobListWidget::onProgressJobDetail(QList<QVariant> datas)
{
  Q_D(ctkDICOMJobListWidget);
  foreach (QVariant data, datas)
  {
    ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();

    if(td.JobType == ctkDICOMJobResponseSet::JobType::None)
    {
      continue;
    }

    d->dataModel->updateProgressBar(td, d->Scheduler->dicomDatabase());
  }
}

//----------------------------------------------------------------------------
void ctkDICOMJobListWidget::onJobAttemptFailed(QList<QVariant> datas)
{
  Q_D(ctkDICOMJobListWidget);
  foreach (QVariant data, datas)
  {
    ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();

    if(td.JobClass.isEmpty())
    {
      continue;
    }

    d->dataModel->updateJobStatus(td, QCenteredItemModel::AttemptFailed);
  }
}

//----------------------------------------------------------------------------
void ctkDICOMJobListWidget::onJobFailed(QList<QVariant> datas)
{
  Q_D(ctkDICOMJobListWidget);
  foreach (QVariant data, datas)
  {
    ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();

    if(td.JobClass.isEmpty())
    {
      continue;
    }

    d->dataModel->updateJobStatus(td, QCenteredItemModel::Failed);
  }
}

//----------------------------------------------------------------------------
void ctkDICOMJobListWidget::onJobUserStopped(QList<QVariant> datas)
{
  Q_D(ctkDICOMJobListWidget);
  foreach (QVariant data, datas)
  {
    ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();

    if(td.JobClass.isEmpty())
    {
      continue;
    }

    d->dataModel->updateJobStatus(td, QCenteredItemModel::UserStopped);
  }
}

//----------------------------------------------------------------------------
void ctkDICOMJobListWidget::onFilterTextChanged(QString text)
{
  Q_D(ctkDICOMJobListWidget);
  d->proxyModel->setFilterRegExp(text);
}

//----------------------------------------------------------------------------
void ctkDICOMJobListWidget::onFilterColumnChanged(QString text)
{
  Q_D(ctkDICOMJobListWidget);
  d->setFilterKeyColumn(text);
}

//----------------------------------------------------------------------------
void ctkDICOMJobListWidget::onJobsViewSelectionChanged()
{
  Q_D(ctkDICOMJobListWidget);
  QItemSelectionModel *select = d->JobsView->selectionModel();
  QModelIndexList selectedRows = select->selectedRows();

  bool failedJobSelected = false;
  foreach (QModelIndex rowIndex, selectedRows)
  {
    int row = rowIndex.row();
    QString status = d->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::Status).data().toString();
    QString jobClass = d->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::JobClass).data().toString();

    if ((status == tr("failed") || status == tr("user-stopped")) &&
        (jobClass == "ctkDICOMQueryJob" || jobClass == "ctkDICOMRetrieveJob"))
    {
      failedJobSelected = true;
      break;
    }
  }
  d->RetryButton->setEnabled(failedJobSelected);

  bool inProgressJobSelected = false;
  foreach (QModelIndex rowIndex, selectedRows)
  {
    int row = rowIndex.row();
    QString status = d->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::Status).data().toString();
    QString jobUID = d->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::JobUID).data().toString();
    QString jobClass = d->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::JobClass).data().toString();

    if ((status == tr("in-progress") || status == tr("queued") || status == tr("initialized")) &&
        jobClass != "ctkDICOMStorageListenerJob")
    {
      inProgressJobSelected = true;
      break;
    }
  }
  d->StopButton->setEnabled(inProgressJobSelected);

  d->updateJobsDetailsWidget();
}

//----------------------------------------------------------------------------
void ctkDICOMJobListWidget::onSelectAllButtonClicked()
{
  Q_D(ctkDICOMJobListWidget);
  d->JobsView->selectAll();
}

//----------------------------------------------------------------------------
void ctkDICOMJobListWidget::onStopButtonClicked()
{
  Q_D(ctkDICOMJobListWidget);
  QStringList jobsUIDsToStop;
  QItemSelectionModel *select = d->JobsView->selectionModel();
  QModelIndexList selectedRows = select->selectedRows();
  foreach (QModelIndex rowIndex, selectedRows)
  {
    int row = rowIndex.row();
    QString status = d->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::Status).data().toString();
    QString jobUID = d->showCompletedProxyModel->index
      (row, QCenteredItemModel::Columns::JobUID).data().toString();
    if (status == tr("in-progress") || status == tr("queued") || status == tr("initialized"))
    {
      jobsUIDsToStop.append(jobUID);
    }
  }

  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
  d->Scheduler->stopJobsByJobUIDs(jobsUIDsToStop);
  QApplication::restoreOverrideCursor();
  d->JobsView->clearSelection();
}

//----------------------------------------------------------------------------
void ctkDICOMJobListWidget::onRetryButtonClicked()
{
  Q_D(ctkDICOMJobListWidget);
  d->retryJobs();
}

//----------------------------------------------------------------------------
void ctkDICOMJobListWidget::onResetFiltersButtonClicked()
{
  Q_D(ctkDICOMJobListWidget);
  d->ShowCompletedButton->setChecked(false);
  d->FilterLineEdit->setText("");
  d->FilterColumnComboBox->setCurrentIndex(0);
}

//----------------------------------------------------------------------------
void ctkDICOMJobListWidget::onShowCompletedButtonToggled(bool toggled)
{
  Q_D(ctkDICOMJobListWidget);
  QString filterText;
  if (!toggled)
  {
    filterText = QString("^%1$|^%2$|^%3$|^%4$|^%5$")
      .arg(tr("initialized"))
      .arg(tr("queued"))
      .arg(tr("in-progress"))
      .arg(tr("user-stopped"))
      .arg(tr("failed"));
  }
  d->showCompletedProxyModel->setFilterRegExp(filterText);
}

//----------------------------------------------------------------------------
void ctkDICOMJobListWidget::onClearCompletedButtonClicked()
{
  Q_D(ctkDICOMJobListWidget);
  d->dataModel->clearCompletedJobs();
}

//----------------------------------------------------------------------------
void ctkDICOMJobListWidget::onClearAllButtonClicked()
{
  Q_D(ctkDICOMJobListWidget);
  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
  d->Scheduler->stopAllJobs();
  QApplication::restoreOverrideCursor();
  d->dataModel->removeRows(0, d->dataModel->rowCount());
}
