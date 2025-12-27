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
#include <QContextMenuEvent>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QEnterEvent>
#endif
#include <QHelpEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QScrollBar>
#include <QItemSelectionModel>
#include <QItemSelection>
#include <QToolTip>
#include <QWheelEvent>
#include <QDebug>

// CTK includes
#include "ctkDICOMStudyListView.h"
#include "ctkDICOMStudyDelegate.h"
#include "ctkDICOMStudyModel.h"
#include "ctkDICOMStudyFilterProxyModel.h"
#include "ctkDICOMStudyMergedFilterProxyModel.h"
#include "ctkDICOMSeriesTableView.h"
#include "ctkDICOMSeriesModel.h"
#include "ctkDICOMSeriesDelegate.h"
#include "ctkDICOMSeriesFilterProxyModel.h"

//------------------------------------------------------------------------------
class ctkDICOMStudyListViewPrivate
{
  Q_DECLARE_PUBLIC(ctkDICOMStudyListView);
protected:
  ctkDICOMStudyListView* const q_ptr;

public:
  ctkDICOMStudyListViewPrivate(ctkDICOMStudyListView& object);
  ~ctkDICOMStudyListViewPrivate();

  // Model state tracking
  bool ModelResetInProgress;

  // Series view management for expanded studies
  QMap<QString, ctkDICOMSeriesTableView*> SeriesViewCache; // StudyInstanceUID -> SeriesTableView

  // Hover state tracking for visual effects
  QModelIndex HoveredIndex;
  QPoint HoveredPos;

  // Mouse state tracking for collapse indicators
  QModelIndex PressedIndex;
  bool IsCollapseIndicatorPressed;
  bool IsCollapseIndicatorHovered;

  // Mouse state tracking for select all icon
  bool IsSelectAllIconPressed;
  bool IsSelectAllIconHovered;

  // Mouse state tracking for context menu button
  QModelIndex ActiveContextMenuIndex;
  bool IsContextMenuButtonPressed;
  bool IsContextMenuButtonHovered;

  // Mouse state tracking for OperationStatus icon
  bool IsOperationStatusIconPressed;
  bool IsOperationStatusIconHovered;

  // Helper methods
  void cleanupSeriesView(const QString& studyInstanceUID);
  void clearSeriesViewCache();
  void selectAllSeriesForStudy(const QModelIndex& studyIndex,
                               QItemSelectionModel::SelectionFlags selectionMode = QItemSelectionModel::Select);
  void selectSeriesInstanceUIDsForStudy(const QModelIndex& studyIndex,
                                        const QStringList &seriesInstanceUIDs,
                                        QItemSelectionModel::SelectionFlags selectionMode = QItemSelectionModel::ClearAndSelect);
  void updateSeriesViewPositions();
  QPair<ctkDICOMStudyModel*, QModelIndex> mapToStudyModelAndIndex(const QModelIndex& proxyIndex) const;
};

//------------------------------------------------------------------------------
ctkDICOMStudyListViewPrivate::ctkDICOMStudyListViewPrivate(ctkDICOMStudyListView& object)
  : q_ptr(&object)
{
  this->ModelResetInProgress = false;
  this->IsCollapseIndicatorPressed = false;
  this->IsCollapseIndicatorHovered = false;
  this->IsSelectAllIconPressed = false;
  this->IsSelectAllIconHovered = false;
  this->IsContextMenuButtonPressed = false;
  this->IsContextMenuButtonHovered = false;
  this->IsOperationStatusIconPressed = false;
  this->IsOperationStatusIconHovered = false;
}

//------------------------------------------------------------------------------
ctkDICOMStudyListViewPrivate::~ctkDICOMStudyListViewPrivate()
{
  // Clean up all cached series views
  this->clearSeriesViewCache();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListViewPrivate::cleanupSeriesView(const QString& studyInstanceUID)
{
  Q_Q(ctkDICOMStudyListView);

  if (this->SeriesViewCache.contains(studyInstanceUID))
  {
    ctkDICOMSeriesTableView* seriesView = this->SeriesViewCache.take(studyInstanceUID);
    if (seriesView)
    {
      // Disconnect all signals from the series view
      seriesView->disconnect(q);

      // Disconnect signals from the selection model
      if (seriesView->selectionModel())
      {
        seriesView->selectionModel()->disconnect(q);
      }

      // Disconnect signals from the series filter proxy model (includes gridColumnsChanged)
      if (seriesView->model())
      {
        seriesView->model()->disconnect(q);
      }

      seriesView->deleteLater();
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListViewPrivate::clearSeriesViewCache()
{
  // Get all study UIDs before iterating (to avoid iterator invalidation)
  QStringList studyUIDs = this->SeriesViewCache.keys();

  // Use cleanupSeriesView to properly disconnect signals before deleting
  foreach (const QString& studyUID, studyUIDs)
  {
    this->cleanupSeriesView(studyUID);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListViewPrivate::selectAllSeriesForStudy(const QModelIndex& studyIndex,
                                                           QItemSelectionModel::SelectionFlags selectionMode)
{
  Q_Q(ctkDICOMStudyListView);

  QString studyInstanceUID = q->studyInstanceUID(studyIndex);
  if (studyInstanceUID.isEmpty())
  {
    return;
  }

  // Select all series of this study
  ctkDICOMSeriesTableView* seriesView = this->SeriesViewCache.value(studyInstanceUID, nullptr);
  if (!seriesView || !seriesView->seriesFilterProxyModel() || !seriesView->selectionModel())
  {
    return;
  }

  ctkDICOMSeriesFilterProxyModel* proxyModel = seriesView->seriesFilterProxyModel();
  int rowCount = proxyModel->rowCount();
  int columnCount = proxyModel->columnCount();
  QItemSelection selection;
  for (int row = 0; row < rowCount; ++row)
  {
    for (int col = 0; col < columnCount; ++col)
    {
      QModelIndex idx = proxyModel->index(row, col);
      selection.select(idx, idx);
    }
  }
  seriesView->selectionModel()->select(selection, selectionMode);
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListViewPrivate::selectSeriesInstanceUIDsForStudy(const QModelIndex &studyIndex,
                                                                    const QStringList &seriesInstanceUIDs,
                                                                    QItemSelectionModel::SelectionFlags selectionMode)
{
  Q_Q(ctkDICOMStudyListView);

  QString studyInstanceUID = q->studyInstanceUID(studyIndex);
  if (studyInstanceUID.isEmpty())
  {
    return;
  }

  ctkDICOMSeriesTableView* seriesView = this->SeriesViewCache.value(studyInstanceUID, nullptr);
  if (!seriesView || !seriesView->seriesFilterProxyModel() || !seriesView->selectionModel())
  {
    return;
  }

  ctkDICOMSeriesFilterProxyModel* proxyModel = seriesView->seriesFilterProxyModel();
  int rowCount = proxyModel->rowCount();
  int columnCount = proxyModel->columnCount();
  QItemSelection selection;
  for (int row = 0; row < rowCount; ++row)
  {
    for (int col = 0; col < columnCount; ++col)
    {
      QModelIndex idx = proxyModel->index(row, col);
      QString seriesUID = idx.data(ctkDICOMSeriesModel::SeriesInstanceUIDRole).toString();
      if (!seriesInstanceUIDs.contains(seriesUID))
      {
        continue;
      }
      selection.select(idx, idx);
    }
  }
  seriesView->selectionModel()->select(selection, selectionMode);
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListViewPrivate::updateSeriesViewPositions()
{
  Q_Q(ctkDICOMStudyListView);
  if (!q->model())
  {
    return;
  }

  ctkDICOMStudyDelegate* delegate = q->studyDelegate();
  if (!delegate)
  {
    return;
  }

  // Ensure the view and viewport are properly initialized
  if (!q->viewport() || q->viewport()->size().isEmpty())
  {
    return;
  }

  // Get the visible area of the viewport
  QRect viewportRect = q->viewport()->rect();

  // Validate viewport rect
  if (!viewportRect.isValid() || viewportRect.isEmpty())
  {
    return;
  }

  // Position all series views based on their study positions
  for (int row = 0; row < q->model()->rowCount(); ++row)
  {
    QModelIndex studyIndex = q->model()->index(row, 0);
    if (!studyIndex.isValid())
    {
      continue;
    }

    // Check if study is visible according to filtering (IsVisibleRole)
    QVariant visibleVariant = studyIndex.data(ctkDICOMStudyModel::IsVisibleRole);
    bool studyVisible = !visibleVariant.isValid() || visibleVariant.toBool();
    bool isExpanded = !delegate->isStudyCollapsed(studyIndex);
    if (!isExpanded || !studyVisible)
    {
      ctkDICOMSeriesTableView* seriesView = q->getSeriesViewForStudy(studyIndex);
      if (seriesView)
      {
        seriesView->setVisible(false);
      }
      continue;
    }

    ctkDICOMSeriesTableView* seriesView = q->createSeriesViewForStudy(studyIndex);
    if (!seriesView)
    {
      continue;
    }

    // Get the visual rectangle for this study item in viewport coordinates
    QRect itemRect = q->visualRect(studyIndex);

    // Check if the study item is visible in the viewport and also visible according to filtering
    bool itemVisible = viewportRect.intersects(itemRect);

    // Study must be both viewport-visible and filter-visible
    itemVisible = itemVisible && studyVisible;

    // Check if study is expanded
    if (isExpanded && itemVisible)
    {
      // Calculate the series area rectangle relative to the study item
      QRect seriesRect = delegate->seriesAreaRect(itemRect, studyIndex);
      // Validate the series rectangle before setting geometry
      if (seriesRect.isValid() && !seriesRect.isEmpty() &&
          seriesRect.width() > 0 && seriesRect.height() > 0)
      {
        // Position the series view as a child widget
        // The coordinates are already in viewport space because visualRect() returns viewport coordinates
        seriesView->setGeometry(seriesRect);
        seriesView->setVisible(true);
        seriesView->raise(); // Ensure series view is on top
      }
      else
      {
        // Hide if invalid geometry
        seriesView->setVisible(false);
      }
    }
    else
    {
      // Hide series view when study is collapsed or not visible
      seriesView->setVisible(false);
    }
  }
}

//------------------------------------------------------------------------------
QPair<ctkDICOMStudyModel*, QModelIndex> ctkDICOMStudyListViewPrivate::mapToStudyModelAndIndex(const QModelIndex& index) const
{
  Q_Q(const ctkDICOMStudyListView);
  QPair<ctkDICOMStudyModel*, QModelIndex> sourceInfo;

  sourceInfo.first = q->studyModel();
  sourceInfo.second = index;
  ctkDICOMStudyFilterProxyModel* proxyModel = q->studyFilterProxyModel();
  if (proxyModel && index.model() == proxyModel)
  {
    sourceInfo.second = proxyModel->mapToSource(index);
  }

  ctkDICOMStudyMergedFilterProxyModel* mergedProxyModel = q->studyMergedFilterProxyModel();
  if (mergedProxyModel && index.model() == mergedProxyModel)
  {
    QPair<ctkDICOMStudyModel*, QModelIndex> mergedSourceInfo = mergedProxyModel->mapToSource(index);
    sourceInfo.first = mergedSourceInfo.first;
    sourceInfo.second = mergedSourceInfo.second;
  }

  return sourceInfo;
}

//------------------------------------------------------------------------------
ctkDICOMStudyListView::ctkDICOMStudyListView(QWidget* parent)
  : Superclass(parent)
  , d_ptr(new ctkDICOMStudyListViewPrivate(*this))
{
  // Configure list view for vertical study display
  this->setSelectionMode(QAbstractItemView::NoSelection);
  this->setDragDropMode(QAbstractItemView::NoDragDrop);
  this->setLayoutMode(QListView::Batched);

  // Enable mouse tracking for hover effects
  this->setMouseTracking(true);

  // Configure list appearance
  this->setUniformItemSizes(false);

  // Remove frame border
  this->setFrameShape(QFrame::NoFrame);
  this->setFrameShadow(QFrame::Plain);
  // To Do: add an option to disable styling
  this->verticalScrollBar()->setStyleSheet(QString(R"(
      QScrollBar:vertical {
        border: none;
        width: 10px;
        margin: 0px 0px 0px 0px;
        background: none;
      }
      QScrollBar::handle:vertical {
        min-height: 50px;
        margin: 4px 0px 4px 0px;
        width: 10px;
        border-width: 1px;
        border-radius: 2px;
        background-color: #808080;
      }
      QScrollBar::handle:vertical:hover {
        background-color: #5A5A5A;
      }
      QScrollBar::handle:vertical:pressed {
        background-color: #75a7ff;
      }

      QScrollBar::add-line:vertical {
        height: 0px;
        subcontrol-position: bottom;
        subcontrol-origin: margin;
      }
      QScrollBar::sub-line:vertical {
        height: 0 px;
        subcontrol-position: top;
        subcontrol-origin: margin;
      }
    )"
  ));

  // Ensure the list view always shows at least one study
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

  // Configure scrolling - disable scrollbars
  this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  // Set scroll mode to pixel-based scrolling for smooth scrolling
  this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

  // Set scroll step size for smooth scrolling (pixels per scroll wheel step)
  if (this->verticalScrollBar())
  {
    this->verticalScrollBar()->setSingleStep(20); // 20 pixels per scroll step
  }

  // Connect to scroll events to update series view positions
  if (this->verticalScrollBar())
  {
    this->connect(this->verticalScrollBar(), &QScrollBar::valueChanged,
                  this, &ctkDICOMStudyListView::onScrollBarValueChanged);
  }
}

//------------------------------------------------------------------------------
ctkDICOMStudyListView::~ctkDICOMStudyListView()
{
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::setModel(QAbstractItemModel* model)
{
  Q_D(ctkDICOMStudyListView);
  if (model == this->model())
  {
    return;
  }

  // Disconnect from old model
  if (this->model())
  {
    disconnect(this->model(), nullptr, this, nullptr);
  }

  // Set new model
  Superclass::setModel(model);

  // Connect to new model
  if (model)
  {
    connect(this->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &ctkDICOMStudyListView::onStudiesSelectionChanged);
    connect(model, &QAbstractItemModel::dataChanged,
            this, &ctkDICOMStudyListView::onDataChanged);
    connect(model, &QAbstractItemModel::rowsInserted,
            this, &ctkDICOMStudyListView::onRowsInserted);
    connect(model, &QAbstractItemModel::rowsAboutToBeRemoved,
            this, &ctkDICOMStudyListView::onRowsAboutToBeRemoved);
    connect(model, &QAbstractItemModel::rowsRemoved,
            this, &ctkDICOMStudyListView::onRowsRemoved);
    connect(model, &QAbstractItemModel::modelAboutToBeReset,
            this, &ctkDICOMStudyListView::onModelAboutToBeReset);
    connect(model, &QAbstractItemModel::modelReset,
            this, &ctkDICOMStudyListView::onModelReset);
  }

  // Clean previous model status
  d->ModelResetInProgress = false;
  d->IsCollapseIndicatorPressed = false;
  d->IsCollapseIndicatorHovered = false;
  d->IsSelectAllIconPressed = false;
  d->IsSelectAllIconHovered = false;
  d->IsOperationStatusIconPressed = false;
  d->IsOperationStatusIconHovered = false;

  d->clearSeriesViewCache();

  // Update layout
  this->refreshLayout();
}

//------------------------------------------------------------------------------
QStringList ctkDICOMStudyListView::selectedStudyInstanceUIDs() const
{
  QStringList selectedUIDs;
  if (!this->model())
  {
    return selectedUIDs;
  }
  if (!this->selectionModel())
  {
    return selectedUIDs;
  }

  QModelIndexList selected = this->selectionModel()->selectedIndexes();
  foreach (const QModelIndex& index, selected)
  {
    QString studyUID = this->studyInstanceUID(index);
    if (!studyUID.isEmpty() && !selectedUIDs.contains(studyUID))
    {
      selectedUIDs.append(studyUID);
    }
  }

  return selectedUIDs;
}

//------------------------------------------------------------------------------
QString ctkDICOMStudyListView::currentStudyInstanceUID() const
{
  QModelIndex current = this->currentIndex();
  return this->studyInstanceUID(current);
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::selectStudyInstanceUID(const QString& studyInstanceUID,
                                                   QItemSelectionModel::SelectionFlag flag)
{
  if (studyInstanceUID.isEmpty())
  {
    return;
  }

  QModelIndex index = this->indexForStudyInstanceUID(studyInstanceUID);
  if (index.isValid())
  {
    this->selectionModel()->select(index, flag);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::clearSelection()
{
  Q_D(ctkDICOMStudyListView);
  if (!this->selectionModel())
  {
    return;
  }
  this->selectionModel()->clearSelection();
  for (QMap<QString, ctkDICOMSeriesTableView*>::const_iterator it = d->SeriesViewCache.constBegin();
       it != d->SeriesViewCache.constEnd(); ++it)
  {
    ctkDICOMSeriesTableView* seriesView = it.value();
    if (!seriesView)
    {
      continue;
    }
    seriesView->clearSelection();
  }
  this->setCurrentIndex(QModelIndex());
}

//------------------------------------------------------------------------------
int ctkDICOMStudyListView::selectedCount() const
{
  return this->selectedStudyInstanceUIDs().count();
}

//------------------------------------------------------------------------------
QString ctkDICOMStudyListView::studyInstanceUID(const QModelIndex& index) const
{
  Q_D(const ctkDICOMStudyListView);
  if (!index.isValid())
  {
    return QString();
  }

  QPair<ctkDICOMStudyModel*, QModelIndex> sourceInfo;
  sourceInfo = d->mapToStudyModelAndIndex(index);

  if (!sourceInfo.second.isValid() || !sourceInfo.first)
  {
    return QString();
  }

  return sourceInfo.first->studyInstanceUID(sourceInfo.second);
}

//------------------------------------------------------------------------------
QModelIndex ctkDICOMStudyListView::indexForStudyInstanceUID(const QString& studyInstanceUID) const
{
  // Try with studyModel and studyFilterProxyModel
  ctkDICOMStudyModel* studyModel = this->studyModel();
  if (studyModel)
  {
    QModelIndex sourceIndex = studyModel->indexFromStudyInstanceUID(studyInstanceUID);
    ctkDICOMStudyFilterProxyModel* proxyModel = this->studyFilterProxyModel();
    if (proxyModel && sourceIndex.isValid())
    {
      return proxyModel->mapFromSource(sourceIndex);
    }
    if (sourceIndex.isValid())
    {
      return sourceIndex;
    }
  }

  // Try with studyMergedFilterProxyModel
  ctkDICOMStudyMergedFilterProxyModel* mergedProxyModel = this->studyMergedFilterProxyModel();
  if (mergedProxyModel)
  {
    QPair<ctkDICOMStudyModel*, QModelIndex> mergedSourceInfo = QPair<ctkDICOMStudyModel*, QModelIndex>(nullptr, QModelIndex());
    // Find the source model and index for the given UID
    QList<ctkDICOMStudyModel*> sourceModels = mergedProxyModel->sourceModels();
    for (QList<ctkDICOMStudyModel*>::const_iterator it = sourceModels.constBegin(); it != sourceModels.constEnd(); ++it)
    {
      ctkDICOMStudyModel* model = *it;
      QModelIndex sourceIndex = model->indexFromStudyInstanceUID(studyInstanceUID);
      if (sourceIndex.isValid())
      {
        mergedSourceInfo.first = model;
        mergedSourceInfo.second = sourceIndex;
        break;
      }
    }
    if (mergedSourceInfo.first && mergedSourceInfo.second.isValid())
    {
      return mergedProxyModel->mapFromSource(mergedSourceInfo);
    }
  }

  return QModelIndex();
}

//------------------------------------------------------------------------------
bool ctkDICOMStudyListView::isCollapseIndicatorHovered(const QModelIndex& studyIndex) const
{
  Q_D(const ctkDICOMStudyListView);
  return (studyIndex == d->HoveredIndex && d->IsCollapseIndicatorHovered);
}

//------------------------------------------------------------------------------
bool ctkDICOMStudyListView::isCollapseIndicatorPressed(const QModelIndex& studyIndex) const
{
  Q_D(const ctkDICOMStudyListView);
  return (studyIndex == d->PressedIndex && d->IsCollapseIndicatorPressed);
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::toggleStudyCollapsed(const QModelIndex& index)
{
  bool currentlyCollapsed = this->isStudyCollapsed(index);
  this->setStudyCollapsed(index, !currentlyCollapsed);
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::setStudyCollapsed(const QModelIndex& index, bool collapsed)
{
  Q_D(ctkDICOMStudyListView);
  if (!index.isValid())
  {
    return;
  }

  QPair<ctkDICOMStudyModel*, QModelIndex> sourceInfo;
  sourceInfo = d->mapToStudyModelAndIndex(index);

  if (collapsed == sourceInfo.first->isStudyCollapsed(sourceInfo.second))
  {
    return;
  }

  sourceInfo.first->setStudyCollapsed(sourceInfo.second, collapsed);
}

//------------------------------------------------------------------------------
bool ctkDICOMStudyListView::isStudyCollapsed(const QModelIndex& index) const
{
  Q_D(const ctkDICOMStudyListView);
  if (!index.isValid())
  {
    return false;
  }

  QPair<ctkDICOMStudyModel*, QModelIndex> sourceInfo;
  sourceInfo = d->mapToStudyModelAndIndex(index);

  if (!sourceInfo.second.isValid() || !sourceInfo.first)
  {
    return false;
  }

  return sourceInfo.first->isStudyCollapsed(sourceInfo.second);
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::collapseAllStudies()
{
  if (!this->model())
  {
    return;
  }

  for (int row = 0; row < this->model()->rowCount(); ++row)
  {
    QModelIndex index = this->model()->index(row, 0);
    if (index.isValid())
    {
      this->setStudyCollapsed(index, true);
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::expandAllStudies()
{
  if (!this->model())
  {
    return;
  }

  for (int row = 0; row < this->model()->rowCount(); ++row)
  {
    QModelIndex index = this->model()->index(row, 0);
    if (index.isValid())
    {
      this->setStudyCollapsed(index, false);
    }
  }
}

//------------------------------------------------------------------------------
ctkDICOMStudyModel* ctkDICOMStudyListView::studyModel() const
{
  ctkDICOMStudyFilterProxyModel* proxyModel = qobject_cast<ctkDICOMStudyFilterProxyModel*>(this->model());
  if (!proxyModel)
  {
    return nullptr;
  }

  return qobject_cast<ctkDICOMStudyModel*>(proxyModel->sourceModel());
}

//------------------------------------------------------------------------------
ctkDICOMStudyFilterProxyModel* ctkDICOMStudyListView::studyFilterProxyModel() const
{
  return qobject_cast<ctkDICOMStudyFilterProxyModel*>(this->model());
}

//------------------------------------------------------------------------------
ctkDICOMStudyMergedFilterProxyModel* ctkDICOMStudyListView::studyMergedFilterProxyModel() const
{
  return qobject_cast<ctkDICOMStudyMergedFilterProxyModel*>(this->model());
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::scrollToStudyInstanceUID(const QString& studyInstanceUID)
{
  QModelIndex index = this->indexForStudyInstanceUID(studyInstanceUID);
  if (index.isValid())
  {
    this->scrollTo(index, QAbstractItemView::EnsureVisible);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::refreshLayout(bool scheduleDelayedItemsLayout)
{
  Q_D(ctkDICOMStudyListView);
  d->updateSeriesViewPositions();
  if (scheduleDelayedItemsLayout)
  {
    this->scheduleDelayedItemsLayout();
  }
  emit this->layoutChanged();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::refreshSeriesLayout()
{
  Q_D(ctkDICOMStudyListView);
  // Iterate through all cached series views
  for (QMap<QString, ctkDICOMSeriesTableView*>::const_iterator it = d->SeriesViewCache.constBegin();
       it != d->SeriesViewCache.constEnd(); ++it)
  {
    ctkDICOMSeriesTableView* seriesView = it.value();
    if (!seriesView)
    {
      continue;
    }

    seriesView->refreshLayout(true);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::clean()
{
  Q_D(ctkDICOMStudyListView);
  // Clear all series views
  d->clearSeriesViewCache();

  // Reset interaction states
  d->HoveredIndex = QModelIndex();
  d->PressedIndex = QModelIndex();
  d->ActiveContextMenuIndex = QModelIndex();
  d->IsCollapseIndicatorPressed = false;
  d->IsSelectAllIconPressed = false;
  d->IsOperationStatusIconPressed = false;
  d->IsContextMenuButtonPressed = false;
  d->IsCollapseIndicatorHovered = false;
  d->IsSelectAllIconHovered = false;
  d->IsOperationStatusIconHovered = false;
  d->IsContextMenuButtonHovered = false;
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::refreshStudiesModels(QList<ctkDICOMStudyModel*> models)
{
  Q_D(ctkDICOMStudyListView);
  QAbstractItemModel* model = this->model();
  if (!model || d->ModelResetInProgress)
  {
    return;
  }
  ctkDICOMStudyModel* studyModel = this->studyModel();
  ctkDICOMStudyMergedFilterProxyModel* mergedStudyModel = this->studyMergedFilterProxyModel();
  if (studyModel)
  {
    if (!models.isEmpty() && !models.contains(studyModel))
    {
      return;
    }
    studyModel->refreshStudies();
  }
  else if (mergedStudyModel)
  {
    mergedStudyModel->refreshStudies(models);
  }
}

//------------------------------------------------------------------------------
ctkDICOMSeriesTableView* ctkDICOMStudyListView::createSeriesViewForStudy(const QModelIndex& studyIndex)
{
  return this->createSeriesViewForStudy(this->studyInstanceUID(studyIndex));
}

//------------------------------------------------------------------------------
ctkDICOMSeriesTableView* ctkDICOMStudyListView::createSeriesViewForStudy(const QString& studyInstanceUID)
{
  Q_D(ctkDICOMStudyListView);

  ctkDICOMSeriesTableView* seriesView = this->getSeriesViewForStudy(studyInstanceUID);
  if (seriesView)
  {
    return seriesView;
  }

  // Don't create series views during model reset to avoid crashes
  if (d->ModelResetInProgress)
  {
    return nullptr;
  }

  QModelIndex studyIndex = this->indexForStudyInstanceUID(studyInstanceUID);
  if (!studyIndex.isValid())
  {
    return nullptr;
  }


  // Create a new series table view for this study as a child widget
  seriesView = new ctkDICOMSeriesTableView(this->viewport());

  // Configure as child widget
  seriesView->setStudyInstanceUID(studyInstanceUID);
  seriesView->setVisible(false);

  // Set up the series delegate for proper rendering
  ctkDICOMSeriesDelegate* seriesDelegate = new ctkDICOMSeriesDelegate(seriesView);
  seriesView->setItemDelegate(seriesDelegate);

  QPair<ctkDICOMStudyModel*, QModelIndex> sourceInfo;
  sourceInfo = d->mapToStudyModelAndIndex(studyIndex);
  if (sourceInfo.second.isValid() && sourceInfo.first)
  {
    // Get the series model for this study using source index
    ctkDICOMSeriesModel* seriesModel = sourceInfo.first->seriesModelForStudy(sourceInfo.second);
    ctkDICOMSeriesFilterProxyModel* seriesProxyModel = sourceInfo.first->seriesFilterProxyModelForStudy(sourceInfo.second);
    if (seriesProxyModel && seriesModel)
    {
      // Set the proxy model on the view
      seriesView->setModel(seriesProxyModel);

      // Enable auto-generation and generate thumbnails only if this is one of the first studies
      // Use the proxy row for comparison with numberOfOpenedStudies
      if (studyIndex.row() < sourceInfo.first->numberOfOpenedStudies())
      {
        seriesModel->setAutoGenerateThumbnails(true);
        seriesModel->generateThumbnails();
      }
      else
      {
        seriesModel->setAutoGenerateThumbnails(false);
      }

      // Connect gridColumnsChanged to update study item size hint
      this->connect(seriesProxyModel, &ctkDICOMSeriesFilterProxyModel::gridColumnsChanged,
                    this, [this, studyInstanceUID]() {
                            this->onSeriesGridColumnsChanged(studyInstanceUID);
                          });
    }
  }

  // Cache the series view
  d->SeriesViewCache[studyInstanceUID] = seriesView;

  // Connect series selection changes to trigger repaint and synchronize selection
  if (seriesView->selectionModel())
  {
    connect(seriesView, &ctkDICOMSeriesTableView::seriesSelectionChanged,
             this, &ctkDICOMStudyListView::onSeriesSelectionChanged);
    connect(seriesView, &ctkDICOMSeriesTableView::seriesTableViewEntered,
             this, &ctkDICOMStudyListView::onSeriesViewEntered);
  }

  // Connect series activation signal (double-click)
  connect(seriesView, &ctkDICOMSeriesTableView::seriesActivated,
           this, &ctkDICOMStudyListView::seriesActivated);

  // Connect series context menu signal
  connect(seriesView, &ctkDICOMSeriesTableView::contextMenuRequested,
           this, &ctkDICOMStudyListView::onSeriesContextMenuRequested);

  return seriesView;
}

//------------------------------------------------------------------------------
QMap<QString, ctkDICOMSeriesTableView *> ctkDICOMStudyListView::createAllSeriesView()
{
  Q_D(ctkDICOMStudyListView);
  if (!this->model())
  {
    return QMap<QString, ctkDICOMSeriesTableView*>();
  }

  for (int row = 0; row < this->model()->rowCount(); ++row)
  {
    QModelIndex studyIndex = this->model()->index(row, 0);
    this->createSeriesViewForStudy(studyIndex);
  }

  return d->SeriesViewCache;
}

//------------------------------------------------------------------------------
ctkDICOMSeriesTableView* ctkDICOMStudyListView::getSeriesViewForStudy(const QModelIndex& studyIndex)
{
  return this->getSeriesViewForStudy(this->studyInstanceUID(studyIndex));
}

//------------------------------------------------------------------------------
ctkDICOMSeriesTableView* ctkDICOMStudyListView::getSeriesViewForStudy(const QString& studyInstanceUID)
{
  Q_D(ctkDICOMStudyListView);
  // Don't create series views during model reset to avoid crashes
  if (d->ModelResetInProgress)
  {
    return nullptr;
  }

  if (studyInstanceUID.isEmpty() || !d->SeriesViewCache.contains(studyInstanceUID))
  {
    return nullptr;
  }

  return d->SeriesViewCache[studyInstanceUID];
}

//------------------------------------------------------------------------------
QMap<QString, ctkDICOMSeriesTableView *> ctkDICOMStudyListView::getAllSeriesView()
{
  Q_D(ctkDICOMStudyListView);
  return d->SeriesViewCache;
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::removeSeriesViewForStudy(const QModelIndex& studyIndex)
{
  Q_D(ctkDICOMStudyListView);

  QString studyInstanceUID = this->studyInstanceUID(studyIndex);
  if (!studyInstanceUID.isEmpty())
  {
    d->cleanupSeriesView(studyInstanceUID);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::removeSeriesViewForStudy(const QString& studyInstanceUID)
{
  Q_D(ctkDICOMStudyListView);
  d->cleanupSeriesView(studyInstanceUID);
}

//------------------------------------------------------------------------------
bool ctkDICOMStudyListView::hasSeriesViewForStudy(const QModelIndex& studyIndex) const
{
  return this->hasSeriesViewForStudy(this->studyInstanceUID(studyIndex));
}

//------------------------------------------------------------------------------
bool ctkDICOMStudyListView::hasSeriesViewForStudy(const QString& studyInstanceUID) const
{
  Q_D(const ctkDICOMStudyListView);
  return d->SeriesViewCache.contains(studyInstanceUID);
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::resizeEvent(QResizeEvent* event)
{
  Superclass::resizeEvent(event);
  this->refreshLayout();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::wheelEvent(QWheelEvent *event)
{
  if (event->modifiers() & Qt::ControlModifier)
  {
    return;
  }

  this->verticalScrollBar()->event(event);
  event->accept();
  return;
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::keyPressEvent(QKeyEvent* event)
{
  switch (event->key())
  {
    case Qt::Key_Backspace:
    {
      QModelIndex current = this->currentIndex();
      if (current.isValid())
      {
        this->setStudyCollapsed(current, true);
      }
      event->accept();
      return;
    }
    break;
    case Qt::Key_Return:
    {
      QModelIndex current = this->currentIndex();
      if (current.isValid())
      {
        this->setStudyCollapsed(current, false);
      }
      event->accept();
      return;
    }
    break;
    case Qt::Key_Enter:
    {
      QModelIndex current = this->currentIndex();
      if (current.isValid())
      {
        this->setStudyCollapsed(current, false);
      }
      event->accept();
      return;
    }
    break;
    case Qt::Key_Left:
    {
      QModelIndex current = this->currentIndex();
      if (current.isValid())
      {
        this->setStudyCollapsed(current, true);
      }
      event->accept();
      return;
    }
    break;
    case Qt::Key_Right:
    {
      QModelIndex current = this->currentIndex();
      if (current.isValid())
      {
        this->setStudyCollapsed(current, false);
      }
      event->accept();
      return;
    }
    case Qt::Key_Escape:
    {
      this->clearSelection();
      event->accept();
      return;
    }
    break;
    default:
    {
      Superclass::keyPressEvent(event);
    }
    break;
  }
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::mousePressEvent(QMouseEvent* event)
{
  Q_D(ctkDICOMStudyListView);

  // Reset pressed state
  d->IsCollapseIndicatorPressed = false;
  d->IsSelectAllIconPressed = false;
  d->IsOperationStatusIconPressed = false;
  d->IsContextMenuButtonPressed = false;
  d->PressedIndex = QModelIndex();

  // Check if the click is on a study item
  QModelIndex index = this->indexAt(event->pos());
  if (index.isValid() && event->button() == Qt::LeftButton)
  {
    // Get the delegate to calculate header area
    ctkDICOMStudyDelegate* delegate = qobject_cast<ctkDICOMStudyDelegate*>(this->itemDelegate());
    if (delegate)
    {
      // Get the item rectangle
      QRect itemRect = this->visualRect(index);

      // Check if click is in the collapse indicator area
      QRect headerRect = delegate->studyHeaderRect(itemRect);
      QRect collapseRect = delegate->collapseIndicatorRect(headerRect);

      if (collapseRect.contains(event->pos()))
      {
        // Click is on the collapse indicator - mark as pressed
        d->IsCollapseIndicatorPressed = true;
        d->PressedIndex = index;
        this->viewport()->update(collapseRect);
      }

      // Check if click is in the select all icon area
      QRect selectAllIconRect = delegate->selectAllIconRect(headerRect);
      if (selectAllIconRect.contains(event->pos()))
      {
        // Click is on the select all icon - mark as pressed
        d->IsSelectAllIconPressed = true;
        d->PressedIndex = index;
        this->viewport()->update(selectAllIconRect);
      }

      // Check if click is in the context menu button area
      QRect contextMenuButtonRect = delegate->contextMenuButtonRect(headerRect);
      if (contextMenuButtonRect.contains(event->pos()))
      {
        // Click is on the context menu button - mark as pressed
        d->IsContextMenuButtonPressed = true;
        d->PressedIndex = index;
        this->viewport()->update(contextMenuButtonRect);
      }

      // Check if click is in the OperationStatus icon area
      QRect operationStatusIconRect = delegate->operationStatusButtonRect(headerRect);
      if (operationStatusIconRect.contains(event->pos()))
      {
        // Click is on the select all icon - mark as pressed
        d->IsOperationStatusIconPressed = true;
        d->PressedIndex = index;
        this->viewport()->update(operationStatusIconRect);
      }
    }
  }

  // For clicks outside study items or in series areas, handle normally
  Superclass::mousePressEvent(event);
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::mouseMoveEvent(QMouseEvent* event)
{
  Q_D(ctkDICOMStudyListView);

  QModelIndex previousHoveredIndex = d->HoveredIndex;
  d->HoveredIndex = this->indexAt(event->pos());
  d->HoveredPos = event->pos();

  bool previousCollapseHovered = d->IsCollapseIndicatorHovered;
  bool previousSelectAllHovered = d->IsSelectAllIconHovered;
  bool previousOperationStatusHovered = d->IsOperationStatusIconHovered;
  bool previousContextMenuHovered = d->IsContextMenuButtonHovered;

  d->IsCollapseIndicatorHovered = false;
  d->IsSelectAllIconHovered = false;
  d->IsOperationStatusIconHovered = false;
  d->IsContextMenuButtonHovered = false;
  d->ActiveContextMenuIndex = QModelIndex();

  // Handle hover effects for interactive elements
  if (d->HoveredIndex.isValid())
  {
    // Get the delegate to calculate interactive areas
    ctkDICOMStudyDelegate* delegate = qobject_cast<ctkDICOMStudyDelegate*>(this->itemDelegate());
    if (delegate)
    {
      // Get the item rectangle
      QRect itemRect = this->visualRect(d->HoveredIndex);
      QRect headerRect = delegate->studyHeaderRect(itemRect);
      QRect collapseRect = delegate->collapseIndicatorRect(headerRect);
      QRect selectAllIconRect = delegate->selectAllIconRect(headerRect);
      QRect contextMenuButtonRect = delegate->contextMenuButtonRect(headerRect);
      QRect operationStatusIconRect = delegate->operationStatusButtonRect(headerRect);

      // Check if mouse is over collapse indicator
      if (collapseRect.contains(event->pos()))
      {
        d->IsCollapseIndicatorHovered = true;
        this->viewport()->update(collapseRect);
      }
      // Check if mouse is over select all icon
      else if (selectAllIconRect.contains(event->pos()))
      {
        d->IsSelectAllIconHovered = true;
        this->viewport()->update(selectAllIconRect);
      }
      // Check if mouse is over context menu button
      else if (contextMenuButtonRect.contains(event->pos()))
      {
        d->IsContextMenuButtonHovered = true;
        this->viewport()->update(contextMenuButtonRect);
      }
      // Check if mouse is over OperationStatus icon
      else if (operationStatusIconRect.contains(event->pos()))
      {
        d->IsOperationStatusIconHovered = true;
        this->viewport()->update(operationStatusIconRect);
      }
    }
  }

  // If the hovered index changed, update both old and new items
  if (previousHoveredIndex != d->HoveredIndex)
  {
    if (previousHoveredIndex.isValid())
    {
      QRect oldRect = this->visualRect(previousHoveredIndex);
      this->viewport()->update(oldRect);
    }
    if (d->HoveredIndex.isValid())
    {
      QRect newRect = this->visualRect(d->HoveredIndex);
      this->viewport()->update(newRect);
    }
  }

  // Update if hover state changed for interactive elements
  if (previousCollapseHovered != d->IsCollapseIndicatorHovered ||
      previousSelectAllHovered != d->IsSelectAllIconHovered ||
      previousOperationStatusHovered != d->IsOperationStatusIconHovered ||
      previousContextMenuHovered != d->IsContextMenuButtonHovered)
  {
    if (d->HoveredIndex.isValid())
    {
      QRect itemRect = this->visualRect(d->HoveredIndex);
      this->viewport()->update(itemRect);
    }
  }

  // Call superclass to ensure Qt's internal hover tracking works
  Superclass::mouseMoveEvent(event);
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::mouseReleaseEvent(QMouseEvent* event)
{
  Q_D(ctkDICOMStudyListView);

  if (event->button() == Qt::LeftButton && d->PressedIndex.isValid())
  {
    QModelIndex index = this->indexAt(event->pos());

    // Handle collapse indicator release
    if (d->IsCollapseIndicatorPressed && index == d->PressedIndex)
    {
      // Get the delegate to calculate header area
      ctkDICOMStudyDelegate* delegate = qobject_cast<ctkDICOMStudyDelegate*>(this->itemDelegate());
      if (delegate)
      {
        // Get the item rectangle
        QRect itemRect = this->visualRect(index);
        QRect headerRect = delegate->studyHeaderRect(itemRect);
        QRect collapseRect = delegate->collapseIndicatorRect(headerRect);

        if (collapseRect.contains(event->pos()))
        {
          // Complete the toggle action
          this->toggleStudyCollapsed(index);
        }
      }
    }
    // Handle select all icon release
    else if (d->IsSelectAllIconPressed && index == d->PressedIndex)
    {
      // Get the delegate to calculate select all icon area
      ctkDICOMStudyDelegate* delegate = qobject_cast<ctkDICOMStudyDelegate*>(this->itemDelegate());
      if (delegate)
      {
        // Get the item rectangle
        QRect itemRect = this->visualRect(index);
        QRect headerRect = delegate->studyHeaderRect(itemRect);
        QRect selectAllIconRect = delegate->selectAllIconRect(headerRect);

        if (selectAllIconRect.contains(event->pos()))
        {
          // Complete the toggle action
          QItemSelectionModel::SelectionFlags selectionMode = QItemSelectionModel::Select;
          if (this->numberOfSeriesSelectedByStudy(index) != 0)
          {
            selectionMode = QItemSelectionModel::Clear;
          }
          this->selectAllSeriesByStudy(index, selectionMode);
        }
      }
    }
    // Handle context menu button release
    else if (d->IsContextMenuButtonPressed && index == d->PressedIndex)
    {
      // Get the delegate to calculate context menu button area
      ctkDICOMStudyDelegate* delegate = qobject_cast<ctkDICOMStudyDelegate*>(this->itemDelegate());
      if (delegate)
      {
        // Get the item rectangle
        QRect itemRect = this->visualRect(index);
        QRect headerRect = delegate->studyHeaderRect(itemRect);
        QRect contextMenuButtonRect = delegate->contextMenuButtonRect(headerRect);

        if (contextMenuButtonRect.contains(event->pos()))
        {
          // Open context menu at global position
          this->onStudyContextMenuRequested(event->globalPos(), index);
          event->accept();
        }
      }
    }
    else if (d->IsOperationStatusIconPressed && index == d->PressedIndex)
    {
      // Get the delegate to calculate select all icon area
      ctkDICOMStudyDelegate* delegate = qobject_cast<ctkDICOMStudyDelegate*>(this->itemDelegate());
      if (delegate)
      {
        // Get the item rectangle
        QRect itemRect = this->visualRect(index);
        QRect headerRect = delegate->studyHeaderRect(itemRect);
        QRect operationStatusIconRect = delegate->operationStatusButtonRect(headerRect);

        if (operationStatusIconRect.contains(event->pos()))
        {
          QString studyInstanceUID = this->studyInstanceUID(index);
          this->studyModel()->forceUpdateStudyJobs(studyInstanceUID);
          event->accept();
        }
      }
    }

    // Reset pressed state
    d->IsCollapseIndicatorPressed = false;
    d->IsSelectAllIconPressed = false;
    d->IsOperationStatusIconPressed = false;
    d->IsContextMenuButtonPressed = false;
    QModelIndex oldPressedIndex = d->PressedIndex;
    d->PressedIndex = QModelIndex();

    // Trigger repaint to remove pressed state
    if (oldPressedIndex.isValid())
    {
      QRect itemRect = this->visualRect(oldPressedIndex);
      this->update(itemRect);
      this->viewport()->update();
    }

    Superclass::mouseReleaseEvent(event);
    return;
  }

  // Reset pressed state for any other release
  d->IsCollapseIndicatorPressed = false;
  d->IsSelectAllIconPressed = false;
  d->IsOperationStatusIconPressed = false;
  d->PressedIndex = QModelIndex();

  // For other releases, handle normally but don't call superclass to prevent selection
  Superclass::mouseReleaseEvent(event);
}

//------------------------------------------------------------------------------
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void ctkDICOMStudyListView::enterEvent(QEnterEvent *event)
#else
void ctkDICOMStudyListView::enterEvent(QEvent *event)
#endif
{
  Q_UNUSED(event)
  emit studyListViewEntered();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::leaveEvent(QEvent* event)
{
  Q_D(ctkDICOMStudyListView);

  // Reset all hover states when mouse leaves the widget
  bool needsUpdate = false;

  if (d->IsCollapseIndicatorHovered)
  {
    d->IsCollapseIndicatorHovered = false;
    needsUpdate = true;
  }

  if (d->IsSelectAllIconHovered)
  {
    d->IsSelectAllIconHovered = false;
    needsUpdate = true;
  }

  if (d->IsContextMenuButtonHovered)
  {
    d->IsContextMenuButtonHovered = false;
    needsUpdate = true;
  }

  if (d->IsOperationStatusIconHovered)
  {
    d->IsOperationStatusIconHovered = false;
    needsUpdate = true;
  }

  if (needsUpdate)
  {
    this->viewport()->update();
  }

  // Call parent implementation
  Superclass::leaveEvent(event);
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::contextMenuEvent(QContextMenuEvent* event)
{
  Q_D(ctkDICOMStudyListView);

  // Get the index at the click position
  QModelIndex clickedIndex = this->indexAt(event->pos());

  if (!clickedIndex.isValid())
  {
    event->accept();
    return;
  }

  // Get the delegate to check if click is on study header
  ctkDICOMStudyDelegate* delegate = this->studyDelegate();
  if (!delegate)
  {
    event->accept();
    return;
  }
  QRect itemRect = this->visualRect(clickedIndex);
  QRect headerRect = delegate->studyHeaderRect(itemRect);

  // Check if the click is within the study header area (not in series area)
  if (!headerRect.contains(event->pos()))
  {
    event->accept();
    return;
  }
  // Right-click on study header - select all series of this study and show context menu
  QString studyInstanceUID = this->studyInstanceUID(clickedIndex);
  if (studyInstanceUID.isEmpty())
  {
    event->accept();
    return;
  }

  d->selectAllSeriesForStudy(clickedIndex);

  QStringList selectedUIDs = this->selectedStudyInstanceUIDs();
  emit studyContextMenuRequested(event->globalPos(), selectedUIDs);
  event->accept();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
  // Call base class implementation
  Superclass::currentChanged(current, previous);

  // Trigger repaint for both old and new current items to update visual state
  if (previous.isValid())
  {
    this->viewport()->update(this->visualRect(previous));
  }
  if (current.isValid())
  {
    this->viewport()->update(this->visualRect(current));
  }
}

//------------------------------------------------------------------------------
QSize ctkDICOMStudyListView::viewportSizeHint() const
{
  // Provide a reasonable default size
  return QSize(400, 600);
}

//------------------------------------------------------------------------------
int ctkDICOMStudyListView::sizeHintForRow(int row) const
{
  QModelIndex index = this->model()->index(row, 0);
  if (!index.isValid())
  {
    return -1;
  }

  ctkDICOMStudyDelegate* delegate = this->studyDelegate();
  if (!delegate)
  {
    return -1;
  }

  QStyleOptionViewItem option;
  option.initFrom(this);
  option.rect = QRect(0, 0, this->width(), -1);
  option.decorationSize = this->iconSize();
  return delegate->sizeHint(option, index).height();
}

//------------------------------------------------------------------------------
int ctkDICOMStudyListView::sizeHintForColumn(int col) const
{
  QModelIndex index = this->model()->index(0, col);
  if (!index.isValid())
  {
    return -1;
  }

  ctkDICOMStudyDelegate* delegate = this->studyDelegate();
  if (!delegate)
  {
    return -1;
  }

  QStyleOptionViewItem option;
  option.initFrom(this);
  option.rect = QRect(0, 0, this->width(), -1);
  option.decorationSize = this->iconSize();
  return delegate->sizeHint(option, index).width();
}

//------------------------------------------------------------------------------
ctkDICOMStudyDelegate* ctkDICOMStudyListView::studyDelegate() const
{
  return qobject_cast<ctkDICOMStudyDelegate*>(this->itemDelegate());
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::onStudiesSelectionChanged()
{
  QStringList selectedUIDs = this->selectedStudyInstanceUIDs();
  emit this->studySelectionChanged(selectedUIDs);
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::onSeriesSelectionChanged(const QString& studyInstanceUID, const QStringList& selectedSeriesInstanceUIDs)
{
  if (studyInstanceUID.isEmpty())
  {
    return;
  }

  int numberOfSelectedSeries = selectedSeriesInstanceUIDs.count();
  this->selectStudyInstanceUID(studyInstanceUID, numberOfSelectedSeries > 0 ? QItemSelectionModel::Select : QItemSelectionModel::Deselect);
  emit this->seriesSelectionChanged(studyInstanceUID, selectedSeriesInstanceUIDs);
  this->viewport()->update();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::onSeriesContextMenuRequested(const QPoint& globalPos, const QStringList& selectedSeriesInstanceUIDs)
{
  Q_D(ctkDICOMStudyListView);
  Q_UNUSED(selectedSeriesInstanceUIDs);

  // Collect selections from ALL series views (across all studies/patients)
  QStringList allSelectedSeriesUIDs;

  // Iterate through all cached series views
  for (QMap<QString, ctkDICOMSeriesTableView*>::const_iterator it = d->SeriesViewCache.constBegin();
       it != d->SeriesViewCache.constEnd(); ++it)
  {
    ctkDICOMSeriesTableView* seriesView = it.value();
    if (seriesView)
    {
      QStringList seriesUIDs = seriesView->selectedSeriesInstanceUIDs();
      allSelectedSeriesUIDs.append(seriesUIDs);
    }
  }

  // Remove duplicates (though there shouldn't be any)
  allSelectedSeriesUIDs.removeDuplicates();

  // Forward the signal with all selected series from all studies
  emit this->seriesContextMenuRequested(globalPos, allSelectedSeriesUIDs);
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::onStudyContextMenuRequested(const QPoint& globalPos, const QModelIndex& studyIndex)
{
  Q_D(ctkDICOMStudyListView);

  if (!studyIndex.isValid())
  {
    return;
  }

  QString studyInstanceUID = this->studyInstanceUID(studyIndex);
  if (studyInstanceUID.isEmpty())
  {
    return;
  }

  this->createSeriesViewForStudy(studyInstanceUID);

  // Track which study has an active context menu
  d->ActiveContextMenuIndex = studyIndex;

  d->selectAllSeriesForStudy(studyIndex);
  QStringList selectedStudyUIDs = this->selectedStudyInstanceUIDs();

  // Calculate the position below the context menu button
  ctkDICOMStudyDelegate* delegate = this->studyDelegate();
  QPoint menuPos = globalPos;
  if (delegate)
  {
    QRect itemRect = this->visualRect(studyIndex);
    QRect headerRect = delegate->studyHeaderRect(itemRect);
    QRect buttonRect = delegate->contextMenuButtonRect(headerRect);
    // Position menu at the bottom-center of the button
    QPoint buttonBottomCenter = this->viewport()->mapToGlobal(QPoint(buttonRect.left() - 1, buttonRect.bottom() + 2));
    menuPos = buttonBottomCenter;
  }

  // Emit the study context menu signal with adjusted position
  emit this->studyContextMenuRequested(menuPos, selectedStudyUIDs);
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::onScrollBarValueChanged(int value)
{
  Q_UNUSED(value);

  this->refreshLayout(false);
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
  if(roles.contains(ctkDICOMStudyModel::OperationStatusRole))
  {
    // Update only the affected visual rects
    for (int row = topLeft.row(); row <= bottomRight.row(); ++row)
    {
      QModelIndex index = this->model()->index(row, topLeft.column());
      if (index.isValid())
      {
        this->viewport()->update(this->visualRect(index));
      }
    }
  }
  else
  {
    this->refreshLayout();
  }
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::onRowsInserted(const QModelIndex& parent, int first, int last)
{
  Q_UNUSED(parent);
  Q_UNUSED(first);
  Q_UNUSED(last);

  this->refreshLayout();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::onRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
{
  Q_D(ctkDICOMStudyListView);
  Q_UNUSED(parent);

  // Clean up series views for studies about to be removed
  if (!this->model())
  {
    return;
  }

  for (int row = first; row <= last; ++row)
  {
    QModelIndex studyIndex = this->model()->index(row, 0, parent);
    if (studyIndex.isValid())
    {
      QString studyUID = this->studyInstanceUID(studyIndex);
      if (!studyUID.isEmpty())
      {
        d->cleanupSeriesView(studyUID);
      }
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::onRowsRemoved(const QModelIndex& parent, int first, int last)
{
  Q_UNUSED(parent);
  Q_UNUSED(first);
  Q_UNUSED(last);

  this->refreshLayout();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::onModelAboutToBeReset()
{
  Q_D(ctkDICOMStudyListView);

  // Mark that model reset is in progress
  d->ModelResetInProgress = true;

  // Clear all series view cache to avoid issues during reset
  d->clearSeriesViewCache();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::onModelReset()
{
  Q_D(ctkDICOMStudyListView);

  // Mark that model reset is complete
  d->ModelResetInProgress = false;

  this->refreshLayout();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::onNumberOfOpenedStudiesChanged(int count)
{
  QAbstractItemModel* model = this->model();
  if (!model)
  {
    return;
  }
  ctkDICOMStudyModel* studyModel = this->studyModel();
  ctkDICOMStudyMergedFilterProxyModel* mergedStudyModel = this->studyMergedFilterProxyModel();
  if (studyModel)
  {
    ctkDICOMStudyFilterProxyModel* proxyModel = this->studyFilterProxyModel();
    int rowCount = proxyModel->rowCount();
    for (int row = 0; row < rowCount; ++row)
    {
      QModelIndex proxyIndex = model->index(row, 0);
      if (!proxyIndex.isValid())
      {
        continue;
      }
      QModelIndex sourceIndex = proxyIndex;
      if (proxyModel && proxyIndex.model() == proxyModel)
      {
        sourceIndex = proxyModel->mapToSource(proxyIndex);
      }
      studyModel->setStudyCollapsed(sourceIndex, row >= count);
    }
  }
  else if (mergedStudyModel)
  {
    int rowCount = mergedStudyModel->rowCount();
    for (int row = 0; row < rowCount; ++row)
    {
      QModelIndex mergedIndex = mergedStudyModel->index(row, 0);
      if (!mergedIndex.isValid())
      {
        continue;
      }

      QPair<ctkDICOMStudyModel*, QModelIndex> sourceInfo = mergedStudyModel->mapToSource(mergedIndex);
      ctkDICOMStudyModel* sourceStudyModel = sourceInfo.first;
      QModelIndex sourceIndex = sourceInfo.second;
      if (sourceStudyModel)
      {
        sourceStudyModel->setStudyCollapsed(sourceIndex, row >= count);
      }
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::selectAllSeries(QItemSelectionModel::SelectionFlags selectionMode)
{
  Q_D(ctkDICOMStudyListView);
  if (!this->model())
  {
    return;
  }
  for (int row = this->model()->rowCount() - 1; row >= 0; --row)
  {
    QModelIndex studyIndex = this->model()->index(row, 0);
    if (studyIndex.isValid())
    {
      d->selectAllSeriesForStudy(studyIndex, selectionMode);
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::selectAllSeriesByPatient(QString patientUID,
                                                     QItemSelectionModel::SelectionFlags selectionMode)
{
  Q_D(ctkDICOMStudyListView);
  if (!this->model())
  {
    return;
  }

  for (int row = this->model()->rowCount() - 1; row >= 0; --row)
  {
    QModelIndex studyIndex = this->model()->index(row, 0);
    if (!studyIndex.isValid())
    {
      continue;
    }

    // Get patient ID from the study index
    QString studyPatientUID = studyIndex.data(ctkDICOMStudyModel::PatientUIDRole).toString();
    // Only select series if this study belongs to the specified patient
    if (studyPatientUID == patientUID)
    {
      d->selectAllSeriesForStudy(studyIndex, selectionMode);
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::selectSeriesInstanceUIDs(const QStringList &seriesInstanceUIDs)
{
  Q_D(ctkDICOMStudyListView);
  if (!this->model())
  {
    return;
  }
  for (int row = this->model()->rowCount() - 1; row >= 0; --row)
  {
    QModelIndex studyIndex = this->model()->index(row, 0);
    if (studyIndex.isValid())
    {
      d->selectSeriesInstanceUIDsForStudy(studyIndex, seriesInstanceUIDs, QItemSelectionModel::ClearAndSelect);
    }
  }
}

//------------------------------------------------------------------------------
QStringList ctkDICOMStudyListView::selectedStudiesInstanceUIDsByPatient(QString patientUID) const
{
  if (!this->model())
  {
    return QStringList();
  }

  QStringList studyInstanceUIDs;
  for (int row = this->model()->rowCount() - 1; row >= 0; --row)
  {
    QModelIndex studyIndex = this->model()->index(row, 0);
    if (!studyIndex.isValid())
    {
      continue;
    }

    QString studyPatientUID = studyIndex.data(ctkDICOMStudyModel::PatientUIDRole).toString();
    if (studyPatientUID == patientUID)
    {
      QString studyInstanceUID = this->studyInstanceUID(studyIndex);
      if (!studyInstanceUID.isEmpty())
      {
        QModelIndex index = this->indexForStudyInstanceUID(studyInstanceUID);
        if (index.isValid() && this->selectionModel()->isSelected(index))
        {
          studyInstanceUIDs.append(studyInstanceUID);
        }
      }
    }
  }

  return studyInstanceUIDs;
}

//------------------------------------------------------------------------------
QStringList ctkDICOMStudyListView::selectedSeriesInstanceUIDsByPatient(QString patientUID) const
{
  if (!this->model())
  {
    return QStringList();
  }

  QStringList seriesInstanceUIDs;
  for (int row = this->model()->rowCount() - 1; row >= 0; --row)
  {
    QModelIndex studyIndex = this->model()->index(row, 0);
    if (!studyIndex.isValid())
    {
      continue;
    }

    // Get patient ID from the study index
    QString studyPatientUID = studyIndex.data(ctkDICOMStudyModel::PatientUIDRole).toString();

    // Only count series if this study belongs to the specified patient
    if (studyPatientUID == patientUID)
    {
      seriesInstanceUIDs.append(this->selectedSeriesInstanceUIDsByStudy(studyIndex));
    }
  }

  return seriesInstanceUIDs;
}

//------------------------------------------------------------------------------
QStringList ctkDICOMStudyListView::selectedSeriesInstanceUIDsByStudy(const QModelIndex &studyIndex) const
{
  Q_D(const ctkDICOMStudyListView);

  QString studyInstanceUID = this->studyInstanceUID(studyIndex);
  if (studyInstanceUID.isEmpty())
  {
    return QStringList();
  }

  // Get the series view for this study
  ctkDICOMSeriesTableView* seriesView = d->SeriesViewCache.value(studyInstanceUID, nullptr);
  if (!seriesView || !seriesView->model() || !seriesView->selectionModel())
  {
    return QStringList();
  }

  return seriesView->selectedSeriesInstanceUIDs();
}

//------------------------------------------------------------------------------
QStringList ctkDICOMStudyListView::selectedSeriesInstanceUIDs() const
{
  if (!this->model())
  {
    return QStringList();
  }
  QStringList seriesInstanceUIDs;
  for (int row = this->model()->rowCount() - 1; row >= 0; --row)
  {
    QModelIndex studyIndex = this->model()->index(row, 0);
    if (studyIndex.isValid())
    {
      seriesInstanceUIDs.append(this->selectedSeriesInstanceUIDsByStudy(studyIndex));
    }
  }

  return seriesInstanceUIDs;
}

//------------------------------------------------------------------------------
int ctkDICOMStudyListView::numberOfStudiesSelectedByPatient(QString patientUID) const
{
  return this->selectedStudiesInstanceUIDsByPatient(patientUID).count();
}

//------------------------------------------------------------------------------
int ctkDICOMStudyListView::numberOfSeriesSelectedByPatient(QString patientUID) const
{
  return this->selectedSeriesInstanceUIDsByPatient(patientUID).count();
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::selectAllSeriesByStudy(const QModelIndex& studyIndex,
                                                   QItemSelectionModel::SelectionFlags selectionMode)
{
  Q_D(ctkDICOMStudyListView);

  QString studyInstanceUID = this->studyInstanceUID(studyIndex);
  if (studyInstanceUID.isEmpty())
  {
    return;
  }

  this->createSeriesViewForStudy(studyInstanceUID);
  d->selectAllSeriesForStudy(studyIndex, selectionMode);
}

//------------------------------------------------------------------------------
int ctkDICOMStudyListView::numberOfSeriesSelectedByStudy(const QModelIndex& studyIndex) const
{
  return this->selectedSeriesInstanceUIDsByStudy(studyIndex).count();
}

//------------------------------------------------------------------------------
bool ctkDICOMStudyListView::isSelectAllIconHovered(const QModelIndex& studyIndex) const
{
  Q_D(const ctkDICOMStudyListView);
  return (studyIndex == d->HoveredIndex && d->IsSelectAllIconHovered);
}

//------------------------------------------------------------------------------
bool ctkDICOMStudyListView::isSelectAllIconPressed(const QModelIndex& studyIndex) const
{
  Q_D(const ctkDICOMStudyListView);
  return (studyIndex == d->PressedIndex && d->IsSelectAllIconPressed);
}

//------------------------------------------------------------------------------
bool ctkDICOMStudyListView::isContextMenuButtonHovered(const QModelIndex& studyIndex) const
{
  Q_D(const ctkDICOMStudyListView);
  return (studyIndex == d->HoveredIndex && d->IsContextMenuButtonHovered);
}

//------------------------------------------------------------------------------
bool ctkDICOMStudyListView::isContextMenuButtonPressed(const QModelIndex& studyIndex) const
{
  Q_D(const ctkDICOMStudyListView);
  return (studyIndex == d->PressedIndex && d->IsContextMenuButtonPressed);
}

//------------------------------------------------------------------------------
bool ctkDICOMStudyListView::hasActiveContextMenu(const QModelIndex& studyIndex) const
{
  Q_D(const ctkDICOMStudyListView);
  return (studyIndex.isValid() && d->ActiveContextMenuIndex.isValid() &&
          studyIndex.row() == d->ActiveContextMenuIndex.row());
}

//------------------------------------------------------------------------------
bool ctkDICOMStudyListView::isOperationStatusIconHovered(const QModelIndex& studyIndex) const
{
  Q_D(const ctkDICOMStudyListView);
  return (studyIndex == d->HoveredIndex && d->IsOperationStatusIconHovered);
}

//------------------------------------------------------------------------------
bool ctkDICOMStudyListView::isOperationStatusIconPressed(const QModelIndex& studyIndex) const
{
  Q_D(const ctkDICOMStudyListView);
  return (studyIndex == d->PressedIndex && d->IsOperationStatusIconPressed);
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::onSeriesGridColumnsChanged(const QString& studyInstanceUID)
{
  // Find the study index for this series
  QModelIndex studyIdx = this->indexForStudyInstanceUID(studyInstanceUID);
  if (studyIdx.isValid())
  {
    this->refreshLayout();
  }
}

//------------------------------------------------------------------------------
void ctkDICOMStudyListView::onSeriesViewEntered()
{
  QEvent leaveEv(QEvent::Leave);
  this->leaveEvent(&leaveEv);
}
