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
#include <QDebug>
#include <QAbstractItemModel>
#include <QKeyEvent>
#include <QScrollBar>
#include <QStandardItemModel>

// CTK includes
#include "ctkErrorLogWidget.h"
#include "ui_ctkErrorLogWidget.h"
#include <ctkErrorLogModel.h>

class ctkErrorLogWidgetPrivate : public Ui_ctkErrorLogWidget
{
  Q_DECLARE_PUBLIC(ctkErrorLogWidget);
protected:
  ctkErrorLogWidget* const q_ptr;
public:
  typedef ctkErrorLogWidgetPrivate Self;
  ctkErrorLogWidgetPrivate(ctkErrorLogWidget& object);

  Qt::Orientation LayoutOrientation;

  ctkErrorLogLevel::LogLevels ErrorButtonFilter;
  ctkErrorLogLevel::LogLevels WarningButtonFilter;
  ctkErrorLogLevel::LogLevels InfoButtonFilter;

  void init();

  QSharedPointer<QItemSelectionModel> SelectionModel;
};

// --------------------------------------------------------------------------
// ctkErrorLogWidgetPrivate methods

// --------------------------------------------------------------------------
ctkErrorLogWidgetPrivate::ctkErrorLogWidgetPrivate(ctkErrorLogWidget& object)
  : q_ptr(&object)
{
  this->LayoutOrientation = Qt::Vertical;
  this->ErrorButtonFilter = ctkErrorLogLevel::Error | ctkErrorLogLevel::Critical | ctkErrorLogLevel::Fatal;
  this->WarningButtonFilter = ctkErrorLogLevel::Warning;
  this->InfoButtonFilter = ctkErrorLogLevel::Info | ctkErrorLogLevel::Debug | ctkErrorLogLevel::Trace | ctkErrorLogLevel::Status;
}

// --------------------------------------------------------------------------
void ctkErrorLogWidgetPrivate::init()
{
  Q_Q(ctkErrorLogWidget);

#if QT_VERSION >= QT_VERSION_CHECK(5,2,0)
  this->ErrorLogDescription->setPlaceholderText(ctkErrorLogWidget::tr("Select messages in the list to see details here."));
#endif

  // this->ShowAllEntryButton->setIcon();
  this->ShowErrorEntryButton->setIcon(q->style()->standardIcon(QStyle::SP_MessageBoxCritical));
  this->ShowWarningEntryButton->setIcon(q->style()->standardIcon(QStyle::SP_MessageBoxWarning));
  this->ShowInfoEntryButton->setIcon(q->style()->standardIcon(QStyle::SP_MessageBoxInformation));
  this->ClearButton->setIcon(q->style()->standardIcon(QStyle::SP_DialogDiscardButton));

  // Make the iconless "All" button the same height as other buttons that have icons
  // (they are shown in the same row, so it does not look nice if their height is different)
  this->ShowAllEntryButton->setFixedHeight(this->ShowErrorEntryButton->sizeHint().height());

  QObject::connect(this->ShowAllEntryButton, SIGNAL(clicked()),
                   q, SLOT(setAllEntriesVisible()));

  QObject::connect(this->ShowErrorEntryButton, SIGNAL(clicked(bool)),
                   q, SLOT(setErrorEntriesVisible(bool)));

  QObject::connect(this->ShowWarningEntryButton, SIGNAL(clicked(bool)),
                   q, SLOT(setWarningEntriesVisible(bool)));

  QObject::connect(this->ShowInfoEntryButton, SIGNAL(clicked(bool)),
                   q, SLOT(setInfoEntriesVisible(bool)));

  QObject::connect(this->ClearButton, SIGNAL(clicked()),
                   q, SLOT(removeEntries()));

  QScrollBar* verticalScrollBar = this->ErrorLogTableView->verticalScrollBar();
  QObject::connect(verticalScrollBar, SIGNAL(valueChanged(int)),
    q, SIGNAL(userViewed()));

  this->ErrorLogTableView->installEventFilter(q);
}

// --------------------------------------------------------------------------
// ctkErrorLogWidget methods

//------------------------------------------------------------------------------
ctkErrorLogWidget::ctkErrorLogWidget(QWidget * newParent)
  : Superclass(newParent)
  , d_ptr(new ctkErrorLogWidgetPrivate(*this))
{
  Q_D(ctkErrorLogWidget);

  d->setupUi(this);
  d->init();

  this->setErrorLogModel(new ctkErrorLogModel(d->ErrorLogTableView));
}

//------------------------------------------------------------------------------
ctkErrorLogWidget::~ctkErrorLogWidget()
{
}

//------------------------------------------------------------------------------
ctkErrorLogAbstractModel* ctkErrorLogWidget::errorLogModel()const
{
  Q_D(const ctkErrorLogWidget);
  QAbstractItemModel* model = d->ErrorLogTableView->model();
  ctkErrorLogAbstractModel * errorLogModel = qobject_cast<ctkErrorLogAbstractModel*>(model);
  Q_ASSERT(model ? errorLogModel != 0 : true);
  return errorLogModel;
}

//------------------------------------------------------------------------------
void ctkErrorLogWidget::setErrorLogModel(ctkErrorLogAbstractModel * newErrorLogModel)
{
  Q_D(ctkErrorLogWidget);

  if (newErrorLogModel == this->errorLogModel())
  {
    return;
  }

  if (this->errorLogModel())
  {
    disconnect(this->errorLogModel(), SIGNAL(rowsInserted(QModelIndex,int,int)),
               this, SLOT(onRowsInserted(QModelIndex,int,int)));

    disconnect(this->errorLogModel(), SIGNAL(logLevelFilterChanged()),
               this, SLOT(onLogLevelFilterChanged()));

    disconnect(d->SelectionModel.data(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
               this, SLOT(onSelectionChanged(QItemSelection,QItemSelection)));

    d->SelectionModel.clear();
  }

  d->ErrorLogTableView->setModel(newErrorLogModel);

  if (newErrorLogModel)
  {
    connect(this->errorLogModel(), SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(onRowsInserted(QModelIndex,int,int)));

    connect(this->errorLogModel(), SIGNAL(logLevelFilterChanged()),
            this, SLOT(onLogLevelFilterChanged()));

    ctkErrorLogLevel::LogLevels logLevelFilter = newErrorLogModel->logLevelFilter();
    this->setErrorEntriesVisible(logLevelFilter & d->ErrorButtonFilter);
    this->setWarningEntriesVisible(logLevelFilter & d->WarningButtonFilter);
    this->setInfoEntriesVisible(logLevelFilter & d->InfoButtonFilter);
    this->errorLogModel()->filterEntry(logLevelFilter & ctkErrorLogLevel::Unknown);

    // Setup selection model
    d->SelectionModel = QSharedPointer<QItemSelectionModel>(new QItemSelectionModel(this->errorLogModel()));
    d->SelectionModel->reset();

    connect(d->SelectionModel.data(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(onSelectionChanged(QItemSelection,QItemSelection)));

    d->ErrorLogTableView->setSelectionModel(d->SelectionModel.data());

    // Resize time column only if there are rows
    if (this->errorLogModel()->rowCount() > 0)
    {
      d->ErrorLogTableView->resizeColumnToContents(ctkErrorLogModel::TimeColumn);
    }
  }
  else
  {
    this->setAllEntriesVisible(0);
  }

  d->ErrorLogTableView->setColumnHidden(ctkErrorLogModel::ThreadIdColumn, true);
}

// --------------------------------------------------------------------------
void ctkErrorLogWidget::setColumnHidden(int columnId, bool hidden) const
{
  Q_D(const ctkErrorLogWidget);
  d->ErrorLogTableView->setColumnHidden(columnId, hidden);
}

// --------------------------------------------------------------------------
void ctkErrorLogWidget::setLayoutOrientation(Qt::Orientation orientation)
{
  Q_D(ctkErrorLogWidget);
  if (d->LayoutOrientation == orientation)
  {
    return;
  }

  d->ctkErrorLogGridLayout->removeWidget(d->ErrorLogDescription);

  int errorLogTableViewIndex = d->ctkErrorLogGridLayout->indexOf(d->ErrorLogTableView);
  int errorLogTableViewRowIndex = -1;
  int errorLogTableViewColIndex = -1;
  int errorLogTableViewRowSpan = -1;
  int errorLogTableViewColSpan = -1;

  d->ctkErrorLogGridLayout->getItemPosition(errorLogTableViewIndex,
    &errorLogTableViewRowIndex, &errorLogTableViewColIndex,
    &errorLogTableViewRowSpan, &errorLogTableViewColSpan);

  if (orientation == Qt::Vertical)
  {
    // Description is below message table
    d->ctkErrorLogGridLayout->addWidget(d->ErrorLogDescription,
      errorLogTableViewRowIndex + 1, errorLogTableViewColIndex, // row, col
      1, 1); // rowSpan, colSpan
  }
  else
  {
    // Description is in a second column, beside the message table.
    // Specifying rowSpan = -1 ensures the widget fills the entire second column.
    d->ctkErrorLogGridLayout->addWidget(d->ErrorLogDescription,
      0, errorLogTableViewColIndex + 1, // row, col
      -1, 1); // rowSpan, colSpan
  }

  d->LayoutOrientation = orientation;
}

// --------------------------------------------------------------------------
Qt::Orientation ctkErrorLogWidget::layoutOrientation() const
{
  Q_D(const ctkErrorLogWidget);
  return d->LayoutOrientation;
}

// --------------------------------------------------------------------------
void ctkErrorLogWidget::setAllEntriesVisible(bool visibility)
{
  this->setErrorEntriesVisible(visibility);
  this->setWarningEntriesVisible(visibility);
  this->setInfoEntriesVisible(visibility);
  this->setUnknownEntriesVisible(visibility);
}

// --------------------------------------------------------------------------
void ctkErrorLogWidget::setErrorEntriesVisible(bool visibility)
{
  Q_D(ctkErrorLogWidget);
  if (!this->errorLogModel())
  {
    return;
  }
  this->errorLogModel()->filterEntry(d->ErrorButtonFilter, /* disableFilter= */ !visibility);
}

// --------------------------------------------------------------------------
void ctkErrorLogWidget::setWarningEntriesVisible(bool visibility)
{
  Q_D(ctkErrorLogWidget);
  if (!this->errorLogModel())
  {
    return;
  }
  this->errorLogModel()->filterEntry(d->WarningButtonFilter, /* disableFilter= */ !visibility);
}

// --------------------------------------------------------------------------
void ctkErrorLogWidget::setInfoEntriesVisible(bool visibility)
{
  Q_D(ctkErrorLogWidget);
  if (!this->errorLogModel())
  {
    return;
  }
  this->errorLogModel()->filterEntry(d->InfoButtonFilter, /* disableFilter= */ !visibility);
}

// --------------------------------------------------------------------------
void ctkErrorLogWidget::setUnknownEntriesVisible(bool visibility)
{
  if (!this->errorLogModel())
  {
    return;
  }
  this->errorLogModel()->filterEntry(ctkErrorLogLevel::Unknown,
      /* disableFilter= */ !visibility);
}

// --------------------------------------------------------------------------
void ctkErrorLogWidget::onRowsInserted(const QModelIndex &/*parent*/, int /*first*/, int /*last*/)
{
  Q_D(ctkErrorLogWidget);
  if (d->ErrorLogTableView->model()->rowCount() == 1)
  {
    // For performance reason, resize first column only when first entry is added
    d->ErrorLogTableView->resizeColumnToContents(ctkErrorLogModel::TimeColumn);
  }
}

// --------------------------------------------------------------------------
void ctkErrorLogWidget::onLogLevelFilterChanged()
{
  Q_D(ctkErrorLogWidget);
  Q_ASSERT(this->errorLogModel());
  ctkErrorLogLevel::LogLevels logLevelFilter = this->errorLogModel()->logLevelFilter();
  d->ShowErrorEntryButton->setChecked(logLevelFilter & d->ErrorButtonFilter);
  d->ShowWarningEntryButton->setChecked(logLevelFilter & d->WarningButtonFilter);
  d->ShowInfoEntryButton->setChecked(logLevelFilter & d->InfoButtonFilter);
  emit userViewed();
}

// --------------------------------------------------------------------------
void ctkErrorLogWidget::removeEntries()
{
  Q_ASSERT(this->errorLogModel());
  this->errorLogModel()->clear();
  emit userViewed();
}

// --------------------------------------------------------------------------
void ctkErrorLogWidget::onSelectionChanged(const QItemSelection & selected,
                                           const QItemSelection & deselected)
{
  // QTime start = QTime::currentTime();

  Q_D(ctkErrorLogWidget);
  Q_UNUSED(selected);
  Q_UNUSED(deselected);

  QModelIndexList selectedRows =
      d->SelectionModel->selectedRows(ctkErrorLogModel::DescriptionColumn);

#if QT_VERSION >= QT_VERSION_CHECK(5,2,0)
  std::sort(selectedRows.begin(), selectedRows.end());
#else
  qSort(selectedRows.begin(), selectedRows.end());
#endif

  QStringList descriptions;

  foreach(const QModelIndex& index, selectedRows)
  {
    descriptions << index.data(ctkErrorLogModel::DescriptionTextRole).toString();
  }

  d->ErrorLogDescription->setText(descriptions.join("\n"));

  emit userViewed();

  // fprintf(stdout, "onSelectionChanged: %d\n", start.msecsTo(QTime::currentTime()));
}

//---------------------------------------------------------------------------
bool ctkErrorLogWidget::eventFilter(QObject* target, QEvent* event)
{
  Q_D(ctkErrorLogWidget);
  if (target == d->ErrorLogTableView && event->type() == QEvent::KeyPress)
  {
    // Make Home/End keys jump to first/last message in the list
    // (without this, the keys would jump to the first/last cell in the current row)
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
    if (keyEvent->key() == Qt::Key_Home)
    {
      QModelIndex firstIndex = d->ErrorLogTableView->model()->index(0, 0);
      QItemSelectionModel* select = d->ErrorLogTableView->selectionModel();
      select->select(firstIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
      d->ErrorLogTableView->scrollToTop();
      return true;
    }
    else if (keyEvent->key() == Qt::Key_End)
    {
      int rowCount = d->ErrorLogTableView->model()->rowCount();
      QModelIndex lastIndex = d->ErrorLogTableView->model()->index(rowCount - 1, 0);
      QItemSelectionModel* select = d->ErrorLogTableView->selectionModel();
      select->select(lastIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
      d->ErrorLogTableView->scrollToBottom();
      return true;
    }
  }
  return this->Superclass::eventFilter(target, event);
}
