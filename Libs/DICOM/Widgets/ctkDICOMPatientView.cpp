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
#include <QHelpEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QPushButton>
#include <QResizeEvent>
#include <QScrollBar>
#include <QSharedPointer>
#include <QItemSelectionModel>
#include <QItemSelection>
#include <QTimer>
#include <QToolTip>
#include <QWheelEvent>
#include <QDebug>

// CTK includes
#include "ctkCheckableComboBox.h"
#include "ctkDICOMPatientView.h"
#include "ctkDICOMPatientDelegate.h"
#include "ctkDICOMPatientModel.h"
#include "ctkDICOMPatientFilterProxyModel.h"
#include "ctkDICOMScheduler.h"
#include "ctkDICOMServer.h"
#include "ctkDICOMStudyDelegate.h"
#include "ctkDICOMStudyListView.h"
#include "ctkDICOMStudyModel.h"
#include "ctkDICOMStudyFilterProxyModel.h"
#include "ctkDICOMStudyMergedFilterProxyModel.h"

//------------------------------------------------------------------------------
class ctkDICOMPatientViewPrivate
{
  Q_DECLARE_PUBLIC(ctkDICOMPatientView);

protected:
  ctkDICOMPatientView* const q_ptr;

public:
  ctkDICOMPatientViewPrivate(ctkDICOMPatientView& object);
  ~ctkDICOMPatientViewPrivate();

  void init();
  void setupViewForDisplayMode();

  // Display mode
  ctkDICOMPatientView::DisplayMode DisplayMode;

  // Model state tracking
  bool ModelResetInProgress;

  // Mouse pressed tracking
  bool MousePressed;

  // Single study list view - reused for all patients
  QSharedPointer<ctkDICOMStudyListView> StudyListView;

  // Merged proxy model for multi-patient selection in ListMode
  QSharedPointer<ctkDICOMStudyMergedFilterProxyModel> MergedStudyProxyModel;

  // Cache for selected patient items and their models
  QHash<QModelIndex, ctkDICOMStudyFilterProxyModel*> CachedStudyFilterProxyModels;
  bool StudyModelsNeedRefresh;
  bool PatientsCacheNeedsUpdate;

  // Hover state tracking for visual effects
  QModelIndex HoveredIndex;

  // Context menu button tracking
  QModelIndex ActiveContextMenuIndex;
  QModelIndex ContextMenuButtonHoveredIndex;
  QModelIndex ContextMenuButtonPressedIndex;

  // Select all icon tracking
  QModelIndex SelectAllIconHoveredIndex;
  QModelIndex SelectAllIconPressedIndex;

  // Splitter for ListMode
  int SplitterPosition;
  bool SplitterDragging;
  QCursor SplitterCursor;
  bool SplitterHovered;

  // Display mode toggle button
  QPushButton* DisplayModeButton;

  // Allowed servers combo box and label for TabMode
  QLabel* AllowedServersLabel;
  ctkCheckableComboBox* AllowedServersComboBox;

  // Helper methods
  QString getPatientUID(const QModelIndex& patientIndex) const;
  void updateStudyListViewGeometry();
  void updateScrollBarGeometry();
  void updateAllowedServersComboBoxGeometry(const QModelIndex& index = QModelIndex());
  void updateAllowedServersComboBoxFromModel(const QModelIndex& index = QModelIndex());
  bool isOverSplitter(const QPoint& pos) const;
  void updateSplitterCursor(const QPoint& pos);
  void updateDisplayModeButtonPosition();
  void updateSelectedPatientsCache();
};

//------------------------------------------------------------------------------
ctkDICOMPatientViewPrivate::ctkDICOMPatientViewPrivate(ctkDICOMPatientView& object)
  : q_ptr(&object)
{
  this->DisplayMode = ctkDICOMPatientView::TabMode;
  this->MousePressed = false;
  this->ModelResetInProgress = false;
  this->SplitterPosition = 175;
  this->SplitterDragging = false;
  this->SplitterHovered = false;
  this->SplitterCursor = QCursor(Qt::SplitVCursor);
  this->DisplayModeButton = nullptr;
  this->AllowedServersLabel = nullptr;
  this->AllowedServersComboBox = nullptr;
  this->StudyModelsNeedRefresh = false;
  this->PatientsCacheNeedsUpdate = false;
}

//------------------------------------------------------------------------------
ctkDICOMPatientViewPrivate::~ctkDICOMPatientViewPrivate()
{
}

//------------------------------------------------------------------------------
void ctkDICOMPatientViewPrivate::setupViewForDisplayMode()
{
  Q_Q(ctkDICOMPatientView);

  if (this->DisplayMode == ctkDICOMPatientView::TabMode)
  {
    q->setFlow(QListView::LeftToRight);
  }
  else if (this->DisplayMode == ctkDICOMPatientView::ListMode)
  {
    q->setFlow(QListView::TopToBottom);
    // Hide allowed servers combo box and label in ListMode
    if (this->AllowedServersComboBox)
    {
      this->AllowedServersComboBox->hide();
    }
    if (this->AllowedServersLabel)
    {
      this->AllowedServersLabel->hide();
    }
  }

  q->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  q->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  q->setUniformItemSizes(false);

  // Force view update
  q->refreshLayout();
}

//------------------------------------------------------------------------------
QString ctkDICOMPatientViewPrivate::getPatientUID(const QModelIndex& patientIndex) const
{
  Q_Q(const ctkDICOMPatientView);
  if (!patientIndex.isValid())
  {
    return QString();
  }

  // Get patient model (handles proxy mapping internally)
  ctkDICOMPatientModel* patientModel = q->patientModel();
  if (patientModel)
  {
    // If we have a proxy model, map proxy index to source index
    QAbstractItemModel* model = const_cast<QAbstractItemModel*>(patientIndex.model());
    ctkDICOMPatientFilterProxyModel* proxyModel = qobject_cast<ctkDICOMPatientFilterProxyModel*>(model);
    if (proxyModel)
    {
      QModelIndex sourceIndex = proxyModel->mapToSource(patientIndex);
      return patientModel->patientUID(sourceIndex);
    }
  }

  return QString();
}

//------------------------------------------------------------------------------
void ctkDICOMPatientViewPrivate::updateStudyListViewGeometry()
{
  Q_Q(ctkDICOMPatientView);

  if (!this->StudyListView || !this->MergedStudyProxyModel)
  {
    return;
  }

  ctkDICOMPatientDelegate* delegate = qobject_cast<ctkDICOMPatientDelegate*>(q->itemDelegate());
  if (!delegate)
  {
    return;
  }

  ctkDICOMPatientFilterProxyModel* patientFilterProxyModel = q->patientFilterProxyModel();
  if (!patientFilterProxyModel)
  {
    return;
  }

  ctkDICOMPatientModel* patientModel = q->patientModel();
  if (!patientModel)
  {
    this->StudyListView->hide();
    return;
  }

  QRect viewportRect = q->viewport()->rect();
  const int spacing = delegate->spacing();

  QList<ctkDICOMStudyModel*> addedSourceModels;
  if (this->DisplayMode == ctkDICOMPatientView::TabMode)
  {
    // TabMode: Single selection, use single study filter proxy model
    QModelIndex currentIndex = q->currentIndex();
    if (!currentIndex.isValid())
    {
      this->StudyListView->hide();
      return;
    }

    // Check if current patient is visible
    bool isVisible = patientFilterProxyModel->data(currentIndex, ctkDICOMPatientModel::IsVisibleRole).toBool();
    if (!isVisible || !q->selectionModel()->isSelected(currentIndex))
    {
      this->StudyListView->hide();
      return;
    }

    // Get the study filter proxy model for the current patient
    QString currentPatientUID = q->patientUID(currentIndex);
    if (currentPatientUID.isEmpty())
    {
      this->StudyListView->hide();
      return;
    }

    ctkDICOMStudyFilterProxyModel* studyFilterProxyModel =
      patientModel->studyFilterProxyModelForPatientUID(currentPatientUID);
    if (!studyFilterProxyModel)
    {
      this->StudyListView->hide();
      return;
    }

    // Set single model
    if (this->StudyListView->model() != studyFilterProxyModel)
    {
      addedSourceModels.append(qobject_cast<ctkDICOMStudyModel*>(studyFilterProxyModel->sourceModel()));
      this->StudyListView->setModel(studyFilterProxyModel);
      this->StudyModelsNeedRefresh = true;
    }

    const int tabRowHeight = delegate->tabModeHeight();
    const int patientInfoHeight = delegate->patientHeaderHeight();

    // Calculate study area: below tabs and patient info area with margins
    int contentTop = tabRowHeight + spacing * 5 + patientInfoHeight;
    QRect studyAreaRect(spacing, contentTop, viewportRect.width(), viewportRect.height() - contentTop);

    // Only set geometry if study area has valid size
    if (studyAreaRect.height() > 10 && studyAreaRect.width() > 10)
    {
      this->StudyListView->setGeometry(studyAreaRect);
      this->StudyListView->show();
      this->StudyListView->raise();
    }
    else
    {
      this->StudyListView->hide();
    }
  }
  else if (this->DisplayMode == ctkDICOMPatientView::ListMode)
  {
    // Update cache if needed (lazy evaluation)
    if (this->PatientsCacheNeedsUpdate)
    {
      this->updateSelectedPatientsCache();
      this->PatientsCacheNeedsUpdate = false;

      // Use cached study filter proxy models if available
      if (this->CachedStudyFilterProxyModels.isEmpty())
      {
        this->StudyListView->hide();
        return;
      }

      // Set the model only if it needs to change (avoid unnecessary rebuilds)
      if (this->CachedStudyFilterProxyModels.count() == 1)
      {
        QHash<QModelIndex, ctkDICOMStudyFilterProxyModel*>::const_iterator it = this->CachedStudyFilterProxyModels.constBegin();
        ctkDICOMStudyFilterProxyModel* studyFilterProxyModel = it.value();
        if (this->StudyListView->model() != studyFilterProxyModel)
        {
          addedSourceModels.append(qobject_cast<ctkDICOMStudyModel*>(studyFilterProxyModel->sourceModel()));
          this->StudyListView->setModel(studyFilterProxyModel);
          this->StudyModelsNeedRefresh = true;
        }
      }
      else
      {
        // Only update merged model if the source models have changed
        QList<ctkDICOMStudyFilterProxyModel*> cachedProxyModels = this->CachedStudyFilterProxyModels.values();
        QList<ctkDICOMStudyFilterProxyModel*> currentProxyModels = this->MergedStudyProxyModel->sourceProxyFilterModels();
        // Compute difference: models in cachedModels but not in currentModels, and vice versa
        QList<ctkDICOMStudyFilterProxyModel*> addedProxyModels;
        QList<ctkDICOMStudyFilterProxyModel*> removedProxyModels;
        for (int index = 0; index < cachedProxyModels.size(); ++index)
        {
          ctkDICOMStudyFilterProxyModel* model = cachedProxyModels.at(index);
          if (!currentProxyModels.contains(model))
          {
            addedProxyModels.append(model);
          }
        }
        for (int index = 0; index < currentProxyModels.size(); ++index)
        {
          ctkDICOMStudyFilterProxyModel* model = currentProxyModels.at(index);
          if (!cachedProxyModels.contains(model))
          {
            removedProxyModels.append(model);
          }
        }

        bool modelsChanged = !addedProxyModels.isEmpty() || !removedProxyModels.isEmpty() || this->StudyListView->model() != this->MergedStudyProxyModel;
        if (modelsChanged)
        {
          for (int index = 0; index < addedProxyModels.size(); ++index)
          {
            ctkDICOMStudyModel* sourceModel = qobject_cast<ctkDICOMStudyModel*>(addedProxyModels.at(index)->sourceModel());
            if (sourceModel && !addedSourceModels.contains(sourceModel))
            {
              addedSourceModels.append(sourceModel);
            }
          }
          this->MergedStudyProxyModel->setSourceProxyFilterModels(this->CachedStudyFilterProxyModels.values());
          if (this->StudyListView->model() != this->MergedStudyProxyModel)
          {
            this->StudyListView->setModel(this->MergedStudyProxyModel.data());
          }
          this->StudyModelsNeedRefresh = true;
        }
      }
    }

    // Position study list below the splitter, spanning full width
    int studyTop = this->SplitterPosition + spacing * 2;
    int studyLeft = spacing * 2;
    int studyWidth = viewportRect.width() - spacing * 4;
    int studyHeight = viewportRect.height() - studyTop - spacing * 2;

    QRect studyAreaRect(studyLeft, studyTop, studyWidth, studyHeight);

    // Only show and position if the study area has valid size
    if (studyAreaRect.height() > 50 && studyAreaRect.width() > 10)
    {
      this->StudyListView->setGeometry(studyAreaRect);
      this->StudyListView->show();
      this->StudyListView->raise();
    }
    else
    {
      this->StudyListView->hide();
    }
  }

  // Only refresh study models when needed (not on every scroll)
  if (this->StudyModelsNeedRefresh)
  {
    // Connect studies selection changes to trigger repaint and synchronize selection
    if (this->StudyListView->selectionModel())
    {
      // Disconnect any previous connections to avoid duplicate signals
      QObject::disconnect(this->StudyListView->selectionModel(), &QItemSelectionModel::selectionChanged, q, &ctkDICOMPatientView::onStudySelectionChanged);
      q->connect(this->StudyListView->selectionModel(), &QItemSelectionModel::selectionChanged,
                  q, &ctkDICOMPatientView::onStudySelectionChanged);
    }

    this->StudyListView->refreshStudiesModels(addedSourceModels);
    ctkDICOMScheduler* scheduler = patientModel->scheduler();
    QModelIndex currentIndex = q->currentIndex();
    QString currentPatientID = q->patientID(currentIndex);
    // NOTE: if running query jobs, wait to open the first 2 studies of the patient.
    if (scheduler && scheduler->getJobsByDICOMUIDs({currentPatientID}).count() == 0)
    {
      this->StudyListView->onNumberOfOpenedStudiesChanged(patientModel->numberOfOpenedStudiesPerPatient());
    }

    this->StudyModelsNeedRefresh = false;
  }
}

//------------------------------------------------------------------------------
bool ctkDICOMPatientViewPrivate::isOverSplitter(const QPoint& pos) const
{
  if (this->DisplayMode != ctkDICOMPatientView::ListMode)
  {
    return false;
  }

  int tolerance = 5;
  return qAbs(pos.y() - this->SplitterPosition) <= tolerance;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientViewPrivate::updateSplitterCursor(const QPoint& pos)
{
  Q_Q(ctkDICOMPatientView);
  if (this->DisplayMode != ctkDICOMPatientView::ListMode)
  {
    return;
  }

  bool overSplitter = this->isOverSplitter(pos);
  if (overSplitter != this->SplitterHovered)
  {
    this->SplitterHovered = overSplitter;
    if (overSplitter)
    {
      q->setCursor(this->SplitterCursor);
    }
    else
    {
      q->unsetCursor();
    }
    // Update only the splitter area
    QRect splitterRect(0, this->SplitterPosition - 5, q->viewport()->width(), 10);
    q->viewport()->update(splitterRect);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMPatientViewPrivate::updateScrollBarGeometry()
{
  Q_Q(ctkDICOMPatientView);

  // Only adjust scrollbar in ListMode
  if (this->DisplayMode != ctkDICOMPatientView::ListMode)
  {
    return;
  }

  QScrollBar* vScrollBar = q->verticalScrollBar();
  if (!vScrollBar)
  {
    return;
  }

  // Get the default geometry from Qt
  QRect defaultGeometry = vScrollBar->geometry();

  // Set the new geometry - keep X and width same, but adjust Y and height
  vScrollBar->setGeometry(defaultGeometry.x(), 0, defaultGeometry.width(), this->SplitterPosition);
}

//------------------------------------------------------------------------------
void ctkDICOMPatientViewPrivate::updateDisplayModeButtonPosition()
{
  Q_Q(ctkDICOMPatientView);

  if (!this->DisplayModeButton)
  {
    return;
  }

  ctkDICOMPatientDelegate* delegate = qobject_cast<ctkDICOMPatientDelegate*>(q->itemDelegate());
  if (!delegate)
  {
    return;
  }

  // This slot updates the position and visibility of the display mode button.
  // It should be called after layout or model changes to ensure correct placement.
  if (this->DisplayMode == ctkDICOMPatientView::TabMode)
  {
    int spacing = delegate->spacing();
    int xPos = spacing;
    int visibleCount = 0;
    QAbstractItemModel* model = q->model();
    if (model)
    {
      for (int row = 0; row < model->rowCount(); ++row)
      {
        QModelIndex index = model->index(row, 0);
        bool isVisible = index.data(ctkDICOMPatientModel::IsVisibleRole).toBool();
        if (isVisible)
        {
          QRect itemRect = q->visualRect(index);
          if (itemRect.isValid())
          {
            visibleCount++;
            xPos = itemRect.right() + spacing;
          }
        }
      }
    }
    int y = spacing;
    this->DisplayModeButton->move(xPos, y);
    this->DisplayModeButton->setVisible(visibleCount > 0);
  }
  else if (this->DisplayMode == ctkDICOMPatientView::ListMode)
  {
    // In ListMode, position in top-right corner
    int margin = delegate->spacing() * 4;
    int x = q->viewport()->rect().width() - this->DisplayModeButton->width() * 2 - margin;
    int y = margin;

    // Adjust for scrollbar if visible
    QScrollBar* vScrollBar = q->verticalScrollBar();
    if (vScrollBar && vScrollBar->isVisible())
    {
      x -= vScrollBar->width();
    }

    QAbstractItemModel* model = q->model();
    int visibleCount = 0;
    if (model)
    {
      for (int row = 0; row < model->rowCount(); ++row)
      {
        QModelIndex index = model->index(row, 0);
        bool isVisible = index.data(ctkDICOMPatientModel::IsVisibleRole).toBool();
        if (isVisible)
        {
          visibleCount++;
          break;
        }
      }
    }

    this->DisplayModeButton->move(x, y);
    this->DisplayModeButton->setVisible(visibleCount > 0);
  }
  this->DisplayModeButton->raise();
}

//------------------------------------------------------------------------------
void ctkDICOMPatientViewPrivate::updateAllowedServersComboBoxGeometry(const QModelIndex& index)
{
  Q_Q(ctkDICOMPatientView);

  if (!this->AllowedServersComboBox || !this->AllowedServersLabel)
  {
    return;
  }

  QModelIndex currentIndex = index;
  if (!currentIndex.isValid())
  {
    currentIndex = q->currentIndex();
  }
  // Don't hide if the combo box is currently active (being interacted with)
  // Check if the combo box's popup view is visible
  bool isComboBoxActive = this->AllowedServersComboBox->view() && this->AllowedServersComboBox->view()->isVisible();
  if (!currentIndex.isValid() ||
    !q->selectionModel()->isSelected(currentIndex) ||
    (!q->isIndexHovered(currentIndex) && !isComboBoxActive))
  {
    this->AllowedServersComboBox->hide();
    this->AllowedServersLabel->hide();
    return;
  }

  ctkDICOMPatientDelegate* delegate = qobject_cast<ctkDICOMPatientDelegate*>(q->itemDelegate());
  if (!delegate)
  {
    this->AllowedServersComboBox->hide();
    this->AllowedServersLabel->hide();
    return;
  }

  // Position in a dedicated row below the patient header
  QRect itemRect = q->visualRect(currentIndex);
  if (!itemRect.isValid())
  {
    this->AllowedServersComboBox->hide();
    this->AllowedServersLabel->hide();
    return;
  }

  QStyleOptionViewItem option;
  option.rect = itemRect;
  option.widget = q;

  QRect patientsRect = delegate->patientsRect(itemRect, option, currentIndex);
  QRect patientHeaderRect = delegate->patientHeaderRect(patientsRect, option);

  int spacing = delegate->spacing();
  int comboWidth = 200;
  int comboHeight = 24;

  if (this->DisplayMode == ctkDICOMPatientView::TabMode)
  {
    // Position label and combo box below patient header in their own row
    int startX = patientHeaderRect.left() + spacing * 4;
    int y = patientHeaderRect.bottom() - comboHeight - spacing * 2;

    QFontMetrics fm(this->AllowedServersLabel->font());
    int labelWidth = fm.horizontalAdvance(this->AllowedServersLabel->text()) + spacing;
    int labelHeight = fm.height();

    // Position combo box next to label
    int comboX = startX + labelWidth + spacing;

    this->AllowedServersLabel->setGeometry(startX, y + (comboHeight - labelHeight) / 2, labelWidth, labelHeight);
    this->AllowedServersLabel->show();
    this->AllowedServersLabel->raise();

    this->AllowedServersComboBox->setGeometry(comboX, y, comboWidth, comboHeight);
    this->AllowedServersComboBox->show();
    this->AllowedServersComboBox->raise();
  }
  else
  {
    // Position label and combo box below patient header in their own row
    int x = patientsRect.right() - comboWidth - spacing * 16;
    int y = patientsRect.top() + spacing * 0.75;

    this->AllowedServersLabel->hide();

    this->AllowedServersComboBox->setGeometry(x, y, comboWidth, comboHeight);
    this->AllowedServersComboBox->show();
    this->AllowedServersComboBox->raise();
  }
}

//------------------------------------------------------------------------------
void ctkDICOMPatientViewPrivate::updateAllowedServersComboBoxFromModel(const QModelIndex& index)
{
  Q_Q(ctkDICOMPatientView);

  if (!this->AllowedServersComboBox)
  {
    return;
  }

  QModelIndex currentIndex = index;
  if (!currentIndex.isValid())
  {
    currentIndex = q->currentIndex();
  }
  if (!currentIndex.isValid())
  {
    this->AllowedServersComboBox->clear();
    return;
  }

  QString patientUID = q->patientUID(currentIndex);
  if (patientUID.isEmpty())
  {
    this->AllowedServersComboBox->clear();
    return;
  }

  ctkDICOMPatientModel* patientModel = q->patientModel();
  if (!patientModel || !patientModel->dicomDatabase())
  {
    this->AllowedServersComboBox->clear();
    return;
  }

  // Get allowed servers from model for current patient
  QModelIndex sourceIndex = currentIndex;
  ctkDICOMPatientFilterProxyModel* proxyModel = q->patientFilterProxyModel();
  if (proxyModel)
  {
    sourceIndex = proxyModel->mapToSource(currentIndex);
  }

  QStringList allowedServers = sourceIndex.data(ctkDICOMPatientModel::AllowedServersRole).toStringList();

  // Get scheduler to get all active servers
  ctkDICOMScheduler* scheduler = patientModel->scheduler();
  if (!scheduler)
  {
    this->AllowedServersComboBox->clear();
    return;
  }

  QStringList allActiveConnectionNames = scheduler->getConnectionNamesForActiveServers();
  if (allActiveConnectionNames.count() == 0)
  {
    this->AllowedServersComboBox->clear();
    return;
  }

  // Get connection information from database to determine explicitly allowed/denied servers
  QMap<QString, QStringList> connectionsInformation = patientModel->dicomDatabase()->connectionsInformationForPatient(patientUID);
  QStringList explicitAllowList = connectionsInformation["allow"];
  QStringList explicitDenyList = connectionsInformation["deny"];

  // Populate combo box
  int wasBlocking = this->AllowedServersComboBox->blockSignals(true);
  this->AllowedServersComboBox->clear();
  this->AllowedServersComboBox->addItems(allActiveConnectionNames);

  QAbstractItemModel* model = this->AllowedServersComboBox->checkableModel();
  for (int index = 0; index < this->AllowedServersComboBox->count(); ++index)
  {
    QString connectionName = this->AllowedServersComboBox->itemText(index);
    QModelIndex modelIndex = model->index(index, 0);

    Qt::CheckState checkState = Qt::Unchecked;

    if (explicitAllowList.contains(connectionName))
    {
      // Explicitly allowed by user
      checkState = Qt::Checked;
    }
    else if (!explicitDenyList.contains(connectionName))
    {
      // Check if it's a trusted server (shows as partially checked)
      ctkDICOMServer* server = scheduler->server(connectionName);
      if (server && server->trustedEnabled())
      {
        checkState = Qt::PartiallyChecked;
      }
    }

    this->AllowedServersComboBox->setCheckState(modelIndex, checkState);
  }

  this->AllowedServersComboBox->blockSignals(wasBlocking);
}

//------------------------------------------------------------------------------
void ctkDICOMPatientViewPrivate::updateSelectedPatientsCache()
{
  Q_Q(ctkDICOMPatientView);

  // Only cache in ListMode with multiple selections
  if (this->DisplayMode != ctkDICOMPatientView::ListMode)
  {
    return;
  }

  ctkDICOMPatientFilterProxyModel* patientFilterProxyModel = q->patientFilterProxyModel();
  if (!patientFilterProxyModel)
  {
    return;
  }

  ctkDICOMPatientModel* patientModel = q->patientModel();
  if (!patientModel)
  {
    return;
  }

  // Get all selected patient indices
  QModelIndexList selectedIndexes = q->selectionModel()->selectedIndexes();
  if (selectedIndexes.isEmpty())
  {
    this->CachedStudyFilterProxyModels.clear();
  }

  QList<QModelIndex> previousSelectedIndexes = this->CachedStudyFilterProxyModels.keys();
  QList<QModelIndex> keysToRemove;
  for (QHash<QModelIndex, ctkDICOMStudyFilterProxyModel*>::const_iterator it = this->CachedStudyFilterProxyModels.constBegin();
       it != this->CachedStudyFilterProxyModels.constEnd(); ++it)
  {
    const QModelIndex& index = it.key();
    if (!selectedIndexes.contains(index))
    {
      keysToRemove.append(index);
    }
  }
  for (const QModelIndex& index : keysToRemove)
  {
    this->CachedStudyFilterProxyModels.remove(index);
  }
  previousSelectedIndexes = this->CachedStudyFilterProxyModels.keys();

  // Build cache of patient items and their study filter proxy models
  for (QModelIndexList::const_iterator it = selectedIndexes.constBegin(); it != selectedIndexes.constEnd(); ++it)
  {
    const QModelIndex& index = *it;
    if (!index.isValid() || previousSelectedIndexes.contains(index))
    {
      continue;
    }

    QString patientUID = q->patientUID(index);
    if (patientUID.isEmpty())
    {
      continue;
    }

    ctkDICOMStudyFilterProxyModel* studyFilterProxyModel = patientModel->studyFilterProxyModelForPatientUID(patientUID);
    if (studyFilterProxyModel)
    {
      this->CachedStudyFilterProxyModels.insert(index, studyFilterProxyModel);
    }
  }
}

//------------------------------------------------------------------------------
ctkDICOMPatientView::ctkDICOMPatientView(QWidget* parent)
  : Superclass(parent)
  , d_ptr(new ctkDICOMPatientViewPrivate(*this))
{
  Q_D(ctkDICOMPatientView);

  // Configure list view for patient display
  this->setSelectionMode(QAbstractItemView::SingleSelection);
  this->setDragDropMode(QAbstractItemView::NoDragDrop);

  // Enable mouse tracking for hover effects
  this->setMouseTracking(true);

  // Configure list appearance
  this->setAlternatingRowColors(false);
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

  // Set size policy
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

  // Set scroll mode to pixel-based scrolling for smooth scrolling
  this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

  // Set scroll step size for smooth scrolling
  if (this->verticalScrollBar())
  {
    this->verticalScrollBar()->setSingleStep(20);
  }
  if (this->horizontalScrollBar())
  {
    this->horizontalScrollBar()->setSingleStep(20);
  }

  // Connect to scroll events to update study list view positions (throttled)
  if (this->verticalScrollBar())
  {
    connect(this->verticalScrollBar(), &QScrollBar::valueChanged,
            this, &ctkDICOMPatientView::onScrollBarValueChanged);
  }

  // Create display mode toggle button (FAB style)
  d->DisplayModeButton = new QPushButton(this);
  d->DisplayModeButton->setToolTip(tr("Toggle between Tab Mode and List Mode"));
  d->DisplayModeButton->setCursor(Qt::PointingHandCursor);
  d->DisplayModeButton->setAttribute(Qt::WA_Hover, true); // Enable hover events

  // To Do: add an option to disable styling
  // FAB-style circular button with subtle appearance and hover effect
  d->DisplayModeButton->setStyleSheet(QString(R"(
    QPushButton {
      background-color: rgba(255, 255, 255, 0.8);
      border: 2px solid rgb(240, 240, 240);
      border-radius: 8px;
      font-size: 28px;
      font-weight: bold;
      color: rgba(80, 80, 80, 0.5);
      padding: 0px;
    }
    QPushButton:hover {
      border: 2px solid rgb(220, 220, 220);
      background-color: rgba(255, 255, 255, 0.8);
      color: rgba(60, 60, 60, 1.0);
    }
    QPushButton:pressed {
      border: 2px solid rgb(200, 200, 200);
      background-color: rgba(255, 255, 255, 0.8);
      color: rgba(40, 40, 40, 1.0);
    }
  )"));

  // Set initial icon based on current mode
  if (d->DisplayMode == ctkDICOMPatientView::TabMode)
  {
    d->DisplayModeButton->setFixedSize(36, 36);
    d->DisplayModeButton->setIconSize(QSize(28, 28));
    d->DisplayModeButton->setIcon(QIcon(":/Icons/more_horiz.svg"));
    d->DisplayModeButton->setToolTip(tr("Show all patients"));
  }
  else
  {
    d->DisplayModeButton->setFixedSize(48, 48);
    d->DisplayModeButton->setIconSize(QSize(32, 32));
    d->DisplayModeButton->setIcon(QIcon(":/Icons/tab.svg"));
    d->DisplayModeButton->setToolTip(tr("Switch to Tab Mode"));
  }

  // Connect button to toggle display mode
  this->connect(d->DisplayModeButton, &QPushButton::clicked,
                this, &ctkDICOMPatientView::onDisplayModeButtonClicked);

  // Install event filter on the button to clear hover state when mouse enters it
  d->DisplayModeButton->installEventFilter(this);

  // Create allowed servers label and combo box for TabMode
  d->AllowedServersLabel = new QLabel(tr("Allowed servers:"), this->viewport());
  d->AllowedServersLabel->hide();

  // Style the label for better readability
  QFont labelFont = d->AllowedServersLabel->font();
  d->AllowedServersLabel->setFont(labelFont);

  d->AllowedServersComboBox = new ctkCheckableComboBox(this->viewport());
  d->AllowedServersComboBox->hide();

  this->connect(d->AllowedServersComboBox, SIGNAL(checkedIndexesChanged()),
                this, SLOT(onAllowedServersComboBoxChanged()));

  // Create the single study list view that will be reused for all patients
  d->StudyListView.reset(new ctkDICOMStudyListView(this->viewport()));
  d->MergedStudyProxyModel.reset(new ctkDICOMStudyMergedFilterProxyModel(this));

  // Create and set the delegate for study rendering
  ctkDICOMStudyDelegate* studyDelegate = new ctkDICOMStudyDelegate(d->StudyListView.data());
  d->StudyListView->setItemDelegate(studyDelegate);
  d->StudyListView->hide();

  // Setup for initial display mode
  d->setupViewForDisplayMode();

  // Connect signals
  this->connect(this, &ctkDICOMPatientView::layoutChanged, this, &ctkDICOMPatientView::onLayoutRefreshed);
  this->connect(d->StudyListView.data(), &ctkDICOMStudyListView::studySelectionChanged,
                this, &ctkDICOMPatientView::onStudiesSelectionChanged);
  this->connect(d->StudyListView.data(), &ctkDICOMStudyListView::studyListViewEntered,
                this, &ctkDICOMPatientView::onStudyViewEntered);
}

//------------------------------------------------------------------------------
ctkDICOMPatientView::~ctkDICOMPatientView() = default;

//------------------------------------------------------------------------------
void ctkDICOMPatientView::setModel(QAbstractItemModel* model)
{
  if (model == this->model())
  {
    return;
  }

  // Disconnect signals from old model
  if (this->model())
  {
    disconnect(this->model(), nullptr, this, nullptr);
    disconnect(this->selectionModel(), nullptr, this, nullptr);
  }

  // Set the new model
  Superclass::setModel(model);

  ctkDICOMPatientFilterProxyModel* proxyModel = this->patientFilterProxyModel();
  if (proxyModel)
  {
    proxyModel->setViewWidgetFont(this->font());
  }

  // Connect signals for new model
  if (model)
  {
    connect(model, &QAbstractItemModel::dataChanged,
            this, &ctkDICOMPatientView::onDataChanged);
    connect(model, &QAbstractItemModel::rowsInserted,
            this, &ctkDICOMPatientView::onRowsInserted);
    connect(model, &QAbstractItemModel::rowsRemoved,
            this, &ctkDICOMPatientView::onRowsRemoved);
    connect(model, &QAbstractItemModel::modelAboutToBeReset,
            this, &ctkDICOMPatientView::onModelAboutToBeReset);
    connect(model, &QAbstractItemModel::modelReset,
            this, &ctkDICOMPatientView::onModelReset);
  }

  // Connect selection model signals
  if (this->selectionModel())
  {
    connect(this->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &ctkDICOMPatientView::onPatientSelectionChanged);
  }
}

//------------------------------------------------------------------------------
ctkDICOMPatientView::DisplayMode ctkDICOMPatientView::displayMode() const
{
  Q_D(const ctkDICOMPatientView);
  return d->DisplayMode;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::setDisplayMode(DisplayMode mode)
{
  Q_D(ctkDICOMPatientView);

  if (d->DisplayMode == mode)
  {
    return;
  }

  if (mode == ctkDICOMPatientView::TabMode)
  {
    this->setSelectionMode(QAbstractItemView::SingleSelection);
  }
  else if (mode == ctkDICOMPatientView::ListMode)
  {
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
  }
  else
  {
    qWarning() << Q_FUNC_INFO << " - Unknown display mode:" << mode;
    return;
  }

  QModelIndexList selectedIndexes = this->selectionModel()->selectedIndexes();
  if (!selectedIndexes.isEmpty())
  {
    QModelIndex index = selectedIndexes.first();
    this->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
    this->setCurrentIndex(index);
  }
  else
  {
    this->selectPatientUID("");
  }

  d->DisplayMode = mode;
  d->setupViewForDisplayMode();

  // Clear cache when switching modes
  d->CachedStudyFilterProxyModels.clear();

  // Update button icon and tooltip based on mode
  if (d->DisplayModeButton)
  {
    // Set initial icon based on current mode
    if (d->DisplayMode == ctkDICOMPatientView::TabMode)
    {
      d->DisplayModeButton->setFixedSize(36, 36);
      d->DisplayModeButton->setIconSize(QSize(28, 28));
      d->DisplayModeButton->setIcon(QIcon(":/Icons/more_horiz.svg"));
      d->DisplayModeButton->setToolTip(tr("Show all patients"));
    }
    else
    {
      d->DisplayModeButton->setFixedSize(48, 48);
      d->DisplayModeButton->setIconSize(QSize(32, 32));
      d->DisplayModeButton->setIcon(QIcon(":/Icons/tab.svg"));
      d->DisplayModeButton->setToolTip(tr("Switch to Tab Mode"));
    }

    // Update button position for the new mode
    d->updateDisplayModeButtonPosition();
  }

  emit displayModeChanged(mode);
}

//------------------------------------------------------------------------------
QStringList ctkDICOMPatientView::selectedPatientUIDs() const
{
  // Collect all selected patient indices
  QModelIndexList selectedIndices;
  if (this->selectionModel())
  {
    selectedIndices = this->selectionModel()->selectedIndexes();
  }

  QStringList patientUIDs;
  foreach (const QModelIndex& index, selectedIndices)
  {
    QString patientUID = this->patientUID(index);
    if (!patientUID.isEmpty() && !patientUIDs.contains(patientUID))
    {
      patientUIDs.append(patientUID);
    }
  }

  return patientUIDs;
}

//------------------------------------------------------------------------------
QString ctkDICOMPatientView::currentPatientUID() const
{
  QModelIndex current = this->currentIndex();
  return this->patientUID(current);
}

//------------------------------------------------------------------------------
QString ctkDICOMPatientView::currentPatientID() const
{
  QModelIndex current = this->currentIndex();
  return this->patientID(current);
}

//------------------------------------------------------------------------------
QString ctkDICOMPatientView::currentPatientName() const
{
  QModelIndex current = this->currentIndex();

  ctkDICOMPatientModel* model = this->patientModel();
  if (model && current.isValid())
  {
    return model->patientName(current);
  }

  return QString();
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::selectPatientUIDs(const QStringList &patientUIDs)
{
  if (!this->selectionModel())
  {
    return;
  }

  this->selectionModel()->clearSelection();
  foreach (const QString& patientUID, patientUIDs)
  {
    QModelIndex index = this->indexForPatientUID(patientUID);
    if (!index.isValid())
    {
      continue;
    }

    this->selectionModel()->select(index, QItemSelectionModel::Select);

    // Set current index to the first valid patient
    if (!this->currentIndex().isValid() || !this->selectionModel()->isSelected(this->currentIndex()))
    {
      this->setCurrentIndex(index);
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::selectPatientUID(const QString& patientUID,
                                           QItemSelectionModel::SelectionFlag flag)
{
  QModelIndex index = this->indexForPatientUID(patientUID);
  if (index.isValid())
  {
    this->selectionModel()->select(index, flag);
    this->setCurrentIndex(index);
  }
  else
  {
    ctkDICOMPatientFilterProxyModel* patientFilterProxyModel = this->patientFilterProxyModel();
    if (patientFilterProxyModel)
    {
      QModelIndex index = patientFilterProxyModel->index(0, 0);
      this->selectionModel()->select(index, flag);
      this->setCurrentIndex(index);
    }
    else
    {
      this->selectionModel()->clearSelection();
      this->setCurrentIndex(QModelIndex());
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::selectPatientIDs(const QStringList &patientIDs)
{
  if (!this->selectionModel())
  {
    return;
  }
  this->selectionModel()->clearSelection();
  foreach (const QString& patientID, patientIDs)
  {
    QModelIndex index = this->indexForPatientID(patientID);
    if (index.isValid())
    {
      this->selectionModel()->select(index, QItemSelectionModel::Select);
    }
    // Set current index to the first valid patient
    if (!this->currentIndex().isValid() || !this->selectionModel()->isSelected(this->currentIndex()))
    {
      this->setCurrentIndex(index);
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::selectPatientID(const QString& patientID,
                                          QItemSelectionModel::SelectionFlag flag)
{
  QModelIndex index = this->indexForPatientID(patientID);
  this->selectPatientUID(this->patientUID(index), flag);
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::clearSelection()
{
  Q_D(ctkDICOMPatientView);
  this->selectionModel()->clearSelection();
  d->StudyListView->clearSelection();
  this->setCurrentIndex(QModelIndex());
}

//------------------------------------------------------------------------------
QModelIndex ctkDICOMPatientView::currentPatientIndex() const
{
  return this->currentIndex();
}

//------------------------------------------------------------------------------
QStringList ctkDICOMPatientView::selectedSeriesInstanceUIDs() const
{
  Q_D(const ctkDICOMPatientView);
  return d->StudyListView->selectedSeriesInstanceUIDs();
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::selectSeriesInstanceUIDs(const QStringList &seriesInstanceUIDs)
{
  Q_D(const ctkDICOMPatientView);
  d->StudyListView->selectSeriesInstanceUIDs(seriesInstanceUIDs);
}

//------------------------------------------------------------------------------
QString ctkDICOMPatientView::patientUID(const QModelIndex& index) const
{
  ctkDICOMPatientModel* model = this->patientModel();
  if (model && index.isValid())
  {
    // If we have a proxy model, map proxy index to source index
    ctkDICOMPatientFilterProxyModel* proxyModel = qobject_cast<ctkDICOMPatientFilterProxyModel*>(this->model());
    if (proxyModel)
    {
      QModelIndex sourceIndex = proxyModel->mapToSource(index);
      return model->patientUID(sourceIndex);
    }
  }
  return QString();
}

//------------------------------------------------------------------------------
QString ctkDICOMPatientView::patientID(const QModelIndex& index) const
{
  ctkDICOMPatientModel* model = this->patientModel();
  if (model && index.isValid())
  {
    // If we have a proxy model, map proxy index to source index
    ctkDICOMPatientFilterProxyModel* proxyModel = qobject_cast<ctkDICOMPatientFilterProxyModel*>(this->model());
    if (proxyModel)
    {
      QModelIndex sourceIndex = proxyModel->mapToSource(index);
      return model->patientID(sourceIndex);
    }
  }
  return QString();
}

//------------------------------------------------------------------------------
QModelIndex ctkDICOMPatientView::indexForPatientUID(const QString& patientUID) const
{
  ctkDICOMPatientModel* model = this->patientModel();
  if (model)
  {
    QModelIndex sourceIndex = model->indexFromPatientUID(patientUID);

    // If we have a proxy model, map the source index to proxy index
    ctkDICOMPatientFilterProxyModel* proxyModel = qobject_cast<ctkDICOMPatientFilterProxyModel*>(this->model());
    if (proxyModel && sourceIndex.isValid())
    {
      return proxyModel->mapFromSource(sourceIndex);
    }
  }
  return QModelIndex();
}

//------------------------------------------------------------------------------
QModelIndex ctkDICOMPatientView::indexForPatientID(const QString& patientID) const
{
  ctkDICOMPatientModel* model = this->patientModel();
  if (model)
  {
    QModelIndex sourceIndex = model->indexFromPatientID(patientID);

    // If we have a proxy model, map the source index to proxy index
    ctkDICOMPatientFilterProxyModel* proxyModel = qobject_cast<ctkDICOMPatientFilterProxyModel*>(this->model());
    if (proxyModel && sourceIndex.isValid())
    {
      return proxyModel->mapFromSource(sourceIndex);
    }
  }
  return QModelIndex();
}

//------------------------------------------------------------------------------
ctkDICOMStudyListView* ctkDICOMPatientView::studyListView() const
{
  Q_D(const ctkDICOMPatientView);
  return d->StudyListView.data();
}

//------------------------------------------------------------------------------
ctkDICOMPatientModel* ctkDICOMPatientView::patientModel() const
{
  ctkDICOMPatientFilterProxyModel* proxyModel = qobject_cast<ctkDICOMPatientFilterProxyModel*>(this->model());
  if (!proxyModel)
  {
    return nullptr;
  }

  return qobject_cast<ctkDICOMPatientModel*>(proxyModel->sourceModel());
}

//------------------------------------------------------------------------------
ctkDICOMPatientFilterProxyModel* ctkDICOMPatientView::patientFilterProxyModel() const
{
  return qobject_cast<ctkDICOMPatientFilterProxyModel*>(this->model());
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::scrollToPatientUID(const QString& patientUID)
{
  Q_D(ctkDICOMPatientView);
  QModelIndex index = indexForPatientUID(patientUID);
  if (!index.isValid())
  {
    return;
  }
  this->scrollTo(index);

  // Ensure the item is not hidden below the splitter in ListMode
  if (d->DisplayMode != ctkDICOMPatientView::ListMode)
  {
    return;
  }
  QRect rect = this->visualRect(index);
  // If the bottom of the item is below the splitter, scroll more
  if (rect.bottom() < d->SplitterPosition)
  {
    return;
  }
  QScrollBar* vScrollBar = this->verticalScrollBar();
  if (!vScrollBar)
  {
    return;
  }
  ctkDICOMPatientDelegate* delegate = this->patientDelegate();
  if (!delegate)
  {
    return;
  }

  int offset = rect.bottom() - d->SplitterPosition + delegate->iconSize() + delegate->spacing() * 2;
  vScrollBar->setValue(vScrollBar->value() + offset);
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::refreshLayout()
{
  Q_D(ctkDICOMPatientView);

  d->updateStudyListViewGeometry();
  emit this->layoutChanged();
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::clean()
{
  Q_D(ctkDICOMPatientView);

  // Clean the study list view which will clean all series views
  if (d->StudyListView)
  {
    // First, disconnect the model to avoid any signals during cleanup
    d->StudyListView->setModel(nullptr);
    // Then clean all series views
    d->StudyListView->clean();
    d->StudyListView->hide();
  }

  // Clear merged proxy model's source models before clearing cache
  // This must happen while the cached models are still valid
  if (d->MergedStudyProxyModel)
  {
    d->MergedStudyProxyModel->setSourceProxyFilterModels(QList<ctkDICOMStudyFilterProxyModel*>());
  }

  // Clear cached models
  d->CachedStudyFilterProxyModels.clear();

  // Reset flags
  d->StudyModelsNeedRefresh = false;
  d->PatientsCacheNeedsUpdate = false;

  // Reset selection and hover states
  d->HoveredIndex = QModelIndex();
  d->ActiveContextMenuIndex = QModelIndex();
  d->ContextMenuButtonHoveredIndex = QModelIndex();
  d->ContextMenuButtonPressedIndex = QModelIndex();
  d->SelectAllIconHoveredIndex = QModelIndex();
  d->SelectAllIconPressedIndex = QModelIndex();
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::resizeEvent(QResizeEvent* event)
{
  Q_D(ctkDICOMPatientView);
  Superclass::resizeEvent(event);

  if (d->DisplayMode == ctkDICOMPatientView::ListMode)
  {
    d->updateScrollBarGeometry();
  }
  else
  {
    this->patientFilterProxyModel()->setWidgetWidth(this->viewport()->width());
  }
  this->refreshLayout();
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::wheelEvent(QWheelEvent *event)
{
  if (event->modifiers() & Qt::ControlModifier)
  {
    return;
  }

  Superclass::wheelEvent(event);
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::paintEvent(QPaintEvent* event)
{
  Q_D(ctkDICOMPatientView);

  // Call base class paint
  Superclass::paintEvent(event);

  QAbstractItemModel* model = this->model();
  int visibleCount = 0;
  if (model)
  {
    for (int row = 0; row < model->rowCount(); ++row)
    {
      QModelIndex index = model->index(row, 0);
      bool isVisible = index.data(ctkDICOMPatientModel::IsVisibleRole).toBool();
      if (isVisible)
      {
        visibleCount++;
        break;
      }
    }
  }

  if (visibleCount == 0)
  {
    // Paint a message that no results have been found in the database
    QPainter painter(this->viewport());
    painter.setRenderHint(QPainter::Antialiasing, true);
    QString message = tr("No results found in the database");
    QFont font = painter.font();
    font.setPointSize(font.pointSize() + 4);
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(QColor(128, 128, 128));
    QRect rect = this->viewport()->rect();
    painter.drawText(rect, Qt::AlignCenter, message);
    return;
  }

  // Draw splitter line and handle on top in ListMode
  if (d->DisplayMode != ctkDICOMPatientView::ListMode || d->SplitterPosition == 0)
  {
    return;
  }

  QPainter painter(this->viewport());
  painter.setRenderHint(QPainter::Antialiasing, true);

  // Fill entire area below splitter with background color to clear artifacts
  QRect belowSplitter(0, d->SplitterPosition, this->viewport()->width(),
                       this->viewport()->height() - d->SplitterPosition);
  painter.fillRect(belowSplitter, this->palette().base());

  // Clear a small area around the splitter line to ensure clean rendering
  int clearMargin = 3;
  QRect clearRect(0, d->SplitterPosition - clearMargin,
                  this->viewport()->width(), clearMargin * 2);
  painter.fillRect(clearRect, this->palette().base());

  // Draw splitter line
  QColor splitterColor = QColor(128, 128, 128);
  if (d->SplitterHovered && !d->SplitterDragging)
  {
    splitterColor = QColor(90, 90, 90);
  }
  else if (d->SplitterDragging)
  {
    splitterColor = QColor(117, 167, 255);
  }

  int lineWidth = d->SplitterHovered || d->SplitterDragging ? 3 : 2;

  painter.setPen(QPen(splitterColor, lineWidth));
  painter.drawLine(0, d->SplitterPosition, this->viewport()->width(), d->SplitterPosition);

  // Draw grab handle in the middle
  int handleWidth = 40;
  int handleHeight = d->SplitterHovered ? 8 : 6;
  int handleX = (this->viewport()->width() - handleWidth) / 2;
  int handleY = d->SplitterPosition - handleHeight / 2;

  QRect handleRect(handleX, handleY, handleWidth, handleHeight);
  painter.setBrush(splitterColor);
  painter.setPen(Qt::NoPen);
  painter.drawRoundedRect(handleRect, 3, 3);
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::keyPressEvent(QKeyEvent* event)
{
  switch (event->key())
  {
  case Qt::Key_Escape:
  {
    this->clearSelection();
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
void ctkDICOMPatientView::mousePressEvent(QMouseEvent* event)
{
  Q_D(ctkDICOMPatientView);

  // In ListMode, check if clicking below splitter - ignore if so
  if (d->DisplayMode == ctkDICOMPatientView::ListMode)
  {
    // Check if clicking on splitter
    if (d->isOverSplitter(event->pos()))
    {
      d->SplitterDragging = true;
      event->accept();
      return;
    }

    // Ignore clicks below the splitter
    if (event->pos().y() >= d->SplitterPosition)
    {
      event->accept();
      return;
    }
  }

  QModelIndex index = this->indexAt(event->pos());

  ctkDICOMPatientDelegate* delegate = this->patientDelegate();
  if (delegate)
  {
    if (d->DisplayMode == ctkDICOMPatientView::TabMode)
    {
      int spacing = delegate->spacing();
      int tabRowHeight = delegate->tabModeHeight() + spacing * 2;
      int patientHeaderHeight = delegate->patientHeaderHeight() + spacing * 2;
      int totalHeight = tabRowHeight + patientHeaderHeight;

      if (event->pos().y() >= totalHeight)
      {
        index = QModelIndex();
      }
      else if (event->pos().y() >= tabRowHeight)
      {
        index = this->currentIndex();
        QRect itemRect = this->visualRect(index);
        QStyleOptionViewItem option;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        initViewItemOption(&option);
#else
        option = viewOptions();
#endif
        QRect selectAllIconRect = delegate->selectAllIconRect(itemRect, option);
        QRect contextMenuIconRect = delegate->contextMenuButtonRect(itemRect, option);
        if (selectAllIconRect.contains(event->pos()))
        {
          d->SelectAllIconPressedIndex = index;
        }
        else if (contextMenuIconRect.contains(event->pos()))
        {
          d->ContextMenuButtonPressedIndex = index;
        }
        else
        {
          d->MousePressed = true;
          event->accept();
          return;
        }
      }
    }
    else if (index.isValid()) // List Mode
    {
      QRect itemRect = this->visualRect(index);
      QStyleOptionViewItem option;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
      initViewItemOption(&option);
#else
      option = viewOptions();
#endif
      QRect selectAllIconRect = delegate->selectAllIconRect(itemRect, option);
      QRect contextMenuIconRect = delegate->contextMenuButtonRect(itemRect, option);
      if (selectAllIconRect.contains(event->pos()))
      {
        d->SelectAllIconPressedIndex = index;
      }
      else if (contextMenuIconRect.contains(event->pos()))
      {
        d->ContextMenuButtonPressedIndex = index;
      }
    }
  }

  if (!index.isValid())
  {
    event->accept();
    return;
  }
  else
  {
    this->viewport()->update(this->visualRect(index));
  }

  Superclass::mousePressEvent(event);
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::mouseMoveEvent(QMouseEvent* event)
{
  Q_D(ctkDICOMPatientView);

  d->ContextMenuButtonHoveredIndex = QModelIndex();
  d->SelectAllIconHoveredIndex = QModelIndex();
  d->ActiveContextMenuIndex = QModelIndex();

  d->HoveredIndex = this->indexAt(event->pos());

  // Handle splitter dragging in ListMode
  if (d->DisplayMode == ctkDICOMPatientView::ListMode)
  {
    if (d->SplitterDragging)
    {
      QRect viewportRect = this->viewport()->rect();
      int newPos = event->pos().y();
      int minPos = 10;
      int maxPos = viewportRect.height() - 10;
      d->SplitterPosition = qBound(minPos, newPos, maxPos);
      d->updateScrollBarGeometry();
      // Update geometries to recalculate scroll bars with new viewport size hint
      this->refreshLayout();
      this->viewport()->update();
      event->accept();
      return;
    }

    // Update cursor if over splitter
    d->updateSplitterCursor(event->pos());

    // Don't process hover for items below splitter
    if (event->pos().y() >= d->SplitterPosition)
    {
      d->HoveredIndex = QModelIndex();
    }
  }
  else if (d->DisplayMode == ctkDICOMPatientView::TabMode)
  {
    ctkDICOMPatientDelegate* delegate = this->patientDelegate();
    if (delegate)
    {
      int spacing = delegate->spacing();
      int tabRowHeight = delegate->tabModeHeight() + spacing * 2;
      int patientHeaderHeight = delegate->patientHeaderHeight() + spacing * 2;
      int totalHeight = tabRowHeight + patientHeaderHeight;

      if (event->pos().y() >= totalHeight)
      {
        d->HoveredIndex = QModelIndex();
      }
      else if (event->pos().y() >= tabRowHeight)
      {
        d->HoveredIndex =  this->currentIndex();
      }
    }
  }

  if (!d->HoveredIndex.isValid())
  {
    event->accept();
    return;
  }

  d->updateAllowedServersComboBoxGeometry(d->HoveredIndex);
  d->updateAllowedServersComboBoxFromModel(d->HoveredIndex);

  // Check if hovering over interactive icons
  ctkDICOMPatientDelegate* delegate = this->patientDelegate();
  if (delegate)
  {
    QStyleOptionViewItem option;
    option.rect = this->visualRect(d->HoveredIndex);
    option.widget = this;

    // Check select all icon first
    if (delegate->isSelectAllIconAt(event->pos(), option.rect, option))
    {
      d->SelectAllIconHoveredIndex = d->HoveredIndex;
      d->ContextMenuButtonHoveredIndex = QModelIndex();
    }
    // Then check context menu button
    else if (delegate->isContextMenuButtonAt(event->pos(), option.rect, option))
    {
      d->ContextMenuButtonHoveredIndex = d->HoveredIndex;
      d->SelectAllIconHoveredIndex = QModelIndex();
    }
    else
    {
      d->ContextMenuButtonHoveredIndex = QModelIndex();
      d->SelectAllIconHoveredIndex = QModelIndex();
    }
  }

  if (d->MousePressed)
  {
    event->accept();
    return;
  }

  // Update only affected areas
  if (d->DisplayMode == ctkDICOMPatientView::TabMode)
  {
    this->viewport()->update();
  }
  else
  {
    this->viewport()->update(this->visualRect(d->HoveredIndex));
  }

  Superclass::mouseMoveEvent(event);
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::mouseReleaseEvent(QMouseEvent* event)
{
  Q_D(ctkDICOMPatientView);

  d->MousePressed = false;

  if (d->DisplayMode == ctkDICOMPatientView::ListMode && d->SplitterDragging)
  {
    d->SplitterDragging = false;
    d->updateSplitterCursor(event->pos());
    QRect splitterRect(0, d->SplitterPosition - 5, this->viewport()->width(), 10);
    this->viewport()->update(splitterRect);
    event->accept();
    return;
  }

  // Handle clicks on interactive icons
  if (event->button() == Qt::LeftButton)
  {
    QModelIndex index = this->indexAt(event->pos());

    // In TabMode, check if click is in patient header area
    if (d->DisplayMode == ctkDICOMPatientView::TabMode && !index.isValid())
    {
      ctkDICOMPatientDelegate* delegate = this->patientDelegate();
      if (delegate)
      {
        int spacing = delegate->spacing();
        int tabRowHeight = delegate->tabModeHeight() + spacing * 2;
        if (event->pos().y() >= tabRowHeight)
        {
          index = this->currentIndex();
        }
      }
    }

    if (index.isValid() && this->patientDelegate())
    {
      QRect itemRect = this->visualRect(index);
      QStyleOptionViewItem option;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
      initViewItemOption(&option);
#else
      option = viewOptions();
#endif

      // Check if select all icon was clicked
      if (d->SelectAllIconPressedIndex == index &&
          this->patientDelegate()->isSelectAllIconAt(event->pos(), itemRect, option))
      {
        this->onSelectAllIconClicked(index);
        d->SelectAllIconPressedIndex = QModelIndex();
        event->accept();
        return;
      }

      // Check if context menu button was clicked
      if (d->ContextMenuButtonPressedIndex == index &&
          this->patientDelegate()->isContextMenuButtonAt(event->pos(), itemRect, option))
      {
        this->onPatientContextMenuRequested(event->globalPos(), index);
        d->ContextMenuButtonPressedIndex = QModelIndex();
        event->accept();
        return;
      }
    }
  }

  // Clear interactive icon pressed states on release
  if (d->ContextMenuButtonPressedIndex.isValid() || d->SelectAllIconPressedIndex.isValid())
  {
    if (d->ContextMenuButtonPressedIndex.isValid())
    {
      this->viewport()->update(this->visualRect(d->ContextMenuButtonPressedIndex));
    }
    if (d->SelectAllIconPressedIndex.isValid())
    {
      this->viewport()->update(this->visualRect(d->SelectAllIconPressedIndex));
    }
    d->ContextMenuButtonPressedIndex = QModelIndex();
    d->SelectAllIconPressedIndex = QModelIndex();
  }

  Superclass::mouseReleaseEvent(event);
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::leaveEvent(QEvent* event)
{
  Q_D(ctkDICOMPatientView);

  d->MousePressed = false;
  d->SplitterHovered = false;
  this->unsetCursor();
  d->updateAllowedServersComboBoxFromModel(d->HoveredIndex);
  d->updateAllowedServersComboBoxGeometry(d->HoveredIndex);
  d->HoveredIndex = QModelIndex();
  d->ContextMenuButtonHoveredIndex = QModelIndex();
  d->ContextMenuButtonPressedIndex = QModelIndex();
  d->SelectAllIconHoveredIndex = QModelIndex();
  d->SelectAllIconPressedIndex = QModelIndex();
  this->viewport()->update();
  Superclass::leaveEvent(event);
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::contextMenuEvent(QContextMenuEvent* event)
{
  Q_D(ctkDICOMPatientView);
  // Get the index at the click position
  QModelIndex index = this->indexAt(event->pos());

  // In TabMode, if right-click is in patient header area (below tab row), use current patient
  if (d->DisplayMode == ctkDICOMPatientView::TabMode)
  {
    ctkDICOMPatientDelegate* delegate = this->patientDelegate();
    if (delegate)
    {
      int spacing = delegate->spacing();
      int tabRowHeight = delegate->tabModeHeight() + spacing * 2;
      int patientHeaderHeight = delegate->patientHeaderHeight() + spacing * 2;
      int totalHeight = tabRowHeight + patientHeaderHeight;

      if (event->pos().y() >= totalHeight)
      {
        index = QModelIndex();
      }
      else if (event->pos().y() >= tabRowHeight)
      {
        index = this->currentIndex();
      }
    }

    if (!index.isValid())
    {
      event->accept();
      return;
    }
  }
  else // ListMode
  {
    QRect itemRect = this->visualRect(index);
    ctkDICOMPatientDelegate* delegate = this->patientDelegate();
    if (delegate)
    {
      QStyleOptionViewItem option;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
      initViewItemOption(&option);
#else
      option = viewOptions();
#endif
      QRect patientTabRect = delegate->patientsRect(itemRect, option, index);
      // Check if the click is within the patient controls area
      if (!patientTabRect.contains(event->pos()))
      {
        event->accept();
        return;
      }
    }
  }

  // Right-click on patient controls - show context menu
  QString patientUID = this->patientUID(index);
  if (patientUID.isEmpty())
  {
    event->accept();
    return;
  }

  ctkDICOMStudyListView* studyListView = this->studyListView();
  if (!studyListView)
  {
    event->accept();
    return;
  }

  studyListView->createAllSeriesView();
  studyListView->selectAllSeries();

  // Collect all selected patient indices
  QModelIndexList selectedIndices;
  if (this->selectionModel())
  {
    selectedIndices = this->selectionModel()->selectedIndexes();

    // If the clicked index is not in the selection, use only the clicked index
    // This handles the case where user right-clicks on an unselected item
    if (!selectedIndices.contains(index))
    {
      selectedIndices.clear();
      selectedIndices.append(index);
    }
  }
  else
  {
    // No selection model, just use the clicked index
    selectedIndices.append(index);
  }

  QStringList patientUIDs;
  foreach (const QModelIndex& index, selectedIndices)
  {
    QString patientUID = this->patientUID(index);
    if (!patientUID.isEmpty() && !patientUIDs.contains(patientUID))
    {
      patientUIDs.append(patientUID);
    }
  }

  emit this->patientContextMenuRequested(event->globalPos(), patientUIDs);
  event->accept();
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::changeEvent(QEvent* event)
{
  Superclass::changeEvent(event);

  if (event->type() == QEvent::FontChange)
  {
    ctkDICOMPatientFilterProxyModel* proxyModel = this->patientFilterProxyModel();
    if (proxyModel)
    {
      proxyModel->setViewWidgetFont(this->font());
    }
  }
}

//------------------------------------------------------------------------------
ctkDICOMPatientDelegate* ctkDICOMPatientView::patientDelegate() const
{
  return qobject_cast<ctkDICOMPatientDelegate*>(this->itemDelegate());
}

//------------------------------------------------------------------------------
bool ctkDICOMPatientView::eventFilter(QObject* watched, QEvent* event)
{
  Q_D(ctkDICOMPatientView);

  // Handle display mode button events
  if (watched == d->DisplayModeButton)
  {
    if (event->type() == QEvent::Enter)
    {
      // Clear hover state when mouse enters the display mode button
      this->viewport()->update();
      d->HoveredIndex = QModelIndex();
    }
  }

  return Superclass::eventFilter(watched, event);
}

//------------------------------------------------------------------------------
int ctkDICOMPatientView::splitterPosition() const
{
  Q_D(const ctkDICOMPatientView);
  return d->SplitterPosition;
}

//------------------------------------------------------------------------------
bool ctkDICOMPatientView::isIndexHovered(const QModelIndex& index) const
{
  Q_D(const ctkDICOMPatientView);
  return d->HoveredIndex == index;
}

//------------------------------------------------------------------------------
bool ctkDICOMPatientView::isContextMenuButtonPressed(const QModelIndex& index) const
{
  Q_D(const ctkDICOMPatientView);
  return d->ContextMenuButtonPressedIndex == index;
}

//------------------------------------------------------------------------------
bool ctkDICOMPatientView::isContextMenuButtonHovered(const QModelIndex& index) const
{
  Q_D(const ctkDICOMPatientView);
  return d->ContextMenuButtonHoveredIndex == index;
}

//------------------------------------------------------------------------------
bool ctkDICOMPatientView::hasActiveContextMenu(const QModelIndex& index) const
{
  Q_D(const ctkDICOMPatientView);
  return d->ActiveContextMenuIndex == index;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::setContextMenuButtonPressed(const QModelIndex& index)
{
  Q_D(ctkDICOMPatientView);
  d->ContextMenuButtonPressedIndex = index;
  if (index.isValid())
  {
    this->viewport()->update(this->visualRect(index));
  }
}

//------------------------------------------------------------------------------
bool ctkDICOMPatientView::isSelectAllIconPressed(const QModelIndex& index) const
{
  Q_D(const ctkDICOMPatientView);
  return d->SelectAllIconPressedIndex == index;
}

//------------------------------------------------------------------------------
bool ctkDICOMPatientView::isSelectAllIconHovered(const QModelIndex& index) const
{
  Q_D(const ctkDICOMPatientView);
  return d->SelectAllIconHoveredIndex == index;
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::onPatientSelectionChanged()
{
  Q_D(ctkDICOMPatientView);

  // Mark cache as needing update (lazy evaluation - will update when geometry is updated)
  d->PatientsCacheNeedsUpdate = true;

  this->refreshLayout();
  this->viewport()->update();

  this->scrollToPatientUID(this->currentPatientUID());

  // Update allowed servers combo box for new selection
  d->updateAllowedServersComboBoxFromModel();
  d->updateAllowedServersComboBoxGeometry();
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::onScrollBarValueChanged(int value)
{
  Q_D(ctkDICOMPatientView);
  Q_UNUSED(value);
  d->updateStudyListViewGeometry();
  this->viewport()->update();
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::onStudiesSelectionChanged(const QStringList &selectedStudiesInstanceUIDs)
{
  emit this->studiesSelectionChanged(selectedStudiesInstanceUIDs);
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::onPatientContextMenuRequested(const QPoint& globalPos, const QModelIndex& clickedPatientIndex)
{
  Q_D(ctkDICOMPatientView);

  if (!clickedPatientIndex.isValid())
  {
    return;
  }

  ctkDICOMStudyListView* studyListView = this->studyListView();
  if (!studyListView)
  {
    return;
  }

  QString patientUID = this->patientUID(clickedPatientIndex);
  if (patientUID.isEmpty())
  {
    return;
  }

  studyListView->createAllSeriesView();
  studyListView->selectAllSeries();

  // Mark the clicked index as having an active context menu
  d->ActiveContextMenuIndex = clickedPatientIndex;

  // Collect all selected patient indices
  QModelIndexList selectedIndices;

  if (this->selectionModel())
  {
    selectedIndices = this->selectionModel()->selectedIndexes();

    // If the clicked index is not in the selection, use only the clicked index
    // This handles the case where user right-clicks on an unselected item
    if (!selectedIndices.contains(clickedPatientIndex))
    {
      selectedIndices.clear();
      selectedIndices.append(clickedPatientIndex);
    }
  }
  else
  {
    // No selection model, just use the clicked index
    selectedIndices.append(clickedPatientIndex);
  }

  // Calculate the position below the context menu button
  ctkDICOMPatientDelegate* delegate = this->patientDelegate();
  QPoint menuPos = globalPos;
  if (delegate)
  {
    QRect itemRect = this->visualRect(clickedPatientIndex);
    QStyleOptionViewItem option;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    initViewItemOption(&option);
#else
    option = viewOptions();
#endif
    QRect buttonRect = delegate->contextMenuButtonRect(itemRect, option);
    // Position menu at the bottom-center of the button
    QPoint buttonBottomCenter = this->viewport()->mapToGlobal(QPoint(buttonRect.left() - 1, buttonRect.bottom() + 2));
    menuPos = buttonBottomCenter;
  }

  QStringList patientUIDs;
  foreach (const QModelIndex& index, selectedIndices)
  {
    QString patientUID = this->patientUID(index);
    if (!patientUID.isEmpty() && !patientUIDs.contains(patientUID))
    {
      patientUIDs.append(patientUID);
    }
  }

  // Re-emit the signal with all selected indices for the widget to handle
  emit this->patientContextMenuRequested(menuPos, patientUIDs);
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::onSelectAllIconClicked(const QModelIndex& patientIndex)
{
  if (!patientIndex.isValid())
  {
    return;
  }

  ctkDICOMStudyListView* studyListView = this->studyListView();
  if (!studyListView || !studyListView->model())
  {
    return;
  }

  QString patientUID = this->patientUID(patientIndex);
  if (patientUID.isEmpty())
  {
    return;
  }

  studyListView->createAllSeriesView();

  // Get current selection count
  int selectedCount = studyListView->numberOfSeriesSelectedByPatient(patientUID);
  if (selectedCount > 0)
  {
    studyListView->selectAllSeriesByPatient(patientUID, QItemSelectionModel::Clear);
  }
  else
  {
    studyListView->selectAllSeriesByPatient(patientUID);
  }

  // Update the viewport to refresh the icon
  if (patientIndex.isValid())
  {
    this->viewport()->update(this->visualRect(patientIndex));
  }
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::onStudyViewEntered()
{
  QEvent leaveEv(QEvent::Leave);
  this->leaveEvent(&leaveEv);
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
  Q_D(ctkDICOMPatientView);

  // Check if AllowedServersRole changed for the current patient
  if (roles.contains(ctkDICOMPatientModel::AllowedServersRole))
  {
    d->updateAllowedServersComboBoxFromModel();
  }
  else if(roles.contains(ctkDICOMPatientModel::OperationStatusRole))
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
void ctkDICOMPatientView::onRowsInserted(const QModelIndex& parent, int first, int last)
{
  Q_UNUSED(parent);
  Q_UNUSED(first);
  Q_UNUSED(last);

  this->refreshLayout();
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::onRowsRemoved(const QModelIndex& parent, int first, int last)
{
  Q_UNUSED(parent);
  Q_UNUSED(first);
  Q_UNUSED(last);

  this->refreshLayout();
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::onModelAboutToBeReset()
{
  Q_D(ctkDICOMPatientView);
  d->ModelResetInProgress = true;

  // Clear the model from the study list view
  if (d->StudyListView)
  {
    d->StudyListView->setModel(nullptr);
    d->StudyListView->hide();
  }

  // Clear cache
  d->CachedStudyFilterProxyModels.clear();
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::onModelReset()
{
  Q_D(ctkDICOMPatientView);
  d->ModelResetInProgress = false;

  this->refreshLayout();

  // Automatically select the first patient if available
  if (this->model() && this->model()->rowCount() > 0)
  {
    QModelIndex firstIndex = this->model()->index(0, 0);
    if (firstIndex.isValid())
    {
      this->setCurrentIndex(firstIndex);
      this->scrollTo(firstIndex);
    }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::onLayoutRefreshed()
{
  Q_D(ctkDICOMPatientView);
  QTimer::singleShot(5, this, [d]() {
    d->updateDisplayModeButtonPosition();
    d->updateAllowedServersComboBoxFromModel();
    d->updateAllowedServersComboBoxGeometry();
    d->StudyListView->refreshLayout(false);
  });
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::onStudySelectionChanged()
{
  this->viewport()->update();
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::onDisplayModeButtonClicked()
{
  Q_D(ctkDICOMPatientView);
  if (d->DisplayMode == ctkDICOMPatientView::TabMode)
  {
    this->setDisplayMode(ctkDICOMPatientView::ListMode);
  }
  else
  {
    this->setDisplayMode(ctkDICOMPatientView::TabMode);
  }
}

//------------------------------------------------------------------------------
void ctkDICOMPatientView::onAllowedServersComboBoxChanged()
{
  Q_D(ctkDICOMPatientView);

  // Get current patient
  QModelIndex currentIndex = this->currentIndex();
  if (!currentIndex.isValid())
  {
    return;
  }

  QString patientUID = this->patientUID(currentIndex);
  if (patientUID.isEmpty())
  {
    return;
  }

  ctkDICOMPatientModel* patientModel = this->patientModel();
  if (!patientModel)
  {
    return;
  }

  // Build the list of allowed servers from checked items
  QStringList allowedServers;
  QAbstractItemModel* comboModel = d->AllowedServersComboBox->checkableModel();
  for (int i = 0; i < d->AllowedServersComboBox->count(); ++i)
  {
    QModelIndex modelIndex = comboModel->index(i, 0);
    if (d->AllowedServersComboBox->checkState(modelIndex) == Qt::Checked)
    {
      QString connectionName = d->AllowedServersComboBox->itemText(i);
      allowedServers.append(connectionName);
    }
  }

  // Save to database and update model
  patientModel->saveAllowedServersToDB(patientUID, allowedServers);
}
