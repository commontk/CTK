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
/*=========================================================================

   Program: ParaView
   Module:  pqCheckableHeaderView.cxx

   Copyright (c) 2005-2008 Sandia Corporation, Kitware Inc.
   All rights reserved.

   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2. 

   See http://www.paraview.org/paraview/project/license.html for the full ParaView license.
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
#include <ctkCheckableModelHelper.h>
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

  ctkCheckableModelHelper* CheckableModelHelper;
  int                 Pressed;
  ctkCheckBoxPixmaps* CheckBoxPixmaps;
  bool                HeaderIsUpdating;
};

//----------------------------------------------------------------------------
ctkCheckableHeaderViewPrivate::ctkCheckableHeaderViewPrivate(ctkCheckableHeaderView& object)
  :q_ptr(&object)
{
  this->CheckableModelHelper = 0;
  this->Pressed = -1;
  this->CheckBoxPixmaps = 0;
  this->HeaderIsUpdating = false;
}

//-----------------------------------------------------------------------------
ctkCheckableHeaderViewPrivate::~ctkCheckableHeaderViewPrivate()
{
  if (this->CheckBoxPixmaps)
    {
    delete this->CheckBoxPixmaps;
    this->CheckBoxPixmaps = 0;
    }
  if (this->CheckableModelHelper)
    {
    delete this->CheckableModelHelper;
    this->CheckableModelHelper = 0;
    }
}

//----------------------------------------------------------------------------
void ctkCheckableHeaderViewPrivate::init()
{
  Q_Q(ctkCheckableHeaderView);
  this->CheckBoxPixmaps = new ctkCheckBoxPixmaps(q);
  this->CheckableModelHelper = new ctkCheckableModelHelper(q->orientation(), q);
}

//----------------------------------------------------------------------------
ctkCheckableHeaderView::ctkCheckableHeaderView(
  Qt::Orientation orient, QWidget *widgetParent)
  : QHeaderView(orient, widgetParent)
  , d_ptr(new ctkCheckableHeaderViewPrivate(*this))
{
  Q_D(ctkCheckableHeaderView);
  d->init();
  // TODO: doesn't support reparenting here.
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
Qt::CheckState ctkCheckableHeaderView::checkState(int section)const
{
  Q_D(const ctkCheckableHeaderView);
  return d->CheckableModelHelper->headerCheckState(section);
}

//-----------------------------------------------------------------------------
bool ctkCheckableHeaderView::checkState(int section, Qt::CheckState& checkState)const
{
  Q_D(const ctkCheckableHeaderView);
  return d->CheckableModelHelper->headerCheckState(section, checkState);
}

//-----------------------------------------------------------------------------
void ctkCheckableHeaderView::setCheckState(int section, Qt::CheckState checkState)
{
  Q_D(ctkCheckableHeaderView);
  d->CheckableModelHelper->setHeaderCheckState(section, checkState);
}

//-----------------------------------------------------------------------------
ctkCheckableModelHelper* ctkCheckableHeaderView::checkableModelHelper()const
{
  Q_D(const ctkCheckableHeaderView);
  return d->CheckableModelHelper;
}

//-----------------------------------------------------------------------------
bool ctkCheckableHeaderView::eventFilter(QObject *, QEvent *e)
{
  if(e->type() != QEvent::FocusIn && 
     e->type() != QEvent::FocusOut)
    {
    return false;
    }
  //this->updateHeaderPixmaps();
  return false;
}

//-----------------------------------------------------------------------------
void ctkCheckableHeaderView::setModel(QAbstractItemModel *newModel)
{
  Q_D(ctkCheckableHeaderView);
  QAbstractItemModel *current = this->model();
  if (current)
    {
    this->disconnect(
      current, SIGNAL(headerDataChanged(Qt::Orientation,int,int)),
      this, SLOT(onHeaderDataChanged(Qt::Orientation,int,int)));
    this->disconnect(
      current, SIGNAL(modelReset()),
      this, SLOT(updateHeaderPixmaps()));
    this->disconnect(
      current, SIGNAL(columnsInserted(QModelIndex,int,int)), 
      this, SLOT(onHeaderSectionInserted()));
    this->disconnect(
      current, SIGNAL(rowsInserted(QModelIndex,int,int)),
      this, SLOT(onHeaderSectionInserted()));
    }
  this->QHeaderView::setModel(newModel);
  d->CheckableModelHelper->setModel(newModel);
  if(newModel)
    {
    this->connect(
      newModel, SIGNAL(headerDataChanged(Qt::Orientation,int,int)),
      this, SLOT(onHeaderDataChanged(Qt::Orientation,int,int)));
    this->connect(
      newModel, SIGNAL(modelReset()),
      this, SLOT(updateHeaderPixmaps()));
    if(this->orientation() == Qt::Horizontal)
      {
      this->connect(
        newModel, SIGNAL(columnsInserted(QModelIndex,int,int)),
        this, SLOT(onHeaderSectionInserted()));
      }
    else
      {
      this->connect(
        newModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
        this, SLOT(onHeaderSectionInserted()));
      }
    }
  this->updateHeaderPixmaps();
}

//-----------------------------------------------------------------------------
void ctkCheckableHeaderView::setRootIndex(const QModelIndex &index)
{
  Q_D(ctkCheckableHeaderView);
  this->QHeaderView::setRootIndex(index);
  d->CheckableModelHelper->setRootIndex(index);
}

//-----------------------------------------------------------------------------
void ctkCheckableHeaderView::onHeaderDataChanged(Qt::Orientation orient,
                                              int firstSection,
                                              int lastSection)
{
  if(orient != this->orientation())
    {
    return;
    }
  // update pixmap
  this->updateHeaderPixmaps(firstSection, lastSection);
}

//-----------------------------------------------------------------------------
void ctkCheckableHeaderView::updateHeaderPixmaps(int firstSection, int lastSection)
{
  Q_D(ctkCheckableHeaderView);
  QAbstractItemModel *current = this->model();
  if(d->HeaderIsUpdating || !current)
    {
    return;
    }
  d->HeaderIsUpdating = true;

  firstSection = qBound(0, firstSection, this->count() -1);
  lastSection = qBound(0, lastSection, this->count() -1);

  bool active = true;
  if(this->parentWidget())
    {
    active = this->parentWidget()->hasFocus();
    }
  for(int i = firstSection; i <= lastSection; i++)
    {
    QVariant decoration;
    Qt::CheckState checkState;
    if (d->CheckableModelHelper->headerCheckState(i, checkState))
      {
      decoration = d->CheckBoxPixmaps->pixmap(checkState, active);
      }
    current->setHeaderData(i, this->orientation(), decoration,
                           Qt::DecorationRole);
    }
  d->HeaderIsUpdating = false;
}

//-----------------------------------------------------------------------------
void ctkCheckableHeaderView::onHeaderSectionInserted()
{
  this->updateHeaderPixmaps();
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
  if (d->CheckableModelHelper->isHeaderCheckable(section) &&
      this->isPointInCheckBox(section, e->pos()))
    {
    d->Pressed = section;
    }
  else
    {
    this->QHeaderView::mousePressEvent(e);
    }
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
    d->CheckableModelHelper->toggleHeaderCheckState(section);
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
  if (foregroundBrush.canConvert<QBrush>())
    {
    option->palette.setBrush(QPalette::ButtonText, qvariant_cast<QBrush>(foregroundBrush));
    }

  //QPointF oldBO = painter->brushOrigin();
  QVariant backgroundBrush = this->model()->headerData(section, this->orientation(),
                                                  Qt::BackgroundRole);
  if (backgroundBrush.canConvert<QBrush>())
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
    
