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
#include <QHeaderView>
#include <QHelpEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QScrollBar>
#include <QItemSelectionModel>
#include <QItemSelection>
#include <QToolTip>
#include <QWheelEvent>
#include <QDebug>

// CTK includes
#include "ctkDICOMSeriesTableView.h"
#include "ctkDICOMSeriesDelegate.h"
#include "ctkDICOMSeriesModel.h"
#include "ctkDICOMSeriesFilterProxyModel.h"

//------------------------------------------------------------------------------
class ctkDICOMSeriesTableViewPrivate
{
public:
  ctkDICOMSeriesTableViewPrivate();
  QString StudyInstanceUID;

  // Context menu button state
  QModelIndex ContextMenuButtonHoveredIndex;
  QModelIndex ContextMenuButtonPressedIndex;
  QModelIndex ActiveContextMenuIndex;
  QModelIndex OperationStatusButtonHoveredIndex;
  QModelIndex OperationStatusButtonPressedIndex;
};

//------------------------------------------------------------------------------
ctkDICOMSeriesTableViewPrivate::ctkDICOMSeriesTableViewPrivate()
{
  this->StudyInstanceUID = "";
  this->ContextMenuButtonHoveredIndex = QModelIndex();
  this->ContextMenuButtonPressedIndex = QModelIndex();
  this->OperationStatusButtonHoveredIndex = QModelIndex();
  this->OperationStatusButtonPressedIndex = QModelIndex();
}

//------------------------------------------------------------------------------
ctkDICOMSeriesTableView::ctkDICOMSeriesTableView(QWidget* parent)
  : Superclass(parent)
  , d_ptr(new ctkDICOMSeriesTableViewPrivate)
{
  // Configure table view for horizontal series display (single row with multiple columns)
  this->setSelectionMode(QAbstractItemView::MultiSelection);
  this->setDragDropMode(QAbstractItemView::NoDragDrop);

  // Hide headers - we want a clean grid
  this->horizontalHeader()->setVisible(false);
  this->verticalHeader()->setVisible(false);

  // Configure table appearance
  this->setShowGrid(false);

  // Disable default selection highlighting since we handle it in the delegate
  this->setStyleSheet(
    "QTableView { "
    "  border: none; "
    "  background: transparent; "
    "  gridline-color: transparent; "
    "  selection-background-color: transparent; "
    "}"
    "QTableView::item { "
    "  border: none; "
    "  background: transparent; "
    "}"
    "QTableView::item:focus { "
    "  background: transparent; "
    "  border: none; "
    "  outline: none; "
    "}"
    "QTableView::item:selected { "
    "  background: transparent; "
    "}"
  );

  // Remove frame and set transparent background
  this->setFrameStyle(QFrame::NoFrame);

  // Ensure the table view always shows at least one thumbnail and its description
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

  // Ensure headers are completely hidden and take no space
  this->horizontalHeader()->setMinimumSectionSize(0);
  this->horizontalHeader()->setDefaultSectionSize(0);
  this->verticalHeader()->setMinimumSectionSize(0);
  this->verticalHeader()->setDefaultSectionSize(0);

  // Configure scrolling - horizontal scroll for series navigation
  this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  // Set scroll mode to pixel-based scrolling for smooth scrolling
  this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

  // Enable mouse tracking for hover effects
  this->setMouseTracking(true);
}

//------------------------------------------------------------------------------
ctkDICOMSeriesTableView::~ctkDICOMSeriesTableView()
{
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesTableView::setStudyInstanceUID(const QString& studyInstanceUID)
{
  Q_D(ctkDICOMSeriesTableView);
  d->StudyInstanceUID = studyInstanceUID;
  emit this->studyInstanceUIDChanged(studyInstanceUID);
}

//------------------------------------------------------------------------------
QString ctkDICOMSeriesTableView::studyInstanceUID() const
{
  Q_D(const ctkDICOMSeriesTableView);
  return d->StudyInstanceUID;
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesTableView::setModel(QAbstractItemModel* model)
{
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
            this, &ctkDICOMSeriesTableView::onSeriesSelectionChanged);
    connect(model, &QAbstractItemModel::dataChanged,
            this, &ctkDICOMSeriesTableView::onDataChanged);
    connect(model, &QAbstractItemModel::rowsInserted,
            this, &ctkDICOMSeriesTableView::onRowsInserted);
    connect(model, &QAbstractItemModel::rowsRemoved,
            this, &ctkDICOMSeriesTableView::onRowsRemoved);
    connect(model, &QAbstractItemModel::modelReset,
            this, &ctkDICOMSeriesTableView::onModelReset);
  }

  // Update layout
  this->updateGridLayout();
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesTableView::setItemDelegate(QAbstractItemDelegate* delegate)
{
  Superclass::setItemDelegate(delegate);
}

//------------------------------------------------------------------------------
QStringList ctkDICOMSeriesTableView::selectedSeriesInstanceUIDs() const
{
  QStringList seriesInstanceUIDs;
  QItemSelection selection = this->selectionModel()->selection();
  for (const QItemSelectionRange& range : selection)
  {
    for (int row = range.top(); row <= range.bottom(); ++row)
    {
      for (int col = range.left(); col <= range.right(); ++col)
      {
        QModelIndex selectedIndex = range.model()->index(row, col);
        QString seriesInstanceUID = this->seriesInstanceUID(selectedIndex);
        if (!seriesInstanceUID.isEmpty())
        {
          seriesInstanceUIDs.append(seriesInstanceUID);
        }
      }
    }
  }

  return seriesInstanceUIDs;
}

//------------------------------------------------------------------------------
QString ctkDICOMSeriesTableView::currentSeriesInstanceUID() const
{
  QModelIndex currentIndex = this->selectionModel()->currentIndex();
  return this->seriesInstanceUID(currentIndex);
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesTableView::selectSeriesInstanceUID(const QString& seriesInstanceUID,
                                                      QItemSelectionModel::SelectionFlag flag)
{
  QModelIndex index = this->indexForSeriesInstanceUID(seriesInstanceUID);
  if (!index.isValid())
  {
    return;
  }

  this->selectionModel()->select(index, flag);
  this->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Current);
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesTableView::selectSeriesInstanceUIDs(const QStringList &seriesInstanceUIDs,
                                                       QItemSelectionModel::SelectionFlag flag)
{
  QItemSelection selection;
  for (const QString& seriesInstanceUID : seriesInstanceUIDs)
  {
    QModelIndex index = this->indexForSeriesInstanceUID(seriesInstanceUID);
    if (!index.isValid())
    {
      continue;
    }
    selection.select(index, index);
  }
  this->selectionModel()->select(selection, flag);
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesTableView::clearSelection()
{
  if (!this->selectionModel())
  {
    return;
  }
  this->selectionModel()->clearSelection();
  this->setCurrentIndex(QModelIndex());
}

//------------------------------------------------------------------------------
int ctkDICOMSeriesTableView::selectedCount() const
{
  // Return count of unique selected series (not all selected indexes)
  return this->selectedSeriesInstanceUIDs().size();
}

//------------------------------------------------------------------------------
QString ctkDICOMSeriesTableView::seriesInstanceUID(const QModelIndex& index) const
{
  if (!index.isValid())
  {
    return QString();
  }

  return this->model()->data(index, ctkDICOMSeriesModel::SeriesInstanceUIDRole).toString();
}

//------------------------------------------------------------------------------
QModelIndex ctkDICOMSeriesTableView::indexForSeriesInstanceUID(const QString& seriesInstanceUID) const
{
  ctkDICOMSeriesFilterProxyModel* proxyModel = this->seriesFilterProxyModel();
  if (proxyModel)
  {
    return proxyModel->indexForSeriesInstanceUID(seriesInstanceUID);
  }

  return QModelIndex();
}

//------------------------------------------------------------------------------
ctkDICOMSeriesModel* ctkDICOMSeriesTableView::seriesModel() const
{
  ctkDICOMSeriesModel* soruceModel = qobject_cast<ctkDICOMSeriesModel*>(this->model());
  if (soruceModel)
  {
    return soruceModel;
  }

  ctkDICOMSeriesFilterProxyModel* proxyModel = qobject_cast<ctkDICOMSeriesFilterProxyModel*>(this->model());
  if (!proxyModel)
  {
    return nullptr;
  }

  return qobject_cast<ctkDICOMSeriesModel*>(proxyModel->sourceModel());
}

//------------------------------------------------------------------------------
ctkDICOMSeriesFilterProxyModel* ctkDICOMSeriesTableView::seriesFilterProxyModel() const
{
  return qobject_cast<ctkDICOMSeriesFilterProxyModel*>(this->model());
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesTableView::scrollToSeriesInstanceUID(const QString& seriesInstanceUID)
{
  QModelIndex index = this->indexForSeriesInstanceUID(seriesInstanceUID);
  if (index.isValid())
  {
    this->scrollTo(index, QAbstractItemView::EnsureVisible);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesTableView::refreshLayout(bool forceGridColumnsUpdate)
{
  this->updateGridLayout(forceGridColumnsUpdate);
  this->viewport()->update();
}

//------------------------------------------------------------------------------
ctkDICOMSeriesDelegate* ctkDICOMSeriesTableView::seriesDelegate() const
{
  return qobject_cast<ctkDICOMSeriesDelegate*>(this->itemDelegate());
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesTableView::resizeEvent(QResizeEvent* event)
{
  Superclass::resizeEvent(event);
  this->refreshLayout();
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesTableView::wheelEvent(QWheelEvent* event)
{
  if (event->modifiers() & Qt::ControlModifier)
  {
    return;
  }

  // Forward wheel events directly to the parent study list view
  // This allows scrolling the study list when the mouse is over the series view
  QWidget* widget = this->parentWidget();
  while (widget)
  {
    QAbstractScrollArea* scrollArea = qobject_cast<QAbstractScrollArea*>(widget);
    if (scrollArea)
    {
      QScrollBar* verticalScroolBar = scrollArea->verticalScrollBar();
      if (verticalScroolBar)
      {
        verticalScroolBar->event(event);
        return;
      }
    }

    widget = widget->parentWidget();
  }

  Superclass::wheelEvent(event);
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesTableView::keyPressEvent(QKeyEvent* event)
{
  switch (event->key())
  {
    case Qt::Key_Return:
    {
      QString currentUID = this->currentSeriesInstanceUID();
      this->selectSeriesInstanceUID(currentUID);
      event->accept();
      return;
    }
    break;
    case Qt::Key_Enter:
    {
      QString currentUID = this->currentSeriesInstanceUID();
      this->selectSeriesInstanceUID(currentUID);
      event->accept();
      return;
    }
    break;
    case Qt::Key_Backspace:
    {
      QString currentUID = this->currentSeriesInstanceUID();
      this->selectSeriesInstanceUID(currentUID, QItemSelectionModel::Deselect);
      event->accept();
      return;
    }
    break;
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
void ctkDICOMSeriesTableView::mouseMoveEvent(QMouseEvent* event)
{
  Q_D(ctkDICOMSeriesTableView);

  QModelIndex index = this->indexAt(event->pos());
  QModelIndex previousHoveredIndex;
  if (d->ContextMenuButtonHoveredIndex.isValid())
  {
    previousHoveredIndex = d->ContextMenuButtonHoveredIndex;
  }
  else if (d->OperationStatusButtonHoveredIndex.isValid())
  {
    previousHoveredIndex = d->OperationStatusButtonHoveredIndex;
  }

  d->ContextMenuButtonHoveredIndex = QModelIndex();
  d->ActiveContextMenuIndex = QModelIndex();
  d->OperationStatusButtonHoveredIndex = QModelIndex();

  if (index.isValid())
  {
    // Check if mouse is over the context menu button
    ctkDICOMSeriesDelegate* delegate = this->seriesDelegate();
    if (delegate)
    {
      QRect itemRect = this->visualRect(index);
      if (delegate->isContextMenuButtonAt(event->pos(), itemRect, index))
      {
        d->ContextMenuButtonHoveredIndex = index;
      }
      else if (delegate->isStatusButtonAt(event->pos(), itemRect, index))
      {
        d->OperationStatusButtonHoveredIndex = index;
      }
    }
  }

  // Trigger repaint if hover state changed
  if (previousHoveredIndex != d->ContextMenuButtonHoveredIndex ||
      previousHoveredIndex != d->OperationStatusButtonHoveredIndex)
  {
    if (previousHoveredIndex.isValid())
    {
      this->viewport()->update(this->visualRect(previousHoveredIndex));
    }
    if (d->ContextMenuButtonHoveredIndex.isValid())
    {
      this->viewport()->update(this->visualRect(d->ContextMenuButtonHoveredIndex));
    }
    if (d->OperationStatusButtonHoveredIndex.isValid())
    {
      this->viewport()->update(this->visualRect(d->OperationStatusButtonHoveredIndex));
    }
  }

  Superclass::mouseMoveEvent(event);
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesTableView::mousePressEvent(QMouseEvent* event)
{
  Q_D(ctkDICOMSeriesTableView);

  if (event->button() == Qt::LeftButton)
  {
    QModelIndex index = this->indexAt(event->pos());
    if (index.isValid())
    {
      ctkDICOMSeriesDelegate* delegate = this->seriesDelegate();
      if (delegate)
      {
        QRect itemRect = this->visualRect(index);
        if (delegate->isContextMenuButtonAt(event->pos(), itemRect, index))
        {
          d->ContextMenuButtonPressedIndex = index;
          this->viewport()->update(this->visualRect(index));
        }
        else if (delegate->isStatusButtonAt(event->pos(), itemRect, index))
        {
          d->OperationStatusButtonPressedIndex = index;
          this->viewport()->update(this->visualRect(index));
          event->accept();
          return;
        }
      }
    }
  }

  Superclass::mousePressEvent(event);
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesTableView::mouseReleaseEvent(QMouseEvent* event)
{
  Q_D(ctkDICOMSeriesTableView);

  QModelIndex previousPressedIndex;
  if (d->ContextMenuButtonPressedIndex.isValid())
  {
    previousPressedIndex = d->ContextMenuButtonPressedIndex;
  }
  else if (d->OperationStatusButtonPressedIndex.isValid())
  {
    previousPressedIndex = d->OperationStatusButtonPressedIndex;
  }

  if (event->button() == Qt::LeftButton)
  {
    QModelIndex index = this->indexAt(event->pos());
    if (index.isValid())
    {
      ctkDICOMSeriesDelegate* delegate = this->seriesDelegate();
      if (delegate)
      {
        QRect itemRect = this->visualRect(index);
        // Handle context menu button release
        if (index == d->ContextMenuButtonPressedIndex)
        {
          QRect contextMenuButtonRect = delegate->contextMenuButtonRect(itemRect, index);
          if (contextMenuButtonRect.contains(event->pos()))
          {
            // Open context menu at global position
            this->onDelegateContextMenuRequested(event->globalPos(), index);
            event->accept();
          }
        }
        else if (index == d->OperationStatusButtonPressedIndex)
        {
          QRect statusButtonRect = delegate->statusButtonRect(itemRect, index);
          if (statusButtonRect.contains(event->pos()))
          {
            QString seriesInstanceUID = this->seriesInstanceUID(index);
            this->seriesModel()->forceUpdateSeriesJobs(seriesInstanceUID);
            event->accept();
          }
        }
      }
    }
  }

  d->ContextMenuButtonPressedIndex = QModelIndex();
  d->OperationStatusButtonPressedIndex = QModelIndex();

  if (previousPressedIndex.isValid())
  {
    this->viewport()->update(this->visualRect(previousPressedIndex));
  }

  Superclass::mouseReleaseEvent(event);
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesTableView::mouseDoubleClickEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton)
  {
    QModelIndex index = this->indexAt(event->pos());
    if (index.isValid())
    {
      QString seriesInstanceUID = this->seriesInstanceUID(index);
      if (!seriesInstanceUID.isEmpty())
      {
        this->selectSeriesInstanceUID(seriesInstanceUID, QItemSelectionModel::Select);

        // Emit activation signal
        emit seriesActivated(seriesInstanceUID);
        event->accept();
        return;
      }
    }
  }

  Superclass::mouseDoubleClickEvent(event);
}

//------------------------------------------------------------------------------
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void ctkDICOMSeriesTableView::enterEvent(QEnterEvent *event)
#else
void ctkDICOMSeriesTableView::enterEvent(QEvent *event)
#endif
{
  Q_UNUSED(event)
  emit seriesTableViewEntered();
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesTableView::leaveEvent(QEvent* event)
{
  Q_D(ctkDICOMSeriesTableView);

  QModelIndex previousHoveredIndex;
  if (d->ContextMenuButtonHoveredIndex.isValid())
  {
    previousHoveredIndex = d->ContextMenuButtonHoveredIndex;
  }
  else if (d->OperationStatusButtonHoveredIndex.isValid())
  {
    previousHoveredIndex = d->OperationStatusButtonHoveredIndex;
  }

  d->ContextMenuButtonHoveredIndex = QModelIndex();
  d->OperationStatusButtonHoveredIndex = QModelIndex();

  if (previousHoveredIndex.isValid())
  {
    this->viewport()->update(this->visualRect(previousHoveredIndex));
  }

  Superclass::leaveEvent(event);
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesTableView::contextMenuEvent(QContextMenuEvent* event)
{
  // Get the item at the right-click position
  QModelIndex index = this->indexAt(event->pos());

  if (index.isValid())
  {
    // If the clicked item is not selected, add it to the selection
    // If it's already selected, keep the existing selection unchanged
    if (!this->selectionModel()->isSelected(index))
    {
      QString seriesUID = this->seriesInstanceUID(index);
      if (!seriesUID.isEmpty())
      {
        this->selectSeriesInstanceUID(seriesUID, QItemSelectionModel::Select);
      }
    }
  }

  QStringList selectedUIDs = this->selectedSeriesInstanceUIDs();
  emit contextMenuRequested(event->globalPos(), selectedUIDs);
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesTableView::currentChanged(const QModelIndex& current, const QModelIndex& previous)
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
QSize ctkDICOMSeriesTableView::viewportSizeHint() const
{
  QSize itemSize;
  if (this->model() && this->model()->rowCount() > 0 && this->model()->columnCount() > 0)
  {
    // Get size hint from delegate for the first valid item
    QModelIndex firstIndex = this->model()->index(0, 0);
    if (firstIndex.isValid())
    {
      ctkDICOMSeriesDelegate* delegate = this->seriesDelegate();
      if (delegate)
      {
        QStyleOptionViewItem option;
        itemSize = delegate->sizeHint(option, firstIndex);
      }
    }
  }

  int cols = this->model() ? this->model()->columnCount() : 0;
  int rows = this->model() ? this->model()->rowCount() : 0;
  return QSize(itemSize.width() * cols, itemSize.height() * rows);
}

//------------------------------------------------------------------------------
int ctkDICOMSeriesTableView::sizeHintForRow(int row) const
{
  QModelIndex index = this->model()->index(row, 0);
  if (!index.isValid())
  {
    return -1;
  }

  ctkDICOMSeriesDelegate* delegate = this->seriesDelegate();
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
int ctkDICOMSeriesTableView::sizeHintForColumn(int col) const
{
  QModelIndex index = this->model()->index(0, col);
  if (!index.isValid())
  {
    return -1;
  }

  ctkDICOMSeriesDelegate* delegate = this->seriesDelegate();
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
void ctkDICOMSeriesTableView::updateGridLayout(bool forceGridColumnsUpdate)
{
  // Get series model
  ctkDICOMSeriesFilterProxyModel* seriesProxyModel = this->seriesFilterProxyModel();
  ctkDICOMSeriesModel* seriesModel = this->seriesModel();
  if (!seriesProxyModel || !seriesModel)
  {
    return;
  }

  // Calculate optimal number of columns based on current width
  int optimalColumns = this->calculateOptimalColumns();
  if (optimalColumns > 0 && optimalColumns != seriesProxyModel->gridColumns())
  {
    QStringList seriesInstanceUIDs = this->selectedSeriesInstanceUIDs();
    seriesProxyModel->setGridColumns(optimalColumns);
    this->selectSeriesInstanceUIDs(seriesInstanceUIDs);
  }
  if (forceGridColumnsUpdate)
  {
    seriesProxyModel->setGridColumns(0);
    seriesProxyModel->setGridColumns(optimalColumns);
  }

  // Force table to exactly fit content with no extra space
  this->resizeRowsToContents();
  this->resizeColumnsToContents();

  emit layoutChanged();
}

//------------------------------------------------------------------------------
int ctkDICOMSeriesTableView::calculateOptimalColumns() const
{
  int viewWidth = this->viewport()->width();
  int itemWidth = 0;

  // Get the source model to check if we have any series
  ctkDICOMSeriesModel* sourceModel = this->seriesModel();
  if (!sourceModel)
  {
    return 0;
  }

  // Check if there are any series in the source model
  if (sourceModel->rowCount() > 0)
  {
    // Get the first series index from the source model (linear: row 0, column 0)
    QModelIndex sourceIndex = sourceModel->index(0, 0);
    if (sourceIndex.isValid())
    {
      ctkDICOMSeriesDelegate* delegate = this->seriesDelegate();
      if (delegate)
      {
        QStyleOptionViewItem option;
        itemWidth = delegate->sizeHint(option, sourceIndex).width();
      }
    }
  }

  if (itemWidth <= 0 || viewWidth < itemWidth * 2)
  {
    return 0;
  }

  return qMax(1, viewWidth / itemWidth);
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesTableView::onSeriesSelectionChanged()
{
  Q_D(ctkDICOMSeriesTableView);
  QStringList selectedUIDs = this->selectedSeriesInstanceUIDs();
  emit seriesSelectionChanged(d->StudyInstanceUID, selectedUIDs);
  this->viewport()->update();
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesTableView::onDelegateContextMenuRequested(const QPoint& globalPos, const QModelIndex& index)
{
  Q_D(ctkDICOMSeriesTableView);

  // Get series UID from the clicked index
  QString seriesUID = this->seriesInstanceUID(index);
  if (seriesUID.isEmpty())
  {
    return;
  }

  // Track which series has an active context menu
  d->ActiveContextMenuIndex = index;

  // If the clicked item is not selected, add it to the selection
  // If it's already selected, keep the existing selection unchanged
  if (!this->selectionModel()->isSelected(index))
  {
    this->selectSeriesInstanceUID(seriesUID, QItemSelectionModel::Select);
  }

  // Get all selected series UIDs
  QStringList selectedUIDs = this->selectedSeriesInstanceUIDs();

  // Calculate the position below the context menu button
  ctkDICOMSeriesDelegate* delegate = this->seriesDelegate();
  QPoint menuPos = globalPos;
  if (delegate)
  {
    QRect itemRect = this->visualRect(index);
    QRect buttonRect = delegate->contextMenuButtonRect(itemRect, index);
    // Position menu at the bottom-center of the button
    QPoint buttonBottomCenter = this->viewport()->mapToGlobal(QPoint(buttonRect.left() - 1, buttonRect.bottom() + 2));
    menuPos = buttonBottomCenter;
  }

  // Emit context menu signal with adjusted position
  emit contextMenuRequested(menuPos, selectedUIDs);

  // Force viewport update after menu closes to restore hover state
  // This ensures the 3-dot button reappears if mouse is still over the item
  this->viewport()->update();
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesTableView::onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
  if (roles.contains(ctkDICOMSeriesModel::ThumbnailPathRole) ||
      roles.contains(ctkDICOMSeriesModel::ThumbnailSizeRole) ||
      roles.contains(ctkDICOMSeriesModel::ThumbnailGeneratedRole) ||
      roles.contains(ctkDICOMSeriesModel::IsCloudRole) ||
      roles.contains(ctkDICOMSeriesModel::IsLoadedRole) ||
      roles.contains(ctkDICOMSeriesModel::OperationProgressRole) ||
      roles.contains(ctkDICOMSeriesModel::OperationStatusRole))
  {
    // The proxy model (if used) already transformed source indices to grid indices
    // So topLeft and bottomRight are already in the correct coordinate system for this view

    // Update all affected items in the range
    for (int row = topLeft.row(); row <= bottomRight.row(); ++row)
    {
      for (int col = topLeft.column(); col <= bottomRight.column(); ++col)
      {
        QModelIndex index = this->model()->index(row, col);
        if (index.isValid())
        {
          this->viewport()->update(this->visualRect(index));
        }
      }
    }

    return;
  }

  this->refreshLayout();
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesTableView::onRowsInserted(const QModelIndex& parent, int first, int last)
{
  Q_UNUSED(parent);
  Q_UNUSED(first);
  Q_UNUSED(last);

  this->refreshLayout();
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesTableView::onRowsRemoved(const QModelIndex& parent, int first, int last)
{
  Q_UNUSED(parent);
  Q_UNUSED(first);
  Q_UNUSED(last);

  this->refreshLayout();
}

//------------------------------------------------------------------------------
void ctkDICOMSeriesTableView::onModelReset()
{
  // Nothing
}

//------------------------------------------------------------------------------
bool ctkDICOMSeriesTableView::isContextMenuButtonHovered(const QModelIndex& index) const
{
  Q_D(const ctkDICOMSeriesTableView);
  return d->ContextMenuButtonHoveredIndex == index;
}

//------------------------------------------------------------------------------
bool ctkDICOMSeriesTableView::isContextMenuButtonPressed(const QModelIndex& index) const
{
  Q_D(const ctkDICOMSeriesTableView);
  return d->ContextMenuButtonPressedIndex == index;
}

//------------------------------------------------------------------------------
bool ctkDICOMSeriesTableView::hasActiveContextMenu(const QModelIndex& index) const
{
  Q_D(const ctkDICOMSeriesTableView);
  return (index.isValid() && d->ActiveContextMenuIndex.isValid() &&
          index.row() == d->ActiveContextMenuIndex.row() &&
          index.column() == d->ActiveContextMenuIndex.column());
}

//------------------------------------------------------------------------------
bool ctkDICOMSeriesTableView::isOperationStatusButtonHovered(const QModelIndex& index) const
{
  Q_D(const ctkDICOMSeriesTableView);
  return d->OperationStatusButtonHoveredIndex == index;
}

//------------------------------------------------------------------------------
bool ctkDICOMSeriesTableView::isOperationStatusButtonPressed(const QModelIndex& index) const
{
  Q_D(const ctkDICOMSeriesTableView);
  return d->OperationStatusButtonPressedIndex == index;
}
