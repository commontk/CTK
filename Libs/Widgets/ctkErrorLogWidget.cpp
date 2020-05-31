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
  this->ErrorButtonFilter = ctkErrorLogLevel::Error | ctkErrorLogLevel::Critical | ctkErrorLogLevel::Fatal;
  this->WarningButtonFilter = ctkErrorLogLevel::Warning;
  this->InfoButtonFilter = ctkErrorLogLevel::Info | ctkErrorLogLevel::Debug | ctkErrorLogLevel::Trace | ctkErrorLogLevel::Status;
}

// --------------------------------------------------------------------------
void ctkErrorLogWidgetPrivate::init()
{
  Q_Q(ctkErrorLogWidget);

  // this->ShowAllEntryButton->setIcon();
  this->ShowErrorEntryButton->setIcon(q->style()->standardIcon(QStyle::SP_MessageBoxCritical));
  this->ShowWarningEntryButton->setIcon(q->style()->standardIcon(QStyle::SP_MessageBoxWarning));
  this->ShowInfoEntryButton->setIcon(q->style()->standardIcon(QStyle::SP_MessageBoxInformation));
  this->ClearButton->setIcon(q->style()->standardIcon(QStyle::SP_DialogDiscardButton));

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
}

// --------------------------------------------------------------------------
void ctkErrorLogWidget::removeEntries()
{
  Q_ASSERT(this->errorLogModel());
  this->errorLogModel()->clear();
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

  std::sort(selectedRows.begin(), selectedRows.end());

  QStringList descriptions;

  foreach(const QModelIndex& index, selectedRows)
    {
    descriptions << index.data(ctkErrorLogModel::DescriptionTextRole).toString();
    }

  d->ErrorLogDescription->setText(descriptions.join("\n"));

  // fprintf(stdout, "onSelectionChanged: %d\n", start.msecsTo(QTime::currentTime()));
}
