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
#include <QDebug>
#include <QLabel>
#include <QScreen>
#include <QTableWidgetItem>

// CTK includes
#include <ctkLogger.h>

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMJob.h"
#include "ctkDICOMJobResponseSet.h"
#include "ctkDICOMScheduler.h"

// ctkDICOMWidgets includes
#include "ctkDICOMStudyItemWidget.h"
#include "ui_ctkDICOMStudyItemWidget.h"

#include <math.h>

static ctkLogger logger("org.commontk.DICOM.Widgets.DICOMStudyItemWidget");

//----------------------------------------------------------------------------
static void skipDelete(QObject* obj)
{
  Q_UNUSED(obj);
  // this deleter does not delete the object from memory
  // useful if the pointer is not owned by the smart pointer
}

//----------------------------------------------------------------------------
class ctkDICOMStudyItemWidgetPrivate : public Ui_ctkDICOMStudyItemWidget
{
  Q_DECLARE_PUBLIC(ctkDICOMStudyItemWidget);

protected:
  ctkDICOMStudyItemWidget* const q_ptr;

public:
  ctkDICOMStudyItemWidgetPrivate(ctkDICOMStudyItemWidget& obj);
  ~ctkDICOMStudyItemWidgetPrivate();

  void init(QWidget* parent);
  void updateColumnsWidths();
  void createSeries();
  int getScreenWidth();
  int getScreenHeight();
  int calculateNumerOfSeriesPerRow();
  int calculateThumbnailSizeInPixel(const ctkDICOMStudyItemWidget::ThumbnailSizeOption& thumbnailSize);
  void addEmptySeriesItemWidget(int rowIndex, int columnIndex);
  ctkDICOMSeriesItemWidget* isSeriesItemAlreadyAdded(const QString& seriesItem);

  QString FilteringSeriesDescription;
  QStringList FilteringModalities;

  QSharedPointer<ctkDICOMDatabase> DicomDatabase;
  QSharedPointer<ctkDICOMScheduler> Scheduler;
  QSharedPointer<QWidget> VisualDICOMBrowser;

  ctkDICOMStudyItemWidget::ThumbnailSizeOption ThumbnailSize;
  int ThumbnailSizePixel;
  QString PatientID;
  QString StudyInstanceUID;
  QString StudyItem;

  QStringList AllowedServers;
  ctkDICOMStudyItemWidget::OperationStatus Status;
  QString StoppedJobUID;

  bool IsGUIUpdating;
  bool QueryOn;
  bool RetrieveOn;
  int FilteredSeriesCount;
};

//----------------------------------------------------------------------------
// ctkDICOMStudyItemWidgetPrivate methods

//----------------------------------------------------------------------------
ctkDICOMStudyItemWidgetPrivate::ctkDICOMStudyItemWidgetPrivate(ctkDICOMStudyItemWidget& obj)
  : q_ptr(&obj)
{
  this->ThumbnailSize = ctkDICOMStudyItemWidget::ThumbnailSizeOption::Medium;
  this->ThumbnailSizePixel = 200;

  this->AllowedServers = QStringList();
  this->Status = ctkDICOMStudyItemWidget::NoOperation;

  this->DicomDatabase = nullptr;
  this->Scheduler = nullptr;
  this->VisualDICOMBrowser = nullptr;

  this->IsGUIUpdating = false;
  this->QueryOn = true;
  this->RetrieveOn = true;
  this->FilteredSeriesCount = 0;
}

//----------------------------------------------------------------------------
ctkDICOMStudyItemWidgetPrivate::~ctkDICOMStudyItemWidgetPrivate()
{
  for (int row = 0; row < this->SeriesListTableWidget->rowCount(); row++)
  {
    for (int column = 0; column < this->SeriesListTableWidget->columnCount(); column++)
    {
      ctkDICOMSeriesItemWidget* seriesItemWidget =
        qobject_cast<ctkDICOMSeriesItemWidget*>(this->SeriesListTableWidget->cellWidget(row, column));
      if (!seriesItemWidget)
      {
        continue;
      }

      QObject::disconnect(seriesItemWidget, SIGNAL(customContextMenuRequested(const QPoint&)),
                          this->VisualDICOMBrowser.data(), SLOT(showSeriesContextMenu(const QPoint&)));
    }
  }
}

//----------------------------------------------------------------------------
void ctkDICOMStudyItemWidgetPrivate::init(QWidget* parent)
{
  Q_Q(ctkDICOMStudyItemWidget);
  this->setupUi(q);

  this->VisualDICOMBrowser = QSharedPointer<QWidget>(parent, skipDelete);
  this->StudyItemCollapsibleGroupBox->setCollapsed(false);
  this->OperationStatusPushButton->hide();

  QObject::connect(this->StudySelectionCheckBox, SIGNAL(clicked(bool)),
                   q, SLOT(onStudySelectionClicked(bool)));
  QObject::connect(this->OperationStatusPushButton, SIGNAL(clicked(bool)),
                   q, SLOT(onOperationStatusButtonClicked(bool)));
}

//------------------------------------------------------------------------------
void ctkDICOMStudyItemWidgetPrivate::updateColumnsWidths()
{
  for (int columnIndex = 0; columnIndex < this->SeriesListTableWidget->columnCount(); ++columnIndex)
  {
    this->SeriesListTableWidget->setColumnWidth(columnIndex, this->ThumbnailSizePixel);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMStudyItemWidgetPrivate::createSeries()
{
  Q_Q(ctkDICOMStudyItemWidget);
  if (this->IsGUIUpdating)
    {
    return;
    }

  if (!this->DicomDatabase)
  {
    logger.error("createSeries failed, no DICOM Database has been set. \n");
    return;
  }

  QStringList seriesList = this->DicomDatabase->seriesForStudy(this->StudyInstanceUID);
  if (seriesList.count() == 0)
  {
    q->setVisible(false);
    q->emit updateGUIFinished();
    return;
  }

  this->IsGUIUpdating = true;

  QSettings settings;
  bool queryRetrieveEnabled = settings.value("DICOM/QueryRetrieveEnabled", "").toBool();
  bool queryEnabled = this->QueryOn && queryRetrieveEnabled;
  bool retrieveEnabled = this->RetrieveOn && queryRetrieveEnabled;

  // Sort by SeriesNumber
  QMap<int, QString> seriesMap;
  this->FilteredSeriesCount = 0;
  int seriesIndex = 0;
  foreach (QString seriesItem, seriesList)
  {
    ctkDICOMSeriesItemWidget* seriesItemWidget = this->isSeriesItemAlreadyAdded(seriesItem);
    if (seriesItemWidget)
    {
      this->FilteredSeriesCount++;
      seriesIndex++;
      seriesItemWidget->generateInstances(queryEnabled, retrieveEnabled);
      continue;
    }

    QString modality = this->DicomDatabase->fieldForSeries("Modality", seriesItem);
    QString seriesDescription = this->DicomDatabase->fieldForSeries("SeriesDescription", seriesItem);
    // Filter with modality and seriesDescription
    if ((this->FilteringSeriesDescription.isEmpty() ||
         seriesDescription.contains(this->FilteringSeriesDescription, Qt::CaseInsensitive)) &&
        (this->FilteringModalities.contains("Any") || this->FilteringModalities.contains(modality)))
    {
      int seriesNumber = this->DicomDatabase->fieldForSeries("SeriesNumber", seriesItem).toInt();
      while (seriesMap.contains(seriesNumber))
      {
        seriesNumber++;
      }
      // QMap automatically sort in ascending with the key
      seriesMap[seriesNumber] = seriesItem;
      this->FilteredSeriesCount++;
    }
  }

  int numberOfSeries = seriesMap.count() + seriesIndex;
  foreach (QString seriesItem, seriesMap)
  {
    QString seriesInstanceUID = this->DicomDatabase->fieldForSeries("SeriesInstanceUID", seriesItem);
    if (seriesInstanceUID.isEmpty())
    {
      numberOfSeries--;
      continue;
    }

    QString modality = this->DicomDatabase->fieldForSeries("Modality", seriesItem);
    QString seriesDescription = this->DicomDatabase->fieldForSeries("SeriesDescription", seriesItem);
    if (seriesDescription.isEmpty())
    {
      seriesDescription = ctkDICOMStudyItemWidget::tr("UNDEFINED");
    }
    ctkDICOMSeriesItemWidget* seriesItemWidget =
      q->addSeriesItemWidget(seriesIndex, seriesItem, seriesInstanceUID, modality, seriesDescription);
    if (seriesItemWidget)
    {
      seriesItemWidget->generateInstances(this->QueryOn, this->RetrieveOn);
    }
    seriesIndex++;

    if (seriesIndex == numberOfSeries)
    {
      int emptyIndex = seriesIndex;
      int columnIndex = emptyIndex % this->SeriesListTableWidget->columnCount();
      while (columnIndex != 0)
      {
        int rowIndex = floor(emptyIndex / this->SeriesListTableWidget->columnCount());
        columnIndex = emptyIndex % this->SeriesListTableWidget->columnCount();
        this->addEmptySeriesItemWidget(rowIndex, columnIndex);
        emptyIndex++;
      }
    }

    int iHeight = 0;
    for (int rowIndex = 0; rowIndex < this->SeriesListTableWidget->rowCount(); ++rowIndex)
    {
      iHeight += this->SeriesListTableWidget->verticalHeader()->sectionSize(rowIndex);
    }
    if (iHeight < this->ThumbnailSizePixel)
    {
      iHeight = this->ThumbnailSizePixel;
    }
    iHeight += 25;
    this->SeriesListTableWidget->setMinimumHeight(iHeight);
  }

  this->IsGUIUpdating = false;
  q->setVisible(this->FilteredSeriesCount != 0);
  q->emit updateGUIFinished();
}

//------------------------------------------------------------------------------
int ctkDICOMStudyItemWidgetPrivate::getScreenWidth()
{
  QList<QScreen*> screens = QApplication::screens();
  int width = 1920;
  foreach (QScreen* screen, screens)
  {
    QRect rec = screen->geometry();
    if (rec.width() > width)
    {
      width = rec.width();
    }
  }

  return width;
}

//------------------------------------------------------------------------------
int ctkDICOMStudyItemWidgetPrivate::getScreenHeight()
{
  QList<QScreen*> screens = QApplication::screens();
  int height = 1080;
  foreach (QScreen* screen, screens)
  {
    QRect rec = screen->geometry();
    if (rec.height() > height)
    {
      height = rec.height();
    }
  }

  return height;
}

//------------------------------------------------------------------------------
int ctkDICOMStudyItemWidgetPrivate::calculateNumerOfSeriesPerRow()
{
  int width = this->getScreenWidth();
  int numberOfSeriesPerRow = 1;
  numberOfSeriesPerRow = floor(width / this->ThumbnailSizePixel) - 1;

  return numberOfSeriesPerRow;
}

//------------------------------------------------------------------------------
int ctkDICOMStudyItemWidgetPrivate::calculateThumbnailSizeInPixel(const ctkDICOMStudyItemWidget::ThumbnailSizeOption& thumbnailSize)
{
  int height = this->getScreenHeight();
  int thumbnailSizeInPixel = 1;
  switch (thumbnailSize)
  {
    case ctkDICOMStudyItemWidget::ThumbnailSizeOption::Small:
    {
      thumbnailSizeInPixel = floor(height / 7.);
    }
    break;
    case ctkDICOMStudyItemWidget::ThumbnailSizeOption::Medium:
    {
      thumbnailSizeInPixel = floor(height / 5.5);
    }
    break;
    case ctkDICOMStudyItemWidget::ThumbnailSizeOption::Large:
    {
      thumbnailSizeInPixel = floor(height / 4.);
    }
    break;
  }

  return thumbnailSizeInPixel;
}

//------------------------------------------------------------------------------
void ctkDICOMStudyItemWidgetPrivate::addEmptySeriesItemWidget(int rowIndex, int columnIndex)
{
  QTableWidgetItem* tableItem = new QTableWidgetItem;
  tableItem->setFlags(Qt::NoItemFlags);
  tableItem->setSizeHint(QSize(this->ThumbnailSizePixel, this->ThumbnailSizePixel));

  this->SeriesListTableWidget->setItem(rowIndex, columnIndex, tableItem);
}

//------------------------------------------------------------------------------
ctkDICOMSeriesItemWidget* ctkDICOMStudyItemWidgetPrivate::isSeriesItemAlreadyAdded(const QString& seriesItem)
{
  ctkDICOMSeriesItemWidget* seriesItemWidgetFound = nullptr;
  for (int rowIndex = 0; rowIndex < this->SeriesListTableWidget->rowCount(); rowIndex++)
  {
    for (int columnIndex = 0; columnIndex < this->SeriesListTableWidget->columnCount(); columnIndex++)
    {
      ctkDICOMSeriesItemWidget* seriesItemWidget =
        qobject_cast<ctkDICOMSeriesItemWidget*>(this->SeriesListTableWidget->cellWidget(rowIndex, columnIndex));
      if (!seriesItemWidget)
      {
        continue;
      }

      if (seriesItemWidget->seriesItem() == seriesItem)
      {
        seriesItemWidgetFound = seriesItemWidget;
        break;
      }
    }

    if (seriesItemWidgetFound)
    {
      break;
    }
  }

  return seriesItemWidgetFound;
}

//----------------------------------------------------------------------------
// ctkDICOMStudyItemWidget methods

//----------------------------------------------------------------------------
ctkDICOMStudyItemWidget::ctkDICOMStudyItemWidget(QWidget* parent)
  : Superclass(parent)
  , d_ptr(new ctkDICOMStudyItemWidgetPrivate(*this))
{
  Q_D(ctkDICOMStudyItemWidget);
  d->init(parent);
}

//----------------------------------------------------------------------------
ctkDICOMStudyItemWidget::~ctkDICOMStudyItemWidget()
{
}

//------------------------------------------------------------------------------
CTK_SET_CPP(ctkDICOMStudyItemWidget, const QStringList&, setAllowedServers, AllowedServers);
CTK_GET_CPP(ctkDICOMStudyItemWidget, QStringList, allowedServers, AllowedServers);
CTK_SET_CPP(ctkDICOMStudyItemWidget, const OperationStatus&, setOperationStatus, Status);
CTK_GET_CPP(ctkDICOMStudyItemWidget, ctkDICOMStudyItemWidget::OperationStatus, operationStatus, Status);
CTK_SET_CPP(ctkDICOMStudyItemWidget, const QString&, setStudyItem, StudyItem);
CTK_GET_CPP(ctkDICOMStudyItemWidget, QString, studyItem, StudyItem);
CTK_SET_CPP(ctkDICOMStudyItemWidget, const QString&, setPatientID, PatientID);
CTK_GET_CPP(ctkDICOMStudyItemWidget, QString, patientID, PatientID);
CTK_SET_CPP(ctkDICOMStudyItemWidget, const QString&, setStudyInstanceUID, StudyInstanceUID);
CTK_GET_CPP(ctkDICOMStudyItemWidget, QString, studyInstanceUID, StudyInstanceUID);
CTK_GET_CPP(ctkDICOMStudyItemWidget, ctkDICOMStudyItemWidget::ThumbnailSizeOption, thumbnailSize, ThumbnailSize);
CTK_GET_CPP(ctkDICOMStudyItemWidget, int, thumbnailSizePixel, ThumbnailSizePixel);
CTK_SET_CPP(ctkDICOMStudyItemWidget, const QString&, setFilteringSeriesDescription, FilteringSeriesDescription);
CTK_GET_CPP(ctkDICOMStudyItemWidget, QString, filteringSeriesDescription, FilteringSeriesDescription);
CTK_SET_CPP(ctkDICOMStudyItemWidget, const QStringList&, setFilteringModalities, FilteringModalities);
CTK_GET_CPP(ctkDICOMStudyItemWidget, QStringList, filteringModalities, FilteringModalities);
CTK_GET_CPP(ctkDICOMStudyItemWidget, int, filteredSeriesCount, FilteredSeriesCount);
CTK_GET_CPP(ctkDICOMStudyItemWidget, QString, stoppedJobUID, StoppedJobUID);

//----------------------------------------------------------------------------
void ctkDICOMStudyItemWidget::setTitle(const QString& title)
{
  Q_D(ctkDICOMStudyItemWidget);

  QString studyIDText;
  QString elidedText = title;
  QString truncatedText = "";
  QString studyIDSearchString = "  -  ID:";
  int index = title.indexOf(studyIDSearchString);
  if (index != -1)
  {
    studyIDText = title.mid(index);
    elidedText = title.left(index);
  }

  QFontMetrics metrics(d->StudyItemCollapsibleGroupBox->font());
  int textWidth = metrics.horizontalAdvance(elidedText);
  int widgetWidth = this->width();
  if (textWidth > widgetWidth)
  {
    elidedText = metrics.elidedText(elidedText, Qt::ElideRight, widgetWidth);
    int ellipsisPos = elidedText.indexOf("…");
    if (ellipsisPos != -1)
    {
      truncatedText = title.mid(ellipsisPos + 3);
      elidedText += "    ";
    }
  }

  d->StudyItemCollapsibleGroupBox->setTitle(elidedText);
  if (truncatedText.isEmpty())
  {
    studyIDText.replace(" - ", "");
    d->StudyItemCollapsibleGroupBox->setToolTip(studyIDText);
  }
  else
  {
    d->StudyItemCollapsibleGroupBox->setToolTip(title);
  }
}

//------------------------------------------------------------------------------
QString ctkDICOMStudyItemWidget::title() const
{
  Q_D(const ctkDICOMStudyItemWidget);
  return d->StudyItemCollapsibleGroupBox->title();
}

//----------------------------------------------------------------------------
void ctkDICOMStudyItemWidget::setCollapsed(bool collapsed)
{
  Q_D(ctkDICOMStudyItemWidget);
  d->StudyItemCollapsibleGroupBox->setCollapsed(collapsed);
}

//------------------------------------------------------------------------------
bool ctkDICOMStudyItemWidget::collapsed() const
{
  Q_D(const ctkDICOMStudyItemWidget);
  return d->StudyItemCollapsibleGroupBox->collapsed();
}

//------------------------------------------------------------------------------
int ctkDICOMStudyItemWidget::numberOfSeriesPerRow() const
{
  Q_D(const ctkDICOMStudyItemWidget);
  return d->SeriesListTableWidget->columnCount();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyItemWidget::setThumbnailSize(const ctkDICOMStudyItemWidget::ThumbnailSizeOption& thumbnailSize)
{
  Q_D(ctkDICOMStudyItemWidget);
  d->ThumbnailSize = thumbnailSize;
  d->ThumbnailSizePixel = d->calculateThumbnailSizeInPixel(d->ThumbnailSize);
  d->SeriesListTableWidget->setColumnCount(d->calculateNumerOfSeriesPerRow());
  d->updateColumnsWidths();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyItemWidget::setSelection(bool selected)
{
  Q_D(const ctkDICOMStudyItemWidget);
  if (selected)
  {
    d->SeriesListTableWidget->selectAll();
  }
  else
  {
    d->SeriesListTableWidget->clearSelection();
  }

  d->StudySelectionCheckBox->setChecked(selected);
}

//------------------------------------------------------------------------------
bool ctkDICOMStudyItemWidget::selection() const
{
  Q_D(const ctkDICOMStudyItemWidget);
  return d->StudySelectionCheckBox->isChecked();
}

//----------------------------------------------------------------------------
ctkDICOMScheduler* ctkDICOMStudyItemWidget::scheduler() const
{
  Q_D(const ctkDICOMStudyItemWidget);
  return d->Scheduler.data();
}

//----------------------------------------------------------------------------
QSharedPointer<ctkDICOMScheduler> ctkDICOMStudyItemWidget::schedulerShared() const
{
  Q_D(const ctkDICOMStudyItemWidget);
  return d->Scheduler;
}

//----------------------------------------------------------------------------
void ctkDICOMStudyItemWidget::setScheduler(ctkDICOMScheduler& scheduler)
{
  Q_D(ctkDICOMStudyItemWidget);
  d->Scheduler = QSharedPointer<ctkDICOMScheduler>(&scheduler, skipDelete);
}

//----------------------------------------------------------------------------
void ctkDICOMStudyItemWidget::setScheduler(QSharedPointer<ctkDICOMScheduler> scheduler)
{
  Q_D(ctkDICOMStudyItemWidget);
  d->Scheduler = scheduler;
}

//----------------------------------------------------------------------------
ctkDICOMDatabase* ctkDICOMStudyItemWidget::dicomDatabase() const
{
  Q_D(const ctkDICOMStudyItemWidget);
  return d->DicomDatabase.data();
}

//----------------------------------------------------------------------------
QSharedPointer<ctkDICOMDatabase> ctkDICOMStudyItemWidget::dicomDatabaseShared() const
{
  Q_D(const ctkDICOMStudyItemWidget);
  return d->DicomDatabase;
}

//----------------------------------------------------------------------------
void ctkDICOMStudyItemWidget::setDicomDatabase(ctkDICOMDatabase& dicomDatabase)
{
  Q_D(ctkDICOMStudyItemWidget);
  d->DicomDatabase = QSharedPointer<ctkDICOMDatabase>(&dicomDatabase, skipDelete);
}

//----------------------------------------------------------------------------
void ctkDICOMStudyItemWidget::setDicomDatabase(QSharedPointer<ctkDICOMDatabase> dicomDatabase)
{
  Q_D(ctkDICOMStudyItemWidget);
  d->DicomDatabase = dicomDatabase;
}

//------------------------------------------------------------------------------
QTableWidget* ctkDICOMStudyItemWidget::seriesListTableWidget()
{
  Q_D(ctkDICOMStudyItemWidget);
  return d->SeriesListTableWidget;
}

//------------------------------------------------------------------------------
QList<ctkDICOMSeriesItemWidget*> ctkDICOMStudyItemWidget::seriesItemWidgetsList() const
{
  Q_D(const ctkDICOMStudyItemWidget);
  QList<ctkDICOMSeriesItemWidget*> seriesItemWidgetsList;

  for (int row = 0; row < d->SeriesListTableWidget->rowCount(); row++)
  {
    for (int column = 0; column < d->SeriesListTableWidget->columnCount(); column++)
    {
      ctkDICOMSeriesItemWidget* seriesItemWidget =
        qobject_cast<ctkDICOMSeriesItemWidget*>(d->SeriesListTableWidget->cellWidget(row, column));
      if (!seriesItemWidget)
      {
        continue;
      }

      seriesItemWidgetsList.append(seriesItemWidget);
    }
  }

  return seriesItemWidgetsList;
}

//------------------------------------------------------------------------------
ctkDICOMSeriesItemWidget* ctkDICOMStudyItemWidget::addSeriesItemWidget(int tableIndex,
                                                                       const QString& seriesItem,
                                                                       const QString& seriesInstanceUID,
                                                                       const QString& modality,
                                                                       const QString& seriesDescription)
{
  Q_D(ctkDICOMStudyItemWidget);
  if (!d->DicomDatabase)
  {
    logger.error("addSeriesItemWidget failed, no DICOM Database has been set. \n");
    return nullptr;
  }

  QString seriesNumber = d->DicomDatabase->fieldForSeries("SeriesNumber", seriesItem);
  ctkDICOMSeriesItemWidget* seriesItemWidget = new ctkDICOMSeriesItemWidget();
  seriesItemWidget->setSeriesItem(seriesItem);
  seriesItemWidget->setPatientID(d->PatientID);
  seriesItemWidget->setStudyInstanceUID(d->StudyInstanceUID);
  seriesItemWidget->setSeriesInstanceUID(seriesInstanceUID);
  seriesItemWidget->setSeriesNumber(seriesNumber);
  seriesItemWidget->setModality(modality);
  seriesItemWidget->setSeriesDescription(seriesDescription);
  seriesItemWidget->setThumbnailSizePixel(d->ThumbnailSizePixel);
  seriesItemWidget->setDicomDatabase(d->DicomDatabase);
  seriesItemWidget->setScheduler(d->Scheduler);
  seriesItemWidget->setAllowedServers(d->AllowedServers);
  seriesItemWidget->setContextMenuPolicy(Qt::CustomContextMenu);

  this->connect(seriesItemWidget, SIGNAL(customContextMenuRequested(const QPoint&)),
                d->VisualDICOMBrowser.data(), SLOT(showSeriesContextMenu(const QPoint&)));

  QTableWidgetItem* tableItem = new QTableWidgetItem;
  tableItem->setSizeHint(QSize(d->ThumbnailSizePixel, d->ThumbnailSizePixel));

  int rowIndex = floor(tableIndex / d->SeriesListTableWidget->columnCount());
  int columnIndex = tableIndex % d->SeriesListTableWidget->columnCount();
  if (columnIndex == 0)
  {
    d->SeriesListTableWidget->insertRow(rowIndex);
    d->SeriesListTableWidget->setRowHeight(rowIndex, d->ThumbnailSizePixel + 30);
  }

  d->SeriesListTableWidget->setItem(rowIndex, columnIndex, tableItem);
  d->SeriesListTableWidget->setCellWidget(rowIndex, columnIndex, seriesItemWidget);

  return seriesItemWidget;
}

//------------------------------------------------------------------------------
void ctkDICOMStudyItemWidget::removeSeriesItemWidget(const QString& seriesItem)
{
  Q_D(ctkDICOMStudyItemWidget);

  for (int row = 0; row < d->SeriesListTableWidget->rowCount(); row++)
  {
    for (int column = 0; column < d->SeriesListTableWidget->columnCount(); column++)
    {
      ctkDICOMSeriesItemWidget* seriesItemWidget =
        qobject_cast<ctkDICOMSeriesItemWidget*>(d->SeriesListTableWidget->cellWidget(row, column));
      if (!seriesItemWidget || seriesItemWidget->seriesItem() != seriesItem)
      {
        continue;
      }

      d->SeriesListTableWidget->removeCellWidget(row, column);
      this->disconnect(seriesItemWidget, SIGNAL(customContextMenuRequested(const QPoint&)),
                       d->VisualDICOMBrowser.data(), SLOT(showSeriesContextMenu(const QPoint&)));
      delete seriesItemWidget;
      QTableWidgetItem* tableItem = d->SeriesListTableWidget->item(row, column);
      delete tableItem;

      d->addEmptySeriesItemWidget(row, column);
      break;
    }
  }
}

//------------------------------------------------------------------------------
ctkDICOMSeriesItemWidget *ctkDICOMStudyItemWidget::seriesItemWidgetBySeriesItem(const QString &seriesItem)
{
Q_D(ctkDICOMStudyItemWidget);

  for (int row = 0; row < d->SeriesListTableWidget->rowCount(); row++)
  {
    for (int column = 0; column < d->SeriesListTableWidget->columnCount(); column++)
    {
      ctkDICOMSeriesItemWidget* seriesItemWidget =
        qobject_cast<ctkDICOMSeriesItemWidget*>(d->SeriesListTableWidget->cellWidget(row, column));
      if (!seriesItemWidget || seriesItemWidget->seriesItem() != seriesItem)
      {
        continue;
      }

      return seriesItemWidget;
    }
  }
  return nullptr;
}

//------------------------------------------------------------------------------
ctkDICOMSeriesItemWidget *ctkDICOMStudyItemWidget::seriesItemWidgetBySeriesInstanceUID(const QString &seriesInstanceUID)
{
  Q_D(ctkDICOMStudyItemWidget);

  for (int row = 0; row < d->SeriesListTableWidget->rowCount(); row++)
  {
    for (int column = 0; column < d->SeriesListTableWidget->columnCount(); column++)
    {
      ctkDICOMSeriesItemWidget* seriesItemWidget =
        qobject_cast<ctkDICOMSeriesItemWidget*>(d->SeriesListTableWidget->cellWidget(row, column));
      if (!seriesItemWidget || seriesItemWidget->seriesInstanceUID() != seriesInstanceUID)
      {
        continue;
      }

      return seriesItemWidget;
    }
  }
  return nullptr;
}

//------------------------------------------------------------------------------
ctkCollapsibleGroupBox* ctkDICOMStudyItemWidget::collapsibleGroupBox()
{
  Q_D(ctkDICOMStudyItemWidget);
  return d->StudyItemCollapsibleGroupBox;
}

//------------------------------------------------------------------------------
void ctkDICOMStudyItemWidget::generateSeries(bool query, bool retrieve)
{
  Q_D(ctkDICOMStudyItemWidget);

  d->QueryOn = query;
  d->RetrieveOn = retrieve;
  d->createSeries();
  if (query && d->Scheduler &&
      d->Scheduler->queryRetrieveServersCount() > 0)
  {
    d->Scheduler->querySeries(d->PatientID,
                              d->StudyInstanceUID,
                              QThread::NormalPriority,
                              d->AllowedServers);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMStudyItemWidget::updateGUIFromScheduler(const QVariant& data)
{
  Q_D(ctkDICOMStudyItemWidget);

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  if (td.JobUID.isEmpty())
  {
    d->createSeries();
    return;
  }

  if (td.StudyInstanceUID != d->StudyInstanceUID)
  {
    return;
  }

  if (td.JobType == ctkDICOMJobResponseSet::JobType::QuerySeries)
  {
    d->createSeries();
  }
  else if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryInstances ||
           td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
           td.JobType == ctkDICOMJobResponseSet::JobType::StoreSOPInstance ||
           td.JobType == ctkDICOMJobResponseSet::JobType::ThumbnailGenerator ||
           td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries)
  {
    ctkDICOMSeriesItemWidget* seriesItemWidget = this->seriesItemWidgetBySeriesInstanceUID(td.SeriesInstanceUID);
    if (seriesItemWidget)
    {
      seriesItemWidget->updateGUIFromScheduler(data);
      if (td.JobType == ctkDICOMJobResponseSet::JobType::StoreSOPInstance ||
          td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries)
      {
        seriesItemWidget->updateSeriesProgressBar(data);
      }
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMStudyItemWidget::onJobStarted(const QVariant &data)
{
  Q_D(ctkDICOMStudyItemWidget);

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  if (td.JobUID.isEmpty() ||
      td.StudyInstanceUID != d->StudyInstanceUID)
  {
    return;
  }

  if (td.JobType == ctkDICOMJobResponseSet::JobType::QuerySeries)
  {
    d->Status = ctkDICOMStudyItemWidget::InProgress;
    d->OperationStatusPushButton->show();
    d->OperationStatusPushButton->setIcon(QIcon(":/Icons/pending.svg"));
  }
  else if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryInstances ||
           td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
           td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries)
  {
    ctkDICOMSeriesItemWidget* seriesItemWidget = this->seriesItemWidgetBySeriesInstanceUID(td.SeriesInstanceUID);
    if (seriesItemWidget)
    {
      seriesItemWidget->onJobStarted(data);
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMStudyItemWidget::onJobUserStopped(const QVariant &data)
{
  Q_D(ctkDICOMStudyItemWidget);

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  if (td.JobUID.isEmpty() ||
      td.StudyInstanceUID != d->StudyInstanceUID)
  {
    return;
  }

  if (td.JobType == ctkDICOMJobResponseSet::JobType::QuerySeries)
  {
    d->Status = ctkDICOMStudyItemWidget::Failed;
    d->OperationStatusPushButton->setIcon(QIcon(":/Icons/error_red.svg"));
    d->StoppedJobUID = td.JobUID;
  }
  else if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryInstances ||
           td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
           td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries)
  {
    ctkDICOMSeriesItemWidget* seriesItemWidget = this->seriesItemWidgetBySeriesInstanceUID(td.SeriesInstanceUID);
    if (seriesItemWidget)
    {
      seriesItemWidget->onJobUserStopped(data);
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMStudyItemWidget::onJobFailed(const QVariant &data)
{
  Q_D(ctkDICOMStudyItemWidget);

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  if (td.JobUID.isEmpty() ||
      td.StudyInstanceUID != d->StudyInstanceUID)
  {
    return;
  }

  if (td.JobType == ctkDICOMJobResponseSet::JobType::QuerySeries)
  {
    d->Status = ctkDICOMStudyItemWidget::Failed;
    d->OperationStatusPushButton->setIcon(QIcon(":/Icons/error_red.svg"));
    d->StoppedJobUID = td.JobUID;
  }
  else if (td.JobType == ctkDICOMJobResponseSet::JobType::QueryInstances ||
           td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance ||
           td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries)
  {
    ctkDICOMSeriesItemWidget* seriesItemWidget = this->seriesItemWidgetBySeriesInstanceUID(td.SeriesInstanceUID);
    if (seriesItemWidget)
    {
      seriesItemWidget->onJobFailed(data);
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMStudyItemWidget::onJobFinished(const QVariant &data)
{
  Q_D(ctkDICOMStudyItemWidget);

  ctkDICOMJobDetail td = data.value<ctkDICOMJobDetail>();
  if (td.JobUID.isEmpty() ||
      td.StudyInstanceUID != d->StudyInstanceUID)
  {
    return;
  }

  if (td.JobType == ctkDICOMJobResponseSet::JobType::QuerySeries)
  {
    d->Status = ctkDICOMStudyItemWidget::Completed;
    d->OperationStatusPushButton->setIcon(QIcon(":/Icons/accept.svg"));
  }
  else if (td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSeries ||
           td.JobType == ctkDICOMJobResponseSet::JobType::RetrieveSOPInstance)
  {
    ctkDICOMSeriesItemWidget* seriesItemWidget = this->seriesItemWidgetBySeriesInstanceUID(td.SeriesInstanceUID);
    if (seriesItemWidget)
    {
      seriesItemWidget->onJobFinished(data);
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMStudyItemWidget::onStudySelectionClicked(bool toggled)
{
  this->setSelection(toggled);
}

//------------------------------------------------------------------------------
void ctkDICOMStudyItemWidget::onOperationStatusButtonClicked(bool)
{
  Q_D(ctkDICOMStudyItemWidget);

  ctkDICOMStudyItemWidget::OperationStatus status = d->Status;
  if (status == ctkDICOMStudyItemWidget::InProgress)
  {
    d->Scheduler->stopJobsByDICOMUIDs(QStringList(),
                                      QStringList(d->StudyInstanceUID));
  }
  else if (status > ctkDICOMStudyItemWidget::InProgress)
  {
    if (!d->Scheduler->retryJob(d->StoppedJobUID))
    {
    logger.info(QString("Unable to restart job %1 (study level) because the job has been fully cleared from the system. "
                        "Please initiate a new job if further processing is required.").arg(d->StoppedJobUID));
    }
  }
}
