/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/
/*=========================================================================

   Program: ParaView
   Module:    $RCSfile: pqCheckableHeaderView.cxx,v $

   Copyright (c) 2005-2008 Sandia Corporation, Kitware Inc.
   All rights reserved.

   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2. 

   See License_v1.2.txt for the full ParaView license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

// Qt includes
#include <QAbstractItemModel>
#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <QList>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QStyle>

// CTK includes
#include "ctkCheckableHeaderView.h"
#include "ctkCheckBoxPixmaps.h"

//-----------------------------------------------------------------------------
class ctkCheckableHeaderViewPrivate
{
  Q_DECLARE_PUBLIC(ctkCheckableHeaderView);
protected:
  ctkCheckableHeaderView* const q_ptr;
public:
  ctkCheckableHeaderViewPrivate(ctkCheckableHeaderView& object);
  ~ctkCheckableHeaderViewPrivate();
  void init();

  int Pressed;
  ctkCheckBoxPixmaps* CheckBoxPixmaps;
  bool HeaderIsUpdating;
  bool ItemsAreUpdating;
  bool PropagateToItems;
};

//----------------------------------------------------------------------------
ctkCheckableHeaderViewPrivate::ctkCheckableHeaderViewPrivate(ctkCheckableHeaderView& object)
  :q_ptr(&object)
{
  this->HeaderIsUpdating = false;
  this->ItemsAreUpdating = false;
  this->CheckBoxPixmaps = 0;
  this->Pressed = -1;
  this->PropagateToItems = true;
}

//-----------------------------------------------------------------------------
ctkCheckableHeaderViewPrivate::~ctkCheckableHeaderViewPrivate()
{
  if (this->CheckBoxPixmaps)
    {
    delete this->CheckBoxPixmaps;
    this->CheckBoxPixmaps = 0;
    }
}

//----------------------------------------------------------------------------
void ctkCheckableHeaderViewPrivate::init()
{
  Q_Q(ctkCheckableHeaderView);
  this->CheckBoxPixmaps = new ctkCheckBoxPixmaps(q);
}

//----------------------------------------------------------------------------
ctkCheckableHeaderView::ctkCheckableHeaderView(
  Qt::Orientation orient, QWidget *widgetParent)
  : QHeaderView(orient, widgetParent)
  , d_ptr(new ctkCheckableHeaderViewPrivate(*this))
{
  Q_D(ctkCheckableHeaderView);
  d->init();
  if(widgetParent)
    {
    // Listen for focus change events.
    widgetParent->installEventFilter(this);
    }
}

//-----------------------------------------------------------------------------
ctkCheckableHeaderView::~ctkCheckableHeaderView()
{
}

//-----------------------------------------------------------------------------
bool ctkCheckableHeaderView::eventFilter(QObject *, QEvent *e)
{
  if(e->type() != QEvent::FocusIn && 
     e->type() != QEvent::FocusOut)
    {
    return false;
    }
  this->updateHeaders();
  return false;
}

//-----------------------------------------------------------------------------
void ctkCheckableHeaderView::setModel(QAbstractItemModel *newModel)
{
  Q_D(ctkCheckableHeaderView);
  QAbstractItemModel *current = this->model();
  if (current == newModel)
    {
    return;
    }
  if(current)
    {
    this->disconnect(
      current, SIGNAL(headerDataChanged(Qt::Orientation, int, int)),
      this, SLOT(updateHeaderData(Qt::Orientation, int, int)));
    this->disconnect(
      current, SIGNAL(modelReset()),
      this, SLOT(updateHeaders()));
    this->disconnect(
      current, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
      this, SLOT(updateHeadersFromItems(const QModelIndex&, const QModelIndex&)));
    this->disconnect(
      current, SIGNAL(columnsInserted(const QModelIndex &, int, int)), 
      this, SLOT(insertHeaderSection(const QModelIndex &, int, int)));
    this->disconnect(
      current, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
      this, SLOT(insertHeaderSection(const QModelIndex &, int, int)));
    }

  this->QHeaderView::setModel(newModel);
  if(newModel)
    {
    this->connect(
      newModel, SIGNAL(headerDataChanged(Qt::Orientation, int, int)),
      this, SLOT(updateHeaderData(Qt::Orientation, int, int)));
    this->connect(
      newModel, SIGNAL(modelReset()),
      this, SLOT(updateHeaders()));
    if (d->PropagateToItems)
      {
      this->connect(
        newModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
        this, SLOT(updateHeadersFromItems(const QModelIndex&, const QModelIndex&)));
      }
    if(this->orientation() == Qt::Horizontal)
      {
      this->connect(
        newModel, SIGNAL(columnsInserted(const QModelIndex &, int, int)),
        this, SLOT(insertHeaderSection(const QModelIndex &, int, int)));
      }
    else
      {
      this->connect(
        newModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
        this, SLOT(insertHeaderSection(const QModelIndex &, int, int)));
      }
    }

  // Determine which sections are clickable and setup the icons.
  this->updateHeaders();
}

//-----------------------------------------------------------------------------
void ctkCheckableHeaderView::setRootIndex(const QModelIndex &index)
{
  this->QHeaderView::setRootIndex(index);
  this->updateHeaders();
}

//-----------------------------------------------------------------------------
void ctkCheckableHeaderView::setPropagateToItems(bool propagate)
{
  Q_D(ctkCheckableHeaderView);
  if (d->PropagateToItems == propagate)
    {
    return;
    }
  d->PropagateToItems = propagate;
  if (!this->model())
    {
    return;
    }
  if (propagate)
    {
    this->connect(
      this->model(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
      this, SLOT(updateHeadersFromItems(const QModelIndex&, const QModelIndex&)));
    this->updateHeadersFromItems();
    }
  else
    {
    this->disconnect(
      this->model(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
      this, SLOT(updateHeadersFromItems(const QModelIndex&, const QModelIndex&)));
    }
}

//-----------------------------------------------------------------------------
bool ctkCheckableHeaderView::propagateToItems()const
{
  Q_D(const ctkCheckableHeaderView);
  return d->PropagateToItems;
}

//-----------------------------------------------------------------------------
void ctkCheckableHeaderView::toggleCheckState(int section)
{
  // If the section is checkable, toggle the check state.
  if(!this->isCheckable(section))
    {    
    return;
    }
  // I've no strong feeling to turn the state checked or unchecked when the 
  // state is PartiallyChecked.
  this->setCheckState(section, this->checkState(section) == Qt::Checked ? Qt::Unchecked : Qt::Checked);
}

//-----------------------------------------------------------------------------
void ctkCheckableHeaderView::setCheckState(int section, Qt::CheckState checkState)
{
  // If the section is checkable, toggle the check state.
  QAbstractItemModel *current = this->model();
  if(current == 0)
    {    
    return;
    }
  // If the state is unchecked or partially checked, the state
  // should be changed to checked.
  current->setHeaderData(section, this->orientation(),
                         checkState, Qt::CheckStateRole);
}

//-----------------------------------------------------------------------------
void ctkCheckableHeaderView::updateHeaderData(Qt::Orientation orient,
                                              int firstSection,
                                              int lastSection)
{
  if(orient != this->orientation())
    {
    return;
    }
  this->updateHeaders(firstSection, lastSection);
}

//-----------------------------------------------------------------------------
void ctkCheckableHeaderView::updateHeaders(int firstSection, int lastSection)
{
  Q_D(ctkCheckableHeaderView);
  if(d->HeaderIsUpdating)
    {
    return;
    }
  d->HeaderIsUpdating = true;
  QAbstractItemModel *current = this->model();
  Q_ASSERT(current);

  firstSection = qBound(0, firstSection, this->count() -1);
  lastSection = qBound(0, lastSection, this->count() -1);

  bool active = true;
  if(this->parentWidget())
    {
    active = this->parentWidget()->hasFocus();
    }
  int maxJ = this->orientation() == Qt::Horizontal ? 
    current->rowCount() : current->columnCount();
  
  for(int i = firstSection; i <= lastSection; i++)
    {
    QVariant decoration;
    Qt::CheckState checkState;
    if (this->checkState(i, checkState))
      {
      decoration = d->CheckBoxPixmaps->pixmap(checkState, active);
      if (d->PropagateToItems && 
          checkState != Qt::PartiallyChecked &&
          !d->ItemsAreUpdating)
        {
        for (int j = 0 ; j < maxJ; ++j)
          {
          QModelIndex index = this->orientation() == Qt::Horizontal ? 
            current->index(j, i,this->rootIndex()) :
            current->index(i, j,this->rootIndex()) ;
          current->setData(index, checkState, Qt::CheckStateRole);
          }
        }
      }
    current->setHeaderData(i, this->orientation(), decoration,
                           Qt::DecorationRole);
    }
  d->HeaderIsUpdating = false;
}

//-----------------------------------------------------------------------------
void ctkCheckableHeaderView::updateHeadersFromItems()
{
  QAbstractItemModel *currentModel = this->model();
  if (!currentModel)
    {
    return;
    }
  QModelIndex firstIndex = currentModel->index(0,0);
  QModelIndex lastIndex =
    currentModel->index(currentModel->rowCount() - 1,
                        currentModel->columnCount() -1);
  this->updateHeadersFromItems(firstIndex, lastIndex);
}

//-----------------------------------------------------------------------------
void ctkCheckableHeaderView::updateHeadersFromItems(const QModelIndex & topLeft,
                                                    const QModelIndex & bottomRight)
{
  Q_UNUSED(bottomRight);
  Q_D(ctkCheckableHeaderView);
  if(d->ItemsAreUpdating || !d->PropagateToItems || 
     topLeft.parent() != this->rootIndex())
    {
    return;
    }
  d->ItemsAreUpdating = true;
  
  QAbstractItemModel *current = this->model();
  Q_ASSERT(current);

  int lastI;
  int lastJ;
  if (this->orientation() == Qt::Horizontal)
    {
    lastI = this->count();
    lastJ = current->rowCount();
    }
  else
    {
    lastI = this->count();
    lastJ = current->columnCount();
    }
  
  for(int i = 0; i <= lastI; ++i)
    {
    Qt::CheckState sectionState;
    if (!this->checkState(i, sectionState))
      {
      continue;
      }
    bool itemIsCheckable = false;
    // get the first item state
    Qt::CheckState itemsState;
    int j = 0;
    for ( ; j <= lastJ; ++j)
      {
      QModelIndex index = this->orientation() == Qt::Horizontal ? 
        current->index(j, i, topLeft.parent()) : 
        current->index(i, j, topLeft.parent());
      itemsState = static_cast<Qt::CheckState>(
        index.data(Qt::CheckStateRole).toInt(&itemIsCheckable));
      if (itemIsCheckable)
        {
        break;
        }
      }
    if (j > lastJ)
      {// the first item check state couldn't be found
      continue;
      }
    // check the other states to make sure it is the same state
    for (; j <= lastJ; ++j)
      {
      QModelIndex index = this->orientation() == Qt::Horizontal ? 
        current->index(j, i, topLeft.parent()) : 
        current->index(i, j, topLeft.parent());
      Qt::CheckState itemState = 
        static_cast<Qt::CheckState>(index.data(Qt::CheckStateRole).toInt(&itemIsCheckable));
      if (itemIsCheckable && itemState!= itemsState)
        {// there is at least 1 item with a different state
        this->setCheckState(i, Qt::PartiallyChecked);
        break;
        }
      }
    if (j > lastJ)
      {
      this->setCheckState(i, itemsState);
      }
    }
  d->ItemsAreUpdating = false;
}


//-----------------------------------------------------------------------------
void ctkCheckableHeaderView::insertHeaderSection(const QModelIndex &parentIndex,
    int first, int last)
{
  if (this->rootIndex() != parentIndex)
    {
    return;
    }
  this->updateHeaders(first, last);
}

//-----------------------------------------------------------------------------
bool ctkCheckableHeaderView::isCheckable(int section)const
{
  return !this->model()->headerData(section, this->orientation(), Qt::CheckStateRole).isNull();
}

//-----------------------------------------------------------------------------
Qt::CheckState ctkCheckableHeaderView::checkState(int section)const
{
  return static_cast<Qt::CheckState>(
    this->model()->headerData(section, this->orientation(), Qt::CheckStateRole).toInt());
}

//-----------------------------------------------------------------------------
bool ctkCheckableHeaderView::checkState(int section, Qt::CheckState& checkState)const
{
  bool checkable = false;
  checkState = static_cast<Qt::CheckState>(
    this->model()->headerData(section, this->orientation(), Qt::CheckStateRole).toInt(&checkable));
  return checkable;
}

//-----------------------------------------------------------------------------
void ctkCheckableHeaderView::mousePressEvent(QMouseEvent *e)
{
  Q_D(ctkCheckableHeaderView);
  if (e->button() != Qt::LeftButton || 
      d->Pressed >= 0)
    {
    d->Pressed = -1;
    this->QHeaderView::mousePressEvent(e);
    return;
    }
  d->Pressed = -1;
  //check if the check box is pressed
  int pos = this->orientation() == Qt::Horizontal ? e->x() : e->y();
  int section = this->logicalIndexAt(pos);
  if (this->isCheckable(section) &&
      this->isPointInCheckBox(section, e->pos()))
    {
    d->Pressed = section;
    }
  this->QHeaderView::mousePressEvent(e);
}

//-----------------------------------------------------------------------------
void ctkCheckableHeaderView::mouseReleaseEvent(QMouseEvent *e)
{
  Q_D(ctkCheckableHeaderView);
  if (e->button() != Qt::LeftButton || 
      d->Pressed < 0)
    {
    d->Pressed = -1;
    this->QHeaderView::mouseReleaseEvent(e);
    return;
    }
  //check if the check box is pressed
  int pos = this->orientation() == Qt::Horizontal ? e->x() : e->y();
  int section = this->logicalIndexAt(pos);
  if (section == d->Pressed && 
      this->isPointInCheckBox(section, e->pos()))
    {
    d->Pressed = -1;
    this->toggleCheckState(section);
    }
  this->QHeaderView::mousePressEvent(e);
}

//-----------------------------------------------------------------------------
bool ctkCheckableHeaderView::isPointInCheckBox(int section, QPoint pos)const
{
  QRect sectionRect = this->orientation() == Qt::Horizontal ? 
    QRect(this->sectionPosition(section), 0, 
          this->sectionSize(section), this->height()):
    QRect(0, this->sectionPosition(section), 
          this->width(), this->sectionSize(section));
  QStyleOptionHeader opt;
  this->initStyleOption(&opt);
  this->initStyleSectionOption(&opt, section, sectionRect);
  QRect headerLabelRect = this->style()->subElementRect(QStyle::SE_HeaderLabel, &opt, this);
  // from qcommonstyle.cpp:1541
  if (opt.icon.isNull()) 
    {
    return false;
    }
  QPixmap pixmap
    = opt.icon.pixmap(this->style()->pixelMetric(QStyle::PM_SmallIconSize), 
                      (opt.state & QStyle::State_Enabled) ? QIcon::Normal : QIcon::Disabled);
  QRect aligned = this->style()->alignedRect(opt.direction, QFlag(opt.iconAlignment), 
                              pixmap.size(), headerLabelRect);
  QRect inter = aligned.intersected(headerLabelRect);
  return inter.contains(pos);
}

//-----------------------------------------------------------------------------
void ctkCheckableHeaderView::initStyleSectionOption(QStyleOptionHeader *option, int section, QRect rect)const
{
  // from qheaderview.cpp:paintsection
  QStyle::State state = QStyle::State_None;
  if (this->isEnabled())
    {
    state |= QStyle::State_Enabled;
    }
  if (this->window()->isActiveWindow())
    {
    state |= QStyle::State_Active;
    }
  if (this->isSortIndicatorShown() && 
      this->sortIndicatorSection() == section)
    {
    option->sortIndicator = (this->sortIndicatorOrder() == Qt::AscendingOrder)
      ? QStyleOptionHeader::SortDown : QStyleOptionHeader::SortUp;
    }

  // setup the style option structure
  QVariant textAlignment = 
    this->model()->headerData(section, this->orientation(),
                              Qt::TextAlignmentRole);
  option->rect = rect;
  option->section = section;
  option->state |= state;
  option->textAlignment = Qt::Alignment(textAlignment.isValid()
                                        ? Qt::Alignment(textAlignment.toInt())
                                        : this->defaultAlignment());
  
  option->iconAlignment = Qt::AlignVCenter;
  option->text = this->model()->headerData(section, this->orientation(),
                                  Qt::DisplayRole).toString();
  if (this->textElideMode() != Qt::ElideNone)
    {
    option->text = option->fontMetrics.elidedText(option->text, this->textElideMode() , rect.width() - 4);
    }

  QVariant variant = this->model()->headerData(section, this->orientation(),
                                          Qt::DecorationRole);
  option->icon = qvariant_cast<QIcon>(variant);
  if (option->icon.isNull())
    {
    option->icon = qvariant_cast<QPixmap>(variant);
    }
  QVariant foregroundBrush = this->model()->headerData(section, this->orientation(),
                                                  Qt::ForegroundRole);
  if (qVariantCanConvert<QBrush>(foregroundBrush))
    {
    option->palette.setBrush(QPalette::ButtonText, qvariant_cast<QBrush>(foregroundBrush));
    }

  //QPointF oldBO = painter->brushOrigin();
  QVariant backgroundBrush = this->model()->headerData(section, this->orientation(),
                                                  Qt::BackgroundRole);
  if (qVariantCanConvert<QBrush>(backgroundBrush)) 
    {
    option->palette.setBrush(QPalette::Button, qvariant_cast<QBrush>(backgroundBrush));
    option->palette.setBrush(QPalette::Window, qvariant_cast<QBrush>(backgroundBrush));
    //painter->setBrushOrigin(option->rect.topLeft());
    }

  // the section position
  int visual = this->visualIndex(section);
  Q_ASSERT(visual != -1);
  if (this->count() == 1)
    {
    option->position = QStyleOptionHeader::OnlyOneSection;
    }
  else if (visual == 0)
    {
    option->position = QStyleOptionHeader::Beginning;
    }
  else if (visual == this->count() - 1)
    {
    option->position = QStyleOptionHeader::End;
    }
  else
    {
    option->position = QStyleOptionHeader::Middle;
    }
  option->orientation = this->orientation();
  /* the selected position
  bool previousSelected = d->isSectionSelected(this->logicalIndex(visual - 1));
  bool nextSelected =  d->isSectionSelected(this->logicalIndex(visual + 1));
  if (previousSelected && nextSelected)
    option->selectedPosition = QStyleOptionHeader::NextAndPreviousAreSelected;
  else if (previousSelected)
    option->selectedPosition = QStyleOptionHeader::PreviousIsSelected;
  else if (nextSelected)
    option->selectedPosition = QStyleOptionHeader::NextIsSelected;
  else
    option->selectedPosition = QStyleOptionHeader::NotAdjacent;
  */
}
    
