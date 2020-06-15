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

// CTK includes
#include "ctkDICOMTableManager.h"
#include "ctkDICOMTableView.h"
#include "ui_ctkDICOMTableManager.h"

// Qt includes
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QSplitter>
#include <QTableView>

class ctkDICOMTableManagerPrivate : public Ui_ctkDICOMTableManager
{
  Q_DECLARE_PUBLIC(ctkDICOMTableManager)

protected:
  ctkDICOMTableManager* const q_ptr;

public:
  ctkDICOMTableManagerPrivate(ctkDICOMTableManager& obj);
  ~ctkDICOMTableManagerPrivate();

  void init();
  void setDICOMDatabase(ctkDICOMDatabase *db);
  void collapseTopHeader(bool collapse);
  void showFilterActiveWarning(ctkSearchBox* searchBox, bool showWarning);

  ctkDICOMDatabase* dicomDatabase;

  bool m_DynamicTableLayout;
  /// Flag storing state whether automatic selection of series is enabled.
  /// It is only needed to be able to provide a read function for the property
  bool m_AutoSelectSeries;
};

//------------------------------------------------------------------------------

ctkDICOMTableManagerPrivate::ctkDICOMTableManagerPrivate(ctkDICOMTableManager &obj)
  : q_ptr(&obj)
  , m_DynamicTableLayout(false)
  , m_AutoSelectSeries(true)
{

}

//------------------------------------------------------------------------------

ctkDICOMTableManagerPrivate::~ctkDICOMTableManagerPrivate()
{

}

//------------------------------------------------------------------------------

void ctkDICOMTableManagerPrivate::init()
{
  //setup UI
  Q_Q(ctkDICOMTableManager);
  this->setupUi(q);

  this->patientsTable->setQueryTableName("Patients");
  this->studiesTable->setQueryTableName("Studies");
  this->studiesTable->setQueryForeignKey("PatientsUID");
  this->seriesTable->setQueryTableName("Series");
  this->seriesTable->setQueryForeignKey("StudyInstanceUID");

  this->patientsSearchBox->setAlwaysShowClearIcon(true);
  this->patientsSearchBox->setShowSearchIcon(true);
  QObject::connect(this->patientsSearchBox, SIGNAL(textChanged(QString)),
    this->patientsTable, SLOT(setFilterText(QString)));
  QObject::connect(this->patientsTable, SIGNAL(filterTextChanged(QString)),
    this->patientsSearchBox, SLOT(setText(QString)));
  QObject::connect(this->patientsTable, SIGNAL(showFilterActiveWarning(bool)),
    q, SLOT(showPatientsFilterActiveWarning(bool)));

  this->studiesSearchBox->setAlwaysShowClearIcon(true);
  this->studiesSearchBox->setShowSearchIcon(true);
  QObject::connect(this->studiesSearchBox, SIGNAL(textChanged(QString)),
    this->studiesTable, SLOT(setFilterText(QString)));
  QObject::connect(this->studiesTable, SIGNAL(filterTextChanged(QString)),
    this->studiesSearchBox, SLOT(setText(QString)));
  QObject::connect(this->studiesTable, SIGNAL(showFilterActiveWarning(bool)),
    q, SLOT(showStudiesFilterActiveWarning(bool)));

  this->seriesSearchBox->setAlwaysShowClearIcon(true);
  this->seriesSearchBox->setShowSearchIcon(true);
  QObject::connect(this->seriesSearchBox, SIGNAL(textChanged(QString)),
    this->seriesTable, SLOT(setFilterText(QString)));
  QObject::connect(this->seriesTable, SIGNAL(filterTextChanged(QString)),
    this->seriesSearchBox, SLOT(setText(QString)));
  QObject::connect(this->seriesTable, SIGNAL(showFilterActiveWarning(bool)),
    q, SLOT(showSeriesFilterActiveWarning(bool)));

  // For propagating patient selection changes
  QObject::connect(this->patientsTable, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
                   q, SIGNAL(patientsSelectionChanged(const QItemSelection&, const QItemSelection&)));
  QObject::connect(this->patientsTable, SIGNAL(selectionChanged(const QStringList&)),
                   q, SIGNAL(patientsSelectionChanged(const QStringList&)));

  // For propagating study selection changes
  QObject::connect(this->studiesTable, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
                   q, SIGNAL(studiesSelectionChanged(const QItemSelection&, const QItemSelection&)));
  QObject::connect(this->studiesTable, SIGNAL(selectionChanged(const QStringList&)),
                   q, SIGNAL(studiesSelectionChanged(const QStringList&)));

  // For propagating series selection changes
  QObject::connect(this->seriesTable, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
                   q, SIGNAL(seriesSelectionChanged(const QItemSelection&, const QItemSelection&)));
  QObject::connect(this->seriesTable, SIGNAL(selectionChanged(const QStringList&)),
                   q, SIGNAL(seriesSelectionChanged(const QStringList&)));

  QObject::connect( this->patientsTable, SIGNAL( doubleClicked( const QModelIndex& ) ),
                    q, SIGNAL( patientsDoubleClicked( const QModelIndex& ) ) );
  QObject::connect( this->studiesTable, SIGNAL( doubleClicked( const QModelIndex& ) ),
                    q, SIGNAL( studiesDoubleClicked( const QModelIndex& ) ) );
  QObject::connect(this->seriesTable, SIGNAL(doubleClicked(const QModelIndex&)),
                   q, SIGNAL(seriesDoubleClicked(const QModelIndex&)));

  // For propagating right clicks, the table takes care of translating to a global position
  QObject::connect(this->patientsTable, SIGNAL(customContextMenuRequested(const QPoint&)),
                   q, SIGNAL(patientsRightClicked(const QPoint&)));
  QObject::connect(this->studiesTable, SIGNAL(customContextMenuRequested(const QPoint&)),
                   q, SIGNAL(studiesRightClicked(const QPoint&)));

  QObject::connect(this->seriesTable, SIGNAL(customContextMenuRequested(const QPoint&)),
                   q, SIGNAL(seriesRightClicked(const QPoint&)));

  q->setTableOrientation(this->tableSplitter->orientation());
}

//------------------------------------------------------------------------------
void ctkDICOMTableManagerPrivate::setDICOMDatabase(ctkDICOMDatabase* db)
{
  this->patientsTable->setDicomDataBase(db);
  this->studiesTable->setDicomDataBase(db);
  this->seriesTable->setDicomDataBase(db);
  this->dicomDatabase = db;
}

//------------------------------------------------------------------------------
void ctkDICOMTableManagerPrivate::collapseTopHeader(bool collapse)
{
  Q_Q(ctkDICOMTableManager);
  this->patientsTable->setHeaderVisible(!collapse);
  this->studiesTable->setHeaderVisible(!collapse);
  this->seriesTable->setHeaderVisible(!collapse);
  this->headerWidget->setVisible(collapse);
}

//------------------------------------------------------------------------------
void ctkDICOMTableManagerPrivate::showFilterActiveWarning(ctkSearchBox* searchBox, bool showWarning)
{
  QPalette palette;
  if (showWarning)
  {
    palette.setColor(QPalette::Base, Qt::yellow);
    palette.setColor(QPalette::Text, Qt::black);
  }
  searchBox->setPalette(palette);
}

//----------------------------------------------------------------------------
// ctkDICOMTableManager methods

//----------------------------------------------------------------------------
ctkDICOMTableManager::ctkDICOMTableManager(QWidget *parent)
  :Superclass(parent)
  , d_ptr(new ctkDICOMTableManagerPrivate(*this))
{
  Q_D(ctkDICOMTableManager);
  d->init();
}

//------------------------------------------------------------------------------
ctkDICOMTableManager::ctkDICOMTableManager(ctkDICOMDatabase *db, QWidget *parent)
  : Superclass(parent)
  , d_ptr(new ctkDICOMTableManagerPrivate(*this))
{
  Q_D(ctkDICOMTableManager);
  d->init();
  d->setDICOMDatabase(db);
}

//------------------------------------------------------------------------------
ctkDICOMTableManager::~ctkDICOMTableManager()
{

}

//------------------------------------------------------------------------------
void ctkDICOMTableManager::setDICOMDatabase(ctkDICOMDatabase* db)
{
  Q_D(ctkDICOMTableManager);
  d->setDICOMDatabase(db);
}

//------------------------------------------------------------------------------
void ctkDICOMTableManager::setTableOrientation(const Qt::Orientation &o)
{
  Q_D(ctkDICOMTableManager);
  d->tableSplitter->setOrientation(o);
  d->collapseTopHeader(o == Qt::Vertical);
}

//------------------------------------------------------------------------------
Qt::Orientation ctkDICOMTableManager::tableOrientation()
{
  Q_D(ctkDICOMTableManager);
  return d->tableSplitter->orientation();
}

//------------------------------------------------------------------------------
QStringList ctkDICOMTableManager::currentPatientsSelection()
{
  Q_D(ctkDICOMTableManager);
  return d->patientsTable->currentSelection();
}

//------------------------------------------------------------------------------
QStringList ctkDICOMTableManager::currentStudiesSelection()
{
  Q_D(ctkDICOMTableManager);
  return d->studiesTable->currentSelection();
}

//------------------------------------------------------------------------------
QStringList ctkDICOMTableManager::currentSeriesSelection()
{
  Q_D(ctkDICOMTableManager);
  return d->seriesTable->currentSelection();
}

//------------------------------------------------------------------------------
void ctkDICOMTableManager::onPatientsQueryChanged(const QStringList &uids)
{
  Q_D(ctkDICOMTableManager);
  const std::pair<QString, QStringList> patientCondition("Patients.UID", uids);
  d->seriesTable->addSqlWhereCondition(patientCondition);
  d->studiesTable->addSqlWhereCondition(patientCondition);
}

//------------------------------------------------------------------------------
void ctkDICOMTableManager::onStudiesQueryChanged(const QStringList &uids)
{
  Q_D(ctkDICOMTableManager);
  const std::pair<QString, QStringList> studiesCondition("Studies.StudyInstanceUID", uids);
  d->seriesTable->addSqlWhereCondition(studiesCondition);
}

//------------------------------------------------------------------------------
void ctkDICOMTableManager::onPatientsSelectionChanged(const QStringList &uids)
{
  std::pair<QString, QStringList> patientCondition;
  patientCondition.first = "Patients.UID";
  Q_D(ctkDICOMTableManager);
  if (!uids.empty())
  {
    patientCondition.second = uids;
  }
  else
  {
    patientCondition.second = d->patientsTable->uidsForAllRows();
  }
  d->studiesTable->addSqlWhereCondition(patientCondition);
  d->seriesTable->addSqlWhereCondition(patientCondition);
}

//------------------------------------------------------------------------------
void ctkDICOMTableManager::onStudiesSelectionChanged(const QStringList &uids)
{
  std::pair<QString, QStringList> studiesCondition;
  studiesCondition.first = "Studies.StudyInstanceUID";
  Q_D(ctkDICOMTableManager);
  if (!uids.empty())
  {
    studiesCondition.second = uids;
  }
  else
  {
    studiesCondition.second = d->studiesTable->uidsForAllRows();
  }
  d->seriesTable->addSqlWhereCondition(studiesCondition);
}

//------------------------------------------------------------------------------
void ctkDICOMTableManager::setDynamicTableLayout(bool dynamic)
{
  Q_D(ctkDICOMTableManager);
  d->m_DynamicTableLayout = dynamic;
}

//------------------------------------------------------------------------------
bool ctkDICOMTableManager::dynamicTableLayout() const
{
  Q_D(const ctkDICOMTableManager);
  return d->m_DynamicTableLayout;
}

//------------------------------------------------------------------------------
void ctkDICOMTableManager::setAutoSelectSeries(bool autoSelect)
{
  Q_D(ctkDICOMTableManager);

  if (autoSelect == d->m_AutoSelectSeries)
  {
    return;
  }

  if (autoSelect)
  {
    QAbstractItemView::SelectionMode selectionMode = static_cast<QAbstractItemView::SelectionMode>(this->selectionMode());
    if (selectionMode == QAbstractItemView::SingleSelection)
    {
      QObject::connect( d->studiesTable, SIGNAL(selectionChanged(const QStringList&)),
                        d->seriesTable, SLOT(selectFirst()) );
    }
    else
    {
      QObject::connect( d->studiesTable, SIGNAL(selectionChanged(const QStringList&)),
                        d->seriesTable, SLOT(selectAll()) );
    }
  }
  else
  {
    QObject::disconnect( d->studiesTable, SIGNAL(selectionChanged(const QStringList&)),
                         d->seriesTable, SLOT(selectAll()) );
    QObject::disconnect( d->studiesTable, SIGNAL(selectionChanged(const QStringList&)),
                         d->seriesTable, SLOT(selectFirst()) );
    // Remove selection to avoid loading any previously auto-selected series
    d->seriesTable->clearSelection();
  }

  d->m_AutoSelectSeries = autoSelect;
}

//------------------------------------------------------------------------------
bool ctkDICOMTableManager::autoSelectSeries() const
{
  Q_D(const ctkDICOMTableManager);
  return d->m_AutoSelectSeries;
}

//------------------------------------------------------------------------------
void ctkDICOMTableManager::setSelectionMode(int mode)
{
  Q_D(ctkDICOMTableManager);

  if (mode == this->selectionMode())
  {
    return;
  }

  QAbstractItemView::SelectionMode selectionMode = static_cast<QAbstractItemView::SelectionMode>(mode);
  d->patientsTable->tableView()->setSelectionMode(selectionMode);
  d->studiesTable->tableView()->setSelectionMode(selectionMode);
  d->seriesTable->tableView()->setSelectionMode(selectionMode);

  // Re-connect the proper slots for studies
  QObject::disconnect( d->patientsTable, SIGNAL(selectionChanged(const QStringList&)),
                       d->studiesTable, SLOT(selectAll()) );
  QObject::disconnect( d->patientsTable, SIGNAL(selectionChanged(const QStringList&)),
                       d->studiesTable, SLOT(selectFirst()) );
  if (selectionMode == QAbstractItemView::SingleSelection)
  {
    QObject::connect( d->patientsTable, SIGNAL(selectionChanged(const QStringList&)),
                      d->studiesTable, SLOT(selectFirst()) );
  }
  else
  {
    QObject::connect( d->patientsTable, SIGNAL(selectionChanged(const QStringList&)),
                      d->studiesTable, SLOT(selectAll()) );
  }

  // Re-connect the proper slots for series
  if (this->autoSelectSeries())
  {
    this->setAutoSelectSeries(false);
    this->setAutoSelectSeries(true);
  }
}

//------------------------------------------------------------------------------
int ctkDICOMTableManager::selectionMode() const
{
  Q_D(const ctkDICOMTableManager);
  QAbstractItemView::SelectionMode patientSelectionMode = d->patientsTable->tableView()->selectionMode();
  QAbstractItemView::SelectionMode studySelectionMode =   d->studiesTable->tableView()->selectionMode();
  QAbstractItemView::SelectionMode seriesSelectionMode =  d->seriesTable->tableView()->selectionMode();
  if (patientSelectionMode != studySelectionMode || patientSelectionMode != seriesSelectionMode)
  {
    qWarning() << Q_FUNC_INFO << ": Inconsistent selection mode in the tables. Patient selection mode is returned";
  }

  return static_cast<int>(patientSelectionMode);
}

//------------------------------------------------------------------------------
void ctkDICOMTableManager::updateTableViews()
{
  Q_D(ctkDICOMTableManager);
  d->patientsTable->setQuery();
  d->studiesTable->setQuery();
  d->seriesTable->setQuery();
}

//------------------------------------------------------------------------------
void ctkDICOMTableManager::resizeEvent(QResizeEvent *e)
{
  this->Superclass::resizeEvent(e);
  Q_D(ctkDICOMTableManager);
  if (!d->m_DynamicTableLayout)
    return;

  // If the table is 2x wider than tall then use horizontal layout
  this->setTableOrientation(e->size().width() > 2 * e->size().height() ? Qt::Horizontal : Qt::Vertical);
}

//------------------------------------------------------------------------------
ctkDICOMTableView* ctkDICOMTableManager::patientsTable()
{
  Q_D( ctkDICOMTableManager );
  return(d->patientsTable);
}
//------------------------------------------------------------------------------
ctkDICOMTableView* ctkDICOMTableManager::studiesTable()
{
  Q_D( ctkDICOMTableManager );
  return(d->studiesTable);
}
//------------------------------------------------------------------------------
ctkDICOMTableView* ctkDICOMTableManager::seriesTable()
{
  Q_D( ctkDICOMTableManager );
  return(d->seriesTable);
}

//------------------------------------------------------------------------------
bool ctkDICOMTableManager::isBatchUpdate()const
{
  Q_D(const ctkDICOMTableManager);
  return d->patientsTable->isBatchUpdate();
}

//------------------------------------------------------------------------------
bool ctkDICOMTableManager::setBatchUpdate(bool enable)
{
  Q_D(ctkDICOMTableManager);
  if (enable == this->isBatchUpdate())
  {
    return enable;
  }
  d->patientsTable->setBatchUpdate(enable);
  d->studiesTable->setBatchUpdate(enable);
  d->seriesTable->setBatchUpdate(enable);
  return !enable;
}

//----------------------------------------------------------------------------
void ctkDICOMTableManager::showPatientsFilterActiveWarning(bool showWarning)
{
  Q_D(ctkDICOMTableManager);
  d->showFilterActiveWarning(d->patientsSearchBox, showWarning);
}

//----------------------------------------------------------------------------
void ctkDICOMTableManager::showStudiesFilterActiveWarning(bool showWarning)
{
  Q_D(ctkDICOMTableManager);
  d->showFilterActiveWarning(d->studiesSearchBox, showWarning);
}
//----------------------------------------------------------------------------
void ctkDICOMTableManager::showSeriesFilterActiveWarning(bool showWarning)
{
  Q_D(ctkDICOMTableManager);
  d->showFilterActiveWarning(d->seriesSearchBox, showWarning);
}
