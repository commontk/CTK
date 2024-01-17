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
#include <QDebug>
#include <QTableWidget>

// CTK includes
#include <ctkLogger.h>

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMScheduler.h"
#include "ctkDICOMJobResponseSet.h"

// ctkDICOMWidgets includes
#include "ctkDICOMSeriesItemWidget.h"
#include "ctkDICOMPatientItemWidget.h"
#include "ui_ctkDICOMPatientItemWidget.h"

static ctkLogger logger("org.commontk.DICOM.Widgets.ctkDICOMPatientItemWidget");

//----------------------------------------------------------------------------
static void skipDelete(QObject* obj)
{
  Q_UNUSED(obj);
  // this deleter does not delete the object from memory
  // useful if the pointer is not owned by the smart pointer
}

//----------------------------------------------------------------------------
class ctkDICOMPatientItemWidgetPrivate: public Ui_ctkDICOMPatientItemWidget
{
  Q_DECLARE_PUBLIC( ctkDICOMPatientItemWidget );

protected:
  ctkDICOMPatientItemWidget* const q_ptr;

public:
  ctkDICOMPatientItemWidgetPrivate(ctkDICOMPatientItemWidget& obj);
  ~ctkDICOMPatientItemWidgetPrivate();

  void init(QWidget* parentWidget);

  QString getPatientItemFromPatientID(const QString& patientID);
  QString formatDate(const QString&);
  bool isStudyItemAlreadyAdded(const QString& studyItem);
  void clearLayout(QLayout* layout, bool deleteWidgets = true);
  void createStudies();

  QSharedPointer<ctkDICOMDatabase> DicomDatabase;
  QSharedPointer<ctkDICOMScheduler> Scheduler;
  QSharedPointer<QWidget> VisualDICOMBrowser;

  int NumberOfStudiesPerPatient;
  ctkDICOMStudyItemWidget::ThumbnailSizeOption ThumbnailSize;

  QString PatientItem;
  QString PatientID;

  QString FilteringStudyDescription;
  ctkDICOMPatientItemWidget::DateType FilteringDate;

  QString FilteringSeriesDescription;
  QStringList FilteringModalities;

  QList<ctkDICOMStudyItemWidget*> StudyItemWidgetsList;
};

//----------------------------------------------------------------------------
// ctkDICOMPatientItemWidgetPrivate methods

//----------------------------------------------------------------------------
ctkDICOMPatientItemWidgetPrivate::ctkDICOMPatientItemWidgetPrivate(ctkDICOMPatientItemWidget& obj)
  : q_ptr(&obj)
{
  this->FilteringDate = ctkDICOMPatientItemWidget::DateType::Any;
  this->NumberOfStudiesPerPatient = 2;
  this->ThumbnailSize = ctkDICOMStudyItemWidget::ThumbnailSizeOption::Medium;
  this->PatientItem = "";
  this->PatientID = "";
  this->FilteringStudyDescription = "";
  this->FilteringSeriesDescription = "";

  this->DicomDatabase = nullptr;
  this->Scheduler = nullptr;
  this->VisualDICOMBrowser = nullptr;
}

//----------------------------------------------------------------------------
ctkDICOMPatientItemWidgetPrivate::~ctkDICOMPatientItemWidgetPrivate()
{
  QLayout *StudiesListWidgetLayout = this->StudiesListWidget->layout();
  this->clearLayout(StudiesListWidgetLayout);
}

//----------------------------------------------------------------------------
void ctkDICOMPatientItemWidgetPrivate::init(QWidget* parentWidget)
{
  Q_Q(ctkDICOMPatientItemWidget);
  this->setupUi(q);

  this->VisualDICOMBrowser = QSharedPointer<QWidget>(parentWidget, skipDelete);

  this->PatientNameValueLabel->setWordWrap(true);
  this->PatientIDValueLabel->setWordWrap(true);
  this->PatientBirthDateValueLabel->setWordWrap(true);
  this->PatientSexValueLabel->setWordWrap(true);
}

//----------------------------------------------------------------------------
QString ctkDICOMPatientItemWidgetPrivate::getPatientItemFromPatientID(const QString& patientID)
{
  if (!this->DicomDatabase)
    {
    return "";
    }

  QStringList patientList = this->DicomDatabase->patients();
  foreach (QString patientItem, patientList)
    {
    QString patientItemID = this->DicomDatabase->fieldForPatient("PatientID", patientItem);

    if (patientID == patientItemID)
      {
      return patientItem;
      }
    }

  return "";
}

//----------------------------------------------------------------------------
QString ctkDICOMPatientItemWidgetPrivate::formatDate(const QString& date)
{
  QString formattedDate = date;
  formattedDate.replace(QString("-"), QString(""));
  return QDate::fromString(formattedDate, "yyyyMMdd").toString();
}

//----------------------------------------------------------------------------
bool ctkDICOMPatientItemWidgetPrivate::isStudyItemAlreadyAdded(const QString &studyItem)
{
  bool alreadyAdded = false;
  foreach (ctkDICOMStudyItemWidget* studyItemWidget, this->StudyItemWidgetsList)
    {
    if (!studyItemWidget)
      {
      continue;
      }

    if (studyItemWidget->studyItem() == studyItem)
      {
      alreadyAdded = true;
      break;
      }
    }

  return alreadyAdded;
}

//----------------------------------------------------------------------------
void ctkDICOMPatientItemWidgetPrivate::clearLayout(QLayout *layout, bool deleteWidgets)
{
  Q_Q(ctkDICOMPatientItemWidget);

  if (!layout)
    {
    return;
    }

  this->StudyItemWidgetsList.clear();
  foreach (ctkDICOMStudyItemWidget* studyItemWidget, this->StudyItemWidgetsList)
    {
    if (!studyItemWidget)
      {
      continue;
      }

    q->disconnect(studyItemWidget, SIGNAL(customContextMenuRequested(const QPoint&)),
                  this->VisualDICOMBrowser.data(), SLOT(showStudyContextMenu(const QPoint&)));

    }

  while (QLayoutItem* item = layout->takeAt(0))
    {
    if (deleteWidgets)
      {
      if (QWidget* widget = item->widget())
        {
        widget->deleteLater();
        }
      }

    if (QLayout* childLayout = item->layout())
      {
      clearLayout(childLayout, deleteWidgets);
      }

    delete item;
    }
}

//----------------------------------------------------------------------------
void ctkDICOMPatientItemWidgetPrivate::createStudies()
{
  Q_Q(ctkDICOMPatientItemWidget);

  if (!this->DicomDatabase)
    {
    logger.error("createStudies failed, no DICOM Database has been set. \n");
    return;
    }

  QLayout *studiesListWidgetLayout = this->StudiesListWidget->layout();
  if (this->PatientItem.isEmpty())
    {
    this->PatientNameValueLabel->setText("");
    this->PatientIDValueLabel->setText("");
    this->PatientSexValueLabel->setText("");
    this->PatientBirthDateValueLabel->setText("");
    return;
    }
  else
    {
    QString patientName = this->DicomDatabase->fieldForPatient("PatientsName", this->PatientItem);
    patientName.replace(R"(^)", R"( )");
    this->PatientNameValueLabel->setText(patientName);
    this->PatientIDValueLabel->setText(this->DicomDatabase->fieldForPatient("PatientID", this->PatientItem));
    this->PatientSexValueLabel->setText(this->DicomDatabase->fieldForPatient("PatientsSex", this->PatientItem));
    this->PatientBirthDateValueLabel->setText(this->formatDate(this->DicomDatabase->fieldForPatient("PatientsBirthDate", this->PatientItem)));
    }

  QStringList studiesList = this->DicomDatabase->studiesForPatient(this->PatientItem);

  if (studiesList.count() == 0)
    {
    return;
    }

  // Filter with studyDescription and studyDate and sort by Date
  QMap<long long, QString> studiesMap;
  foreach (QString studyItem, studiesList)
    {
    if (this->isStudyItemAlreadyAdded(studyItem))
      {
      continue;
      }

    QString studyInstanceUID = this->DicomDatabase->fieldForStudy("StudyInstanceUID", studyItem);
    if (studyInstanceUID.isEmpty())
      {
      continue;
      }

    QString studyDateString = this->DicomDatabase->fieldForStudy("StudyDate", studyItem);
    studyDateString.replace(QString("-"), QString(""));
    QString studyDescription = this->DicomDatabase->fieldForStudy("StudyDescription", studyItem);

    if (studyDateString.isEmpty())
      {
      studyDateString = this->DicomDatabase->fieldForPatient("PatientsBirthDate", this->PatientItem);
      if (studyDateString.isEmpty())
        {
        studyDateString = "19000101";
        }
      }

    if ((!this->FilteringStudyDescription.isEmpty() &&
         !studyDescription.contains(this->FilteringStudyDescription, Qt::CaseInsensitive)))
      {
      continue;
      }

    int nDays = ctkDICOMPatientItemWidget::getNDaysFromFilteringDate(this->FilteringDate);
    QDate studyDate = QDate::fromString(studyDateString, "yyyyMMdd");
    if (nDays != -1)
      {
      QDate endDate = QDate::currentDate();
      QDate startDate = endDate.addDays(-nDays);
      if (studyDate < startDate || studyDate > endDate)
        {
        continue;
        }
      }
    long long date = studyDate.toJulianDay();
    while (studiesMap.contains(date))
      {
      date++;
      }
    // QMap automatically sort in ascending with the key,
    // but we want descending (latest study should be in the first row)
    long long key = LLONG_MAX - date;
    studiesMap[key] = studyItem;
    }

  foreach (QString studyItem, studiesMap)
    {
    q->addStudyItemWidget(studyItem);
    }

  QSpacerItem* verticalSpacer = new QSpacerItem(0, 5, QSizePolicy::Fixed, QSizePolicy::Expanding);
  studiesListWidgetLayout->addItem(verticalSpacer);
}

//----------------------------------------------------------------------------
// ctkDICOMPatientItemWidget methods

//----------------------------------------------------------------------------
ctkDICOMPatientItemWidget::ctkDICOMPatientItemWidget(QWidget* parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new ctkDICOMPatientItemWidgetPrivate(*this))
{
  Q_D(ctkDICOMPatientItemWidget);
  d->init(parentWidget);
}

//----------------------------------------------------------------------------
ctkDICOMPatientItemWidget::~ctkDICOMPatientItemWidget()
{
}

//------------------------------------------------------------------------------
void ctkDICOMPatientItemWidget::setPatientItem(const QString &patientItem)
{
  Q_D(ctkDICOMPatientItemWidget);
  d->PatientItem = patientItem;
}

//------------------------------------------------------------------------------
QString ctkDICOMPatientItemWidget::patientItem() const
{
  Q_D(const ctkDICOMPatientItemWidget);
  return d->PatientItem;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientItemWidget::setPatientID(const QString &patientID)
{
  Q_D(ctkDICOMPatientItemWidget);
  d->PatientID = patientID;
}

//------------------------------------------------------------------------------
QString ctkDICOMPatientItemWidget::patientID() const
{
  Q_D(const ctkDICOMPatientItemWidget);
  return d->PatientID;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientItemWidget::setFilteringStudyDescription(const QString& filteringStudyDescription)
{
  Q_D(ctkDICOMPatientItemWidget);
  d->FilteringStudyDescription = filteringStudyDescription;
}

//------------------------------------------------------------------------------
QString ctkDICOMPatientItemWidget::filteringStudyDescription() const
{
  Q_D(const ctkDICOMPatientItemWidget);
  return d->FilteringStudyDescription;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientItemWidget::setFilteringDate(const ctkDICOMPatientItemWidget::DateType &filteringDate)
{
  Q_D(ctkDICOMPatientItemWidget);
  d->FilteringDate = filteringDate;
}

//------------------------------------------------------------------------------
ctkDICOMPatientItemWidget::DateType ctkDICOMPatientItemWidget::filteringDate() const
{
  Q_D(const ctkDICOMPatientItemWidget);
  return d->FilteringDate;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientItemWidget::setFilteringSeriesDescription(const QString& filteringSeriesDescription)
{
  Q_D(ctkDICOMPatientItemWidget);
  d->FilteringSeriesDescription = filteringSeriesDescription;
}

//------------------------------------------------------------------------------
QString ctkDICOMPatientItemWidget::filteringSeriesDescription() const
{
  Q_D(const ctkDICOMPatientItemWidget);
  return d->FilteringSeriesDescription;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientItemWidget::setFilteringModalities(const QStringList &filteringModalities)
{
  Q_D(ctkDICOMPatientItemWidget);
  d->FilteringModalities = filteringModalities;
}

//------------------------------------------------------------------------------
QStringList ctkDICOMPatientItemWidget::filteringModalities() const
{
  Q_D(const ctkDICOMPatientItemWidget);
  return d->FilteringModalities;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientItemWidget::setNumberOfStudiesPerPatient(int numberOfStudiesPerPatient)
{
  Q_D(ctkDICOMPatientItemWidget);
  d->NumberOfStudiesPerPatient = numberOfStudiesPerPatient;
}

//------------------------------------------------------------------------------
int ctkDICOMPatientItemWidget::numberOfStudiesPerPatient() const
{
  Q_D(const ctkDICOMPatientItemWidget);
  return d->NumberOfStudiesPerPatient;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientItemWidget::setThumbnailSize(const ctkDICOMStudyItemWidget::ThumbnailSizeOption &thumbnailSize)
{
  Q_D(ctkDICOMPatientItemWidget);
  d->ThumbnailSize = thumbnailSize;
}

//------------------------------------------------------------------------------
ctkDICOMStudyItemWidget::ThumbnailSizeOption ctkDICOMPatientItemWidget::thumbnailSize() const
{
  Q_D(const ctkDICOMPatientItemWidget);
  return d->ThumbnailSize;
}

//----------------------------------------------------------------------------
ctkDICOMScheduler* ctkDICOMPatientItemWidget::scheduler()const
{
  Q_D(const ctkDICOMPatientItemWidget);
  return d->Scheduler.data();
}

//----------------------------------------------------------------------------
QSharedPointer<ctkDICOMScheduler> ctkDICOMPatientItemWidget::schedulerShared()const
{
  Q_D(const ctkDICOMPatientItemWidget);
  return d->Scheduler;
}

//----------------------------------------------------------------------------
void ctkDICOMPatientItemWidget::setScheduler(ctkDICOMScheduler& scheduler)
{
  Q_D(ctkDICOMPatientItemWidget);
  if (d->Scheduler)
    {
    QObject::disconnect(d->Scheduler.data(), SIGNAL(progressJobDetail(QVariant)),
                        this, SLOT(updateGUIFromScheduler(QVariant)));
    }

  d->Scheduler = QSharedPointer<ctkDICOMScheduler>(&scheduler, skipDelete);

  if (d->Scheduler)
    {
    QObject::connect(d->Scheduler.data(), SIGNAL(progressJobDetail(QVariant)),
                     this, SLOT(updateGUIFromScheduler(QVariant)));
    }
}

//----------------------------------------------------------------------------
void ctkDICOMPatientItemWidget::setScheduler(QSharedPointer<ctkDICOMScheduler> scheduler)
{
  Q_D(ctkDICOMPatientItemWidget);
  if (d->Scheduler)
    {
    QObject::disconnect(d->Scheduler.data(), SIGNAL(progressJobDetail(QVariant)),
                        this, SLOT(updateGUIFromScheduler(QVariant)));
    }

  d->Scheduler = scheduler;

  if (d->Scheduler)
    {
    QObject::connect(d->Scheduler.data(), SIGNAL(progressJobDetail(QVariant)),
                     this, SLOT(updateGUIFromScheduler(QVariant)));
    }
}

//----------------------------------------------------------------------------
ctkDICOMDatabase* ctkDICOMPatientItemWidget::dicomDatabase()const
{
  Q_D(const ctkDICOMPatientItemWidget);
  return d->DicomDatabase.data();
}

//----------------------------------------------------------------------------
QSharedPointer<ctkDICOMDatabase> ctkDICOMPatientItemWidget::dicomDatabaseShared()const
{
  Q_D(const ctkDICOMPatientItemWidget);
  return d->DicomDatabase;
}

//----------------------------------------------------------------------------
void ctkDICOMPatientItemWidget::setDicomDatabase(ctkDICOMDatabase& dicomDatabase)
{
  Q_D(ctkDICOMPatientItemWidget);
  d->DicomDatabase = QSharedPointer<ctkDICOMDatabase>(&dicomDatabase, skipDelete);
}

//----------------------------------------------------------------------------
void ctkDICOMPatientItemWidget::setDicomDatabase(QSharedPointer<ctkDICOMDatabase> dicomDatabase)
{
  Q_D(ctkDICOMPatientItemWidget);
  d->DicomDatabase = dicomDatabase;
}

//------------------------------------------------------------------------------
QList<ctkDICOMStudyItemWidget *> ctkDICOMPatientItemWidget::studyItemWidgetsList() const
{
  Q_D(const ctkDICOMPatientItemWidget);
  return d->StudyItemWidgetsList;
}

//------------------------------------------------------------------------------
int ctkDICOMPatientItemWidget::getNDaysFromFilteringDate(DateType FilteringDate)
{
  int nDays = 0;
  switch (FilteringDate)
    {
    case ctkDICOMPatientItemWidget::DateType::Any:
      nDays = -1;
      break;
    case ctkDICOMPatientItemWidget::DateType::Today:
      nDays = 0;
      break;
    case ctkDICOMPatientItemWidget::DateType::Yesterday:
      nDays = 1;
      break;
    case ctkDICOMPatientItemWidget::DateType::LastWeek:
      nDays = 7;
      break;
    case ctkDICOMPatientItemWidget::DateType::LastMonth:
      nDays = 30;
      break;
    case ctkDICOMPatientItemWidget::DateType::LastYear:
      nDays = 365;
      break;
    }

  return nDays;
}

//----------------------------------------------------------------------------
void ctkDICOMPatientItemWidget::addStudyItemWidget(const QString &studyItem)
{
  Q_D(ctkDICOMPatientItemWidget);

  if (!d->DicomDatabase)
    {
    logger.error("addStudyItemWidget failed, no DICOM Database has been set. \n");
    return;
    }

  QString studyInstanceUID = d->DicomDatabase->fieldForStudy("StudyInstanceUID", studyItem);
  QString studyID = d->DicomDatabase->fieldForStudy("StudyID", studyItem);
  QString studyDate = d->DicomDatabase->fieldForStudy("StudyDate", studyItem);
  QString formattedStudyDate = d->formatDate(studyDate);
  QString studyDescription = d->DicomDatabase->fieldForStudy("StudyDescription", studyItem);

  ctkDICOMStudyItemWidget* studyItemWidget = new ctkDICOMStudyItemWidget(d->VisualDICOMBrowser.data());
  studyItemWidget->setStudyItem(studyItem);
  studyItemWidget->setPatientID(d->PatientID);
  studyItemWidget->setStudyInstanceUID(studyInstanceUID);
  if (formattedStudyDate.isEmpty() && studyID.isEmpty())
    {
    studyItemWidget->setTitle(tr("Study"));
    }
  else if (formattedStudyDate.isEmpty())
    {
    studyItemWidget->setTitle(tr("Study ID %1").arg(studyID));
    }
  else if (studyID.isEmpty())
    {
    studyItemWidget->setTitle(tr("Study --- %1").arg(formattedStudyDate));
    }
  else
    {
    studyItemWidget->setTitle(tr("Study ID  %1  ---  %2").arg(studyID).arg(formattedStudyDate));
    }

  studyItemWidget->setDescription(studyDescription);
  studyItemWidget->setThumbnailSize(d->ThumbnailSize);
  studyItemWidget->setFilteringSeriesDescription(d->FilteringSeriesDescription);
  studyItemWidget->setFilteringModalities(d->FilteringModalities);
  studyItemWidget->setDicomDatabase(d->DicomDatabase);
  studyItemWidget->setScheduler(d->Scheduler);
  // Show in default (and start query/retrieve) only for the first 2 studies
  // NOTE: in the layout for each studyItemWidget there is a QSpacerItem
  if (d->StudiesListWidget->layout()->count() < d->NumberOfStudiesPerPatient * 2)
    {
    studyItemWidget->generateSeries();
    }
  else
    {
    studyItemWidget->setCollapsed(true);
    this->connect(studyItemWidget->collapsibleGroupBox(), SIGNAL(toggled(bool)),
                  studyItemWidget, SLOT(generateSeries(bool)));
    }
  studyItemWidget->setContextMenuPolicy(Qt::CustomContextMenu);

  this->connect(studyItemWidget->seriesListTableWidget(), SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
                d->VisualDICOMBrowser.data(), SLOT(onLoad()));
  this->connect(studyItemWidget, SIGNAL(customContextMenuRequested(const QPoint&)),
                d->VisualDICOMBrowser.data(), SLOT(showStudyContextMenu(const QPoint&)));
  this->connect(studyItemWidget->seriesListTableWidget(), SIGNAL(itemClicked(QTableWidgetItem *)),
                this, SLOT(onSeriesItemClicked()));
  this->connect(studyItemWidget->seriesListTableWidget(), SIGNAL(itemSelectionChanged()),
                this, SLOT(raiseSelectedSeriesJobsPriority()));

  d->StudiesListWidget->layout()->addWidget(studyItemWidget);
  d->StudyItemWidgetsList.append(studyItemWidget);
}

//----------------------------------------------------------------------------
void ctkDICOMPatientItemWidget::removeStudyItemWidget(const QString &studyItem)
{
  Q_D(ctkDICOMPatientItemWidget);

  for (int studyIndex = 0; studyIndex < d->StudyItemWidgetsList.size(); ++studyIndex)
    {
    ctkDICOMStudyItemWidget *studyItemWidget =
      qobject_cast<ctkDICOMStudyItemWidget*>(d->StudyItemWidgetsList[studyIndex]);
    if (!studyItemWidget || studyItemWidget->studyItem() != studyItem)
      {
      continue;
      }

    this->disconnect(studyItemWidget, SIGNAL(customContextMenuRequested(const QPoint&)),
                     d->VisualDICOMBrowser.data(), SLOT(showStudyContextMenu(const QPoint&)));
    this->disconnect(studyItemWidget->seriesListTableWidget(), SIGNAL(itemClicked(QTableWidgetItem *)),
                     this, SLOT(onSeriesItemClicked()));
    this->disconnect(studyItemWidget->seriesListTableWidget(), SIGNAL(itemSelectionChanged()),
                     this, SLOT(raiseSelectedSeriesJobsPriority()));
    d->StudyItemWidgetsList.removeOne(studyItemWidget);
    delete studyItemWidget;
    break;
  }
}

//------------------------------------------------------------------------------
void ctkDICOMPatientItemWidget::setSelection(bool selected)
{
  Q_D(ctkDICOMPatientItemWidget);
  foreach (ctkDICOMStudyItemWidget* studyItemWidget, d->StudyItemWidgetsList)
    {
    studyItemWidget->setSelection(selected);
    }
}

//------------------------------------------------------------------------------
void ctkDICOMPatientItemWidget::generateStudies()
{
  Q_D(ctkDICOMPatientItemWidget);

  d->createStudies();
  if (d->Scheduler && d->Scheduler->getNumberOfQueryRetrieveServers() > 0)
    {
    d->Scheduler->queryStudies(d->PatientID, QThread::NormalPriority);
    }
}

//------------------------------------------------------------------------------
void ctkDICOMPatientItemWidget::updateGUIFromScheduler(QVariant data)
{
  Q_D(ctkDICOMPatientItemWidget);

  ctkJobDetail td = data.value<ctkJobDetail>();
  if (td.JobUID.isEmpty())
    {
    d->createStudies();
    }

  if (td.JobUID.isEmpty() ||
      td.JobType != ctkDICOMJobResponseSet::JobType::QueryStudies ||
      td.PatientID != d->PatientID)
    {
    return;
    }

  d->createStudies();
}

//------------------------------------------------------------------------------
void ctkDICOMPatientItemWidget::raiseSelectedSeriesJobsPriority()
{
  Q_D(ctkDICOMPatientItemWidget);

  if (!d->Scheduler || d->Scheduler->getNumberOfQueryRetrieveServers() == 0)
    {
    logger.error("raiseSelectedSeriesJobsPriority failed, no task pool has been set. \n");
    return;
    }

  QList<ctkDICOMSeriesItemWidget *> seriesWidgets;
  QList<ctkDICOMSeriesItemWidget *> selectedSeriesWidgets;
  foreach (ctkDICOMStudyItemWidget *studyItemWidget, d->StudyItemWidgetsList)
    {
    if (!studyItemWidget)
      {
      continue;
      }

    QTableWidget *seriesListTableWidget = studyItemWidget->seriesListTableWidget();
     for (int row = 0; row < seriesListTableWidget->rowCount(); row++)
      {
      for (int column = 0; column < seriesListTableWidget->columnCount(); column++)
        {
        ctkDICOMSeriesItemWidget* seriesItemWidget =
          qobject_cast<ctkDICOMSeriesItemWidget*>(seriesListTableWidget->cellWidget(row, column));
        seriesWidgets.append(seriesItemWidget);
        }
      }

    QList<QTableWidgetItem*> selectedItems = seriesListTableWidget->selectedItems();
    foreach (QTableWidgetItem *selectedItem, selectedItems)
      {
      if (!selectedItem)
        {
        continue;
        }

      int row = selectedItem->row();
      int column = selectedItem->column();
      ctkDICOMSeriesItemWidget* seriesItemWidget =
        qobject_cast<ctkDICOMSeriesItemWidget*>(seriesListTableWidget->cellWidget(row, column));

      selectedSeriesWidgets.append(seriesItemWidget);
      }
    }

  QStringList selectedSeriesInstanceUIDs;
  foreach (ctkDICOMSeriesItemWidget* seriesWidget, seriesWidgets)
    {
    if (!seriesWidget)
      {
      continue;
      }

    bool widgetIsSelected = selectedSeriesWidgets.contains(seriesWidget);
    if (widgetIsSelected)
      {
      selectedSeriesInstanceUIDs.append(seriesWidget->seriesInstanceUID());
      }

    seriesWidget->setRaiseJobsPriority(widgetIsSelected);
    }

  d->Scheduler->raiseJobsPriorityForSeries(selectedSeriesInstanceUIDs);
}

//------------------------------------------------------------------------------
void ctkDICOMPatientItemWidget::onSeriesItemClicked()
{
  Q_D(ctkDICOMPatientItemWidget);

  QTableWidget* seriesTable = qobject_cast<QTableWidget*>(sender());
  if (!seriesTable)
    {
    return;
    }

  if (QApplication::keyboardModifiers() & (Qt::ControlModifier | Qt::ShiftModifier))
    {
    return;
    }

  if (seriesTable->selectedItems().count() != 1)
    {
    return;
    }

  foreach (ctkDICOMStudyItemWidget *studyItemWidget, d->StudyItemWidgetsList)
    {
    if (!studyItemWidget)
      {
      continue;
      }

    QTableWidget *studySeriesTable = studyItemWidget->seriesListTableWidget();
    if (studySeriesTable == seriesTable)
      {
      continue;
      }

    studySeriesTable->clearSelection();
    }
}
